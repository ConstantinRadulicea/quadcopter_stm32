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

static void crsf_setLinkUp(crsf_t *crsf){
	crsf->_lastChannelsPacket = crsf->sys_now_us();
	crsf->_linkIsUp = 1;
}

/* Convert RC value from raw to microseconds.
- Minimum: 172 (988us)
- Middle: 992 (1500us)
- Maximum: 1811 (2012us)
- Scale factor = (2012 - 988) / (1811 - 172) = 0.62477120195241
- Offset = 988 - 172 * 0.62477120195241 = 880.53935326418548
*/
static uint16_t crsf_rcToUs(uint16_t rc, crsf_subset_rc_channel_resolution resolution)
{
	float factor, offset;
	if(resolution == CRSF_SUBSET_RC_RES_CONF_11B){
		factor = (float)(CRSF_US_CHANNEL_MAX - CRSF_US_CHANNEL_MIN) / (float)(CRSF_RC_CHANNEL_MAX - CRSF_RC_CHANNEL_MIN);
		offset = (float)(CRSF_RC_CHANNEL_MIN) * factor;
		return (uint16_t)(((float)rc * factor) + offset);
	//    return (uint16_t)((rc * 0.62477120195241f) + 881);
	}
	else if(resolution == CRSF_SUBSET_RC_RES_CONF_10B){
		factor = (float)(CRSF_SUBSET_RC_CHANNEL_SCALE_10B);
		offset = (float)(CRSF_US_CHANNEL_MIN);
		return (uint16_t)(((float)rc * factor) + offset);
	}
	else if(resolution == CRSF_SUBSET_RC_RES_CONF_12B){
		factor = (float)(CRSF_SUBSET_RC_CHANNEL_SCALE_12B);
		offset = (float)(CRSF_US_CHANNEL_MIN);
		return (uint16_t)(((float)rc * factor) + offset);
	}
	else if(resolution == CRSF_SUBSET_RC_RES_CONF_13B){
		factor = (float)(CRSF_SUBSET_RC_CHANNEL_SCALE_13B);
		offset = (float)(CRSF_US_CHANNEL_MIN);
		return (uint16_t)(((float)rc * factor) + offset);
	}
	return 0;
}


/**
 * @brief Converts microseconds to normalized float (-1.0 to 1.0)
 * @param us Input in microseconds (usually 988-2012)
 * @return float Value between -1.0f and 1.0f
 */
static float crsf_usToNormalized(uint16_t us) {
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


static void process_channels_frame(crsf_t *crsf){
	for(int i = 0; i < RC_CHANNEL_COUNT; i++){
		if(crsf_isChannelUpdated(crsf, i) != 0){
			crsf->_rcChannels.value_norm[i] = crsf_usToNormalized(crsf_rcToUs(crsf->_rcChannels.value[i], crsf->_rcChannels.resolution));
		}
	}
}


// returns CRSF_FRAMETYPE_INVALID when a frame is not received
static frameType_t crsf_receive_frame(crsf_t *crsf, uint8_t rxByte)
{
	uint8_t framePosition = crsf->rx_frame_position;
	uint32_t frameStartTime = crsf->rx_frame_start_time_us;
	uint32_t currentTime = crsf->sys_now_us();
	uint32_t timePerFrame = (uint32_t)HzToUs_int(crsf->frame_rate_hz);
	uint32_t fullFrameLength = 0;
	crsf->rcFrameReceived = 0;

	/* Reset the frame position if the frame time has expired. */
	if (currentTime - frameStartTime > timePerFrame) {
		framePosition = 0;

		if (currentTime < frameStartTime) {
			frameStartTime = currentTime;
			crsf->rx_frame_start_time_us = currentTime;
		}
	}

	if (framePosition == 0) {
		frameStartTime = currentTime;
		crsf->rx_frame_start_time_us = currentTime;
	}

	/* Assume the full frame length is 5 bytes until the frame length byte is received. */
	if(framePosition < (CRSF_FRAME_LENGTH_ADDRESS + CRSF_FRAME_LENGTH_FRAMELENGTH)){
		fullFrameLength = (CRSF_FRAME_LENGTH_ADDRESS + CRSF_FRAME_LENGTH_FRAMELENGTH);
	}
	else{
		fullFrameLength = crsf->rxFrame.frame.frameLength + CRSF_FRAME_LENGTH_ADDRESS + CRSF_FRAME_LENGTH_FRAMELENGTH;
	}

	if(crsf->rxFrame.frame.frameLength > CRSF_FRAME_LENGTH_MAX ||
			crsf->rxFrame.frame.frameLength < CRSF_FRAME_LENGTH_MIN){
		/* Clear the frame buffer and reset the frame position. */
		//memset(crsf->rxFrame.raw, 0, CRSF_FRAME_SIZE_MAX);
		framePosition = 0;
		return CRSF_FRAMETYPE_INVALID;
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
					case CRSF_FRAMETYPE_SUBSET_RC_CHANNELS_PACKED:
						if (crsf->rxFrame.frame.deviceAddress == CRSF_ADDRESS_FLIGHT_CONTROLLER)
						{
							memcpy(&(crsf->rcChannelsFrame), &(crsf->rxFrame), sizeof(crsf->rcChannelsFrame));
							crsf_setLinkUp(crsf);
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
#if USE_RX_LINK_UPLINK_POWER != 0
						    int crsfUplinkPowerStatesItemIndex = (linkStatisticsPayload.uplink_tx_power < CRSF_UPLINK_POWER_LEVEL_MW_ITEMS_COUNT) ? linkStatisticsPayload.uplink_tx_power : 0;
							crsf->linkStatistics.tx_power = crsf_tx_power_table[crsfUplinkPowerStatesItemIndex];
#endif
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
				return CRSF_FRAMETYPE_INVALID;
			}

			/* Clear the frame buffer and reset the frame position. */
			//memset(crsf->rxFrame.raw, 0, CRSF_FRAME_SIZE_MAX);
			framePosition = 0;
			return (frameType_t) (crsf->rxFrame.frame.type);
		}
	}

	return CRSF_FRAMETYPE_INVALID;
}



int8_t crsf_getFailSafe(crsf_t *crsf)
{
    /* Set the failsafe flag based on the link statistics thresholds. */
    if (crsf->linkStatistics.lqi <= CRSF_FAILSAFE_LQI_THRESHOLD || crsf->linkStatistics.rssi >= CRSF_FAILSAFE_RSSI_THRESHOLD)
    {
        return (int8_t)1;
    }
    else if (crsf_isLinkUp(crsf) == 0) {
    	return (int8_t)1;
    }

    return (int8_t)0;
}

static void _crsf_getRcChannels(crsf_t *crsf, rcChannels_t *rcChannels)
{
    /* Decode RC frames if one has been received. */
    if (crsf->rcFrameReceived)
    {
//    	crsf->rcFrameReceived = 0;
    	memset(rcChannels->updated_channel, 0, sizeof(rcChannels->updated_channel));
        if (crsf->rcChannelsFrame.frame.type == CRSF_FRAMETYPE_RC_CHANNELS_PACKED)
        {
        	rcChannelsPacked_t rcChannelsPacked;
            memcpy(&rcChannelsPacked, crsf->rcChannelsFrame.frame.payload, sizeof(rcChannelsPacked_t));
            memset(rcChannels->updated_channel, 1, sizeof(rcChannels->updated_channel));

            rcChannels->resolution = CRSF_SUBSET_RC_RES_CONF_11B;
            rcChannels->value[RC_CHANNEL_ROLL] = rcChannelsPacked.channel0;
            rcChannels->value[RC_CHANNEL_PITCH] = rcChannelsPacked.channel1;
            rcChannels->value[RC_CHANNEL_THROTTLE] = rcChannelsPacked.channel2;
            rcChannels->value[RC_CHANNEL_YAW] = rcChannelsPacked.channel3;
            rcChannels->value[RC_CHANNEL_AUX1] = rcChannelsPacked.channel4;
            rcChannels->value[RC_CHANNEL_AUX2] = rcChannelsPacked.channel5;
            rcChannels->value[RC_CHANNEL_AUX3] = rcChannelsPacked.channel6;
            rcChannels->value[RC_CHANNEL_AUX4] = rcChannelsPacked.channel7;
            rcChannels->value[RC_CHANNEL_AUX5] = rcChannelsPacked.channel8;
            rcChannels->value[RC_CHANNEL_AUX6] = rcChannelsPacked.channel9;
            rcChannels->value[RC_CHANNEL_AUX7] = rcChannelsPacked.channel10;
            rcChannels->value[RC_CHANNEL_AUX8] = rcChannelsPacked.channel11;
            rcChannels->value[RC_CHANNEL_AUX9] = rcChannelsPacked.channel12;
            rcChannels->value[RC_CHANNEL_AUX10] = rcChannelsPacked.channel13;
            rcChannels->value[RC_CHANNEL_AUX11] = rcChannelsPacked.channel14;
            rcChannels->value[RC_CHANNEL_AUX12] = rcChannelsPacked.channel15;

            process_channels_frame(crsf);
        }
        else if(crsf->rcChannelsFrame.frame.type == CRSF_FRAMETYPE_SUBSET_RC_CHANNELS_PACKED)
        {
                    // use subset RC frame structure (0x17)
                    uint8_t readByteIndex = 0;
                    const uint8_t *payload = crsf->rcChannelsFrame.frame.payload;

                    // get the configuration byte
                    uint8_t configByte = payload[readByteIndex++];

                    // get the channel number of start channel
                    uint8_t startChannel = configByte & CRSF_SUBSET_RC_STARTING_CHANNEL_MASK;
                    configByte >>= CRSF_SUBSET_RC_STARTING_CHANNEL_BITS;

                    // get the channel resolution settings
                    uint8_t channelBits;
                    uint16_t channelMask;
                    uint8_t channelRes = configByte & CRSF_SUBSET_RC_RES_CONFIGURATION_MASK;
                    configByte >>= CRSF_SUBSET_RC_RES_CONFIGURATION_BITS;
                    switch (channelRes) {
                    case CRSF_SUBSET_RC_RES_CONF_10B:
                    	rcChannels->resolution = CRSF_SUBSET_RC_RES_CONF_10B;
                        channelBits = CRSF_SUBSET_RC_RES_BITS_10B;
                        channelMask = CRSF_SUBSET_RC_RES_MASK_10B;
                        break;
                    default:
                    case CRSF_SUBSET_RC_RES_CONF_11B:
                    	rcChannels->resolution = CRSF_SUBSET_RC_RES_CONF_11B;
                        channelBits = CRSF_SUBSET_RC_RES_BITS_11B;
                        channelMask = CRSF_SUBSET_RC_RES_MASK_11B;
                        break;
                    case CRSF_SUBSET_RC_RES_CONF_12B:
                    	rcChannels->resolution = CRSF_SUBSET_RC_RES_CONF_12B;
                        channelBits = CRSF_SUBSET_RC_RES_BITS_12B;
                        channelMask = CRSF_SUBSET_RC_RES_MASK_12B;
                        break;
                    case CRSF_SUBSET_RC_RES_CONF_13B:
                    	rcChannels->resolution = CRSF_SUBSET_RC_RES_CONF_13B;
                        channelBits = CRSF_SUBSET_RC_RES_BITS_13B;
                        channelMask = CRSF_SUBSET_RC_RES_MASK_13B;
                        break;
                    }

                    // do nothing for the reserved configuration bit
                    configByte >>= CRSF_SUBSET_RC_RESERVED_CONFIGURATION_BITS;

                    // calculate the number of channels packed
                    uint8_t numOfChannels = ((crsf->rcChannelsFrame.frame.frameLength - CRSF_FRAME_LENGTH_TYPE_CRC - 1) * 8) / channelBits;

                    // unpack the channel data
                    uint8_t bitsMerged = 0;
                    uint32_t readValue = 0;
                    for (uint8_t n = 0; n < numOfChannels; n++) {
                        while (bitsMerged < channelBits) {
                            uint8_t readByte = payload[readByteIndex++];
                            readValue |= ((uint32_t) readByte) << bitsMerged;
                            bitsMerged += 8;
                        }
                        rcChannels->value[startChannel + n] = readValue & channelMask;
                        rcChannels->updated_channel[startChannel + n] = 1;
                        readValue >>= channelBits;
                        bitsMerged -= channelBits;
                    }
                    process_channels_frame(crsf);
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

static void crsf_checkLinkDown(crsf_t *crsf)
{
    if (crsf->_linkIsUp && ((crsf->sys_now_us() - crsf->_lastChannelsPacket) > CRSF_FAILSAFE_STAGE1_MS))
    {
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
//    uint16_t arm_switch_val = dev->_rcChannels.value[CRSF_ARM_CHANNEL_INDEX];

    float arm_switch_val = crsf_getChannelNormalized(dev, CRSF_ARM_CHANNEL_INDEX);

    // CASE A: Switch is LOW (Disarm Command)
    if (arm_switch_val < CRSF_ARM_THRESHOLD_NORM) {
        // Always disarm immediately if switch is low
        dev->is_armed = 0;
    }
    else{
    	dev->is_armed = 1;
    }
}

int8_t crsf_isArmed(crsf_t *crsf){
	return crsf->is_armed;
}



static frameType_t crsf_update_byte(crsf_t *crsf, uint8_t rxByte){

	uint8_t byteReceived = (uint8_t)rxByte;
	uint32_t currentTime = crsf->sys_now_us();
	frameType_t frame_received =  crsf_receive_frame(crsf, byteReceived);
	if (frame_received != CRSF_FRAMETYPE_INVALID)
	{

#if CRSF_RC_ENABLED > 0
		_crsf_getRcChannels(crsf, &(crsf->_rcChannels));
		crsf_update_arming_state(crsf);
#endif
	}
#if CRSF_TELEMETRY_ENABLED > 0
	crsf_telemetry_update(&(crsf->telemetry), currentTime);
	uint8_t* data_to_send = crsf_telemetry_get_tx_data(&(crsf->telemetry));
	uint32_t data_size_to_send = crsf_telemetry_get_tx_data_size(&(crsf->telemetry));
	uint32_t data_size_sent = crsf->crsf_output(crsf, data_to_send, data_size_to_send, crsf->crsf_output_cb_fn_ctx);
	crsf_telemetry_update_tx_data_sent(&(crsf->telemetry), data_size_sent);
#endif

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


int8_t crsf_isNewRcDataAvailable(crsf_t *crsf){
	return (crsf->_rcChannels.valid != 0);

//	return (crsf->rcFrameReceived != 0) && (crsf->_rcChannels.valid != 0);
}

int8_t crsf_isChannelUpdated(crsf_t *crsf, rc_channels_t channel){
	return 	(crsf->_rcChannels.updated_channel[channel] != 0) &&
			(crsf->_rcChannels.valid != 0) &&
			(channel < RC_CHANNEL_COUNT);
}



float crsf_getChannelNormalized(crsf_t *crsf, rc_channels_t channel){
	if (channel >= RC_CHANNEL_COUNT) {
		return 0.0f;
	}

	return crsf->_rcChannels.value_norm[(int)channel];
}



frameType_t crsf_update(crsf_t *crsf, uint8_t* rx_buf, uint32_t rx_buf_size, uint32_t *bytes_processed){
	frameType_t crsf_result = CRSF_FRAMETYPE_INVALID;
	*bytes_processed = 0;
	for(uint32_t i=0;i < rx_buf_size; i++){
		crsf_result = crsf_update_byte(crsf, rx_buf[i]);

		if(crsf_result != 0){ // new frame was received
			*bytes_processed = i+1;
			break;
		}
	}

	return crsf_result;
}



