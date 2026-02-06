#ifndef CRSF_CONFIG_H
#define CRSF_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif


/* Failsafe Options
- CRSF_FAILSAFE_LQI_THRESHOLD: The minimum LQI value for the receiver to be considered connected.
- CRSF_FAILSAFE_RSSI_THRESHOLD: The minimum RSSI value for the receiver to be considered connected.
  - NB: It is considered good practice to set this value to the same as the RSSI Sensitivity Limit in your Lua script.
*/
#define CRSF_FAILSAFE_LQI_THRESHOLD  80
#define CRSF_FAILSAFE_RSSI_THRESHOLD 105

/* RC Options
- RC_ENABLED: Enables or disables the RC API.
- RC_MAX_CHANNELS: The maximum number of RC channels to be received.
- RC_CHANNEL_MIN: The minimum value of an RC channel.
- RC_CHANNEL_MAX: The maximum value of an RC channel.
- RC_CHANNEL_CENTER: The center value of an RC channel.
- RC_INITIALISE_CHANNELS: Whether or not to initialise the RC channels to their center values.
- RC_INITIALISE_ARMCHANNEL: When enabled, the arm channel is set to its minimum value.
  - NB: This refers to the Aux1 channel and is intended for use with ExpressLRS receivers.
- RC_INITIALISE_THROTTLECHANNEL: When enabled, the throttle channel is set to its minimum value. */
#define CRSF_RC_ENABLED                    1
#define CRSF_RC_MAX_CHANNELS               16
#define CRSF_RC_CHANNEL_MIN                172
#define CRSF_RC_CHANNEL_CENTER             992
#define CRSF_RC_CHANNEL_MAX                1811
#define CRSF_RC_INITIALISE_CHANNELS        1
#define CRSF_RC_INITIALISE_ARMCHANNEL      1
#define CRSF_RC_INITIALISE_THROTTLECHANNEL 1

#define CRSF_US_CHANNEL_MIN                988
#define CRSF_US_CHANNEL_CENTER             1500
#define CRSF_US_CHANNEL_MAX                2012

// Constants for Arming Logic
#define CRSF_ARM_THRESHOLD_NORM       (0.0f)
#define CRSF_SAFE_THROTTLE_VAL_RC  (CRSF_RC_CHANNEL_MIN + ((CRSF_RC_CHANNEL_MAX - CRSF_RC_CHANNEL_MIN)*0.1f))  // Throttle must be below this to ARM
#define CRSF_ARM_CHANNEL_INDEX	(RC_CHANNEL_AUX1)

#define CRSF_FAILSAFE_STAGE1_MS 300

/* Flight Modes
Enables or disables the Flight Mode API.
When enabled, you are given an event-driven API that allows you to easily implement flight modes
and assign them to a switch on your controller.
Pro Tip: You can combine the Flight Mode API with the Telemetry API to send flight mode
information back to your controller. */
#define CRSF_FLIGHTMODES_ENABLED 1

/* Custom Flight Modes
Enables or disables the Custom Flight Modes.
When enabled, this allows you to implement flight modes with custom names
and assign them to a switch on your controller. */
#define CRSF_CUSTOM_FLIGHT_MODES_ENABLED 0

/* Telemetry Options
- TELEMETRY_ENABLED: Enables or disables the Telemetry API.
- TELEMETRY_ATTITUDE_ENABLED: Enables or disables attitude telemetry output.
- TELEMETRY_BAROALTITUDE_ENABLED: Enables or disables barometric altitude telemetry output.
- TELEMETRY_BATTERY_ENABLED: Enables or disables battery telemetry output.
- TELEMETRY_FLIGHTMODE_ENABLED: Enables or disables flight mode telemetry output.
- TELEMETRY_GPS_ENABLED: Enables or disables GPS telemetry output.  */



#define CRSF_TELEMETRY_ENABLED 1
#define CRSF_CYCLETIME_US (HzToUs_int(10))

#define CRSF_TELEMETRY_HEARTBEAT_ENABLED	1
#define CRSF_TELEMETRY_ATTITUDE_ENABLED     0
#define CRSF_TELEMETRY_BAROALTITUDE_ENABLED 0
#define CRSF_TELEMETRY_BATTERY_ENABLED      0
#define CRSF_TELEMETRY_FLIGHTMODE_ENABLED	1
#define CRSF_TELEMETRY_GPS_ENABLED			0

#if CRSF_FLIGHTMODES_ENABLED == 0
#define CRSF_TELEMETRY_FLIGHTMODE_ENABLED	0
#endif

#define CRSF_LINK_STATISTICS_ENABLED 1
#define USE_RX_LINK_UPLINK_POWER 1




#define CRSFV3_MAX_CHANNEL      24

#define CRSF_SUBSET_RC_STARTING_CHANNEL_BITS        5
#define CRSF_SUBSET_RC_STARTING_CHANNEL_MASK        0x1F
#define CRSF_SUBSET_RC_RES_CONFIGURATION_BITS       2
#define CRSF_SUBSET_RC_RES_CONFIGURATION_MASK       0x03
#define CRSF_SUBSET_RC_RESERVED_CONFIGURATION_BITS  1

#define CRSF_RC_CHANNEL_SCALE_LEGACY                0.62477120195241f
//#define CRSF_SUBSET_RC_RES_CONF_10B                 0
#define CRSF_SUBSET_RC_RES_BITS_10B                 10
#define CRSF_SUBSET_RC_RES_MASK_10B                 0x03FF
#define CRSF_SUBSET_RC_CHANNEL_SCALE_10B            1.0f
//#define CRSF_SUBSET_RC_RES_CONF_11B                 1
#define CRSF_SUBSET_RC_RES_BITS_11B                 11
#define CRSF_SUBSET_RC_RES_MASK_11B                 0x07FF
#define CRSF_SUBSET_RC_CHANNEL_SCALE_11B            0.5f
//#define CRSF_SUBSET_RC_RES_CONF_12B                 2
#define CRSF_SUBSET_RC_RES_BITS_12B                 12
#define CRSF_SUBSET_RC_RES_MASK_12B                 0x0FFF
#define CRSF_SUBSET_RC_CHANNEL_SCALE_12B            0.25f
//#define CRSF_SUBSET_RC_RES_CONF_13B                 3
#define CRSF_SUBSET_RC_RES_BITS_13B                 13
#define CRSF_SUBSET_RC_RES_MASK_13B                 0x1FFF
#define CRSF_SUBSET_RC_CHANNEL_SCALE_13B            0.125f

#define CRSF_RSSI_MIN (-130)
#define CRSF_RSSI_MAX 0
#define CRSF_SNR_MIN (-30)
#define CRSF_SNR_MAX 20



/* All warnings and asserts below this point are to ensure that the configuration is valid. */

/* Compiler warning if both RC and Telemetry are disabled. */
#if CRSF_RC_ENABLED == 0 && CRSF_TELEMETRY_ENABLED == 0
#warning "Both CRSF_RC_ENABLED and CRSF_TELEMETRY_ENABLED are disabled. CRSF for Arduino will not do anything."
#endif

/* Static assert if Flight Modes are enabled, but RC is disabled. */
#if CRSF_FLIGHTMODES_ENABLED == 1 && CRSF_RC_ENABLED == 0
    static_assert(false, "CRSF_FLIGHTMODES_ENABLED is enabled, but CRSF_RC_ENABLED is disabled. Flight Modes require RC to be enabled.");
#endif

/* Static assert if all telemetry options are disabled.
Better to use CRSF_TELEMETRY_ENABLED instead. */
#if CRSF_TELEMETRY_ATTITUDE_ENABLED == 0 && CRSF_TELEMETRY_BAROALTITUDE_ENABLED == 0 && CRSF_TELEMETRY_BATTERY_ENABLED == 0 && CRSF_TELEMETRY_FLIGHTMODE_ENABLED == 0 && CRSF_TELEMETRY_GPS_ENABLED == 0
    static_assert(false, "All telemetry options are disabled. Set CRSF_TELEMETRY_ENABLED to 0 to disable telemetry instead.");
#endif


#ifdef __cplusplus
}
#endif

#endif
