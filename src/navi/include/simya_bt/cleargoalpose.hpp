#include <behaviortree_cpp_v3/action_node.h>
#include <string>
#include <iostream>

class ClearGoalPose : public BT::SyncActionNode
{
public:
    ClearGoalPose(const std::string &name, const BT::NodeConfiguration &config)
        : BT::SyncActionNode(name, config) {}

    static BT::PortsList providedPorts()
    {
        return {BT::OutputPort<std::string>("goalpose")};
    }

    BT::NodeStatus tick() override
    {
        // goalpose를 초기화
        setOutput("goalpose", "");
        this->config().blackboard->set("goalpose", "");
        std::cout << "GoalPose cleared." << std::endl;
        return BT::NodeStatus::SUCCESS;
    }
};
