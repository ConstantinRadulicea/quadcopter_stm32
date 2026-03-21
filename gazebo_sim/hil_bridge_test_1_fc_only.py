import serial
import struct
import time
import base64
import sys

# --- CONFIGURATION ---
SERIAL_PORT = 'COM4'  # Change to your actual test port
BAUD_RATE = 115200


g_ax = float(0.0)
g_ay = float(0.0)
g_az = float(0.0)
g_gx = float(0.0)
g_gy = float(0.0)
g_gz = float(0.0)

g_ax = float(9.81)
g_ay = float(0.0)
g_az = float(0.0)
g_gx = float(0.0)
g_gy = float(0.0)
g_gz = float(0.0)

def calculate_crc8_dvb_s2(payload_bytes):
    """
    Calculates the CRC-8/DVB-S2 checksum.
    Polynomial: 0xD5, Init: 0x00, RefIn: False, RefOut: False, XorOut: 0x00
    """
    crc = 0x00
    for byte in payload_bytes:
        crc ^= byte
        for _ in range(8):
            if crc & 0x80:
                crc = (crc << 1) ^ 0xD5
            else:
                crc <<= 1
            # Keep it constrained to 8 bits
            crc &= 0xFF
    return crc

def calculate_dummy_crc(payload_bytes):
    """
    Placeholder for your actual CRC logic.
    Currently just a simple additive checksum.
    """
    return calculate_crc8_dvb_s2(payload_bytes)


def send_frame_to_fc(ser, ax, ay, az, gx, gy, gz):
    # Pack the 4 floats in Network Byte Order
    tx_floats_bytes = struct.pack('>6f', ax, ay, az, gx, gy, gz)

    # Calculate CRC and append it as a 1-byte unsigned char
    tx_crc = calculate_dummy_crc(tx_floats_bytes)
    tx_raw_payload = tx_floats_bytes + struct.pack('>B', tx_crc)

    # Encode the full 17 bytes to Base64 and attach the newline delimiter
    tx_b64_frame = base64.b64encode(tx_raw_payload) + b'\n'

    ser.write(tx_b64_frame)
    print(f"[TX] Sent Frame: {tx_b64_frame.strip().decode()}")

def main():
    try:
        # timeout=0 makes reads non-blocking
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0)
        print(f"Successfully opened {SERIAL_PORT} at {BAUD_RATE} baud.")
    except Exception as e:
        print(f"Error opening serial port: {e}")
        sys.exit(1)

    # Buffer to hold incoming serial data until a newline is found
    rx_buffer = b''

    # Timing for the non-blocking TX loop
    last_tx_time = time.time()
    tx_interval = 0.1  # Send data every 0.1 seconds (10Hz)

    print("Starting Base64 TX/RX loop. Press Ctrl+C to stop.")

    try:
        while True:
            # ---------------------------------------------------------
            # 1. RECEIVE PROTOCOL (RX)
            # Read whatever is in the serial hardware buffer into our software buffer
            # ---------------------------------------------------------
            if ser.in_waiting > 0:
                rx_buffer += ser.read(ser.in_waiting)

            # Process all complete frames currently in the buffer
            while b'\n' in rx_buffer:
                # Split at the first newline: frame gets the data, rx_buffer keeps the rest
                frame, rx_buffer = rx_buffer.split(b'\n', 1)
                frame = frame.strip()  # Clean up any lingering carriage returns


                if not frame:
                    continue

                try:
                    # Decode Base64 back into the 17 raw bytes
                    raw_bytes = base64.b64decode(frame)

                    if len(raw_bytes) == 17:
                        # Unpack using Network Byte Order ('>'), 4 floats ('4f'), 1 unsigned char ('B')
                        data = struct.unpack('>4fB', raw_bytes)

                        floats = data[0:4]
                        rx_crc = data[4]
                        calculated_crc = calculate_dummy_crc(raw_bytes[0:-1])
                        if rx_crc != calculated_crc:
                            print(f"[RX Warning] Dropped malformed packet. Expected 17 decoded bytes, got {len(raw_bytes)}")
                        else:
                            print(f"[RX] Data: [{floats[0]:.2f}, {floats[1]:.2f}, {floats[2]:.2f}, {floats[3]:.2f}] | CRC: {rx_crc}")
                    else:
                        print(f"[RX Warning] Dropped malformed packet. Expected 17 decoded bytes, got {len(raw_bytes)}")

                except Exception as e:
                    print(f"[RX Error] Base64 Decode failed: {e}")

            # ---------------------------------------------------------
            # 2. TRANSMIT PROTOCOL (TX)
            # Check if it's time to send the next command
            # ---------------------------------------------------------
            current_time = time.time()
            if current_time - last_tx_time >= tx_interval:

                send_frame_to_fc(ser, g_ax, g_ay, g_az, g_gx, g_gy, g_gz)
                # Update dummy value to see it change on the receiving end

                last_tx_time = current_time

            # Small sleep to prevent CPU hogging
            time.sleep(0.001)

    except KeyboardInterrupt:
        print("\nTest stopped by user. Closing port...")
        ser.close()
        sys.exit(0)


if __name__ == '__main__':
    main()