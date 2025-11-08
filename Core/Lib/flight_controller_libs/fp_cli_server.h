#ifndef FP_CLI_SERVER_H
#define FP_CLI_SERVER_H

#include "stdarg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The echo server thread function.
 * @param arg Unused argument
 */
void echo_netconn_server_thread(int *port_in);
void tcp_socket_server_task(void *arg);


#define FP_CLI_SERVER_PORT 5002
#define FP_CLI_SERVER_BUF_SIZE 512
void tcp_socket_fp_cli_server_task(void *arg);

void vfprintfsock( int s, const char* f, va_list va);
void fprintfsock( int s, const char* f, ... );

extern int g_fp_cli_server_socket;

#ifdef __cplusplus
}
#endif


#endif
