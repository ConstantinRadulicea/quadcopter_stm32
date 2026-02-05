#ifndef CRSF_H
#define CRSF_H
// https://github.com/tbs-fpv/tbs-crsf-spec/blob/main/crsf.md
// https://github.com/betaflight/betaflight/blob/8b57f7f10b8073420158b043748e8da352ba9994/src/main/telemetry/crsf.c
#include "crsf_config.h"
#include "stdint.h"
#include "stddef.h"
#include "crsf_protocol.h"
#include "crsf_telemetry.h"

#ifdef __cplusplus
extern "C" {
#endif

struct crsf_s;

typedef uint32_t (*crsf_output_cb_fn)(struct crsf_s *crsf, const void *data, uint32_t len, void *ctx);
typedef uint32_t (*crsf_sys_now_us_cb_fn)(void);

typedef struct crsf_s{
//	uint32_t baudrate_bps;
	uint32_t frame_rate_hz;
	crsf_sys_now_us_cb_fn sys_now_us;
	crsf_output_cb_fn crsf_output;
	void* crsf_output_cb_fn_ctx;
	uint8_t rx_frame_position;
	uint32_t rx_frame_start_time_us;
	crsf_frame_t rxFrame;
	crsf_frame_t rcChannelsFrame;
	crsf_link_statistics_t linkStatistics;
	int8_t rcFrameReceived;
	int8_t _linkIsUp;

	rcChannels_t _rcChannels;
	crsf_telemetry_t telemetry;

	int8_t is_armed;

	uint32_t _lastChannelsPacket;
#if CRSF_FLIGHTMODES_ENABLED != 0
	flightMode_t _flightModes[FLIGHT_MODE_COUNT];
#endif
}crsf_t;

int8_t crsf_init(crsf_t *crsf,
		uint32_t frame_rate_hz,
		crsf_sys_now_us_cb_fn sys_now_us,
		crsf_output_cb_fn crsf_output,
		void* crsf_output_cb_fn_ctx
		);

void crsf_set_output_cb_fn(crsf_t *crsf, crsf_output_cb_fn fn, void *ctx);
int8_t crsf_update(crsf_t *crsf, uint8_t rxByte);

int8_t crsf_isNewRcDataAvailable(crsf_t *crsf);
int8_t crsf_getFailSafe(crsf_t *crsf);
int8_t crsf_isLinkUp(crsf_t *crsf);
int8_t crsf_isArmed(crsf_t *crsf);


void crsf_setAttitudeData(crsf_t *crsf, int16_t roll_rad, int16_t pitch_rad, int16_t yaw_rad);
void crsf_setBaroAltitudeData(crsf_t *crsf, uint16_t altitude, int16_t vario);
void crsf_setBatteryData(crsf_t *crsf, float voltage, float current, uint32_t capacity, uint8_t percent);
void crsf_setGPSData(crsf_t *crsf, float latitude, float longitude, float altitude, float speed, float course, uint8_t satellites);

#if CRSF_FLIGHTMODES_ENABLED != 0
void crsf_setFlightModeData(crsf_t *crsf, flightModeId_t flightMode, int8_t disarmed);
#endif

float crsf_getChannelNormalized(crsf_t *crsf, rc_channels_t channel);

int8_t crsf_isChannelUpdated(crsf_t *crsf, rc_channels_t channel);



/* //////////////////////////////////////////Examples////////////////////////////////////////////////////  */

/*
uint32_t crsf_sys_now_example(void){
	static uint32_t temp_time = 0;
	temp_time += 1000;
	return temp_time;


//    // Get current kernel tick count and tick frequency
//    uint32_t ticks      = osKernelGetTickCount();
//    uint32_t tick_freq  = osKernelGetTickFreq();  // ticks per second
//
//    // Convert ticks to milliseconds safely and portably
//    return (uint32_t)((ticks * 1000U) / tick_freq) * 1000;
}

uint32_t crsf_output_cb_fn_example(crsf_t *crsf, const void *data, uint32_t len, void *ctx) {
	return len;

//	(void) crsf;
//	(void) ctx;
//	return uart_send_data(&usart3_driver, (char*)data, len);
}

static void crsf_loop_example(){
	crsf_t crsf;
	uint32_t frame_rate_hz = 250;
	char rx_data = 0;
	size_t rx_data_size = 0;
	int8_t crsf_result;
	uint16_t raw_channel_data;
	float roll;
	float pitch;
	float yaw;
	int8_t failsafe;
	int8_t is_armed;
	int8_t isLinkUp = 0;

	crsf_init(&crsf, frame_rate_hz, crsf_sys_now_example, crsf_output_cb_fn_example, NULL);

	for(;;) {
//		rx_data_size = uart_recv_data(&usart3_driver, rx_data, sizeof(rx_data));

		crsf_result = 0;
		if(rx_data_size > 0){
			crsf_result = crsf_update(&crsf, rx_data);
		}
		if(crsf_result != 0){ // new frame was received

			roll = 0.0f;
			pitch = 0.0f;
			yaw = 0.0f;
			failsafe = 0;
			is_armed = 0;

			raw_channel_data = crsf_getRcChannel(&crsf, RC_CHANNEL_ROLL);
			roll = crsf_rcToNormalized(raw_channel_data);

			raw_channel_data = crsf_getRcChannel(&crsf, RC_CHANNEL_PITCH);
			pitch = crsf_rcToNormalized(raw_channel_data);

			raw_channel_data = crsf_getRcChannel(&crsf, RC_CHANNEL_YAW);
			yaw = crsf_rcToNormalized(raw_channel_data);

			is_armed = crsf_isArmed(&crsf);

			crsf_setFlightModeData(&crsf, FLIGHT_MODE_ANGLE, is_armed);
		}
		isLinkUp = crsf_isLinkUp(&crsf);
		failsafe = crsf_getFailSafe(&crsf);
	}
}

*/

#ifdef __cplusplus
}
#endif

#endif
