/*KINOPED_LEG.H
  
  Author: Elijah Retzlaff
          elijah@innovationshealth.com
          April 8, 2021
          
  Purpose: Define and descibe the interface of the KinopedLeg object class. Full implementation of the class is completed in kinoped_leg.cpp file. 
           
  Last edited: 04/13/2021
*/

#ifndef KP07_V11_MANIPULATION_KINOPED_LEG_H
#define KP07_V11_MANIPULATION_KINOPED_LEG_H

#include <ros/ros.h>

#include <queue>
#include "std_msgs/Float64MultiArray.h"

#include <moveit/robot_model_loader/robot_model_loader.h>
#include <moveit/robot_model/robot_model.h>
#include <moveit/robot_state/robot_state.h>
#include <moveit/robot_state/conversions.h>
#include <moveit_msgs/DisplayRobotState.h>

#include <geometry_msgs/PoseStamped.h>
#include <sensor_msgs/JointState.h>

#include <tf/transform_broadcaster.h>
#include <tf/transform_listener.h>
#include <tf_conversions/tf_eigen.h>

#include <Eigen/Core>
#include <Eigen/Geometry>


//class for a single leg of the kinoped to implement control of it with MoveIt!
class KinopedLeg {

  private:
  
    //Kinematics
    robot_model::RobotModelPtr kinematic_model;
    robot_state::RobotStatePtr kinematic_state;
    robot_state::JointModelGroup* joint_model_group;
    
    
    //Topic publishers and subscribers 
    tf::TransformBroadcaster tf_broadcaster;


    int controller_foot;

    
  public:
  
    // Methods

    //Default constructor
    KinopedLeg(std::string joint_model_group_name, int controller_foot, robot_model::RobotModelPtr kinematic_model, robot_state::RobotStatePtr kinematic_state, ros::NodeHandle n);
    
    void control(const std_msgs::Float64MultiArray::ConstPtr& msg);

    void broadcastPose(geometry_msgs::Pose pose, std::string name);


};



struct SeedDistanceFunctor {

    using Solution = std::vector<double>;
    const Solution seed;
SeedDistanceFunctor(Solution _seed) : seed(std::move(_seed)) {}
    static double distance(const Solution& a, const Solution& b)
        {
            assert(a.size() == b.size());
            double d = 0.0;
            for (size_t i = 0; i < a.size(); ++i)
            {
                //d += fabs(a[i]-b[i]);
                d += pow(a[i]-b[i], 2);
            }
            return d;
        }

    // NB: priority_queue is a max-heap structure, so less() should actually return >
    // "highest priority"
    bool operator()(const Solution& a, const Solution& b) const
        {
            return distance(seed, a) > distance(seed, b);
        }
        
};


#endif //KP07_V11_MANIPULATION_KINOPED_LEG_H
