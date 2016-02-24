#include <ros/ros.h>

#include <std_msgs/String.h>

#include <people_msgs/PositionMeasurement.h>
#include <people_msgs/PositionMeasurementArray.h>

#include <sensor_msgs/LaserScan.h>

#include <visualization_msgs/Marker.h>

#include<string.h>
#include<math.h>

using namespace std;

struct distanceFromPR2{
  string  objectId;
  float distance;
  double t0, t1, speed, xdistance, ydistance;
}peopleData[100]; 

int noOfPeople = 0;

void peoplePositionCallback(const people_msgs::PositionMeasurementArray::ConstPtr& msg){
  
  people_msgs::PositionMeasurementArray peopleArray;
  people_msgs::PositionMeasurement peoplePos;   
  int newPersonFound = 0;
  float currentDistance, distance_travelled;
  std::vector<people_msgs::PositionMeasurement> people;
  
  //ROS_INFO("Frame id : [%s]", msg->header.frame_id.c_str());
  
  for( int i = 0; i < msg->people.size(); i++ ){
    
    int d = msg->people.size();
    
    ROS_INFO("\n\n\tPerson name : %s (data from laser scan) \n[No of people : %d] \n\t", msg->people[i].name.c_str(), d);
    
    
    for( int j = 0; j < noOfPeople; j++){
      newPersonFound = 0;
      ROS_INFO("Name :  [%s    -    %s] ",msg->people[i].name.c_str(), peopleData[j].objectId.c_str());
      
      //Tracking the person
      if( strcmp(msg->people[i].name.c_str() , peopleData[j].objectId.c_str()) == 0 ) {
        
        ROS_INFO("Match Found [%s]  --  length of structure [%d]", peopleData[j].objectId.c_str(), noOfPeople);
        ROS_INFO("\n\tPerson Details ");
        ROS_INFO("Person Id : %s", peopleData[j].objectId.c_str());
        ROS_INFO("Previous Distance : %f", sqrt(pow(peopleData[j].xdistance,2) + pow(peopleData[j].ydistance,2)));
        
        peopleData[j].xdistance = msg->people[i].pos.x;
        peopleData[j].ydistance = msg->people[i].pos.y;
        ROS_INFO("X distance : %f ", peopleData[j].xdistance);
        ROS_INFO("Y distance : %f ", peopleData[j].ydistance);
        
        
        currentDistance = sqrt(pow(peopleData[j].xdistance,2)+pow(peopleData[j].ydistance,2));
        distance_travelled =  currentDistance - peopleData[j].distance ;
        peopleData[j].distance = currentDistance;
        peopleData[j].t1 = ros::Time::now().toSec();
        peopleData[j].speed =  distance_travelled / ( peopleData[j].t1 -  peopleData[j].t0);
        peopleData[j].t0 =  peopleData[j].t1; 
        
        ROS_INFO("Time T1 : %f", peopleData[j].t1);
        ROS_INFO("Speed : %f", peopleData[j].speed);
        
        ROS_INFO("Current Distance : %f", currentDistance);
        ROS_INFO("Distance travelled : %f", distance_travelled);
        
        newPersonFound = 1;
        //break;
      }
      
    }
    
    //Push the details of new person
    if(newPersonFound == 0){
      peopleData[noOfPeople].objectId = msg->people[i].name.c_str();
      peopleData[noOfPeople].distance = sqrt(pow(msg->people[i].pos.x,2) + pow(msg->people[i].pos.y,2)); //2-D distance
      peopleData[noOfPeople].t0 = ros::Time::now().toSec();
      peopleData[noOfPeople].xdistance = msg->people[i].pos.x;
      peopleData[noOfPeople].ydistance = msg->people[i].pos.y;
      peopleData[noOfPeople].speed = 0.0;
      
      ROS_INFO("New Person to the structure : %s", msg->people[i].name.c_str());
      ROS_INFO("Struct data Person Id : %s", peopleData[noOfPeople].objectId.c_str());
       ROS_INFO("\n\tPerson Details ");
       ROS_INFO("Person Id : %s", peopleData[noOfPeople].objectId.c_str());
       ROS_INFO("Position : %f    %f", peopleData[noOfPeople].xdistance,peopleData[noOfPeople].ydistance);
       ROS_INFO("Distance : %f", peopleData[noOfPeople].distance);
       ROS_INFO("Time T0 : %f", peopleData[noOfPeople].t0);
      ++noOfPeople;
      ROS_INFO("No of objects Incrementd, new : %d", noOfPeople);
      
    }
    
  }
  
}

int main( int argc, char* argv[] ){
  
  // Initialize the ROS system and specify the node name.
  ros::init(argc,argv,"legdata") ;
  
  // Establish this program as a ROS node.
  ros::NodeHandle nh ;  
  noOfPeople = 0;
 
  ros::Subscriber people_sub = nh.subscribe("people_tracker_measurements", 1000, peoplePositionCallback);
  
  ros::spin();
  
  return 0;
}
