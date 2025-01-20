#!/usr/bin/env python3

import os
from os import environ

from ament_index_python.packages import get_package_share_directory

from launch.conditions import IfCondition
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, Shutdown
from launch.substitutions import PathJoinSubstitution, Command, LaunchConfiguration
from launch_ros.substitutions import FindPackageShare
from launch_ros.actions import Node


def generate_launch_description():
  ld = LaunchDescription()

  use_sim_time = LaunchConfiguration("use_sim_time")
  use_sim_time_arg = DeclareLaunchArgument('use_sim_time', default_value="true")
  
  use_rviz = LaunchConfiguration("use_rviz")  
  use_rviz_arg = DeclareLaunchArgument('use_rviz', default_value="true")

  jsp_node = Node(
    package='joint_state_publisher',
    executable='joint_state_publisher',
    name='joint_state_publisher',
    parameters=[{'use_sim_time': use_sim_time}]
  )

  urdf_file = os.path.join(get_package_share_directory('simya_description'),
                                                  'urdf',
                                                  'robot.urdf')
  print(urdf_file)
  
  with open(urdf_file, 'r') as urdf:
      robot_description = urdf.read()


  rsp_node = Node(
    package='robot_state_publisher',
    executable='robot_state_publisher',
    name='robot_state_publisher',
    parameters=[{
      'use_sim_time': use_sim_time,
      'robot_description': robot_description}]
  )


  

  rviz_node = Node(
    package='rviz2',
    executable='rviz2',
    name='rviz2',
    # arguments=['-d', rviz_config_dir],
    output='screen',
    condition=IfCondition(use_rviz)
  )

  ld.add_action(use_rviz_arg)
  ld.add_action(use_sim_time_arg)
  ld.add_action(jsp_node)
  ld.add_action(rsp_node)
  ld.add_action(rviz_node)

  
  return ld