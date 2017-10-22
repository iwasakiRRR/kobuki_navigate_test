#include <ros/ros.h>
#include <iostream>
#include <tf/tf.h>
#include <std_msgs/MultiArrayLayout.h>
#include <std_msgs/MultiArrayDimension.h>
#include <std_msgs/Int32MultiArray.h>
#include <std_msgs/Float32MultiArray.h>
#include <std_msgs/Float32.h>
#include <sensor_msgs/LaserScan.h>
#include <geometry_msgs/Twist.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <move_base_msgs/MoveBaseGoal.h>
#include <actionlib/client/simple_action_client.h>

//actionclient オブジェクト生成
typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseActionClient;

ros::Subscriber sub;

//四元数で検索 角度関連のお話（Quaternion,Roll,Pitch,Yaw）
static geometry_msgs::Quaternion CreateQuaternionFromRPY(double roll, double pitch, double yaw) {
  // https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Source_Code
  // http://docs.ros.org/api/geometry_msgs/html/msg/Quaternion.html
  geometry_msgs::Quaternion q;
  double t0 = cos(yaw * 0.5);
  double t1 = sin(yaw * 0.5);
  double t2 = cos(roll * 0.5);
  double t3 = sin(roll * 0.5);
  double t4 = cos(pitch * 0.5);
  double t5 = sin(pitch * 0.5);
  q.w = t0 * t2 * t4 + t1 * t3 * t5;
  q.x = t0 * t3 * t4 - t1 * t2 * t5;
  q.y = t0 * t2 * t5 + t1 * t3 * t4;
  q.z = t1 * t2 * t4 - t0 * t3 * t5;
  return q;
}

void NavigateCallback(const std_msgs::Float32MultiArray::ConstPtr& array_msg){
  MoveBaseActionClient* move_base_action_client = new MoveBaseActionClient("move_base",true);
  move_base_msgs::MoveBaseGoal goal;
  geometry_msgs::Quaternion qua;
  double array_waypoint[10] = {};
  int i = 0;

  //topicの中身を取り出している
  for(std::vector<float>::const_iterator c_i = array_msg->data.begin(); c_i != array_msg->data.end(); c_i++){
    array_waypoint[i] = *c_i;
    std::cout<<"array_waypoint["<<i<<"] is "<<array_waypoint[i]<<std::endl;
    i++;
  }
  //serverが立ち上がるのを待つ
  if( move_base_action_client->waitForServer(ros::Duration(5)) == 1){
    std::cout<<"wait for action client rising up"<<std::endl;
  }
  //目標を指定
  goal.target_pose.header.frame_id = "map";
  goal.target_pose.header.stamp = ros::Time::now();
  goal.target_pose.pose.position.x = array_waypoint[0];
  goal.target_pose.pose.position.y = array_waypoint[1];

  //角度計算
  qua = CreateQuaternionFromRPY(0.0,0.0,array_waypoint[2]);
  
  goal.target_pose.pose.orientation.x = qua.x;
  goal.target_pose.pose.orientation.y = qua.y;
  goal.target_pose.pose.orientation.z = qua.z;
  goal.target_pose.pose.orientation.w = qua.w;
  
  //ゴールを送る
  move_base_action_client->sendGoal(goal);
  std::cout<<"send goal"<<std::endl;
  //navigationの結果が来るのを待つ
  move_base_action_client->waitForResult();
  //成功すればsucceeded,失敗すればfailed
  if(move_base_action_client->getState() == actionlib::SimpleClientGoalState::SUCCEEDED){
    std::cout<<"Succeeded"<<std::endl;
  }
  else{
    std::cout<<"Failed"<<std::endl;
  }
}

int main(int argc,char** argv){
  ros::init(argc,argv,"kobuki_navi_test");
  ros::NodeHandle nh;
  sub = nh.subscribe("kobuki_navigation_test",10,NavigateCallback);
  std::cout<<"start"<<std::endl;
  ros::spin();
  
}
