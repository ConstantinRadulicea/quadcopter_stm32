#ifndef CRSF_H
#define CRSF_H
// https://github.com/tbs-fpv/tbs-crsf-spec/blob/main/crsf.md
#include "crsf_config.h"
#include "stdint.h"
#include "crsf_protocol.h"
#include "crsf_telemetry.h"

#define CRSF_FAILSAFE_STAGE1_MS 300

typedef struct flightMode_s
{
	char name[CRSF_FRAME_FLIGHT_MODE_PAYLOAD_SIZE];
	uint8_t channel;
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
	int valid : 1;
    int failsafe : 1;
    uint16_t value[RC_CHANNEL_COUNT];
} rcChannels_t;

typedef struct crsf_link_statistics_s
{
	int16_t rssi;
	int16_t lqi;
	int16_t snr;
	int16_t tx_power;
} crsf_link_statistics_t;

const uint16_t crsf_tx_power_table[9] = {
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

struct crsf_s;

typedef uint32_t (*crsf_output_cb_fn)(struct crsf_s *crsf, const void *data, uint32_t len, void *ctx);
typedef uint32_t (*crsf_sys_now_us_cb_fn)(void);

typedef struct crsf_s{
	uint32_t baudrate_bps;
	uint32_t frame_rate_hz;
	crsf_sys_now_us_cb_fn sys_now_us;
	crsf_output_cb_fn crsf_output;
	void* crsf_output_cb_fn_ctx;
	uint8_t rx_frame_position;
	uint32_t rx_frame_start_time_us;
	crsf_frame_t rxFrame;
	crsf_frame_t rcChannelsFrame;
	crsf_link_statistics_t linkStatistics;
	int rcFrameReceived : 1;
	int _linkIsUp : 1;

	rcChannels_t _rcChannels;
	crsf_telemetry_t telemetry;

	uint32_t _lastChannelsPacket;
#if CRSF_FLIGHTMODES_ENABLED > 0
	flightMode_t _flightModes[FLIGHT_MODE_COUNT];
#endif
}crsf_t;

int crsf_init(crsf_t *crsf,
		uint32_t frame_rate_hz,
		crsf_sys_now_us_cb_fn sys_now_us,
		crsf_output_cb_fn crsf_output,
		void* crsf_output_cb_fn_ctx
		);

void crsf_set_output_cb_fn(crsf_t *crsf, crsf_output_cb_fn fn, void *ctx);
int crsf_update(crsf_t *crsf, uint8_t rxByte);

void crsf_getLinkStatistics(crsf_t *crsf, crsf_link_statistics_t *linkStats);
void crsf_getRcChannels(crsf_t *crsf, rcChannels_t* rc_channels);
uint16_t crsf_getRcChannel(crsf_t *crsf, rc_channels_t channel);
int crsf_isRcDataValid(crsf_t *crsf);
int crsf_getFailSafe(crsf_t *crsf);
int crsf_isLinkUp(crsf_t *crsf);


void crsf_setAttitudeData(crsf_t *crsf, int16_t roll_rad, int16_t pitch_rad, int16_t yaw_rad);
void crsf_setBaroAltitudeData(crsf_t *crsf, uint16_t altitude, int16_t vario);
void crsf_setBatteryData(crsf_t *crsf, float voltage, float current, uint32_t capacity, uint8_t percent);
void crsf_setGPSData(crsf_t *crsf, float latitude, float longitude, float altitude, float speed, float course, uint8_t satellites);

#if CRSF_FLIGHTMODES_ENABLED > 0
void crsf_setFlightModeData(crsf_t *crsf, flightModeId_t flightMode, int disarmed);
#endif

#endif
