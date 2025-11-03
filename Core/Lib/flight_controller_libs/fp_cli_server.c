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
    while (1) {

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
