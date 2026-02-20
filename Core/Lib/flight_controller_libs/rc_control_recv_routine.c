#include "rc_control_recv_routine.h"
#include "rtos_flight_controller.h"
#include "flight_control_loop.h"
#include "crc.h"
#include "string.h"
#include "crsf.h"
#include "usart.h"

// Headers for raw socket programming
#include "lwip/api.h"


static int line_buffer_add_char(char c, char *buf, size_t buf_size, size_t *len)
{
    // buf: destination buffer
    // buf_size: total buffer capacity
    // len: pointer to current length (persistent between calls)
    // Returns 1 when a full line (ending with \n\r) is complete, 0 otherwise

    if (*len + 1 >= buf_size) {
        // buffer overflow: reset
        *len = 0;
        return 0;
    }

    buf[(*len)++] = c;

    // Check for \n\r at the end
    if (*len >= 2) {
    	if(buf[*len - 2] == '\r' && buf[*len - 1] == '\n'){
            buf[*len - 2] = '\0'; // terminate string before \n\r
            *len = 0;             // reset for next line
            return 1;             // line complete
    	}
    }

    return 0; // line not complete yet
}


#include "lwip/sockets.h"
#include "lwip/inet.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>

//#define UDP_RX_BUF_SIZE    PBUF_POOL_BUFSIZE   /* enough for full Ethernet MTU */
#define UDP_RX_BUF_SIZE    (64*6)

char rxbuf[UDP_RX_BUF_SIZE];
void rc_control_udp(void *arg)
{
	flight_control_loop_t *fcl_ptr = (flight_control_loop_t*)arg;

    char linebuf[256];
    size_t line_len = 0;

    float target_throttle = 0;
    float temp_target_throttle = 0;

    coord3D target_attitude = {0};
    coord3D temp_target_attitude = {0};

    float arm_flag = 0;
    float temp_arm_flag = 0;

    float disarm_flag = 0;
    float temp_disarm_flag = 0;

    int errors = 0;
    int current_message_corrupted = 0;
    uint16_t crc_calculated;
    unsigned int crc_received = 0;

    uint16_t udpPort = (uint16_t)RC_CONTROL_PORT;
    int sock = -1;
    struct sockaddr_in srv_addr;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        printf("udp_server: socket() failed errno=%d (%s)\n", errno, strerror(errno));
        vTaskDelete(NULL);
        return;
    }

    /* Optional: allow rebind quickly after restart */
    {
        int yes = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    }

    /* optional: set receive timeout so recvfrom doesn't block forever */
//    {
//        struct timeval tv;
//        tv.tv_sec = UDP_RX_TIMEOUT_MS / 1000;
//        tv.tv_usec = (UDP_RX_TIMEOUT_MS % 1000) * 1000;
//        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
//    }

    /* After creating socket 'sock' */
    int on = 1;
    if (ioctl(sock, FIONBIO, &on) < 0) {
        printf("ioctl(FIONBIO) failed errno=%d (%s)\n", errno, strerror(errno));
    }

    fcntl(sock, F_SETFL, O_NONBLOCK);

    memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(udpPort);
    srv_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (struct sockaddr *)&srv_addr, sizeof(srv_addr)) < 0) {
        printf("udp_server: bind() failed errno=%d (%s)\n", errno, strerror(errno));
        closesocket(sock);
        vTaskDelete(NULL);
        return;
    }

    printf("udp_server: listening on port %d\n", udpPort);

  /* Infinite loop */
  for(;;)
  {

	  int r = 0;
	  do {
	  current_message_corrupted = 1;

      struct sockaddr_in client;
      socklen_t client_len = sizeof(client);
       r = recvfrom(sock, rxbuf, UDP_RX_BUF_SIZE, MSG_DONTWAIT,
                       (struct sockaddr *)&client, &client_len);
      if (r < 0) {
          int e = errno;
          if (e == EWOULDBLOCK || e == EAGAIN) {
              /* timed out - not an error, continue */
              //continue;
          }

          /* common lwIP allocation errors you may see here:
             - ENOMEM  : mem_malloc()/memp_malloc() failed
             - ENOBUFS : no pbuf / buffer available
             Print details and keep server alive. */
//          printf("udp_server: recvfrom() failed r=%d errno=%d (%s)\n", r, e, strerror(e));
      }

          for(int i=0; i<r; i++){

			  if (line_buffer_add_char(rxbuf[i], linebuf, sizeof(linebuf), &line_len)) {

				  int temp_str_len = strlen(linebuf);
				  temp_str_len -= 6;
				  crc_calculated = crc16_ccitt_init();
				  crc_calculated = crc16_ccitt_add_arr(crc_calculated, (uint8_t*)linebuf, temp_str_len);

				  if (sscanf(linebuf, "%f;%f;%f;%f;%f;%f;%x",
						  &temp_target_throttle,
						  &temp_target_attitude.x,
						  &temp_target_attitude.y,
						  &temp_target_attitude.z,
						  &temp_arm_flag,
						  &temp_disarm_flag,

						  &crc_received

						  ) == 7) {
					  current_message_corrupted = 0;
					  //printf("%s\n", linebuf);

				  } else {
					  current_message_corrupted = 1;
//					  printf("%s\n", linebuf);
				  }
				  if(crc_received != (unsigned int)crc_calculated){
					  current_message_corrupted = 1;
				  }
			  }




      if(current_message_corrupted == 0){
    	  errors = 0;

          target_attitude = temp_target_attitude;
          arm_flag = (temp_arm_flag);
          disarm_flag = (temp_disarm_flag);
          target_throttle = temp_target_throttle;
      }
          }
	  } while(r>0);

      errors += current_message_corrupted;

      if (errors >= (RC_CONTROLLER_HZ)){
    	  target_attitude.x = 0.0f;
    	  target_attitude.y = 0.0f;
    	  target_attitude.z = 0.0f;
    	  target_throttle = 0.0f;
    	  disarm_flag = 1.0f;
    	  arm_flag = 0.0f;
      }

      if (arm_flag > 0.5f){
    	  flight_control_loop_arm_esc(fcl_ptr);
      }

      if (disarm_flag > 0.5f){
    	  flight_control_loop_disarm_esc(fcl_ptr);
      }

      flight_control_loop_update_rc_control(fcl_ptr, target_attitude, target_throttle);

      vTaskDelay(pdMS_TO_TICKS(HzToMilliSec(RC_CONTROLLER_HZ)));
  }
	closesocket(sock);
}



static uint32_t crsf_sys_now_example(void){
    // Get current kernel tick count and tick frequency
    uint32_t ticks      = osKernelGetTickCount();
    uint32_t tick_freq  = osKernelGetTickFreq();  // ticks per second

    // Convert ticks to milliseconds safely and portably
    return (uint32_t)((ticks * 1000U) / tick_freq) * 1000;
}


//static uint32_t crsf_sys_now_example(void) {
//    uint64_t ticks = (uint64_t)osKernelGetTickCount();
//    uint32_t freq  = osKernelGetTickFreq();
//
//    // Returns microseconds (us)
//    return (uint32_t)((ticks * 1000000ULL) / freq);
//}

static uint32_t crsf_output_cb_fn_example(crsf_t *crsf, const void *data, uint32_t len, void *ctx) {
	(void) crsf;
	(void) ctx;
	return uart_send_data(&usart3_driver, (char*)data, len);
}

void rc_control_crsf(void *arg){
	crsf_t crsf;
	uint32_t frame_rate_hz = 333;
	size_t rx_data_size = 0;
	size_t rx_data_size_processed = 0;
	size_t total_loops = 0;
	frameType_t crsf_result;
	float roll;
	float pitch;
	float yaw;
	float throttle;
	int8_t failsafe;
	int8_t is_armed;
	int8_t isLinkUp = 0;
	int8_t armed_fp = 0;
	coord3D target_attitude = {0};
	flight_control_loop_t *fcl_ptr = (flight_control_loop_t*)arg;
	crsf_init(&crsf, frame_rate_hz, crsf_sys_now_example, crsf_output_cb_fn_example, NULL);

	roll = 0.0f;
	pitch = 0.0f;
	yaw = 0.0f;
	throttle = 0.0f;
	failsafe = 0;
	is_armed = 0;

	uint32_t bytes_processed = 0;

	uart_data_rx_flush(&usart3_driver);
	for(;;) {
		total_loops = 0;
//		do{
			rx_data_size = uart_recv_data(&usart3_driver, rxbuf, UDP_RX_BUF_SIZE);
//			uart_data_rx_flush(&usart3_driver);
			rx_data_size_processed = 0;
			bytes_processed = 0;

			do{
				crsf_result = CRSF_FRAMETYPE_INVALID;
				bytes_processed = 0;
				crsf_result = crsf_update(&crsf, (uint8_t*)&(rxbuf[rx_data_size_processed]), (uint32_t)(rx_data_size - bytes_processed), &bytes_processed);
				rx_data_size_processed += bytes_processed;

				if(crsf_result == CRSF_FRAMETYPE_RC_CHANNELS_PACKED || crsf_result == CRSF_FRAMETYPE_SUBSET_RC_CHANNELS_PACKED){ // new frame was received

					if(crsf_isChannelUpdated(&crsf, RC_CHANNEL_ROLL) != 0){
						roll = crsf_getChannelNormalized(&crsf, RC_CHANNEL_ROLL);
					}

					if(crsf_isChannelUpdated(&crsf, RC_CHANNEL_PITCH) != 0){
						pitch = crsf_getChannelNormalized(&crsf, RC_CHANNEL_PITCH);
					}
					if(crsf_isChannelUpdated(&crsf, RC_CHANNEL_YAW) != 0){
						yaw = crsf_getChannelNormalized(&crsf, RC_CHANNEL_YAW);
					}

					if(crsf_isChannelUpdated(&crsf, RC_CHANNEL_THROTTLE) != 0){
						throttle = crsf_getChannelNormalized(&crsf, RC_CHANNEL_THROTTLE);
						throttle = crsf_transformThrottle(throttle);
					}

					is_armed = crsf_isArmed(&crsf);

					armed_fp = (int8_t)flight_control_loop_are_esc_armed(fcl_ptr);

					crsf_setFlightModeData(&crsf, FLIGHT_MODE_ANGLE, armed_fp);
//					break;
				}
			}while(rx_data_size_processed < rx_data_size);

//		} while(rx_data_size > 0 && total_loops < 64*10);

		isLinkUp = crsf_isLinkUp(&crsf);
		failsafe = crsf_getFailSafe(&crsf);

		target_attitude.x = roll;
		target_attitude.y = pitch;
		target_attitude.z = yaw;

	    if (is_armed != 0 && failsafe == 0){
	    	if(armed_fp == 0){
	    		flight_control_loop_arm_esc(fcl_ptr);
	    	}
	    }
	    else {
	    	if(armed_fp != 0){
	    		flight_control_loop_disarm_esc(fcl_ptr);
	    	}
	     }

	     flight_control_loop_update_rc_control(fcl_ptr, target_attitude, throttle);

	     vTaskDelay(pdMS_TO_TICKS(HzToMilliSec(RC_INPUT_SAMPLE_RATE_HZ)));
//	     vTaskDelay(1);
	}
}





void rc_control_main(void *arg){
	rc_control_crsf(arg);
}
