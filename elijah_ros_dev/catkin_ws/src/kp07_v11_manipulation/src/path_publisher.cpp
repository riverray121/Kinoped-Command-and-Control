/*PATH_PUBLISHER.CPP
  
  Author: Elijah Retzlaff
          elijah@innovationshealth.com
          April 12, 2021
          
  Purpose: Framework for the path_publisher node to read in data points for the movement of the left and right leg of the kinoped, recorded in using the xsens sensors, and then publish these data points for use by the move_legs_basic_node to controll the movement of the legs using MoveIt! 
           
  Last edited: 04/13/2021
*/

#include <ros/ros.h>
#include "std_msgs/Float64MultiArray.h"

#include <sstream>
#include <iostream>
#include <fstream>
#include <string>


int main(int argc, char **argv) {
  
  //setup path publisher node 
  ros::init(argc, argv, "path_publisher");
  
  //fully initialize the node
  ros::NodeHandle n;
  
  //template to publish info to the node 
  ros::Publisher pub_path = n.advertise<std_msgs::Float64MultiArray>("path_publisher", 10); 
  
  //rate to loop the publishing at (in Hz)
    //note: xsens records at 60 fps, so xsens data must be parsed down to every 6 points 
  ros::Rate loop_rate(10);
  
  //input from file setup
  std::ifstream pathFile("/home/elijah/path_document/path.txt");
  float readInFloat;
  
  
  //check that the file has been opened
  if(pathFile.is_open()) {
    
    //publish messages to the publisher as long as "CTR-C" is  not pushed
  while(ros::ok()) {
    
    //create message object "msg"
    std_msgs::Float64MultiArray msg;
    
    //populate the array from read in data 
    for(int i = 0; i < 6; i++) {
      
      //check that the end of the file has not been reached
      if(!pathFile.eof()) {
      
        pathFile >> readInFloat;
      
      } else {
      
        readInFloat = 1.000;
      
      }
      
      //add the float to the message
      msg.data.push_back(readInFloat);
      
    } 
    
    
    //debuging
    //ROS_INFO("%f", msg.data.at(0));
    
    
    //publish message to node using template from above
    pub_path.publish(msg);
    
    //not nessisary unless there is a subscriber?
    ros::spinOnce(); 
    
    //sleep so the publish rate is maintained (from above)
    loop_rate.sleep();
  
  }
  
  //close the file 
  pathFile.close();
  
  //file was not opened 
  } else {
  
    ROS_INFO("Could not open path file");
  
  }
  

  return 0;
}
