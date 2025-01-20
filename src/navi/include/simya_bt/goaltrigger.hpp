#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include <behaviortree_cpp_v3/action_node.h>

class goalTrigger : public BT::SyncActionNode
{
public:
    goalTrigger(const std::string& name, const BT::NodeConfiguration& config)
        : BT::SyncActionNode(name, config), nh(std::make_shared<rclcpp::Node>("goalTrigger"))
    {
        goalpose_sub = nh->create_subscription<std_msgs::msg::String>(
            "goalpose", 10, std::bind(&goalTrigger::goalCallback, this, std::placeholders::_1));

        spin_thread = std::make_shared<std::thread>([this]() {
            rclcpp::spin(this->nh);
        });
    }

    BT::NodeStatus tick() override;

    static BT::PortsList providedPorts()
    {
        return{ BT::OutputPort<std::string>("goal") };
    }

private:
    rclcpp::Node::SharedPtr nh;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr goalpose_sub;
    std::string goal;
    std::shared_ptr<std::thread> spin_thread;

    void goalCallback(const std_msgs::msg::String::SharedPtr msg)
    {
        goal = msg->data;

        RCLCPP_INFO(nh->get_logger(), "Received message: %s", msg->data.c_str());
        RCLCPP_INFO(nh->get_logger(), "goal: %s", goal.c_str()); 
    }
};

BT::NodeStatus goalTrigger::tick()
{
    setOutput("goal", goal);
    
    return BT::NodeStatus::SUCCESS;
}