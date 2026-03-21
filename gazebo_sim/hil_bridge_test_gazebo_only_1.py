import sys
import time
from hil_gazebo_bridge import GazeboController

# ==========================================
# MAIN LOOP
# ==========================================

def main():
    IMU_TOPIC = "/world/x3_world/model/x3/link/base_link/sensor/imu_sensor/imu"
    MOTOR_TOPIC = "/x3model/x3/command/motor_speed"

    # 1. Initialize Objects
    gazebo = GazeboController(IMU_TOPIC, MOTOR_TOPIC)

    print("Bridge is Armed and Operational. Press Ctrl+C to stop.\n")

    try:
        while True:
            imu_data = gazebo.get_imu()
            gazebo.set_motor_speeds([0.5, 0, 1, 0.75])
            print(f"\r[GAZOBO->BRIDGE]: [{imu_data}", end='', flush=True)
            time.sleep(0.001)

    except KeyboardInterrupt:
        print("\n\nCutting simulation power and closing ports...")
        gazebo.stop_motors()
        sys.exit(0)

if __name__ == '__main__':
    main()