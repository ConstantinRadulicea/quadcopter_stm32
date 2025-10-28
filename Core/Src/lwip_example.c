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


static ppp_pcb*     g_ppp = NULL;
static struct netif g_pppif;
static volatile uint8_t tcpip_ready = 0;

/* Called by tcpip_init() in the tcpip_thread context when ready */
static void tcpip_init_done(void *arg) {
  (void)arg;
  tcpip_ready = 1;
}

/* TX callback for PPP: write bytes to UART */
static u32_t ppp_output_cb(ppp_pcb *pcb, const void *data, u32_t len, void *ctx) {
  (void)pcb;
  (void)ctx;
  CDC_Transmit_FS((uint8_t*)data, len);   // blocking is fine at 115200
  return len;
}

static void ppp_status_cb(ppp_pcb* pcb, int err, void* ctx) {
  (void)ctx;
  if (err == PPPERR_NONE) {
    printf("PPP UP: local=%s remote=%s\r\n",
           ipaddr_ntoa(netif_ip_addr4(&g_pppif)),
           ipaddr_ntoa(netif_ip_gw4(&g_pppif)));
  } else {
    printf("PPP status: %d\r\n", err);
  }
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
  ppp_set_default(g_ppp);
  ppp_set_auth(g_ppp, PPPAUTHTYPE_NONE, NULL, NULL);   // dev/no auth

  /* Static IPCP pair: PC 10.0.0.1 <-> MCU 10.0.0.2 */
  ip4_addr_t our_ip = IPADDR4_INIT_BYTES(10,0,0,2);
  ip4_addr_t his_ip = IPADDR4_INIT_BYTES(10,0,0,1);
  ppp_set_ipcp_ouraddr(g_ppp, &our_ip);
  ppp_set_ipcp_hisaddr(g_ppp, &his_ip);

  pppapi_connect(g_ppp, 0);
}

char tmp[256];
/* Task: feed PPP with UART RX */
void ppp_feed_task(void *arg) {
  //net_ppp_start();
  for (;;) {
    size_t n = CDC_recv_data(tmp, sizeof(tmp));
    printf("%.*s", n, tmp);
    if (n && g_ppp && tcpip_ready) pppos_input_tcpip(g_ppp, tmp, (int)n);
    osDelay(1);
  }
}

#include "tcp_echoserver.h"
void tcp_echo_socket_task(void *arg){
    // Wait until TCP/IP stack is initialized
    while (!tcpip_ready) {
        osDelay(100);
    }
    tcp_echoserver_init(5760);
	for(;;){
		osDelay(1000);
	}
}

/* UDP telemetry to PC:5762 @50 Hz */
void udp_telemetry_task(void *arg) {
  int u = socket(AF_INET, SOCK_DGRAM, 0);
  struct sockaddr_in pc = {0};
  pc.sin_family = AF_INET; pc.sin_port = PP_HTONS(5762);
  pc.sin_addr.s_addr = ipaddr_addr("10.0.0.1");
  uint32_t seq=0;
  struct __attribute__((packed)) { uint32_t seq, t_ms; float vbat; } pkt;

  for (;;) {
    pkt.seq=seq++; pkt.t_ms=HAL_GetTick(); pkt.vbat=12.3f;
    sendto(u, &pkt, sizeof(pkt), 0, (struct sockaddr*)&pc, sizeof(pc));
    osDelay(20);
  }
}
