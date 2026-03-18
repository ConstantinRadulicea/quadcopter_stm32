#include "gazebo_link.h"
#include "base64.h"
#include "crc.h"
#include "crsf_def.h"
#include "string.h"
#include "usbd_cdc_if.h"

#define HIL_OUT_WRITE(...) usb_printf(__VA_ARGS__)
//#define HIL_IN_READ(...) usb_printf(__VA_ARGS__)

#define IN_BUFFER_SIZE 256
#define OUT_BUFFER_SIZE 256


hil_link_recv_struct_t hil_link_data_recv;
hil_link_send_struct_t hil_link_data_send;



struct hil_link_recv_struct_raw_s {
	uint32_t accel_x_g;
	uint32_t accel_y_g;
	uint32_t accel_z_g;
	uint32_t gyro_x_rad_s;
	uint32_t gyro_y_rad_s;
	uint32_t gyro_z_rad_s;
} __attribute__((packed));


struct hil_link_send_struct_raw_s {
	uint32_t motor_0;
	uint32_t motor_1;
	uint32_t motor_2;
	uint32_t motor_3;
} __attribute__((packed));




void gazebo_link_recv_loop(void)
{
	struct hil_link_recv_struct_raw_s raw_data;
    static uint8_t in_buffer[IN_BUFFER_SIZE];
    static size_t in_buffer_len = 0;
    static int new_line_found = 0;

    // Rulăm continuu până când nu mai sunt date de citit de la hardware
    while (1) {
    	if(new_line_found != 0) in_buffer_len = 0;
    	new_line_found = 0;
        size_t in_buffer_remaining_len = IN_BUFFER_SIZE - in_buffer_len;

        // Protecție overflow: dacă bufferul s-a umplut și nu am găsit '\n',
        // aruncăm datele corupte pentru a nu bloca sistemul.
        if (in_buffer_remaining_len == 0) {
            in_buffer_len = 0;
            in_buffer_remaining_len = IN_BUFFER_SIZE;
        }

        // Citim ce este disponibil în acest moment
        size_t n = CDC_recv_data(&in_buffer[in_buffer_len], in_buffer_remaining_len);

        // Dacă nu mai sunt caractere de citit din hardware, oprim bucla și lăsăm STM32 să facă altceva
        if (n == 0) {
            break;
        }

        in_buffer_len += n;

        // Căutăm delimitatorul
        uint8_t *newline_ptr = memchr(in_buffer, '\n', in_buffer_len);

        // Procesăm toate cadrele care au fost recepționate complet
        if (newline_ptr != NULL) {
        	CDC_data_rx_flush();
            size_t frame_len = newline_ptr - in_buffer;

            // Trimitem cadrul izolat către decodorul Base64
            in_buffer_len = Base64decode(in_buffer, in_buffer, Base64Decode_nprbytes(in_buffer, frame_len));
            new_line_found = 1;

            break;
        }
    }

    if(new_line_found != 0){
		uint8_t crc_received = in_buffer[in_buffer_len-1];
		uint8_t crc_calculated = crc8_dvb_s2_init();
		crc_calculated = crc8_dvb_s2_add_arr(crc_calculated, in_buffer, in_buffer_len-1);
		if(crc_received != crc_calculated) return;
		// frame is correct set data into hil_link_data;
		raw_data = *((struct hil_link_recv_struct_raw_s*)in_buffer);

		raw_data.accel_x_g = crsf_ntohl(raw_data.accel_x_g);
		raw_data.accel_y_g = crsf_ntohl(raw_data.accel_y_g);
		raw_data.accel_z_g = crsf_ntohl(raw_data.accel_z_g);

		raw_data.gyro_x_rad_s = crsf_ntohl(raw_data.gyro_x_rad_s );
		raw_data.gyro_y_rad_s = crsf_ntohl(raw_data.gyro_y_rad_s );
		raw_data.gyro_z_rad_s = crsf_ntohl(raw_data.gyro_z_rad_s );

		memcpy(&(hil_link_data_recv.accel_x_g), &(raw_data.accel_x_g), sizeof(float));
		memcpy(&(hil_link_data_recv.accel_y_g), &(raw_data.accel_y_g), sizeof(float));
		memcpy(&(hil_link_data_recv.accel_z_g), &(raw_data.accel_z_g), sizeof(float));

		memcpy(&(hil_link_data_recv.gyro_x_rad_s), &(raw_data.gyro_x_rad_s), sizeof(float));
		memcpy(&(hil_link_data_recv.gyro_y_rad_s), &(raw_data.gyro_y_rad_s), sizeof(float));
		memcpy(&(hil_link_data_recv.gyro_z_rad_s), &(raw_data.gyro_z_rad_s), sizeof(float));
    }

}


void gazebo_link_send_loop(){
	static uint8_t out_buffer[128];
	static uint8_t out_buffer_encoded[OUT_BUFFER_SIZE];
	struct hil_link_send_struct_raw_s *raw_data;
	size_t buf_len = 0;

	raw_data = ((struct hil_link_send_struct_raw_s*)out_buffer);


	memcpy(&(raw_data->motor_0), &(hil_link_data_send.motor_0), sizeof(float));
	memcpy(&(raw_data->motor_1), &(hil_link_data_send.motor_1), sizeof(float));
	memcpy(&(raw_data->motor_2), &(hil_link_data_send.motor_2), sizeof(float));
	memcpy(&(raw_data->motor_3), &(hil_link_data_send.motor_3), sizeof(float));

	raw_data->motor_0 = crsf_htonl(raw_data->motor_0);
	raw_data->motor_1 = crsf_htonl(raw_data->motor_1);
	raw_data->motor_2 = crsf_htonl(raw_data->motor_2);
	raw_data->motor_3 = crsf_htonl(raw_data->motor_3 );

	buf_len = (sizeof(uint32_t) * 4);

	uint8_t crc_calculated = crc8_dvb_s2_init();
	crc_calculated = crc8_dvb_s2_add_arr(crc_calculated, out_buffer, buf_len);
	out_buffer[buf_len] = crc_calculated;
	buf_len += sizeof(crc_calculated);

	buf_len = Base64encode(out_buffer_encoded, out_buffer, buf_len);

	out_buffer_encoded[buf_len] = '\n';
	buf_len += sizeof(uint8_t);

	CDC_Transmit_FS((uint8_t*)out_buffer_encoded, (uint16_t)buf_len);
}

