#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include <behaviortree_cpp_v3/condition_node.h>

class Isgoal : public BT::ConditionNode
{
public:
    Isgoal(const std::string& name, const BT::NodeConfiguration& config)
        : BT::ConditionNode(name, config) { }

    BT::NodeStatus tick() override;

    static BT::PortsList providedPorts()
    {
        return{ BT::InputPort<std::string>("value"), BT::InputPort<std::string>("goal") };
    }
};

BT::NodeStatus Isgoal::tick()
{
    BT::Optional<std::string> value = getInput<std::string>("value");
    BT::Optional<std::string> goal = getInput<std::string>("goal");

    if (!value || !goal) {
        return BT::NodeStatus::FAILURE;
        RCLCPP_ERROR(rclcpp::get_logger("Isgoal"), "Either value or goal is missing!");
    }

     // 디버깅 로그 출력
    RCLCPP_INFO(rclcpp::get_logger("Isgoal"), "Value: %s, Goal: %s", value->c_str(), goal->c_str());


    return (*goal == *value) ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
}