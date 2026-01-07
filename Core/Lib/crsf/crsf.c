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

int crsf_init(crsf_t *crsf, uint32_t frame_rate_hz, crsf_sys_now_us_cb_fn sys_now_us, crsf_output_cb_fn crsf_output, void* crsf_output_cb_fn_ctx){
	if(crsf == NULL) {
		return 1;
	}
	memset(crsf, 0, sizeof(*crsf));

	if(sys_now_us == NULL ||
			crsf_output == NULL ||
			frame_rate_hz == 0)
	{
		return 1;
	}

	crsf_set_output_cb_fn(crsf, crsf_output, crsf_output_cb_fn_ctx);
	crsf->frame_rate_hz = frame_rate_hz;
	crsf->sys_now_us = sys_now_us;
	crsf_telemetry_init(&(crsf->telemetry));
	return 0;
}


// returns 1 when a frame is received valid
static int crsf_receive_frame(crsf_t *crsf, uint8_t rxByte)
{
	uint8_t framePosition = crsf->rx_frame_position;
	uint32_t frameStartTime = crsf->rx_frame_start_time_us;
	uint32_t currentTime = crsf->sys_now_us();
	uint32_t timePerFrame = HzToUs_int(crsf->frame_rate_hz);
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


int crsf_getFailSafe(crsf_t *crsf)
{
    /* Set the failsafe flag based on the link statistics thresholds. */
    if (crsf->linkStatistics.lqi <= CRSF_FAILSAFE_LQI_THRESHOLD || crsf->linkStatistics.rssi >= CRSF_FAILSAFE_RSSI_THRESHOLD)
    {
        return 1;
    }
    else
    {
    	return 0;
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

int crsf_isLinkUp(crsf_t *crsf){
	return crsf->_linkIsUp;
}



int crsf_update(crsf_t *crsf, uint8_t rxByte){

	uint8_t byteReceived = (uint8_t)rxByte;
	int frame_received =  crsf_receive_frame(crsf, byteReceived);
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
		crsf->_rcChannels.failsafe = crsf_getFailSafe(crsf);
		_crsf_getRcChannels(crsf, crsf->_rcChannels.value);
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
    void crsf_setFlightModeData(crsf_t *crsf, flightModeId_t flightMode, int disarmed)
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

int crsf_isRcDataValid(crsf_t *crsf){
	return crsf->_rcChannels.valid;
}

