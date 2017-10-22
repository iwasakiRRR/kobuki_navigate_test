command

$ roslaunch turtlebot_gazebo turtlebot_world.launch
$ roslaunch turtlebot_gazebo amcl_demo.launch map_file:=/opt/ros/indigo/share/turtlebot_gazebo/maps/playground.yaml
$ roslaunch turtlebot_rviz_launchers view_navigation.launch

$ rosrun kobuki_navigate_test kobuki_navigate_test



If you want to use 'rostopic pub', follow the undermentioned format.

rostopic pub /kobuki_navigation_test std_msgs/Float32MultiArray "layout:
  dim: []
  data_offset: 0
data: [1.4, 4.65,5.2]"
