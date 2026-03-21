import sys
import time
from math import pi

from hil_fc_bridge import FlightControllerBridge
from hil_gazebo_bridge import GazeboController

SERIAL_PORT = '/dev/ttyACM0'  # Update if necessary
BAUD_RATE = 921600
IMU_TOPIC = "/world/x3_world/model/x3/link/base_link/sensor/imu_sensor/imu"
MOTOR_TOPIC = "/x3model/x3/command/motor_speed"
COLTROL_LOOP_FREQUENCY_HZ = 1000.0


# Max velocity in rad/s (920KV * 14.8V * (2*pi/60))
MAX_RADS = (920 * 14.8 * (2*pi/60))
# MAX_RADS = (600 * 14.8 * (2*pi/60))

gazebo = GazeboController(IMU_TOPIC, MOTOR_TOPIC)

def on_packet_received_from_fc(motor_floats, counter):
    """
    This callback is executed automatically whenever a valid 17-byte packet
    passes the CRC check.
    """
    # Convert throttle percentage to rad/s
    rads_1 = motor_floats[0] * MAX_RADS
    rads_3 = motor_floats[2] * MAX_RADS
    rads_2 = motor_floats[1] * MAX_RADS
    rads_4 = motor_floats[3] * MAX_RADS

    gazebo.set_motor_speeds([rads_1, rads_2, rads_3, rads_4])
    # m1, m2, m3, m4 = motor_floats
    # print(f"\r[FC->BRIDGE]: [{m1:.3f}, {m2:.3f}, {m3:.3f}, {m4:.3f}] cntr {counter}", end='', flush=True)


# ==========================================
# MAIN LOOP
# ==========================================

def main():

    fc = FlightControllerBridge(SERIAL_PORT, BAUD_RATE, rx_callback=on_packet_received_from_fc)

    last_tx_time = time.time()
    tx_interval = 1.0 / COLTROL_LOOP_FREQUENCY_HZ  # Send data at 10Hz
    loop_interval = 1.0 / COLTROL_LOOP_FREQUENCY_HZ

    print("Bridge is Armed and Operational. Press Ctrl+C to stop.\n")

    try:
        while True:
            fc.poll()

            current_time = time.time()
            if current_time - last_tx_time >= tx_interval:
                # Pass the entire IMUData object to the bridge
                imu_data = gazebo.get_imu()
                fc.send_imu(imu_data.acc_x_m_s2, -imu_data.acc_y_ms_2, -imu_data.acc_z_m_s2,
                            imu_data.gyro_x_rad_s, -imu_data.gyro_y_rad_s, -imu_data.gyro_z_rad_s)
                last_tx_time = current_time

            time.sleep(loop_interval)

    except KeyboardInterrupt:
        print("\n\nCutting simulation power and closing ports...")
        gazebo.stop_motors()
        fc.close()
        sys.exit(0)

if __name__ == '__main__':
    main()