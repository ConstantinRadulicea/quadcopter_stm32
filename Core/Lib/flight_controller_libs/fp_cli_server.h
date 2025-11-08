#ifndef FP_CLI_SERVER_H
#define FP_CLI_SERVER_H

/**
 * @brief The echo server thread function.
 * @param arg Unused argument
 */
void echo_netconn_server_thread(int *port_in);
void tcp_socket_server_task(void *arg);


#endif
