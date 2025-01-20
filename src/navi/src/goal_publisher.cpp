#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

class GoalPublisherNode : public rclcpp::Node
{
public:
    GoalPublisherNode() : Node("goal_publisher_node")
    {
        this->declare_parameter<std::string>("goal_value", "goal1");
        std::string goal_value = this->get_parameter("goal_value").as_string();

        publisher_ = this->create_publisher<std_msgs::msg::String>("goalpose", 10);

        auto message = std_msgs::msg::String();
        message.data = goal_value;

        RCLCPP_INFO(this->get_logger(), "Publishing goal: %s", goal_value.c_str());
        publisher_->publish(message);
    }

private:
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<GoalPublisherNode>());
    rclcpp::shutdown();
    return 0;
}
