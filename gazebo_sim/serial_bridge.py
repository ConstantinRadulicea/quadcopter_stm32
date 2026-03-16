import os
import sys
import time
import struct
import threading
import serial

# Prevent Protobuf collision
os.environ["PROTOCOL_BUFFERS_PYTHON_IMPLEMENTATION"] = "python"

motor_topic = "/x3model/x3/command/motor_speed"
imu_topic = "/world/x3_world/model/x3/link/base_link/sensor/imu_sensor/imu"

try:
    from gz.transport import Node
    from gz.msgs.imu_pb2 import IMU
    from gz.msgs.actuators_pb2 import Actuators
except ImportError as e:
    print(f"Import Error: {e}")
    sys.exit(1)

# --- SERIAL CONFIGURATION ---
# Change this to match your physical flight controller's port
# Typically /dev/ttyUSB0 (FTDI) or /dev/ttyACM0 (STM32 Native USB)
SERIAL_PORT = '/dev/ttyACM0' 
BAUD_RATE = 115200

try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.01)
    print(f"Serial port {SERIAL_PORT} opened successfully at {BAUD_RATE} baud.")
except Exception as e:
    print(f"Failed to open serial port: {e}")
    print("Make sure your flight controller is plugged in and the container has USB permissions.")
    sys.exit(1)

# Initialize Gazebo Node
node = Node()

# The publisher for sending motor commands TO Gazebo
pub = node.advertise(motor_topic, Actuators)
motor_msg = Actuators()

def imu_cb(msg):
    """
    Catches IMU data from Gazebo, packs it into raw C-struct bytes, 
    and sends it down the Serial cable to the flight controller.
    """
    # '<' = Little Endian (Standard for ARM/STM32)
    # 'c' = 1 char header ('I' for IMU)
    # '6f' = 6 floats (ax, ay, az, gx, gy, gz)
    # Total payload: 1 byte + (6 * 4 bytes) = 25 bytes
    payload = struct.pack('<c6f', b'I',
                          msg.linear_acceleration.x, msg.linear_acceleration.y, msg.linear_acceleration.z,
                          msg.angular_velocity.x, msg.angular_velocity.y, msg.angular_velocity.z)
    
    ser.write(payload)

def serial_listener():
    """
    Runs continuously in the background, listening for incoming
    motor commands from the physical flight controller.
    """
    while True:
        # We expect a header 'M' (1 byte) + 4 floats (16 bytes) = 17 bytes total
        if ser.in_waiting >= 17:
            header = ser.read(1)
            
            if header == b'M':
                # We found a motor command! Read the next 16 bytes.
                data = ser.read(16)
                
                # Unpack the 4 floats using Little Endian
                motors = struct.unpack('<4f', data)
                
                # Send the unpacked floats into Gazebo
                del motor_msg.velocity[:]
                motor_msg.velocity.extend([motors[0], motors[1], motors[2], motors[3]])
                pub.publish(motor_msg)
            else:
                # If we get out of sync, flush the buffer
                ser.reset_input_buffer()
                
        time.sleep(0.001)

def main():
    # 1. Start listening to the serial port in a background thread
    listener_thread = threading.Thread(target=serial_listener, daemon=True)
    listener_thread.start()
    
    # 2. Subscribe to the IMU (This automatically starts sending data to serial)
    if node.subscribe(IMU, imu_topic, imu_cb):
        print("Hardware-In-The-Loop Bridge is ACTIVE.")
        print("Waiting for flight controller commands... Press Ctrl+C to stop.")
    else:
        print("Failed to connect to Gazebo. Is it running?")
        sys.exit(1)
        
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("\nClosing bridge...")
        ser.close()
        sys.exit(0)

if __name__ == '__main__':
    main()