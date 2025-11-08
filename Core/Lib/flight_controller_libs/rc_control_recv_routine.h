#ifndef RC_CONTROL_RECV_ROUTINE_H
#define RC_CONTROL_RECV_ROUTINE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef RC_CONTROL_PORT
#define RC_CONTROL_PORT 5001
#endif
void rc_control_main(void *arg);

#ifdef __cplusplus
}
#endif


#endif
