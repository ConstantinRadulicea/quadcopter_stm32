#include "lwip/init.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "lwip/tcp.h"
#include "netif/ppp/pppapi.h"
#include "netif/ppp/pppos.h"
#include "netif/ppp/ppp.h"
#include "cmsis_os.h"
#include <string.h>
#include <stdio.h>
#include "usbd_cdc_if.h"
#include <time.h>
#include <stdlib.h>
#include <usart.h>
#include "lwip/timeouts.h"

#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/etharp.h"
#include "lwip/dhcp.h"

static ppp_pcb*     g_ppp = NULL;
static struct netif g_pppif;
static volatile uint8_t tcpip_ready = 0;

static void reconnect_timeout(void *arg)
{
    ppp_pcb *pcb = (ppp_pcb *)arg;
    /* If pcb is still valid, call pppapi_connect again */
    if (pcb != NULL) {
        err_t e = pppapi_connect(pcb, 0); /* 0 = immediate retry, or use non-zero for built-in holdoff */
        LWIP_DEBUGF(PPP_DEBUG, ("pppapi_connect returned %d\n", e));
    } else {
        LWIP_DEBUGF(PPP_DEBUG, ("pcb is NULL; recreate pcb before reconnecting\n"));
        /* create new pcb and call connect */
    }
}

static void ppp_status_cb(void *ctx, int err_code, void *arg)
{
    LWIP_DEBUGF(PPP_DEBUG, ("ppp status: err=%d\n", err_code));

    if (err_code == PPPERR_NONE) {
        LWIP_DEBUGF(PPP_DEBUG, ("PPP connected\n"));
        /* interface up: you can configure routes or notify other parts */
    } else {
        LWIP_DEBUGF(PPP_DEBUG, ("PPP disconnected, scheduling reconnect\n"));
        /* schedule reconnect in 3 seconds (3000 ms) */
        sys_timeout(10, (sys_timeout_handler)reconnect_timeout, ctx);
    }
}



/* Called by tcpip_init() in the tcpip_thread context when ready */
static void tcpip_init_done(void *arg) {
  (void)arg;
  tcpip_ready = 1;
}

/* TX callback for PPP: write bytes to UART */
static u32_t ppp_output_cb(ppp_pcb *pcb, const void *data, u32_t len, void *ctx) {
  (void)pcb;
  (void)ctx;
  //CDC_Transmit_FS((uint8_t*)data, len);   // blocking is fine at 115200
  return uart_send_data(&usart1_driver, (char*)data, len);
  return len;
}


void net_ppp_start(void) {
	//srand(1);
	  /* 1) Start lwIP core thread and mboxes */
	  tcpip_init(tcpip_init_done, NULL);

	  while (!tcpip_ready) {
	    osDelay(100);
	  }

	  /* 2) Optionally wait (blocking) until tcpip thread is ready */
	  g_ppp = pppapi_pppos_create(&g_pppif, ppp_output_cb, ppp_status_cb, NULL);
	  //g_ppp = pppapi_pppoe_create(&g_pppif, ppp_status_cb, NULL);
  ppp_set_default(g_ppp);
  ppp_set_auth(g_ppp, PPPAUTHTYPE_NONE, NULL, NULL);   // dev/no auth

  /* Static IPCP pair: PC 10.0.0.1 <-> MCU 10.0.0.2 */
//  ip4_addr_t our_ip = IPADDR4_INIT_BYTES(10,0,0,2);
//  ip4_addr_t his_ip = IPADDR4_INIT_BYTES(10,0,0,1);
//  ppp_set_ipcp_ouraddr(g_ppp, &our_ip);
//  ppp_set_ipcp_hisaddr(g_ppp, &his_ip);

  pppapi_connect(g_ppp, 0);
}

char tmp[256];
/* Task: feed PPP with UART RX */
void ppp_feed_task(void *arg) {
	size_t n;
  for (;;) {
	  //n = CDC_recv_data(tmp, sizeof(tmp));
	  while((n = uart_recv_data(&usart1_driver, tmp, sizeof(tmp))) > 0){
		if ((n > 0) && (g_ppp != NULL) && (tcpip_ready != 0)){
			pppos_input_tcpip(g_ppp, tmp, (int)n);
		}
	  }
	osDelay(1);
  }
}

#include "tcp_echoserver.h"
#include "fp_cli_server.h"
void tcp_echo_socket_task(void *arg){
	int port = 5760;
    // Wait until TCP/IP stack is initialized
    while (!tcpip_ready) {
        osDelay(100);
    }
    //echo_netconn_server_thread(&port);
    tcp_socket_server_task(NULL);
	for(;;){
		osDelay(1000);
	}
}

/* UDP telemetry to PC:5762 @50 Hz */
void udp_telemetry_task(void *arg) {


  for (;;) {
    osDelay(20);
  }
}
