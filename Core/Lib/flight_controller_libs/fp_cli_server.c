#include "fp_cli_server.h"


#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/api.h"


/**
 * @brief The echo server thread function.
 * @param arg Unused argument
 */
void echo_netconn_server_thread(int *port_in)
{
    struct netconn *conn, *newconn;
    err_t err;

    u16_t port = (u16_t)(*port_in);


    /* 1. Create a new TCP connection handle */
    conn = netconn_new(NETCONN_TCP);
    if (conn == NULL) {
        //printf("Echo Server: Failed to create netconn\n");
        return;
    }

    /* 2. Bind the connection to any local IP address and the specified port */
    // Use IP_ADDR_ANY to bind to all network interfaces
    err = netconn_bind(conn, IP_ADDR_ANY, port);
    if (err != ERR_OK) {
        //printf("Echo Server: Failed to bind (err=%d)\n", err);
        netconn_delete(conn);
        return;
    }

    /* 3. Put the connection into listening mode */
    err = netconn_listen(conn);
    if (err != ERR_OK) {
        //printf("Echo Server: Failed to listen (err=%d)\n", err);
        netconn_delete(conn);
        return;
    }

    /* Start listening (backlog may be ignored by netconn implementation) */
    netconn_listen_with_backlog(conn, 5);

    //printf("Echo Server: Listening on port %d\n", port);

    /* 4. Main server loop: Accept new connections */
    for(;;) {

        // netconn_accept() blocks the task until a new client connects
        err = netconn_accept(conn, &newconn);

        if (err == ERR_OK) {
            //printf("Echo Server: New client connected\n");

            struct netbuf *buf;
            /* --- FIX 1: Disable Nagle's Algorithm --- */
                        // This prevents the "freeze" when sending small packets
            //netconn_set_no_delay(newconn, 1);

            while ((err = netconn_recv(newconn, &buf)) == ERR_OK) {
                /* netbuf contains one or more pbufs; netbuf_data gives pointer & length */
                void *data;
                u16_t len;

                do {
                    netbuf_data(buf, &data, &len);
                    if (len > 0) {
                        /* Echo back the data. Use NETCONN_COPY to copy user buffer into lwIP */
                        err_t werr = netconn_write(newconn, data, len, NETCONN_COPY);
                        if (werr != ERR_OK) {
                            /* write failed, break and close connection */
                            LWIP_DEBUGF(LWIP_DBG_ON, ("echo: write error %d\n", werr));
                            break;
                        }
                    }
                    /* move to next pbuf in this netbuf (if any)
                     * netbuf_next() advances the internal pointer; netbuf_data() will then return next part */
                } while (netbuf_next(buf) >= 0);

                netbuf_delete(buf); /* free netbuf */
                buf = NULL;
            }

            if (err != ERR_OK && err != ERR_CLSD) {
                LWIP_DEBUGF(LWIP_DBG_ON, ("echo: recv error %d\n", recv_err));
            }

            /* 7. Client disconnected */
            // netconn_recv() returned an error (e.g., ERR_CLSD)
            //printf("Echo Server: Client disconnected\n");

            // Close the connection
            netconn_close(newconn);

            // Delete the connection handle
            netconn_delete(newconn);
        }
    }
}


#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "lwip/netdb.h"
#include <string.h>
#include <stdio.h>
#include "FreeRTOS.h"

#define TCP_ECHO_PORT 12345
#define TCP_ECHO_BUF_SIZE 512

void tcp_socket_server_task(void *arg)
{
    (void)arg;
    int listen_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buf[TCP_ECHO_BUF_SIZE];
    int ret;

    // 1️⃣ Create socket
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        printf("socket() failed, errno=%d\n", errno);
        //vTaskDelete(NULL);
        return;
    }

    // Allow quick rebinding if the connection closes
    int yes = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    // 2️⃣ Bind socket
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(TCP_ECHO_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    ret = bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret < 0) {
        printf("bind() failed, errno=%d\n", errno);
        closesocket(listen_fd);
        //vTaskDelete(NULL);
        return;
    }

    // 3️⃣ Listen for clients
    ret = listen(listen_fd, 5);
    if (ret < 0) {
        printf("listen() failed, errno=%d\n", errno);
        closesocket(listen_fd);
        //vTaskDelete(NULL);
        return;
    }

    printf("TCP echo server listening on port %d\n", TCP_ECHO_PORT);

    for (;;) {
        // 4️⃣ Accept client
        client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            printf("accept() failed, errno=%d\n", errno);
            continue;
        }

        char ipstr[16];
        inet_ntoa_r(client_addr.sin_addr, ipstr, sizeof(ipstr));
        printf("Client connected: %s:%u\n", ipstr, ntohs(client_addr.sin_port));

        // 5️⃣ Echo loop
        for (;;) {
            ret = recv(client_fd, buf, sizeof(buf), 0);
            if (ret < 0) {
                printf("recv() failed, errno=%d\n", errno);
                break;
            } else if (ret == 0) {
                // client closed connection
                printf("Client disconnected: %s:%u\n", ipstr, ntohs(client_addr.sin_port));
                break;
            }

            // echo data back
            int sent = send(client_fd, buf, ret, 0);
            if (sent < 0) {
                printf("send() failed, errno=%d\n", errno);
                break;
            }
        }

        closesocket(client_fd);
    }
}





#include"fp_cli.h"
int g_fp_cli_server_socket = -1;
void tcp_socket_fp_cli_server_task(void *arg)
{
    (void)arg;
    int listen_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buf[FP_CLI_SERVER_BUF_SIZE];
    int ret;

    // 1️⃣ Create socket
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        printf("socket() failed, errno=%d\n", errno);
        //vTaskDelete(NULL);
        return;
    }

    // Allow quick rebinding if the connection closes
    int yes = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    // 2️⃣ Bind socket
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(FP_CLI_SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    ret = bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret < 0) {
        printf("bind() failed, errno=%d\n", errno);
        closesocket(listen_fd);
        //vTaskDelete(NULL);
        return;
    }

    // 3️⃣ Listen for clients
    ret = listen(listen_fd, 5);
    if (ret < 0) {
        printf("listen() failed, errno=%d\n", errno);
        closesocket(listen_fd);
        //vTaskDelete(NULL);
        return;
    }

    printf("TCP echo server listening on port %d\n", FP_CLI_SERVER_PORT);

    for (;;) {
        // 4️⃣ Accept client
        client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
        g_fp_cli_server_socket = client_fd;
        if (client_fd < 0) {
            printf("accept() failed, errno=%d\n", errno);
            continue;
        }

        char ipstr[16];
        inet_ntoa_r(client_addr.sin_addr, ipstr, sizeof(ipstr));
        printf("Client connected: %s:%u\n", ipstr, ntohs(client_addr.sin_port));

        // 5️⃣ Echo loop
        for (;;) {
            ret = recv(client_fd, buf, sizeof(buf), 0);
            if (ret < 0) {
                printf("recv() failed, errno=%d\n", errno);
                break;
            } else if (ret == 0) {
                // client closed connection
                printf("Client disconnected: %s:%u\n", ipstr, ntohs(client_addr.sin_port));
                break;
            }

            // echo data back
//            int sent = send(client_fd, buf, ret, 0);
//            if (sent < 0) {
//                printf("send() failed, errno=%d\n", errno);
//                break;
//            }
            lwshell_input_ex(&lwshell_cli, buf, ret);
        }
        g_fp_cli_server_socket = -1;
        closesocket(client_fd);
    }
}




#include "stdarg.h"

void vfprintfsock( int s, const char* f, va_list va)
{
	char local_buffer[128];
	int local_buffer_size = sizeof(local_buffer);
	int buffer_size;
	char *buf;
    if(s < 0){
    	return;
    }
    buffer_size = vsnprintf( 0, 0, f, va );
    buffer_size += 1;
    if(buffer_size > local_buffer_size){
    	buf = (char*) malloc(buffer_size);
    	if(buf == NULL) return;
    }
    else{
    	buf = local_buffer;
    }


    vsnprintf( buf, buffer_size, f, va );
    send( s, buf, buffer_size-1, 0 );

    if(buffer_size > local_buffer_size){
    	free( buf );
    }

}


void fprintfsock( int s, const char* f, ... )
{
    va_list a;
    va_start( a, f );
    vfprintfsock(s, f, a);
    va_end(a);
}


