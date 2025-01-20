from launch import LaunchDescription
from launch.actions import OpaqueFunction
import subprocess

def trigger_next_launch(context, *args, **kwargs):
    try:
        while True:
            # 런치 파일 실행
            print("Starting behavior.launch.py...")
            process = subprocess.Popen(
                ["ros2", "launch", "navi", "behavior.launch.py"],
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE
            )

            print("Behavior.launch.py is now running. Monitoring logs...")

            # 실시간 로그 출력
            for line in process.stdout:
                print(line.decode().strip())

            # 프로세스 종료 대기
            process.wait()
            print("Behavior.launch.py has stopped. Restarting...")
    except KeyboardInterrupt:
        print("Execution interrupted by user.")
    except Exception as e:
        print(f"An error occurred: {e}")

def generate_launch_description():
    return LaunchDescription([
        OpaqueFunction(function=trigger_next_launch)  # 실행 상태 기반 반복 실행
    ])
