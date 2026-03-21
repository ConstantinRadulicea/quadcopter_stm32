import os
import sys
from dataclasses import dataclass

# Prevent Protobuf collision
os.environ["PROTOCOL_BUFFERS_PYTHON_IMPLEMENTATION"] = "python"


@dataclass
class IMUData:
    """Structured container for IMU sensor readings."""
    acc_x_m_s2: float = 0.0
    acc_y_ms_2: float = 0.0
    acc_z_m_s2: float = 0.0
    gyro_x_rad_s: float = 0.0
    gyro_y_rad_s: float = 0.0
    gyro_z_rad_s: float = 0.0

    def to_tuple(self):
        """Exports the data as a tuple for struct packing."""
        return (
            self.acc_x_m_s2, self.acc_y_ms_2, self.acc_z_m_s2,
            self.gyro_x_rad_s, self.gyro_y_rad_s, self.gyro_z_rad_s
        )


try:
    from gz.transport import Node
    from gz.msgs.imu_pb2 import IMU
    from gz.msgs.actuators_pb2 import Actuators
except ImportError as e:
    print(f"Gazebo Import Error: {e}")
    sys.exit(1)


# ==========================================
# GAZEBO CONTROLLER CLASS
# ==========================================

class GazeboController:
    def __init__(self, imu_topic, motor_topic):
        print("Initializing Gazebo Node...")
        self.node = Node()

        # Internal state for IMU (ax, ay, az, gx, gy, gz)
        self.imu_data = IMUData()
        self.motor_msg = Actuators()

        # 1. Setup Publisher
        try:
            self.pub = self.node.advertise(motor_topic, Actuators)
        except Exception as e:
            print(f"Failed to create Gazebo publisher: {e}")
            sys.exit(1)

        # 2. Setup Subscriber
        if not self.node.subscribe(IMU, imu_topic, self._imu_cb):
            print("Failed to subscribe to IMU topic. Is Gazebo running?")
            sys.exit(1)

    def _imu_cb(self, msg):
        """Asynchronously called by Gazebo when IMU data arrives."""
        self.imu_data.acc_x_m_s2 = msg.linear_acceleration.x
        self.imu_data.acc_y_ms_2 = msg.linear_acceleration.y
        self.imu_data.acc_z_m_s2 = msg.linear_acceleration.z

        # Gyro is already in rad/s
        self.imu_data.gyro_x_rad_s = msg.angular_velocity.x
        self.imu_data.gyro_y_rad_s = msg.angular_velocity.y
        self.imu_data.gyro_z_rad_s = msg.angular_velocity.z

    def get_imu(self):
        """Returns the latest IMU data as a tuple."""
        return self.imu_data

    def set_motor_speeds(self, motor_floats):
        """Packs and publishes motor speeds to the simulation."""
        m1, m2, m3, m4 = motor_floats

        del self.motor_msg.velocity[:]
        self.motor_msg.velocity.extend([m1, m2, m3, m4])
        self.pub.publish(self.motor_msg)

        # Single-line updating print
        # print(f"\r[HITL] Motors Commanded: [M1:{m1:6.1f} | M2:{m2:6.1f} | M3:{m3:6.1f} | M4:{m4:6.1f}]    ", end='', flush=True)

    def stop_motors(self):
        """Convenience method to cut power."""
        self.set_motor_speeds((0.0, 0.0, 0.0, 0.0))