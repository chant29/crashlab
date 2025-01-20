from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='navi',  # 이 부분을 실제 패키지 이름으로 바꾸세요
            executable='goal_publisher',  # 컴파일된 실행 파일 이름
            name='goal_publisher',
            output='screen',
            parameters=[
                {
                    'goal_value': 'goal1'  # 초기 goal 값 설정
                }
            ]
        )
    ])
