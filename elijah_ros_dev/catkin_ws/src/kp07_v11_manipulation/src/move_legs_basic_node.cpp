/*MOVE_LEGS_BASIC_NODE.CPP
  
  Author: Elijah Retzlaff
          elijah@innovationshealth.com
          April 8, 2021
          
  Purpose: Framework for running the node which subscribes to the the 
           path_publisher, moves the Kinoped robot (in/with MoveIt!) and 
           publishes the Kinoped movements and robot state for visualization.
           Uses the KinopedLeg object for describing and moveing each
           individual leg. 
           
  Last edited: 04/13/2021
  
*/

#include <ros/ros.h>
#include "std_msgs/Float64MultiArray.h"

#include <moveit_msgs/DisplayRobotState.h>
#include <moveit_msgs/DisplayTrajectory.h>

#include <moveit_msgs/AttachedCollisionObject.h>
#include <moveit_msgs/CollisionObject.h>

#include <moveit_visual_tools/moveit_visual_tools.h>


// MoveIt!
#include <moveit/robot_model_loader/robot_model_loader.h>
#include <moveit/robot_model/robot_model.h>
#include <moveit/robot_state/conversions.h>
#include <moveit/robot_state/robot_state.h>

// TF
#include <tf/transform_broadcaster.h>


//KinopedLeg object
#include "kinoped_leg.h"



//CLASS FOR CONTROLLING BOTH LEGS AT THE SAME TIME 
class BothKinopedLegs {

  private:

    ros::NodeHandle n;
    
    ros::Subscriber sub_path;
    
    ros::Publisher pub_display_robot_state;

    robot_model::RobotModelPtr kinematic_model;
    robot_state::RobotStatePtr kinematic_state;

    KinopedLeg* kinoped_legs[2];

  public:

    //DEFAULT CONSTRUCTOR
    //initializes the KinopedLegs object and communicative nodes
    BothKinopedLegs() {
    
      //NodeHandle "n" is initialized
      //ROS subscriber "sub_path" is initialized
      //ROS publisher "pub_display_robot_state" is initialized
      
      //setup subscriber to recieve path from path publisher
      sub_path = n.subscribe("path_publisher", 10, &BothKinopedLegs::callback, this);
      
      //setup pub_display_robot_state publisher
      pub_display_robot_state = n.advertise<moveit_msgs::DisplayRobotState>("display_robot_state", 1);

      // Initialize kinematic model from params server
      robot_model_loader::RobotModelLoader robot_model_load("robot_description");

      kinematic_model = robot_model_load.getModel();
      kinematic_state = std::make_shared<robot_state::RobotState>(kinematic_model);
      
      //Initialize each leg passing in the two planning groups 
      kinoped_legs[0] = new KinopedLeg("left_leg", 1, kinematic_model, kinematic_state, n);
      kinoped_legs[1] = new KinopedLeg("right_leg", 2, kinematic_model, kinematic_state, n);
        
        
    }
    
    
    //CALLBACK FUNCTION
    //called when subscriber recieves message
    void callback(const std_msgs::Float64MultiArray::ConstPtr& msg) {

        //TEMPORARY CODE
        ROS_INFO("I heard: [%f]", msg->data.at(0));

        //for each leg of the kinoped
        for (auto &kinoped_leg : kinoped_legs) {
            //run the control function from KinopedLeg object on each leg to move them as needed
            kinoped_leg->control(msg);
        }        

        //publish the new state of the Kinoped Legs 
        moveit_msgs::DisplayRobotState display_robot_state;
        robot_state::robotStateToRobotStateMsg(*kinematic_state, display_robot_state.state);
        pub_display_robot_state.publish(display_robot_state);
        
    }
    
};



//MAIN TO RUN CONTROLL OF THE KINOPED LEGS
int main(int argc, char** argv) {

    //Initialize the "move_legs_basic_node" node
    ros::init(argc, argv, "move_legs_basic_node");
    
    //Call the BothKinopedLegs constructor to setup the node 
    BothKinopedLegs move_legs_basic_node;

    //Start spinner
    ros::spin();
    
    //end when ros shuts down 
    ros::shutdown();
    return 0;
}
