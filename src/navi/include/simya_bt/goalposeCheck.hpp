#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include <behaviortree_cpp_v3/condition_node.h>
#include <sstream>
#include <vector>
#include <algorithm>

class goalposeCheck : public BT::ConditionNode
{
public:
    goalposeCheck(const std::string& name, const BT::NodeConfiguration& config)
        : BT::ConditionNode(name, config), nh(std::make_shared<rclcpp::Node>("goalposeCheck"))
    {
        goalpose_sub = nh->create_subscription<std_msgs::msg::String>(
            "goalpose", 10, std::bind(&goalposeCheck::goalCallback, this, std::placeholders::_1));

        spin_thread = std::make_shared<std::thread>([this]() {
            while (rclcpp::ok() && goal.empty()) {
                rclcpp::spin_some(this->nh);
            }
        });
    }

    BT::NodeStatus tick() override;

    static BT::PortsList providedPorts()
    {

        return{
            BT::InputPort<std::string>("value"),
            BT::InputPort<std::string>("goal") };
    }

private:
    rclcpp::Node::SharedPtr nh;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr goalpose_sub;
    std::string goal;
    std::shared_ptr<std::thread> spin_thread;

    void goalCallback(const std_msgs::msg::String::SharedPtr msg)
    {
        goal = msg->data;


        setOutput("goal", goal);
        RCLCPP_INFO(nh->get_logger(), "Received message: %s", msg->data.c_str());
        RCLCPP_INFO(nh->get_logger(), "goal: %s", goal.c_str()); 
    }
};

BT::NodeStatus goalposeCheck::tick()
{
    BT::Optional<std::string> value = getInput<std::string>("value");

    if (!value || goal.empty()) {
        RCLCPP_INFO(nh->get_logger(), "Waiting for message...");
        return BT::NodeStatus::RUNNING;
    }
    // 쉼표로 구분된 value를 파싱
    std::vector<std::string> allowed_goals;
    std::istringstream stream(*value);
    std::string token;
    while (std::getline(stream, token, ',')) {
        // 공백 제거
        token.erase(remove_if(token.begin(), token.end(), isspace), token.end());
        allowed_goals.push_back(token);
    }

    // 디버깅: 파싱된 goal 값 출력
    for (const auto& v : allowed_goals) {
        RCLCPP_INFO(nh->get_logger(), "Parsed allowed goal: %s", v.c_str());
    }

    // goal 값이 allowed_goals에 포함되어 있는지 확인
    if (std::find(allowed_goals.begin(), allowed_goals.end(), goal) != allowed_goals.end()) {
        RCLCPP_INFO(nh->get_logger(), "Goal '%s' matches one of the allowed values.", goal.c_str());
        return BT::NodeStatus::SUCCESS;
    } else {
        RCLCPP_INFO(nh->get_logger(), "Goal '%s' does not match any allowed values.", goal.c_str());
        return BT::NodeStatus::FAILURE;
    }
}