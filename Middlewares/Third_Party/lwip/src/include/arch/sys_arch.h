#ifndef LWIP_ARCH_SYS_ARCH_H
#define LWIP_ARCH_SYS_ARCH_H

#include "lwip/opt.h"
#include <stdint.h>

/* ---------------------- Common ---------------------- */

/* Ticks: lwIP expects sys_now() to return ms (implement in sys_arch.c). */
typedef uint32_t sys_time_t;

/* lwIP also uses sys_prot_t for lightweight protection.
 * We provided it in cc.h; if not, define it here. */
#ifndef SYS_ARCH_PROTECT
/* If you want, you can leave protection to cc.h via SYS_ARCH_{PROTECT, UNPROTECT} macros. */
#endif

/* ---------------------- NO_SYS = 1 (bare-metal) ---------------------- */
#if NO_SYS

#include "atomic_custom.h"
/* No OS: lwIP won’t use these, but types must exist. */
typedef uint8_t  sys_sem_t;
typedef uint8_t  sys_mutex_t;
typedef uint8_t  sys_mbox_t;
typedef uint8_t  sys_thread_t;

#define SYS_MBOX_NULL   ((sys_mbox_t)0)
#define SYS_SEM_NULL    ((sys_sem_t)0)

#define SYS_ARCH_DECL_PROTECT(lev)   crit_state_t lev
/* Map protection to global IRQ disable/enable (implement in cc.h or here). */
#ifndef SYS_ARCH_PROTECT
#define SYS_ARCH_PROTECT(lev)        ((lev) = enter_critical())
#define SYS_ARCH_UNPROTECT(lev)      (exit_critical(lev))
#endif

/* Default stack/prio hints (unused in NO_SYS, but some code references them) */
#define SYS_DEFAULT_THREAD_STACKSIZE   1024
#define SYS_DEFAULT_THREAD_PRIO        3

/* Mailbox and sem APIs are not used when NO_SYS=1.
 * Provide small stubs in sys_arch.c if something links against them. */

#else /* ---------------------- NO_SYS = 0 (FreeRTOS) ---------------------- */

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Map lwIP objects to FreeRTOS */
typedef SemaphoreHandle_t   sys_sem_t;    /* binary sem (or counting) */
typedef SemaphoreHandle_t   sys_mutex_t;  /* mutex */
typedef QueueHandle_t       sys_mbox_t;   /* message box (queue of void*) */
typedef TaskHandle_t        sys_thread_t;

#define SYS_MBOX_NULL   ((sys_mbox_t)NULL)
#define SYS_SEM_NULL    ((sys_sem_t)NULL)

/* Protection: use FreeRTOS critical sections */
#define SYS_ARCH_DECL_PROTECT(lev)   UBaseType_t lev
#define SYS_ARCH_PROTECT(lev)        do { (lev) = taskENTER_CRITICAL_FROM_ISR(); } while (0)
#define SYS_ARCH_UNPROTECT(lev)      do { taskEXIT_CRITICAL_FROM_ISR(lev); } while (0)

/* Thread defaults (used by netconn/sockets if you let lwIP spawn threads) */
#ifndef SYS_DEFAULT_THREAD_STACKSIZE
#define SYS_DEFAULT_THREAD_STACKSIZE   (1024)
#endif
#ifndef SYS_DEFAULT_THREAD_PRIO
#define SYS_DEFAULT_THREAD_PRIO        (tskIDLE_PRIORITY + 2)
#endif

/* Mailbox sizing hint (how many pending ptrs) */
#ifndef SYS_MBOX_SIZE
#define SYS_MBOX_SIZE   8
#endif

/* Timeouts: number of simultaneously active timeouts (per-thread or core) */
#ifndef MEMP_NUM_SYS_TIMEOUT
#define MEMP_NUM_SYS_TIMEOUT  (LWIP_SOCKET ? 8 : 4)
#endif

#endif /* NO_SYS */

/* Optional: tell lwIP we have fast timeouts from a single core */
#ifndef LWIP_ARCH_DECL_PROTECT
/* Not required; cc.h usually handles lightweight protection. */
#endif

#endif /* LWIP_ARCH_SYS_ARCH_H */
