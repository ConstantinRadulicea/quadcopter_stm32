import os
import sys
import time

# FIX: Prevent the Protocol Buffers C++ collision on Ubuntu 24.04
os.environ["PROTOCOL_BUFFERS_PYTHON_IMPLEMENTATION"] = "python"

# Import Gazebo Jetty Python bindings
try:
    from gz.transport import Node
    from gz.msgs.imu_pb2 import IMU 
except ImportError as e:
    print(f"Import Error: {e}")
    sys.exit(1)

def imu_cb(msg):
    # Extract linear acceleration (m/s^2)
    ax = msg.linear_acceleration.x
    ay = msg.linear_acceleration.y
    az = msg.linear_acceleration.z
    
    # Extract angular velocity (rad/s)
    gx = msg.angular_velocity.x
    gy = msg.angular_velocity.y
    gz = msg.angular_velocity.z
    
    # Print formatted data on a single refreshing line
    print(f"Accel [X:{ax:7.3f} Y:{ay:7.3f} Z:{az:7.3f}] | Gyro [X:{gx:7.3f} Y:{gy:7.3f} Z:{gz:7.3f}]    ", end='\r')

def main():
    node = Node()
    topic = "/world/x3_world/model/x3/link/base_link/sensor/imu_sensor/imu"
    
    # FIX: The argument order MUST be (MessageClass, TopicString, CallbackFunction)
    if node.subscribe(IMU, topic, imu_cb):
        print("Connected! Streaming data to console...\n")
    else:
        print("Failed to connect. Make sure Gazebo is running.")
        sys.exit(1)
        
    try:
        while True:
            time.sleep(0.1)
    except KeyboardInterrupt:
        print("\n\nExiting reader.")

if __name__ == '__main__':
    main()
