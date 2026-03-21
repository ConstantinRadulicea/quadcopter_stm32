import os
import sys
import time

os.environ["PROTOCOL_BUFFERS_PYTHON_IMPLEMENTATION"] = "python"

try:
    from gz.transport import Node
    from gz.msgs.actuators_pb2 import Actuators
except ImportError as e:
    print(f"Import Error: {e}")
    sys.exit(1)

def main():
    node = Node()
    
    # We are back to the unified Actuators topic
    topic = "/x3model/x3/command/motor_speed"
    
    try:
        pub = node.advertise(topic, Actuators)
    except Exception as e:
        print(f"Failed to create publisher: {e}")
        sys.exit(1)

    print("Aerodynamic engines online. Drone is armed.\n")
    msg = Actuators()

    try:
        while True:
            val = input("Enter target speed (e.g., 700) or Ctrl+C to stop: ")
            
            try:
                speed = float(val)
            except ValueError:
                continue
            
            # Pack the 4 rotor speeds into a single array
            del msg.velocity[:]
            motor_0 = speed # FR
            motor_1 = speed # RL
            motor_2 = speed # FL
            motor_3 = speed # RR
            msg.velocity.extend([motor_0, motor_1, motor_2, motor_3])
            
            pub.publish(msg)
            print(f"--> Thrust commanded: {speed} rad/s on all rotors\n")
            
    except KeyboardInterrupt:
        print("\n\nCutting power...")
        del msg.velocity[:]
        msg.velocity.extend([0.0, 0.0, 0.0, 0.0])
        pub.publish(msg)
        time.sleep(0.5)

if __name__ == '__main__':
    main()