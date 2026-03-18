#ifndef GAZEBO_LINK_H
#define GAZEBO_LINK_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hil_link_recv_struct_s{
	float accel_x_g;
	float accel_y_g;
	float accel_z_g;
	float gyro_x_rad_s;
	float gyro_y_rad_s;
	float gyro_z_rad_s;
}hil_link_recv_struct_t;

typedef struct hil_link_send_struct_s{
	float motor_0;
	float motor_1;
	float motor_2;
	float motor_3;
}hil_link_send_struct_t;

extern hil_link_recv_struct_t hil_link_data_recv;
extern hil_link_send_struct_t hil_link_data_send;

void gazebo_link_recv_loop();
void gazebo_link_send_loop();



#ifdef __cplusplus
}
#endif

#endif
