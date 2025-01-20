#pragma once

#include "behaviortree_cpp_v3/condition_node.h"
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/bool.hpp"

class GoalCheckerPub : public BT::ConditionNode
{
public:
    GoalCheckerPub(const std::string &name, const BT::NodeConfiguration &config)
        : BT::ConditionNode(name, config)
    {
        // Initialize ROS 2 node
        node_ = rclcpp::Node::make_shared("goal_checker_pub");

        // Create a publisher for the 'goalcheck' topic
        goalcheck_publisher_ = node_->create_publisher<std_msgs::msg::Bool>("goalcheck", 10);
    }

    // Method required for ConditionNode
    BT::NodeStatus tick() override
    {
        // Always publish true to the 'goalcheck' topic
        std_msgs::msg::Bool msg;
        msg.data = true;

        goalcheck_publisher_->publish(msg);
        RCLCPP_INFO(node_->get_logger(), "Published goalcheck status: true");

        // Always return SUCCESS for this condition
        return BT::NodeStatus::SUCCESS;
    }

    // Static method to define ports (if needed)
    static BT::PortsList providedPorts()
    {
        return {}; // No input or output ports are required
    }

private:
    rclcpp::Node::SharedPtr node_;
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr goalcheck_publisher_;
};
