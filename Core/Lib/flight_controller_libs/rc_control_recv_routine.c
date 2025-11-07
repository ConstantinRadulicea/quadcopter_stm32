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



void rc_control_main(void *arg)
{
	flight_control_loop_t *fcl_ptr = (flight_control_loop_t*)arg;

	char data_buffer[256];
    char linebuf[256];
    int data_len;
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
    struct netconn *conn;
      err_t err;

      LWIP_UNUSED_ARG(arg); // Suppress compiler warning

      // 1. Create a new UDP connection
      conn = netconn_new(NETCONN_UDP);
      if (conn == NULL) {
        printf("udp_server: Failed to create netconn\n");
        return; // Thread exits
      }

      // 2. Bind the connection to a local port
      //    We listen on IP_ADDR_ANY (all interfaces) and our defined port
      err = netconn_bind(conn, IP_ADDR_ANY, udpPort);
      if (err != ERR_OK) {
        printf("udp_server: Failed to bind, err: %d\n", err);
        netconn_delete(conn);
        return; // Thread exits
      }

      printf("UDP Server listening on port %d\n", udpPort);

  /* Infinite loop */
  for(;;)
  {
	  current_message_corrupted = 1;

	  struct netbuf *buf;

	      // 3. Block and wait for a packet
	      //    This is the main blocking call. The RTOS will switch to
	      //    other tasks while this thread waits for data.
	      err = netconn_recv(conn, &buf);
	      if (err == ERR_OK){
	    	        data_len = netbuf_len(buf);
	    	        if (data_len < sizeof(data_buffer)) {
	    	          netbuf_copy(buf, data_buffer, data_len);
	    	        }
	    	        // 5. Clean up the received buffer
	    	              //    This is CRITICAL. You MUST delete the netbuf
	    	              //    to free its memory from the LwIP PBUF pool.
	    	              netbuf_delete(buf);

          for(int i=0; i<data_len; i++){


			  if (line_buffer_add_char(data_buffer[i], linebuf, sizeof(linebuf), &line_len)) {

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
					  continue;
				  }
			  }
          }
	      }


      if(current_message_corrupted == 0){
    	  errors = 0;

          target_attitude = temp_target_attitude;
          arm_flag = (temp_arm_flag);
          disarm_flag = (temp_disarm_flag);
          target_throttle = temp_target_throttle;
      }

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
}

