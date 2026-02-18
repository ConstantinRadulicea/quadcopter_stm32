#include "crsf_telemetry.h"
#include "crsf_def.h"
#include "crc.h"
#include "string.h"

static uint32_t WriteAttitudeDataFrame(telemetryData_t *_telemetryData, uint8_t *buffer, uint32_t buffer_size)
{
	uint32_t frame_size = 0;
	uint32_t start_of_crc;
	uint8_t crc;
	if(buffer_size < (CRSF_FRAME_ATTITUDE_PAYLOAD_SIZE + CRSF_FRAME_LENGTH_NON_PAYLOAD)) {
		return 0;
	}

	buffer[frame_size++] = CRSF_SYNC_BYTE;
	buffer[frame_size++] = CRSF_FRAME_ATTITUDE_PAYLOAD_SIZE + CRSF_FRAME_LENGTH_TYPE_CRC;

	start_of_crc = frame_size;
	buffer[frame_size++] = CRSF_FRAMETYPE_ATTITUDE;

	(*((uint16_t*)(&(buffer[frame_size])))) = crsf_htons(_telemetryData->attitude.pitch);
	frame_size += 2;

	(*((uint16_t*)(&(buffer[frame_size])))) = crsf_htons(_telemetryData->attitude.roll);
	frame_size += 2;

	(*((uint16_t*)(&(buffer[frame_size])))) = crsf_htons(_telemetryData->attitude.yaw);
	frame_size += 2;

	crc = crc8_dvb_s2_init();
	crc = crc8_dvb_s2_add_arr(crc, &(buffer[start_of_crc]), frame_size - start_of_crc);
	buffer[frame_size++] = crc;

	return frame_size;
}

static uint32_t WriteBaroAltitudeDataFrame(telemetryData_t *_telemetryData, uint8_t *buffer, uint32_t buffer_size)
{
	uint32_t frame_size = 0;
	uint32_t start_of_crc;
	uint8_t crc;
	if(buffer_size < (CRSF_FRAME_BARO_ALTITUDE_PAYLOAD_SIZE + CRSF_FRAME_LENGTH_NON_PAYLOAD)) {
		return 0;
	}

	buffer[frame_size++] = CRSF_SYNC_BYTE;
	buffer[frame_size++] = CRSF_FRAME_BARO_ALTITUDE_PAYLOAD_SIZE + CRSF_FRAME_LENGTH_TYPE_CRC;

	start_of_crc = frame_size;
	buffer[frame_size++] = CRSF_FRAMETYPE_BARO_ALTITUDE;

	(*((uint16_t*)(&(buffer[frame_size])))) = crsf_htons(_telemetryData->baroAltitude.altitude);
	frame_size += 2;

	(*((uint16_t*)(&(buffer[frame_size])))) = crsf_htons(_telemetryData->baroAltitude.vario);
	frame_size += 2;

	crc = crc8_dvb_s2_init();
	crc = crc8_dvb_s2_add_arr(crc, &(buffer[start_of_crc]), frame_size - start_of_crc);
	buffer[frame_size++] = crc;

	return frame_size;
}

static uint32_t WriteBatterySensorDataFrame(telemetryData_t *_telemetryData, uint8_t *buffer, uint32_t buffer_size)
{
	uint32_t frame_size = 0;
	uint32_t start_of_crc;
	uint8_t crc;
	if(buffer_size < (CRSF_FRAME_BATTERY_SENSOR_PAYLOAD_SIZE + CRSF_FRAME_LENGTH_NON_PAYLOAD)) {
		return 0;
	}

	buffer[frame_size++] = CRSF_SYNC_BYTE;
	buffer[frame_size++] = CRSF_FRAME_BATTERY_SENSOR_PAYLOAD_SIZE + CRSF_FRAME_LENGTH_TYPE_CRC;

	start_of_crc = frame_size;
	buffer[frame_size++] = CRSF_FRAMETYPE_BATTERY_SENSOR;

	(*((uint16_t*)(&(buffer[frame_size])))) = crsf_htons(_telemetryData->battery.voltage);
	frame_size += 2;

	(*((uint16_t*)(&(buffer[frame_size])))) = crsf_htons(_telemetryData->battery.current);
	frame_size += 2;

	frame_size += writeU24BE(&(buffer[frame_size]), _telemetryData->battery.capacity);

	buffer[frame_size++] = _telemetryData->battery.percent;

	crc = crc8_dvb_s2_init();
	crc = crc8_dvb_s2_add_arr(crc, &(buffer[start_of_crc]), frame_size - start_of_crc);
	buffer[frame_size++] = crc;

	return frame_size;
}

static uint32_t WriteFlightModeDataFrame(telemetryData_t *_telemetryData, uint8_t *buffer, uint32_t buffer_size)
{
	uint32_t frame_size = 0;
	uint32_t start_of_crc;
	uint8_t crc;

	uint32_t length = strlen(_telemetryData->flightMode.flightMode) + 1;
    if (length > CRSF_FRAME_FLIGHT_MODE_PAYLOAD_SIZE) {
        return 0;
    }

	if(buffer_size < (length + CRSF_FRAME_LENGTH_NON_PAYLOAD)) {
		return 0;
	}

	buffer[frame_size++] = CRSF_SYNC_BYTE;
	buffer[frame_size++] = length + CRSF_FRAME_LENGTH_TYPE_CRC;

	start_of_crc = frame_size;
	buffer[frame_size++] = CRSF_FRAMETYPE_FLIGHT_MODE;

	memcpy(buffer + frame_size, _telemetryData->flightMode.flightMode, length);
	frame_size += length;

	buffer[frame_size++] = '\0';

	crc = crc8_dvb_s2_init();
	crc = crc8_dvb_s2_add_arr(crc, &(buffer[start_of_crc]), frame_size - start_of_crc);
	buffer[frame_size++] = crc;

	return frame_size;
}

static uint32_t WriteGPSDataFrame(telemetryData_t *_telemetryData, uint8_t *buffer, uint32_t buffer_size)
{
	uint32_t frame_size = 0;
	uint32_t start_of_crc;
	uint8_t crc;
	if(buffer_size < (CRSF_FRAME_GPS_PAYLOAD_SIZE + CRSF_FRAME_LENGTH_NON_PAYLOAD)) {
		return 0;
	}

	buffer[frame_size++] = CRSF_SYNC_BYTE;
	buffer[frame_size++] = CRSF_FRAME_GPS_PAYLOAD_SIZE + CRSF_FRAME_LENGTH_TYPE_CRC;

	start_of_crc = frame_size;
	buffer[frame_size++] = CRSF_FRAMETYPE_GPS;


	(*((uint32_t*)(&(buffer[frame_size])))) = crsf_htonl(_telemetryData->gps.latitude);
	frame_size += 4;

	(*((uint32_t*)(&(buffer[frame_size])))) = crsf_htonl(_telemetryData->gps.longitude);
	frame_size += 4;

	(*((uint16_t*)(&(buffer[frame_size])))) = crsf_htons(_telemetryData->gps.speed);
	frame_size += 2;

	(*((uint16_t*)(&(buffer[frame_size])))) = crsf_htons(_telemetryData->gps.groundCourse);
	frame_size += 2;

	(*((uint16_t*)(&(buffer[frame_size])))) = crsf_htons(_telemetryData->gps.altitude);
	frame_size += 2;

	buffer[frame_size++] = _telemetryData->gps.satellites;

	crc = crc8_dvb_s2_init();
	crc = crc8_dvb_s2_add_arr(crc, &(buffer[start_of_crc]), frame_size - start_of_crc);
	buffer[frame_size++] = crc;

	return frame_size;
}

static uint32_t WriteHeartbeatDataFrame(uint8_t *buffer, uint32_t buffer_size)
{
	uint32_t frame_size = 0;
	uint32_t start_of_crc;
	uint8_t crc;
	if(buffer_size < (CRSF_FRAME_HEARTBEAT_PAYLOAD_SIZE + CRSF_FRAME_LENGTH_NON_PAYLOAD)) {
		return 0;
	}

	buffer[frame_size++] = CRSF_SYNC_BYTE;
	buffer[frame_size++] = CRSF_FRAME_HEARTBEAT_PAYLOAD_SIZE + CRSF_FRAME_LENGTH_TYPE_CRC;

	start_of_crc = frame_size;
	buffer[frame_size++] = CRSF_FRAMETYPE_HEARTBEAT;

	(*((uint16_t*)(&(buffer[frame_size])))) = crsf_htons(CRSF_ADDRESS_FLIGHT_CONTROLLER);
	frame_size += 2;

	crc = crc8_dvb_s2_init();
	crc = crc8_dvb_s2_add_arr(crc, &(buffer[start_of_crc]), frame_size - start_of_crc);
	buffer[frame_size++] = crc;

	return frame_size;
}


void crsf_telemetry_setAttitudeData(crsf_telemetry_t *crsf_telemetry, int16_t roll_rad, int16_t pitch_rad, int16_t yaw_rad){
	crsf_telemetry->_telemetryData.attitude.roll = roll_rad;
	crsf_telemetry->_telemetryData.attitude.pitch = -pitch_rad;
	crsf_telemetry->_telemetryData.attitude.yaw = yaw_rad;
}


void crsf_telemetry_setBaroAltitudeData(crsf_telemetry_t *crsf_telemetry, uint16_t altitude, int16_t vario)
{
	crsf_telemetry->_telemetryData.baroAltitude.altitude = altitude + 10000;
	crsf_telemetry->_telemetryData.baroAltitude.vario = vario;
}

void crsf_telemetry_setBatteryData(crsf_telemetry_t *crsf_telemetry, float voltage, float current, uint32_t capacity, uint8_t percent)
{
	crsf_telemetry->_telemetryData.battery.voltage = (voltage + 5) / 10;
	crsf_telemetry->_telemetryData.battery.current = current / 10;
	crsf_telemetry->_telemetryData.battery.capacity = capacity;
	crsf_telemetry->_telemetryData.battery.percent = percent;
}

// strlen(flightMode) must be less than CRSF_FRAME_FLIGHT_MODE_PAYLOAD_SIZE
void crsf_telemetry_setFlightModeData(crsf_telemetry_t *crsf_telemetry, char *flightMode, int armed_bool)
{
	memset(crsf_telemetry->_telemetryData.flightMode.flightMode, 0, sizeof(crsf_telemetry->_telemetryData.flightMode.flightMode));

	size_t length = strlen(flightMode);

	if(length >= CRSF_FRAME_FLIGHT_MODE_PAYLOAD_SIZE){
    	return;
    }

    memcpy(crsf_telemetry->_telemetryData.flightMode.flightMode, flightMode, length);

    if (armed_bool != 0) {
        strcat(crsf_telemetry->_telemetryData.flightMode.flightMode, "*");
    }
}


void crsf_telemetry_setGPSData(crsf_telemetry_t *crsf_telemetry, float latitude, float longitude, float altitude, float speed, float course, uint8_t satellites)
{
	crsf_telemetry->_telemetryData.gps.latitude = latitude * 10000000;
	crsf_telemetry->_telemetryData.gps.longitude = longitude * 10000000;
	crsf_telemetry->_telemetryData.gps.altitude = (CRSF_CLAMP(altitude, 0, 5000 * 100) / 100) + 1000;
    crsf_telemetry->_telemetryData.gps.speed = ((speed * 36 + 50) / 100);
    crsf_telemetry->_telemetryData.gps.groundCourse = (course * 100);
    crsf_telemetry->_telemetryData.gps.satellites = satellites;
}




static uint32_t crsf_telemetry_process_frame_to_send(crsf_telemetry_t *crsf_telemetry){

    const uint8_t currentSchedule = crsf_telemetry->_telemetryFrameSchedule[crsf_telemetry->_telemetryFrameScheduleIndex];

    crsf_telemetry->tx_buffer_tx_len = 0;
    crsf_telemetry->tx_buffer_tx_sent_len = 0;

    if (currentSchedule & (1 << CRSF_TELEMETRY_FRAME_ATTITUDE_INDEX)) {
    	crsf_telemetry->tx_buffer_tx_len += WriteAttitudeDataFrame(&(crsf_telemetry->_telemetryData), crsf_telemetry->tx_buffer, TELEMETRY_TX_BUFFER_SIZE - crsf_telemetry->tx_buffer_tx_len);
    }

    if (currentSchedule & (1 << CRSF_TELEMETRY_FRAME_BARO_ALTITUDE_INDEX)) {
    	crsf_telemetry->tx_buffer_tx_len += WriteBaroAltitudeDataFrame(&(crsf_telemetry->_telemetryData), crsf_telemetry->tx_buffer, TELEMETRY_TX_BUFFER_SIZE - crsf_telemetry->tx_buffer_tx_len);
    }

    if (currentSchedule & (1 << CRSF_TELEMETRY_FRAME_BATTERY_SENSOR_INDEX)) {
    	crsf_telemetry->tx_buffer_tx_len += WriteBatterySensorDataFrame(&(crsf_telemetry->_telemetryData), crsf_telemetry->tx_buffer, TELEMETRY_TX_BUFFER_SIZE - crsf_telemetry->tx_buffer_tx_len);
    }

    if (currentSchedule & (1 << CRSF_TELEMETRY_FRAME_FLIGHT_MODE_INDEX)) {
    	crsf_telemetry->tx_buffer_tx_len += WriteFlightModeDataFrame(&(crsf_telemetry->_telemetryData), crsf_telemetry->tx_buffer, TELEMETRY_TX_BUFFER_SIZE - crsf_telemetry->tx_buffer_tx_len);
    }

    if (currentSchedule & (1 << CRSF_TELEMETRY_FRAME_GPS_INDEX)) {
    	crsf_telemetry->tx_buffer_tx_len += WriteGPSDataFrame(&(crsf_telemetry->_telemetryData), crsf_telemetry->tx_buffer, TELEMETRY_TX_BUFFER_SIZE - crsf_telemetry->tx_buffer_tx_len);
    }

    if (currentSchedule & (1 << CRSF_TELEMETRY_FRAME_HEARTBEAT_INDEX)) {
    	crsf_telemetry->tx_buffer_tx_len += WriteHeartbeatDataFrame(crsf_telemetry->tx_buffer, TELEMETRY_TX_BUFFER_SIZE - crsf_telemetry->tx_buffer_tx_len);
    }


	crsf_telemetry->_telemetryFrameScheduleIndex = (crsf_telemetry->_telemetryFrameScheduleIndex + 1) % crsf_telemetry->_telemetryFrameScheduleCount;
    return crsf_telemetry->tx_buffer_tx_len;
}


/*
 * Called periodically by the scheduler
 */
uint32_t crsf_telemetry_update(crsf_telemetry_t *crsf_telemetry, uint32_t currentTimeUs)
{
    uint32_t written_bytes_in_buf = 0;
    // Actual telemetry data only needs to be sent at a low frequency, ie 10Hz
    // Spread out scheduled frames evenly so each frame is sent at the same frequency.
    if(crsf_telemetry->_telemetryFrameScheduleCount > 0){
        if ((currentTimeUs - crsf_telemetry->crsfLastCycleTime_us) >= (CRSF_TELEMETRY_UPDATE_CYCLETIME_US / crsf_telemetry->_telemetryFrameScheduleCount)) {
        	crsf_telemetry->crsfLastCycleTime_us = currentTimeUs;
            written_bytes_in_buf = crsf_telemetry_process_frame_to_send(crsf_telemetry);
        }
    }

    return written_bytes_in_buf;
}


void crsf_telemetry_init(crsf_telemetry_t *crsf_telemetry)
{
	memset(crsf_telemetry, 0, sizeof(*crsf_telemetry));
	crsf_telemetry->_telemetryFrameScheduleCount = 0;
    memset(crsf_telemetry->_telemetryFrameSchedule, 0, sizeof(crsf_telemetry->_telemetryFrameSchedule));
    memset(&(crsf_telemetry->_telemetryData), 0, sizeof(crsf_telemetry->_telemetryData));

    uint8_t index = 0;
#if CRSF_TELEMETRY_ENABLED > 0 && CRSF_TELEMETRY_ATTITUDE_ENABLED > 0
    crsf_telemetry->_telemetryFrameSchedule[index++] = (1 << CRSF_TELEMETRY_FRAME_ATTITUDE_INDEX);
#endif

#if CRSF_TELEMETRY_ENABLED > 0 && CRSF_TELEMETRY_BAROALTITUDE_ENABLED > 0
    crsf_telemetry->_telemetryFrameSchedule[index++] = (1 << CRSF_TELEMETRY_FRAME_BARO_ALTITUDE_INDEX);
#endif

#if CRSF_TELEMETRY_ENABLED > 0 && CRSF_TELEMETRY_BATTERY_ENABLED > 0
    crsf_telemetry->_telemetryFrameSchedule[index++] = (1 << CRSF_TELEMETRY_FRAME_BATTERY_SENSOR_INDEX);
#endif

#if CRSF_TELEMETRY_ENABLED > 0 && CRSF_TELEMETRY_FLIGHTMODE_ENABLED > 0
    crsf_telemetry->_telemetryFrameSchedule[index++] = (1 << CRSF_TELEMETRY_FRAME_FLIGHT_MODE_INDEX);
#endif

#if CRSF_TELEMETRY_ENABLED > 0 && CRSF_TELEMETRY_GPS_ENABLED > 0
    crsf_telemetry->_telemetryFrameSchedule[index++] = (1 << CRSF_TELEMETRY_FRAME_GPS_INDEX);
#endif

#if CRSF_TELEMETRY_HEARTBEAT_ENABLED > 0
    crsf_telemetry->_telemetryFrameSchedule[index++] = (1 << CRSF_TELEMETRY_FRAME_HEARTBEAT_INDEX);
#endif

    crsf_telemetry->_telemetryFrameScheduleCount = index;
}


uint8_t* crsf_telemetry_get_tx_data(crsf_telemetry_t *crsf_telemetry){
	if(crsf_telemetry->tx_buffer_tx_sent_len > TELEMETRY_TX_BUFFER_SIZE ||
			crsf_telemetry->tx_buffer_tx_len > TELEMETRY_TX_BUFFER_SIZE ||
			crsf_telemetry->tx_buffer_tx_sent_len > crsf_telemetry->tx_buffer_tx_len)
	{
		return NULL;
	}
	return &(crsf_telemetry->tx_buffer[crsf_telemetry->tx_buffer_tx_sent_len]);
}

uint32_t crsf_telemetry_get_tx_data_size(crsf_telemetry_t *crsf_telemetry){
	if(crsf_telemetry->tx_buffer_tx_sent_len > TELEMETRY_TX_BUFFER_SIZE ||
			crsf_telemetry->tx_buffer_tx_len > TELEMETRY_TX_BUFFER_SIZE ||
			crsf_telemetry->tx_buffer_tx_sent_len > crsf_telemetry->tx_buffer_tx_len)
	{
		return 0;
	}
	return crsf_telemetry->tx_buffer_tx_len - crsf_telemetry->tx_buffer_tx_sent_len;
}

void crsf_telemetry_update_tx_data_sent(crsf_telemetry_t *crsf_telemetry, uint32_t data_sent){
	crsf_telemetry->tx_buffer_tx_sent_len += data_sent;
}







