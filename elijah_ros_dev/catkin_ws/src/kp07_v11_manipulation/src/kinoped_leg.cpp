/*KINOPED_LEG.CPP
  
  Author: Elijah Retzlaff
          elijah@innovationshealth.com
          April 8, 2021
          
  Purpose: All functions needed to create and interact with the KinopedLeg object class. 
           
  Last edited: 04/13/2021
*/


#include "kinoped_leg.h"



/* Default constructor
/// \param joint_model_group_name Name of joint model group to be controlled
/// \param controller_foot Foot role (0 = invalid, 1 = left, 2 = right)
/// \param kinematic_model Reference to global kinematic model
/// \param n Reference to global node handle
*/
KinopedLeg::KinopedLeg(std::string joint_model_group_name, int controller_foot, robot_model::RobotModelPtr kinematic_model, robot_state::RobotStatePtr kinematic_state, ros::NodeHandle n) {
    
    //general setup
    std::string name = joint_model_group_name;
    this->controller_foot = controller_foot;
    this->kinematic_model = kinematic_model;
    this->kinematic_state = kinematic_state;
    
    //get joint group to be conrolled
    joint_model_group = kinematic_model->getJointModelGroup(joint_model_group_name);
    kinematic_state->setToDefaultValues();
    
}



/* control         
  THIS IS ONLY CALLED BY THE move-legs-basic-node CALLBACK function
  Called every time a new position is published
*/
void KinopedLeg::control(const std_msgs::Float64MultiArray::ConstPtr& msg) {

  //got the target pose from the message
  
  //foot constant for later
  int foot_const = 0;
  
  //determine what foot is being moved
    //1 = "left, 2 = "right"
  if(controller_foot == 2) {
    foot_const = 1;
  }
  
  
  // Generate IK solutions
  const kinematics::KinematicsBaseConstPtr& solver = joint_model_group->getSolverInstance();
  assert(solver.get());
  
  Eigen::Isometry3d solverTrobot = Eigen::Isometry3d::Identity();
  kinematic_state->setToIKSolverFrame(solverTrobot, solver);
  
  //create target pose from the message
  std::vector<geometry_msgs::Pose> target_poses;
  geometry_msgs::Pose pose;
  pose.orientation.w = 1.0;
  pose.orientation.x = msg->data.at(0 + 3*foot_const);
  pose.orientation.y = msg->data.at(0 + 4*foot_const);
  pose.orientation.z = msg->data.at(0 + 5*foot_const);
  target_poses.push_back(pose);

  //solve for the new target position
  std::vector<double> seed;
  kinematic_state->copyJointGroupPositions(joint_model_group, seed);
  std::vector<std::vector<double>> solutions;
  kinematics::KinematicsResult result;
  kinematics::KinematicsQueryOptions options;
  options.discretization_method = kinematics::DiscretizationMethod::ALL_DISCRETIZED;

  solver->getPositionIK(target_poses, seed, solutions, result, options);

  // Pick the solution that matches closest to the measured joint state
  if (!solutions.empty()) {
    SeedDistanceFunctor functor(seed);
    std::priority_queue<std::vector<double>, std::vector<std::vector<double>>, SeedDistanceFunctor> slnQueue(solutions.begin(), solutions.end(), functor);
    kinematic_state->setJointGroupPositions(joint_model_group, slnQueue.top());

  }

  std::cerr << "Got " << solutions.size() << " solutions for " << joint_model_group->getName() << std::endl;
  
  
  //broadcast pose to tf
  broadcastPose(pose, "End effector pose");
  
  
  //kinoped will be moved to new joint values by the callback function 
  
}



/* broadcastPose
*/
void KinopedLeg::broadcastPose(geometry_msgs::Pose pose, std::string name) {

    tf::Transform tf_pose;
    tf::poseMsgToTF(pose, tf_pose);
    tf_broadcaster.sendTransform(tf::StampedTransform(tf_pose, ros::Time::now(), kinematic_model->getRootLinkName(), joint_model_group->getName() + "/" + name));
}
