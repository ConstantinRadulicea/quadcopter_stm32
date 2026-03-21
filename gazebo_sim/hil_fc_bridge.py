import sys
import serial
import struct
import base64


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
            crc &= 0xFF
    return crc

class FlightControllerBridge:
    def __init__(self, port, baud_rate, rx_callback):
        """
        Initializes the serial connection and sets up the receive callback.
        """
        self.rx_callback = rx_callback
        self.rx_buffer = b''
        self.rx_frame_counter = int(0)
        self.ser = None

        try:
            self.ser = serial.Serial(port, baud_rate, timeout=0)
            print(f"Successfully opened {port} at {baud_rate} baud.")
        except Exception as e:
            print(f"Error opening serial port: {e}")
            sys.exit(1)

    def send_imu(self, ax, ay, az, gx, gy, gz):
        """
        Packs 6 floats, adds a CRC, Base64 encodes it, and sends it to the FC.
        """
        tx_floats_bytes = struct.pack('>6f', ax, ay, az, gx, gy, gz)
        tx_crc = calculate_crc8_dvb_s2(tx_floats_bytes)
        tx_raw_payload = tx_floats_bytes + struct.pack('>B', tx_crc)

        tx_b64_frame = base64.b64encode(tx_raw_payload) + b'\n'

        self.ser.write(tx_b64_frame)
        # print(f"[TX] Sent Frame: {tx_b64_frame.strip().decode()}")

    def poll(self):
        """
        Reads from the serial buffer, parses complete frames, and triggers
        the callback if a valid frame is found. Call this frequently in your main loop.
        """
        # Read incoming bytes into our persistent software buffer
        if self.ser.in_waiting > 0:
            self.rx_buffer += self.ser.read(self.ser.in_waiting)

        # Process all complete frames
        while b'\n' in self.rx_buffer:
            frame, self.rx_buffer = self.rx_buffer.split(b'\n', 1)
            frame = frame.strip()

            if not frame:
                continue

            try:
                raw_bytes = base64.b64decode(frame)

                if len(raw_bytes) == 17:
                    data = struct.unpack('>4fB', raw_bytes)
                    floats = data[0:4]
                    rx_crc = data[4]

                    calculated_crc = calculate_crc8_dvb_s2(raw_bytes[0:-1])

                    if rx_crc != calculated_crc:
                        print(f"[RX Warning] CRC Mismatch. Expected {calculated_crc}, got {rx_crc}")
                    else:
                        # VALID FRAME: Trigger the callback!
                        self.rx_callback(floats, self.rx_frame_counter)

                        self.rx_frame_counter += 1
                        if self.rx_frame_counter >= 255:
                            self.rx_frame_counter = 0
                else:
                    print(f"[RX Warning] Dropped malformed packet. Expected 17 decoded bytes, got {len(raw_bytes)}")

            except Exception as e:
                print(f"[RX Error] Base64 Decode failed: {e}")

    def close(self):
        self.ser.close()

