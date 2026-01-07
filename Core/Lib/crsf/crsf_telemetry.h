#ifndef CRSF_TELEMETRY_H
#define CRSF_TELEMETRY_H

#include "crsf_protocol.h"

#define TELEMETRY_TX_BUFFER_SIZE CRSF_FRAME_SIZE_MAX

typedef struct crsf_telemetry_s{

    uint8_t _telemetryFrameScheduleCount;
    uint8_t _telemetryFrameSchedule[CRSF_TELEMETRY_FRAME_SCHEDULE_MAX];
    uint8_t _telemetryFrameScheduleIndex;
    telemetryData_t _telemetryData;
    uint32_t tx_buffer_tx_len;
    uint32_t tx_buffer_tx_sent_len;
    uint8_t tx_buffer[TELEMETRY_TX_BUFFER_SIZE];
}crsf_telemetry_t;


void crsf_telemetry_init(crsf_telemetry_t *crsf_telemetry);
uint32_t crsf_telemetry_update(crsf_telemetry_t *crsf_telemetry);

uint8_t* crsf_telemetry_get_tx_data(crsf_telemetry_t *crsf_telemetry);
uint32_t crsf_telemetry_get_tx_data_size(crsf_telemetry_t *crsf_telemetry);
void crsf_telemetry_update_tx_data_sent(crsf_telemetry_t *crsf_telemetry, uint32_t data_sent);




void crsf_telemetry_setAttitudeData(crsf_telemetry_t *crsf_telemetry, int16_t roll_rad, int16_t pitch_rad, int16_t yaw_rad);
void crsf_telemetry_setBaroAltitudeData(crsf_telemetry_t *crsf_telemetry, uint16_t altitude, int16_t vario);
void crsf_telemetry_setBatteryData(crsf_telemetry_t *crsf_telemetry, float voltage, float current, uint32_t capacity, uint8_t percent);

// strlen(flightMode) must be less than CRSF_FRAME_FLIGHT_MODE_PAYLOAD_SIZE
void crsf_telemetry_setFlightModeData(crsf_telemetry_t *crsf_telemetry, char *flightMode, int armed_bool);

void crsf_telemetry_setGPSData(crsf_telemetry_t *crsf_telemetry, float latitude, float longitude, float altitude, float speed, float course, uint8_t satellites);


#endif
