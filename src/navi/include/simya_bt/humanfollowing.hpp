
#include <behaviortree_cpp_v3/action_node.h>
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "std_msgs/msg/int32_multi_array.hpp"
#include "std_msgs/msg/bool.hpp"

class HumanFollowing : public BT::AsyncActionNode
{
public:
    HumanFollowing(const std::string &name, const BT::NodeConfiguration &config)
        : BT::AsyncActionNode(name, config), human_stop_(false)
    {
        node_ = rclcpp::Node::make_shared("human_following");
        cmd_vel_publisher_ = node_->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);

        bounding_box_subscriber_ = node_->create_subscription<std_msgs::msg::Int32MultiArray>(
            "bounding_box", 10, std::bind(&HumanFollowing::bounding_box_callback, this, std::placeholders::_1));
        human_stop_subscriber_ = node_->create_subscription<std_msgs::msg::Bool>(
            "human_stop", 10, std::bind(&HumanFollowing::human_stop_callback, this, std::placeholders::_1));

        camera_center_x_ = 640; // Default value
        tolerance_ = 200;        // Default value

        RCLCPP_INFO(node_->get_logger(), "Human Following Node has started.");
    }

    static BT::PortsList providedPorts()
    {
        return {BT::InputPort<int>("camera_center_x"),
                BT::InputPort<int>("tolerance")};
    }

    BT::NodeStatus tick() override
    {
        int camera_center_x, tolerance;

        if (!getInput<int>("camera_center_x", camera_center_x))
        {
            camera_center_x = camera_center_x_;
        }

        if (!getInput<int>("tolerance", tolerance))
        {
            tolerance = tolerance_;
        }

        camera_center_x_ = camera_center_x;
        tolerance_ = tolerance;

        if (human_stop_)
        {
            geometry_msgs::msg::Twist stop_msg;
            stop_msg.linear.x = 0.0;
            stop_msg.angular.z = 0.0;
            cmd_vel_publisher_->publish(stop_msg);

            RCLCPP_INFO(node_->get_logger(), "Human stop is active. Halting the behavior.");
            return BT::NodeStatus::SUCCESS;
        }

        rclcpp::spin_some(node_);
        return BT::NodeStatus::RUNNING;
    }

    void halt() override
    {
        human_stop_ = true;

        geometry_msgs::msg::Twist stop_msg;
        stop_msg.linear.x = 0.0;
        stop_msg.angular.z = 0.0;
        cmd_vel_publisher_->publish(stop_msg);

        RCLCPP_INFO(node_->get_logger(), "Human Following has been halted.");
    }

private:
    rclcpp::Node::SharedPtr node_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_publisher_;
    rclcpp::Subscription<std_msgs::msg::Int32MultiArray>::SharedPtr bounding_box_subscriber_;
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr human_stop_subscriber_;

    int camera_center_x_;
    int tolerance_;
    bool human_stop_;

    void bounding_box_callback(const std_msgs::msg::Int32MultiArray::SharedPtr msg)
    {
        if (human_stop_)
        {
            return;
        }

        int data_x = msg->data[0];
        int error = camera_center_x_ - data_x;

        geometry_msgs::msg::Twist twist;

        if (std::abs(error) > tolerance_)
        {
            twist.angular.z = (error > 0) ? 1.0 : -1.0;
            RCLCPP_INFO(node_->get_logger(), "Aligning: error=%d, angular.z=%.2f", error, twist.angular.z);
        }
        else
        {
            twist.linear.x = -0.1;
            RCLCPP_INFO(node_->get_logger(), "Aligned: Moving forward.");
        }

        cmd_vel_publisher_->publish(twist);
    }

    void human_stop_callback(const std_msgs::msg::Bool::SharedPtr msg)
    {
        human_stop_ = msg->data;
        if (human_stop_)
        {
            RCLCPP_INFO(node_->get_logger(), "Human Stop Received.");
        }
    }
};
