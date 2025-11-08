#include "rc_control_recv_routine.h"
#include "rtos_flight_controller.h"
#include "flight_control_loop.h"
#include "crc.h"
#include "string.h"

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

#define UDP_RX_BUF_SIZE    PBUF_POOL_BUFSIZE   /* enough for full Ethernet MTU */

char rxbuf[UDP_RX_BUF_SIZE];
void rc_control_main(void *arg)
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

      if (errors >= (RC_CONTROLLER_HZ / 2.0f)){
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

