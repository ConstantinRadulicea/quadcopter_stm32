#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

/* ===== System integration ===== */
#define NO_SYS                         0           /* using FreeRTOS */
#define SYS_LIGHTWEIGHT_PROT           1
#define LWIP_TIMERS                    1
#define LWIP_TCPIP_CORE_LOCKING        0           /* default */
#define LWIP_COMPAT_MUTEX              0

/* ===== Core memory/buffers (tuned for PPP MRU≈296) ===== */
#define MEM_ALIGNMENT                  4
#define MEM_SIZE                       (16 * 1024)

#define PBUF_POOL_SIZE                 8
#define PBUF_POOL_BUFSIZE              512         /* ≥ MTU-ish */

#define MEMP_NUM_PBUF                  8
#define MEMP_NUM_TCP_PCB               4
#define MEMP_NUM_UDP_PCB               4
#define MEMP_NUM_TCP_SEG               16

/* ===== Protocol enables ===== */
#define LWIP_TCP                       1
#define LWIP_UDP                       1
#define LWIP_ICMP                      1
#define LWIP_DNS                       0           /* typically not needed over PPP point-to-point */
#define LWIP_RAW                       1

/* ===== PPP over serial ===== */
#define PPP_SUPPORT                    1
#define PPPOS_SUPPORT                  1
#define PPP_IPV6_SUPPORT               0
#define PAP_SUPPORT                    0
#define CHAP_SUPPORT                   0
#define EAP_SUPPORT                    0
#define PPP_NOTIFY_PHASE               1

/* ===== TCP tuning for small MRU =====
   PPP default MRU ~296 ⇒ MSS ≤ MRU - 40 for IPv4/TCP header */
#define TCP_MSS                        216
#define TCP_WND                        (4 * TCP_MSS)
#define TCP_SND_BUF                    (4 * TCP_MSS)
#define TCP_SND_QUEUELEN               (2 * (TCP_SND_BUF / TCP_MSS))
#define TCP_QUEUE_OOSEQ                0
#define LWIP_TCP_KEEPALIVE             1
#define LWIP_WND_SCALE                 0          /* keep simple over PPP */
#define TCP_RCV_SCALE                  0

/* ===== UDP ===== */
#define LWIP_UDP_RECVMBOX_SIZE         8

/* ===== API layers ===== */
#define LWIP_NETCONN                   1           /* use Netconn API with FreeRTOS */
#define LWIP_SOCKET                    1           /* optional: BSD sockets */
#define LWIP_NETCONN_FULLDUPLEX        1
#define LWIP_NETIF_LOOPBACK            0
#define LWIP_NETCONN_SEM_PER_THREAD	   1

/* ===== Timeouts/threads (only used when NO_SYS=0) ===== */
#define TCPIP_MBOX_SIZE                8
#define DEFAULT_THREAD_STACKSIZE       1024
#define DEFAULT_THREAD_PRIO            3
#define LWIP_TCPIP_THREAD_STACKSIZE    1024
#define LWIP_TCPIP_THREAD_PRIO         3
#define MEMP_NUM_SYS_TIMEOUT           8

/* ===== Checksums ===== */
#define CHECKSUM_GEN_IP                1
#define CHECKSUM_GEN_UDP               1
#define CHECKSUM_GEN_TCP               1
#define CHECKSUM_CHECK_IP              1
#define CHECKSUM_CHECK_UDP             1
#define CHECKSUM_CHECK_TCP             1
#define LWIP_CHECKSUM_ON_COPY          1

/* ===== Diagnostics (set to 1 to enable) ===== */
#define LWIP_DEBUG                     0
/* #define PPP_DEBUG                   LWIP_DBG_ON */
/* #define PPPOS_DEBUG                 LWIP_DBG_ON */
/* #define TCP_DEBUG                   LWIP_DBG_ON */
/* #define UDP_DEBUG                   LWIP_DBG_ON */

/* ===== Misc ===== */
#define LWIP_NETIF_HOSTNAME            1
//#define LWIP_TIMEVAL_PRIVATE           1           /* keep lwIP’s timeval */
//#define LWIP_ERRNO_STDINCLUDE          1

#endif /* __LWIPOPTS_H__ */
