import sys
import time
from hil_fc_bridge import FlightControllerBridge

# ==========================================
# MAIN IMPLEMENTATION
# ==========================================

def on_packet_received(motor_floats, counter):
    """
    This callback is executed automatically whenever a valid 17-byte packet
    passes the CRC check.
    """
    m1, m2, m3, m4 = motor_floats
    print(f"\r[FC->BRIDGE]: [{m1:.3f}, {m2:.3f}, {m3:.3f}, {m4:.3f}] cntr {counter}", end='', flush=True)

def main():
    SERIAL_PORT = 'COM4'
    BAUD_RATE = 115200
    COLTROL_LOOP_FREQUENCY_HZ = 1000.0

    # 1. Initialize the bridge and pass the callback function
    fc = FlightControllerBridge(SERIAL_PORT, BAUD_RATE, rx_callback=on_packet_received)

    # Variables for dummy data and timing
    g_ax, g_ay, g_az = 9.81, 0.0, 0.0
    g_gx, g_gy, g_gz = 0.0, 0.0, 0.0

    last_tx_time = time.time()

    tx_interval = 1.0 / COLTROL_LOOP_FREQUENCY_HZ  # Send data at 10Hz
    rx_interval = 1.0 / COLTROL_LOOP_FREQUENCY_HZ

    print("Starting Main Loop. Press Ctrl+C to stop.")

    try:
        while True:
            # 2. Continually poll the serial port (this handles RX and triggers the callback)
            fc.poll()

            # 3. Handle TX timing and send data
            current_time = time.time()
            if current_time - last_tx_time >= tx_interval:
                fc.send_imu(g_ax, g_ay, g_az, g_gx, g_gy, g_gz)
                last_tx_time = current_time

            # Small sleep to keep the loop from eating 100% CPU
            time.sleep(rx_interval)

    except KeyboardInterrupt:
        print("\nTest stopped by user. Closing port...")
        fc.close()
        sys.exit(0)


if __name__ == '__main__':
    main()