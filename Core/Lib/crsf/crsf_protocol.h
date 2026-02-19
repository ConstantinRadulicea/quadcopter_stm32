/**
 * @file CRSFProtocol.hpp
 * @author Cassandra "ZZ Cat" Robinson (nicad.heli.flier@gmail.com)
 * @brief This file contains enums and structs for the CRSF protocol.
 *
 * @copyright Copyright (c) 2024, Cassandra "ZZ Cat" Robinson. All rights reserved.
 *
 * @section License GNU Affero General Public License v3.0
 * This source file is a part of the CRSF for Arduino library.
 * CRSF for Arduino is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * CRSF for Arduino is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with CRSF for Arduino.  If not, see <https://www.gnu.org/licenses/>.
 *
 */



#ifndef CRSF_PROTOCOL_H
#define CRSF_PROTOCOL_H

#include "stdint.h"
#include "crsf_config.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum rc_channels_e
    {
        RC_CHANNEL_ROLL = 0,
        RC_CHANNEL_PITCH,
        RC_CHANNEL_THROTTLE,
        RC_CHANNEL_YAW,
        RC_CHANNEL_AUX1,
        RC_CHANNEL_AUX2,
        RC_CHANNEL_AUX3,
        RC_CHANNEL_AUX4,
        RC_CHANNEL_AUX5,
        RC_CHANNEL_AUX6,
        RC_CHANNEL_AUX7,
        RC_CHANNEL_AUX8,
        RC_CHANNEL_AUX9,
        RC_CHANNEL_AUX10,
        RC_CHANNEL_AUX11,
        RC_CHANNEL_AUX12,
        RC_CHANNEL_COUNT
    }rc_channels_t;

    enum syncByte_e
    {
        CRSF_SYNC_BYTE = 0xC8,
    };


    enum payloadSize_e
    {
        CRSF_FRAME_GPS_PAYLOAD_SIZE = 15,
        CRSF_FRAME_VARIO_PAYLOAD_SIZE = 2,
        CRSF_FRAME_BARO_ALTITUDE_PAYLOAD_SIZE = 4, // TBS is 2, ExpressLRS is 4 (combines vario)
        CRSF_FRAME_BATTERY_SENSOR_PAYLOAD_SIZE = 8,
        CRSF_FRAME_DEVICE_INFO_PAYLOAD_SIZE = 48,
        CRSF_FRAME_FLIGHT_MODE_PAYLOAD_SIZE = 16,
        CRSF_FRAME_HEARTBEAT_PAYLOAD_SIZE = 2,
        CRSF_FRAME_LINK_STATISTICS_PAYLOAD_SIZE = 10,
        CRSF_FRAME_LINK_STATISTICS_TX_PAYLOAD_SIZE = 6,
        CRSF_FRAME_RC_CHANNELS_PAYLOAD_SIZE = 22,
        CRSF_FRAME_ATTITUDE_PAYLOAD_SIZE = 6
    };

    enum frameLength_e
    {
        CRSF_FRAME_LENGTH_ADDRESS = 1,                                               // Length of the address field in bytes.
        CRSF_FRAME_LENGTH_FRAMELENGTH = 1,                                           // Length of the frame length field in bytes.
        CRSF_FRAME_LENGTH_TYPE = 1,                                                  // Length of the type field in bytes.
        CRSF_FRAME_LENGTH_CRC = 1,                                                   // Length of the CRC field in bytes.
        CRSF_FRAME_LENGTH_TYPE_CRC = CRSF_FRAME_LENGTH_TYPE + CRSF_FRAME_LENGTH_CRC, // Length of the type and CRC fields in bytes.
        CRSF_FRAME_LENGTH_EXT_TYPE_CRC = 4,                                          // Length of the extended Dest/Origin, type, and CRC fields in bytes.
        CRSF_FRAME_LENGTH_NON_PAYLOAD = CRSF_FRAME_LENGTH_ADDRESS + CRSF_FRAME_LENGTH_FRAMELENGTH + CRSF_FRAME_LENGTH_TYPE + CRSF_FRAME_LENGTH_CRC     // Combined length of all fields except the payload in bytes.
    };

    enum frameSize_e
    {
        CRSF_FRAME_SIZE_MAX = 64,
//        CRSF_PAYLOAD_SIZE_MAX = CRSF_FRAME_SIZE_MAX - 6,
        CRSF_PAYLOAD_SIZE_MAX = CRSF_FRAME_SIZE_MAX - CRSF_FRAME_LENGTH_NON_PAYLOAD,
		CRSF_FRAME_LENGTH_MIN = CRSF_FRAME_LENGTH_ADDRESS + CRSF_FRAME_LENGTH_FRAMELENGTH,
		CRSF_FRAME_LENGTH_MAX = CRSF_FRAME_SIZE_MAX - CRSF_FRAME_LENGTH_ADDRESS - CRSF_FRAME_LENGTH_FRAMELENGTH
    };

    typedef enum frameType_e
    {
    	CRSF_FRAMETYPE_INVALID = 0x0,
    	CRSF_FRAMETYPE_GPS = 0x02,
        CRSF_FRAMETYPE_VARIO = 0x07,
        CRSF_FRAMETYPE_BATTERY_SENSOR = 0x08,
        CRSF_FRAMETYPE_BARO_ALTITUDE = 0x09,
        CRSF_FRAMETYPE_HEARTBEAT = 0x0B,
        CRSF_FRAMETYPE_LINK_STATISTICS = 0x14,
        CRSF_FRAMETYPE_RC_CHANNELS_PACKED = 0x16,
        CRSF_FRAMETYPE_SUBSET_RC_CHANNELS_PACKED = 0x17,
        CRSF_FRAMETYPE_LINK_STATISTICS_RX = 0x1C,
        CRSF_FRAMETYPE_LINK_STATISTICS_TX = 0x1D,
        CRSF_FRAMETYPE_ATTITUDE = 0x1E,
        CRSF_FRAMETYPE_FLIGHT_MODE = 0x21,

        CRSF_FRAMETYPE_DEVICE_PING = 0x28,
        CRSF_FRAMETYPE_DEVICE_INFO = 0x29,
        CRSF_FRAMETYPE_PARAMETER_SETTINGS_ENTRY = 0x2B,
        CRSF_FRAMETYPE_PARAMETER_READ = 0x2C,
        CRSF_FRAMETYPE_PARAMETER_WRITE = 0x2D,
        CRSF_FRAMETYPE_COMMAND = 0x32,

        CRSF_FRAMETYPE_MSP_REQ = 0x7A,
        CRSF_FRAMETYPE_MSP_RESP = 0x7B,
        CRSF_FRAMETYPE_MSP_WRITE = 0x7C,
        CRSF_FRAMETYPE_DISPLAYPORT_CMD = 0x7D,
    } frameType_t;

#if CRSF_TELEMETRY_ENABLED == 1 || CRSF_LINK_STATISTICS_ENABLED == 1
#define CRSF_FRAME_ORIGIN_DEST_SIZE 2
#endif



//    0x00 Broadcast address
//    0x0E Cloud
//    0x10 USB Device
//    0x12 Bluetooth Module/WiFi
//    0x13 Wi-Fi receiver (mobile game/simulator)
//    0x14 Video Receiver
//    0x20-0x7F Dynamic address space for NAT
//    0x80 OSD / TBS CORE PNP PRO
//    0x90 ESC 1
//    0x91 ESC 2
//    0x92 ESC 3
//    0x93 ESC 4
//    0x94 ESC 5
//    0x95 ESC 6
//    0x96 ESC 7
//    0x97 ESC 8
//    0x8A Reserved
//    0xB0 Crossfire reserved
//    0xB2 Crossfire reserved
//    0xC0 Voltage/ Current Sensor / PNP PRO digital current sensor
//    0xC2 GPS / PNP PRO GPS
//    0xC4 TBS Blackbox
//    0xC8 Flight controller
//    0xCA Reserved
//    0xCC Race tag
//    0xCE VTX
//    0xEA Remote Control
//    0xEC R/C Receiver / Crossfire Rx
//    0xEE R/C Transmitter Module / Crossfire Tx
//    0xF0 reserved
//    0xF2 reserved

    typedef enum address_e
    {
        CRSF_ADDRESS_BROADCAST = 0x00,
		CRSF_ADDRESS_CLOUD = 0x0E,
		CRSF_ADDRESS_USB = 0x10,
		CRSF_ADDRESS_BLUETOOTH_OR_WIFI_MODULE = 0x12,
		CRSF_ADDRESS_WIFI_RECEIVER = 0x13,
		CRSF_ADDRESS_WIFI_VIDEO_RECEIVER = 0x14,

		// 0x20-0x7F Dynamic address space for NAT
		CRSF_ADDRESS_NAT_START = 0x20,
		CRSF_ADDRESS_NAT_END = 0x7F,

        CRSF_ADDRESS_TBS_OSD_OR_CORE_PNP_PRO = 0x80,
		CRSF_ADDRESS_ESC_1 = 0x90,
		CRSF_ADDRESS_ESC_2 = 0x91,
		CRSF_ADDRESS_ESC_3 = 0x92,
		CRSF_ADDRESS_ESC_4 = 0x93,
		CRSF_ADDRESS_ESC_5 = 0x94,
		CRSF_ADDRESS_ESC_6 = 0x95,
		CRSF_ADDRESS_ESC_7 = 0x96,
		CRSF_ADDRESS_ESC_8 = 0x97,
		CRSF_ADDRESS_RESERVED_1 = 0x8A,
		CRSF_ADDRESS_CROSSFIRE_RESERVED_1 = 0xB0,
		CRSF_ADDRESS_CROSSFIRE_RESERVED_2 = 0xB2,
        CRSF_ADDRESS_CURRENT_SENSOR = 0xC0,
        CRSF_ADDRESS_GPS = 0xC2,
        CRSF_ADDRESS_TBS_BLACKBOX = 0xC4,
        CRSF_ADDRESS_FLIGHT_CONTROLLER = 0xC8,
        CRSF_ADDRESS_RESERVED_2 = 0xCA,
        CRSF_ADDRESS_RACE_TAG = 0xCC,
		CRSF_ADDRESS_VTX = 0xCE,
        CRSF_ADDRESS_RADIO_TRANSMITTER = 0xEA,
        CRSF_ADDRESS_CRSF_RECEIVER = 0xEC,
        CRSF_ADDRESS_CRSF_TRANSMITTER = 0xEE,
		CRSF_ADDRESS_RESERVED_3 = 0xF0,
		CRSF_ADDRESS_RESERVED_4 = 0xF2
    } address_t;

    // Schedule array to send telemetry frames.
    typedef enum
    {
        CRSF_TELEMETRY_FRAME_START_INDEX = 0,
        CRSF_TELEMETRY_FRAME_ATTITUDE_INDEX,
        CRSF_TELEMETRY_FRAME_BARO_ALTITUDE_INDEX,
        CRSF_TELEMETRY_FRAME_BATTERY_SENSOR_INDEX,
        CRSF_TELEMETRY_FRAME_FLIGHT_MODE_INDEX,
        CRSF_TELEMETRY_FRAME_GPS_INDEX,
        CRSF_TELEMETRY_FRAME_HEARTBEAT_INDEX,
        // CRSF_TELEMETRY_FRAME_VARIO_INDEX,
        CRSF_TELEMETRY_FRAME_SCHEDULE_MAX
    } telemetryFrame_t;

    // RC Channels Packed. 22 bytes (11 bits per channel, 16 channels) total.
    struct rcChannelsPacked_s
    {
        uint16_t channel0  : 11;
        uint16_t channel1  : 11;
        uint16_t channel2  : 11;
        uint16_t channel3  : 11;
        uint16_t channel4  : 11;
        uint16_t channel5  : 11;
        uint16_t channel6  : 11;
        uint16_t channel7  : 11;
        uint16_t channel8  : 11;
        uint16_t channel9  : 11;
        uint16_t channel10 : 11;
        uint16_t channel11 : 11;
        uint16_t channel12 : 11;
        uint16_t channel13 : 11;
        uint16_t channel14 : 11;
        uint16_t channel15 : 11;
    } __attribute__((packed));

    typedef struct rcChannelsPacked_s rcChannelsPacked_t;

    typedef enum{
    	CRSF_SUBSET_RC_RES_CONF_10B = 0,
		CRSF_SUBSET_RC_RES_CONF_11B,
		CRSF_SUBSET_RC_RES_CONF_12B,
		CRSF_SUBSET_RC_RES_CONF_13B

    }crsf_subset_rc_channel_resolution;

    typedef struct crsf_frame_definition_s
    {
        uint8_t deviceAddress;                                          // Frame address.
        uint8_t frameLength;                                            // Frame length. Includes payload and CRC.
        uint8_t type;                                                   // Frame type.
        uint8_t payload[CRSF_PAYLOAD_SIZE_MAX + CRSF_FRAME_LENGTH_CRC]; // Frame payload.
    } crsf_frame_definition_t;


    typedef struct crsf_extended_frame_definition_s
    {
        uint8_t deviceAddress;                                          // Frame address.
        uint8_t frameLength;                                            // Frame length. Includes payload and CRC.
        uint8_t type;                                                   // Frame type.
        uint8_t Destination_address;        // 0xea = RC
        uint8_t Origin_address;             // 0xee = TX
        uint8_t payload[CRSF_PAYLOAD_SIZE_MAX + CRSF_FRAME_LENGTH_CRC - 2]; // Frame payload. -2 bytes for destination and origin address
    } crsf_extended_frame_definition_t;

    typedef union crsf_frame_u
    {
        uint8_t raw[CRSF_FRAME_SIZE_MAX];
        crsf_frame_definition_t frame;
        crsf_extended_frame_definition_t extended_frame;
    } crsf_frame_t;

#if CRSF_LINK_STATISTICS_ENABLED > 0
    // Link Statistics frame.
    // Uplink is the connection from the transmitter to the receiver. Downlink is in the opposite direction.
    typedef struct crsf_payload_link_statistics_s
    {
        uint8_t uplink_rssi_1;         // Uplink RSSI Antenna 1 (dBm * -1)
        uint8_t uplink_rssi_2;         // Uplink RSSI Antenna 2 (dBm * -1)
        uint8_t uplink_link_quality;   // Uplink Link Quality/Packet Success Rate (%)
        int8_t uplink_snr;             // Uplink Signal-to-Noise Ratio (dB)
        uint8_t active_antenna;        // Active Antenna (0 = Antenna 1, 1 = Antenna 2)
        uint8_t rf_mode;               // RF Mode (4 fps = 0, 50 fps = 1, 150 fps = 2, 250 fps = 3, 500 fps = 4, 1000 fps = 5)
        uint8_t uplink_tx_power;       // Uplink TX Power (0 mW = 0, 10 mW = 1, 25 mW = 2, 100 mW = 3, 250 mW = 4, 500 mW = 5, 1000 mW = 6, 2000 mW = 7)
        uint8_t downlink_rssi;         // Downlink RSSI (dBm * -1)
        uint8_t downlink_link_quality; // Downlink Link Quality/Packet Success Rate (%)
        int8_t downlink_snr;           // Downlink Signal-to-Noise Ratio (dB)
    } crsf_payload_link_statistics_t;
#endif

    // Attitude Data to pass to the telemetry frame.
    typedef struct attitudeData_s
    {
        int16_t roll;  // Roll angle in radians.
        int16_t pitch; // Pitch angle in radians.
        int16_t yaw;   // Yaw angle in radians.
    } attitudeData_t;

    // Barometric Altitude and Variometer Data to pass to the telemetry frame.
    typedef struct baroAltitudeData_s
    {
        uint16_t altitude; // Altitude in decimeters + 10000 or metres if high bit is set.
        int16_t vario;     // Variometer in centimeters per second.
    } baroAltitudeData_t;

    // Battery Sensor Data to pass to the telemetry frame.
    typedef struct batterySensorData_s
    {
        uint16_t voltage;  // Average battery cell voltage.
        uint16_t current;  // Amperage.
        uint32_t capacity; // mAh drawn.
        uint8_t percent;   // Battery % remaining.
    } batterySensorData_t;

    // Flight Mode Data to pass to the telemetry frame.
    typedef struct flightModeData_s
    {
        char flightMode[CRSF_FRAME_FLIGHT_MODE_PAYLOAD_SIZE]; // Flight mode string.
    } flightModeData_t;

    // GPS Data to pass to the telemetry frame.
    typedef struct gpsData_s
    {
        int32_t latitude;
        int32_t longitude;
        uint16_t altitude;
        uint16_t speed;
        uint16_t groundCourse;
        uint8_t satellites;
    } gpsData_t;

    // Struct to hold data for the telemetry frame.
    typedef struct telemetryData_s
    {
        attitudeData_t attitude;
        baroAltitudeData_t baroAltitude;
        batterySensorData_t battery;
        flightModeData_t flightMode;
        gpsData_t gps;
    } telemetryData_t;

    enum baudRate_e
    {
        BAUD_RATE_LEGACY = 420000,
        BAUD_RATE = 416666
    };

    typedef struct flightMode_s
    {
    	char name[CRSF_FRAME_FLIGHT_MODE_PAYLOAD_SIZE];
    	rc_channels_t channel;
    	uint16_t min;
    	uint16_t max;
    } flightMode_t;

    typedef enum flightModeId_e
    {
    	FLIGHT_MODE_DISARMED = 0,
    	FLIGHT_MODE_ACRO,
    	FLIGHT_MODE_WAIT,
    	FLIGHT_MODE_FAILSAFE,
    	FLIGHT_MODE_GPS_RESCUE,
    	FLIGHT_MODE_PASSTHROUGH,
    	FLIGHT_MODE_ANGLE,
    	FLIGHT_MODE_HORIZON,
    	FLIGHT_MODE_AIRMODE,

    	CUSTOM_FLIGHT_MODE1,
    	CUSTOM_FLIGHT_MODE2,
    	CUSTOM_FLIGHT_MODE3,
    	CUSTOM_FLIGHT_MODE4,
    	CUSTOM_FLIGHT_MODE5,
    	CUSTOM_FLIGHT_MODE6,
    	CUSTOM_FLIGHT_MODE7,
    	CUSTOM_FLIGHT_MODE8,

    	FLIGHT_MODE_COUNT
    } flightModeId_t;

    typedef struct rcChannels_s
    {
    	int8_t valid;
    	crsf_subset_rc_channel_resolution resolution;
    	uint32_t value[RC_CHANNEL_COUNT];
    	int8_t updated_channel[RC_CHANNEL_COUNT];
    	float value_norm[RC_CHANNEL_COUNT];
    } rcChannels_t;

    typedef struct crsf_link_statistics_s
    {
    	int16_t rssi;
    	int16_t lqi;
    	int16_t snr;
#if USE_RX_LINK_UPLINK_POWER != 0
    	int16_t tx_power;
#endif
    } crsf_link_statistics_t;


#if USE_RX_LINK_UPLINK_POWER != 0
#define CRSF_UPLINK_POWER_LEVEL_MW_ITEMS_COUNT 9
// Uplink power levels by uplinkTXPower expressed in mW (250 mW is from ver >=4.00, 50 mW in a future version and for ExpressLRS)
    static const uint16_t crsf_tx_power_table[CRSF_UPLINK_POWER_LEVEL_MW_ITEMS_COUNT] = {
    	0,    // 0 mW
    	10,   // 10 mW
    	25,   // 25 mW
     	100,  // 100 mW
    	500,  // 500 mW
    	1000, // 1 W
    	2000, // 2 W
    	250,  // 250 mW
    	50    // 50 mW
    };
#endif



#ifdef __cplusplus
extern "C" {
#endif

#endif
