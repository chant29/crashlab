################################################################################
# Set minimum required version of cmake, project name and compile options
################################################################################
cmake_minimum_required(VERSION 3.5)
project(navi)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

################################################################################
# Find ament packages and libraries for ament and system dependencies
################################################################################
include_directories(include)
# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(nav2_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(std_srvs REQUIRED) 
find_package(tf2_geometry_msgs REQUIRED)
find_package(nav2_behavior_tree REQUIRED)
find_package(behaviortree_cpp_v3 REQUIRED)

add_executable(bt_main src/bt_main.cpp)  
ament_target_dependencies(bt_main rclcpp  rclcpp_action geometry_msgs behaviortree_cpp_v3 std_msgs ament_index_cpp nav2_behavior_tree nav2_msgs) 

add_executable(goal_publisher src/goal_publisher.cpp)  
ament_target_dependencies(goal_publisher rclcpp std_msgs) 
 
target_link_libraries(bt_main ${ZMQ_LIBRARIES})
################################################################################
# Install
################################################################################
install(
  DIRECTORY launch map param rviz behavior_tree include
  DESTINATION share/${PROJECT_NAME}
)

install(TARGETS
  goal_publisher
  bt_main
  DESTINATION lib/${PROJECT_NAME}
)

################################################################################
# Macro for ament package
################################################################################
ament_package()
