#include "simya_bt/nav2_client.hpp"
#include "simya_bt/goalposeCheck.hpp"
#include "simya_bt/Isgoal.hpp"
#include "simya_bt/goaltrigger.hpp"
#include "simya_bt/goalcheckerpub.hpp"
#include "simya_bt/cleargoalpose.hpp"
#include "simya_bt/humanfollowing.hpp"
#include <behaviortree_cpp_v3/bt_factory.h>
#include "std_msgs/msg/string.hpp"
#include <behaviortree_cpp_v3/loggers/bt_cout_logger.h>
#include <ament_index_cpp/get_package_share_directory.hpp>
#include <behaviortree_cpp_v3/loggers/bt_zmq_publisher.h>


using namespace BT;

int main(int argc, char **argv)
{
    setvbuf(stdout, NULL, _IONBF, BUFSIZ);
    rclcpp::init(argc, argv);
  
    auto bt_node = rclcpp::Node::make_shared("simya_bt");
    std::string package_name = "navi";

    std::string bt_xml;
    try {
        std::string package_dir = ament_index_cpp::get_package_share_directory(package_name);
        bt_xml = package_dir + "/behavior_tree/goal.xml";  // 원하는 파일 경로
    } catch (const std::exception& e) {
        std::cerr << "Package [" << package_name << "] not found: " << e.what() << std::endl;
        return -1;
    }

    bt_node->declare_parameter("bt_xml", rclcpp::ParameterValue(std::string(bt_xml)));
    bt_node->get_parameter("bt_xml", bt_xml); //param reading
    RCLCPP_INFO(bt_node->get_logger(), "Loading XML : %s", bt_xml.c_str()); 
  
    // We use the BehaviorTreeFactory to register our custom nodes
    BT::BehaviorTreeFactory factory;
    
  
    factory.registerNodeType<Nav2Client>("Nav2Client");
    factory.registerNodeType<goalposeCheck>("goalposeCheck");
    factory.registerNodeType<Isgoal>("Isgoal");
    factory.registerNodeType<goalTrigger>("goalTrigger");
    factory.registerNodeType<GoalCheckerPub>("goalcheckerpub");
    factory.registerNodeType<HumanFollowing>("humanfollowing");
    factory.registerNodeType<ClearGoalPose>("cleargoalpose");    



    // factory.registerNodeType<InterruptEvent>("InterruptEvent");
    // factory.registerNodeType<SnapshotClient>("SnapshotClient");
    // factory.registerNodeType<SendCommandVel>("SendCommandVel");

    auto tree = factory.createTreeFromFile(bt_xml);
  
    // Create a logger
    StdCoutLogger logger_cout(tree);

    // Create a ZeroMQ publisher
    BT::PublisherZMQ publisher_zmq(tree);
    NodeStatus status = NodeStatus::RUNNING;
    // Keep on ticking until you get either a SUCCESS or FAILURE state
    while (rclcpp::ok() && status == NodeStatus::RUNNING) {
        status = tree.tickRoot();
        
        // Sleep 100 milliseconds
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  
    return 0;
}