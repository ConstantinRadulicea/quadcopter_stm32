#include "crsf.h"
#include "stdint.h"
#include "memory.h"
#include "crsf_protocol.h"
#include "crc.h"
#include "crsf_def.h"
#include "crsf_telemetry.h"

void crsf_set_output_cb_fn(crsf_t *crsf, crsf_output_cb_fn fn, void *ctx){
	crsf->crsf_output = fn;
	crsf->crsf_output_cb_fn_ctx = ctx;
}

int8_t crsf_init(crsf_t *crsf, uint32_t frame_rate_hz, crsf_sys_now_us_cb_fn sys_now_us, crsf_output_cb_fn crsf_output, void* crsf_output_cb_fn_ctx){
	if(crsf == NULL) {
		return (int8_t)1;
	}
	memset(crsf, 0, sizeof(*crsf));

	if(sys_now_us == NULL ||
			crsf_output == NULL ||
			frame_rate_hz == 0)
	{
		return (int8_t)1;
	}

	crsf_set_output_cb_fn(crsf, crsf_output, crsf_output_cb_fn_ctx);
	crsf->frame_rate_hz = frame_rate_hz;
	crsf->sys_now_us = sys_now_us;
	crsf_telemetry_init(&(crsf->telemetry));
	return (int8_t)0;
}


// returns 1 when a frame is received valid
static int8_t crsf_receive_frame(crsf_t *crsf, uint8_t rxByte)
{
	uint8_t framePosition = crsf->rx_frame_position;
	uint32_t frameStartTime = crsf->rx_frame_start_time_us;
	uint32_t currentTime = crsf->sys_now_us();
	uint32_t timePerFrame = (uint32_t)HzToUs_int(crsf->frame_rate_hz);
	uint32_t fullFrameLength = 0;

	/* Reset the frame position if the frame time has expired. */
	if (currentTime - frameStartTime > timePerFrame) {
		framePosition = 0;

		if (currentTime < frameStartTime) {
			frameStartTime = currentTime;
		}
	}

	if (framePosition == 0) {
		frameStartTime = currentTime;
	}

	/* Assume the full frame length is 5 bytes until the frame length byte is received. */
	if(framePosition < (CRSF_FRAME_LENGTH_ADDRESS + CRSF_FRAME_LENGTH_FRAMELENGTH)){
		fullFrameLength = (CRSF_FRAME_LENGTH_ADDRESS + CRSF_FRAME_LENGTH_FRAMELENGTH);
	}
	else{
		fullFrameLength = crsf->rxFrame.frame.frameLength + CRSF_FRAME_LENGTH_ADDRESS + CRSF_FRAME_LENGTH_FRAMELENGTH;
	}

	if(crsf->rxFrame.frame.frameLength > (CRSF_FRAME_SIZE_MAX - CRSF_FRAME_LENGTH_ADDRESS - CRSF_FRAME_LENGTH_FRAMELENGTH) ||
			crsf->rxFrame.frame.frameLength < CRSF_FRAME_LENGTH_TYPE + CRSF_FRAME_LENGTH_CRC){
		/* Clear the frame buffer and reset the frame position. */
		//memset(crsf->rxFrame.raw, 0, CRSF_FRAME_SIZE_MAX);
		framePosition = 0;
		return 0;
	}

	fullFrameLength = CRSF_CLAMP(fullFrameLength, 0, CRSF_FRAME_SIZE_MAX);

	if (framePosition < fullFrameLength)
	{
		/* Store the received byte in the frame buffer. */
		crsf->rxFrame.raw[framePosition] = rxByte;
		framePosition++;

		if (framePosition >= fullFrameLength)
		{
			/* Frame is complete, calculate the CRC and check if it is valid. */
			uint8_t crc = crc8_dvb_s2_init();
			crc = crc8_dvb_s2_add(crc, crsf->rxFrame.frame.type);
			crc = crc8_dvb_s2_add_arr(crc, crsf->rxFrame.frame.payload, crsf->rxFrame.frame.frameLength - CRSF_FRAME_LENGTH_TYPE_CRC);

			if (crc == crsf->rxFrame.raw[fullFrameLength - 1])
			{
				switch (crsf->rxFrame.frame.type)
				{
					case CRSF_FRAMETYPE_RC_CHANNELS_PACKED:
						if (crsf->rxFrame.frame.deviceAddress == CRSF_ADDRESS_FLIGHT_CONTROLLER)
						{
							memcpy(&(crsf->rcChannelsFrame), &(crsf->rxFrame), sizeof(crsf->rcChannelsFrame));
							crsf->rcFrameReceived = 1;
						}
						break;

#if CRSF_LINK_STATISTICS_ENABLED > 0
					case CRSF_FRAMETYPE_LINK_STATISTICS:
						if ((crsf->rxFrame.frame.deviceAddress == CRSF_ADDRESS_FLIGHT_CONTROLLER) && (crsf->rxFrame.frame.frameLength == CRSF_FRAME_ORIGIN_DEST_SIZE + CRSF_FRAME_LINK_STATISTICS_PAYLOAD_SIZE))
						{
							crsf_payload_link_statistics_t linkStatisticsPayload;
							memcpy(&linkStatisticsPayload, crsf->rxFrame.frame.payload, sizeof(crsf_payload_link_statistics_t));

							crsf->linkStatistics.rssi = (linkStatisticsPayload.active_antenna ? linkStatisticsPayload.uplink_rssi_2 : linkStatisticsPayload.uplink_rssi_1);
							crsf->linkStatistics.lqi = linkStatisticsPayload.uplink_link_quality;
							crsf->linkStatistics.snr = linkStatisticsPayload.uplink_snr;
							crsf->linkStatistics.tx_power = (linkStatisticsPayload.uplink_tx_power < 9) ? crsf_tx_power_table[linkStatisticsPayload.uplink_tx_power] : 0;
						}
						break;
#endif
				}
			}
			else{
				// crc is corrupted
				/* Clear the frame buffer and reset the frame position. */
				//memset(crsf->rxFrame.raw, 0, CRSF_FRAME_SIZE_MAX);
				framePosition = 0;
				return 0;
			}

			/* Clear the frame buffer and reset the frame position. */
			//memset(crsf->rxFrame.raw, 0, CRSF_FRAME_SIZE_MAX);
			framePosition = 0;
			return 1;
		}
	}

	return 0;
}


int8_t crsf_getFailSafe(crsf_t *crsf)
{
    /* Set the failsafe flag based on the link statistics thresholds. */
    if (crsf->linkStatistics.lqi <= CRSF_FAILSAFE_LQI_THRESHOLD || crsf->linkStatistics.rssi >= CRSF_FAILSAFE_RSSI_THRESHOLD)
    {
        return (int8_t)1;
    }
    else
    {
    	return (int8_t)0;
    }
}

static void _crsf_getRcChannels(crsf_t *crsf, uint16_t *rcChannels)
{
    /* Decode RC frames if one has been received. */
    if (crsf->rcFrameReceived)
    {
    	crsf->rcFrameReceived = 0;
        if (crsf->rcChannelsFrame.frame.type == CRSF_FRAMETYPE_RC_CHANNELS_PACKED)
        {
            rcChannelsPacked_t rcChannelsPacked;
            memcpy(&rcChannelsPacked, crsf->rcChannelsFrame.frame.payload, sizeof(rcChannelsPacked_t));

            rcChannels[RC_CHANNEL_ROLL] = rcChannelsPacked.channel0;
            rcChannels[RC_CHANNEL_PITCH] = rcChannelsPacked.channel1;
            rcChannels[RC_CHANNEL_THROTTLE] = rcChannelsPacked.channel2;
            rcChannels[RC_CHANNEL_YAW] = rcChannelsPacked.channel3;
            rcChannels[RC_CHANNEL_AUX1] = rcChannelsPacked.channel4;
            rcChannels[RC_CHANNEL_AUX2] = rcChannelsPacked.channel5;
            rcChannels[RC_CHANNEL_AUX3] = rcChannelsPacked.channel6;
            rcChannels[RC_CHANNEL_AUX4] = rcChannelsPacked.channel7;
            rcChannels[RC_CHANNEL_AUX5] = rcChannelsPacked.channel8;
            rcChannels[RC_CHANNEL_AUX6] = rcChannelsPacked.channel9;
            rcChannels[RC_CHANNEL_AUX7] = rcChannelsPacked.channel10;
            rcChannels[RC_CHANNEL_AUX8] = rcChannelsPacked.channel11;
            rcChannels[RC_CHANNEL_AUX9] = rcChannelsPacked.channel12;
            rcChannels[RC_CHANNEL_AUX10] = rcChannelsPacked.channel13;
            rcChannels[RC_CHANNEL_AUX11] = rcChannelsPacked.channel14;
            rcChannels[RC_CHANNEL_AUX12] = rcChannelsPacked.channel15;
        }
    }
}



void crsf_getLinkStatistics(crsf_t *crsf, crsf_link_statistics_t *linkStats)
{
    memcpy(linkStats, &(crsf->linkStatistics), sizeof(*linkStats));
}

void crsf_getRcChannels(crsf_t *crsf, rcChannels_t* rc_channels)
{
    memcpy(rc_channels, &(crsf->_rcChannels), sizeof(*rc_channels));
}

static void crsf_setLinkUp(crsf_t *crsf)
{
    //if (!crsf->_linkIsUp && _linkUpCallback) _linkUpCallback();
    crsf->_linkIsUp = 1;
    crsf->_lastChannelsPacket = crsf->sys_now_us();
}

static void crsf_checkLinkDown(crsf_t *crsf)
{
    if (crsf->_linkIsUp && ((crsf->sys_now_us() - crsf->_lastChannelsPacket) > CRSF_FAILSAFE_STAGE1_MS))
    {
		//if (_linkDownCallback) _linkDownCallback();
		crsf->_linkIsUp = 0;
    }
}

int8_t crsf_isLinkUp(crsf_t *crsf){
	return crsf->_linkIsUp;
}



/**
 * @brief Updates the is_armed state based on the configured switch and throttle safety.
 * Call this function immediately after parsing a new RC frame.
 */
void crsf_update_arming_state(crsf_t *dev) {
    if (!dev) return;

    // 1. Get the current value of the configured Arm Switch
    // Assuming _rcChannels contains a 'channels' array of floats
    // normalized between -1.0 and 1.0
    float arm_switch_val = dev->_rcChannels.value[CRSF_ARM_CHANNEL_INDEX];

    // 2. Get the current Throttle value (for safety check)
    float throttle_val = dev->_rcChannels.value[RC_CHANNEL_THROTTLE];

    // 3. Logic Implementation

    // CASE A: Switch is LOW (Disarm Command)
    if (arm_switch_val < CRSF_ARM_THRESHOLD_RC) {
        // Always disarm immediately if switch is low
        dev->is_armed = 0;
    }
    // CASE B: Switch is HIGH (Arm Command)
    // Only proceed if we are currently DISARMED (to perform the safety check once)
    else if (dev->is_armed == 0) {

        // SAFETY CHECK: PRE-ARM
        // Only allow arming if the throttle stick is at the bottom.
        // This prevents the drone from spinning up unexpectedly.
        if (throttle_val < CRSF_SAFE_THROTTLE_VAL_RC) {
            dev->is_armed = 1;
        }
    }
    // CASE C: Already Armed and Switch is High -> Stay Armed
    // (No code needed, state remains 1)
}

int8_t crsf_isArmed(crsf_t *crsf){
	return crsf->is_armed;
}



int8_t crsf_update(crsf_t *crsf, uint8_t rxByte){

	uint8_t byteReceived = (uint8_t)rxByte;
	int8_t frame_received =  crsf_receive_frame(crsf, byteReceived);
	if (frame_received)
	{
		//flushRemainingFrames();

//#if CRSF_LINK_STATISTICS_ENABLED > 0
//                getLinkStatistics(&_linkStatistics);
//                if (_linkStatisticsCallback != nullptr)
//                {
//                    _linkStatisticsCallback(_linkStatistics);
//                }
//#endif

#if CRSF_TELEMETRY_ENABLED > 0
		crsf_telemetry_update(&(crsf->telemetry));
#endif

#if CRSF_RC_ENABLED > 0
//		crsf->_rcChannels.failsafe = crsf_getFailSafe(crsf);
		_crsf_getRcChannels(crsf, crsf->_rcChannels.value);
		crsf_update_arming_state(crsf);
//		if (_rcChannelsCallback != nullptr)
//		{
//			rcChannelsCallback(_rcChannels);
//		}
#endif
		crsf_setLinkUp(crsf);
	}
#if CRSF_TELEMETRY_ENABLED > 0
	uint8_t* data_to_send = crsf_telemetry_get_tx_data(&(crsf->telemetry));
	uint32_t data_size_to_send = crsf_telemetry_get_tx_data_size(&(crsf->telemetry));
	uint32_t data_size_sent = crsf->crsf_output(crsf, data_to_send, data_size_to_send, crsf->crsf_output_cb_fn_ctx);
	crsf_telemetry_update_tx_data_sent(&(crsf->telemetry), data_size_sent);
#endif

//            if (_rawDataCallback != nullptr)
//            {
//                _rawDataCallback(byteReceived);
//            }
	crsf_checkLinkDown(crsf);
	return frame_received;
}



#if CRSF_FLIGHTMODES_ENABLED > 0
    void crsf_setFlightModeData(crsf_t *crsf, flightModeId_t flightMode, int8_t disarmed)
    {
    	char flightModeStr[CRSF_FRAME_FLIGHT_MODE_PAYLOAD_SIZE];
        if (flightMode != FLIGHT_MODE_DISARMED)
        {
            switch (flightMode)
            {
                case FLIGHT_MODE_FAILSAFE:
                	strncpy(flightModeStr, "!FS!", CRSF_FRAME_FLIGHT_MODE_PAYLOAD_SIZE-2);
                    break;
                case FLIGHT_MODE_GPS_RESCUE:
                	strncpy(flightModeStr, "RTH", CRSF_FRAME_FLIGHT_MODE_PAYLOAD_SIZE-2);
                    break;
                case FLIGHT_MODE_PASSTHROUGH:
                	strncpy(flightModeStr, "MANU", CRSF_FRAME_FLIGHT_MODE_PAYLOAD_SIZE-2);
                    break;
                case FLIGHT_MODE_ANGLE:
                	strncpy(flightModeStr, "STAB", CRSF_FRAME_FLIGHT_MODE_PAYLOAD_SIZE-2);
                    break;
                case FLIGHT_MODE_HORIZON:
                	strncpy(flightModeStr, "HOR", CRSF_FRAME_FLIGHT_MODE_PAYLOAD_SIZE-2);
                    break;
                case FLIGHT_MODE_AIRMODE:
                	strncpy(flightModeStr, "AIR", CRSF_FRAME_FLIGHT_MODE_PAYLOAD_SIZE-2);
                    break;

                /* All 8 custom flight modes are handled here. */
                case CUSTOM_FLIGHT_MODE1:
                case CUSTOM_FLIGHT_MODE2:
                case CUSTOM_FLIGHT_MODE3:
                case CUSTOM_FLIGHT_MODE4:
                case CUSTOM_FLIGHT_MODE5:
                case CUSTOM_FLIGHT_MODE6:
                case CUSTOM_FLIGHT_MODE7:
                case CUSTOM_FLIGHT_MODE8:
                	strncpy(flightModeStr, crsf->_flightModes[flightMode].name, CRSF_FRAME_FLIGHT_MODE_PAYLOAD_SIZE-2);
                    break;

                default:
                	strncpy(flightModeStr, "ACRO", CRSF_FRAME_FLIGHT_MODE_PAYLOAD_SIZE-2);
                    break;
            }
        }
        else
        {
            disarmed = 1;
        }

		crsf_telemetry_setFlightModeData(&(crsf->telemetry), flightModeStr, disarmed);
    }
#endif


void crsf_setAttitudeData(crsf_t *crsf, int16_t roll_rad, int16_t pitch_rad, int16_t yaw_rad){
	return crsf_telemetry_setAttitudeData(&(crsf->telemetry), roll_rad, pitch_rad, yaw_rad);
}
void crsf_setBaroAltitudeData(crsf_t *crsf, uint16_t altitude, int16_t vario){
	return crsf_telemetry_setBaroAltitudeData(&(crsf->telemetry), altitude, vario);
}
void crsf_setBatteryData(crsf_t *crsf, float voltage, float current, uint32_t capacity, uint8_t percent){
	return crsf_telemetry_setBatteryData(&(crsf->telemetry), voltage, current, capacity, percent);
}

void crsf_setGPSData(crsf_t *crsf, float latitude, float longitude, float altitude, float speed, float course, uint8_t satellites){
	return crsf_telemetry_setGPSData(&(crsf->telemetry), latitude, longitude, altitude, speed, course, satellites);
}


uint16_t crsf_getRcChannel(crsf_t *crsf, rc_channels_t channel){
	if (channel >= RC_CHANNEL_COUNT ||
			crsf->_rcChannels.valid == 0)
	{
		return 0;
	}
	uint16_t channel_value = crsf->_rcChannels.value[(int)channel];
	return channel_value;
}

int8_t crsf_isRcDataValid(crsf_t *crsf){
	return crsf->_rcChannels.valid;
}


/* Convert RC value from raw to microseconds.
- Minimum: 172 (988us)
- Middle: 992 (1500us)
- Maximum: 1811 (2012us)
- Scale factor = (2012 - 988) / (1811 - 172) = 0.62477120195241
- Offset = 988 - 172 * 0.62477120195241 = 880.53935326418548
*/
uint16_t crsf_rcToUs(uint16_t rc)
{
	float factor = (float)(CRSF_US_CHANNEL_MAX - CRSF_US_CHANNEL_MIN) / (float)(CRSF_RC_CHANNEL_MAX - CRSF_RC_CHANNEL_MIN);
	float offset = (float)(CRSF_RC_CHANNEL_MIN) * factor;
	return (uint16_t)(((float)rc * factor) + offset);
//    return (uint16_t)((rc * 0.62477120195241f) + 881);
}

uint16_t crsf_usToRc(uint16_t us)
{
	float factor = (float)(CRSF_US_CHANNEL_MAX - CRSF_US_CHANNEL_MIN) / (float)(CRSF_RC_CHANNEL_MAX - CRSF_RC_CHANNEL_MIN);
	float offset = (float)(CRSF_RC_CHANNEL_MIN) * factor;
	return (uint16_t)(((float)us - offset) / factor);
//    return (uint16_t)((us - 881) / 0.62477120195241f);
}

/**
 * @brief Converts microseconds to normalized float (-1.0 to 1.0)
 * @param us Input in microseconds (usually 988-2012)
 * @return float Value between -1.0f and 1.0f
 */
float crsf_usToNormalized(uint16_t us) {
	float factor = 1.0f / (CRSF_US_CHANNEL_MAX - CRSF_US_CHANNEL_CENTER);
    // Use float literal 1500.0f to ensure floating point subtraction
    float val = ((float)us - (float)CRSF_US_CHANNEL_CENTER) * factor;

    // 2. Clamp (Saturation)
    // Ensures value never exceeds -1.0 or 1.0 due to jitter or extended travel
    if (val > 1.0f) {
        return 1.0f;
    } else if (val < -1.0f) {
        return -1.0f;
    }

    return val;
}


/**
 * @brief Converts raw CRSF (11-bit) directly to float (-1.0 to 1.0)
 * Skips the intermediate microsecond conversion for speed.
 */

float crsf_rcToNormalized(uint16_t us) {
	float factor = 1.0f / (CRSF_RC_CHANNEL_MAX - CRSF_RC_CHANNEL_CENTER);
    float val = ((float)us - (float)CRSF_RC_CHANNEL_CENTER) * factor;

    // 2. Clamp (Saturation)
    // Ensures value never exceeds -1.0 or 1.0 due to jitter or extended travel
    if (val > 1.0f) {
        return 1.0f;
    } else if (val < -1.0f) {
        return -1.0f;
    }

    return val;
}


/**
 * @brief Checks if a raw CRSF channel value is within valid bounds.
 * @param raw The 11-bit channel value (0-2047)
 * @return true if valid, false if likely garbage or connection loss
 */
int8_t crsf_isRcValueValid(uint16_t raw) {

    // 2. Check Lower Bound
    if (raw < (CRSF_RC_CHANNEL_MIN)) {
        return 0;
    }

    // 3. Check Upper Bound
    if (raw > (CRSF_RC_CHANNEL_MAX)) {
        return 0;
    }

    return 1;
}


