#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/err.h"
#include "arch/sys_arch.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

u32_t sys_now(void) {
    return (u32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
}

/* ---------- Semaphores ---------- */
err_t sys_sem_new(sys_sem_t *sem, u8_t count) {
    *sem = xSemaphoreCreateBinary();
    if (*sem == NULL) return ERR_MEM;
    if (count) xSemaphoreGive(*sem);
    return ERR_OK;
}
void sys_sem_signal(sys_sem_t *sem) { xSemaphoreGive(*sem); }

u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout) {
    if (timeout == 0) timeout = portMAX_DELAY;
    TickType_t start = xTaskGetTickCount();
    if (xSemaphoreTake(*sem, timeout / portTICK_PERIOD_MS) == pdTRUE)
        return (xTaskGetTickCount() - start) * portTICK_PERIOD_MS;
    else
        return SYS_ARCH_TIMEOUT;
}
void sys_sem_free(sys_sem_t *sem) { vSemaphoreDelete(*sem); }

/* ---------- Mutexes ---------- */
err_t sys_mutex_new(sys_mutex_t *mutex) {
    *mutex = xSemaphoreCreateMutex();
    return (*mutex != NULL) ? ERR_OK : ERR_MEM;
}
void sys_mutex_lock(sys_mutex_t *mutex) { xSemaphoreTake(*mutex, portMAX_DELAY); }
void sys_mutex_unlock(sys_mutex_t *mutex) { xSemaphoreGive(*mutex); }
void sys_mutex_free(sys_mutex_t *mutex) { vSemaphoreDelete(*mutex); }

/* ---------- Mailboxes (message queues) ---------- */
err_t sys_mbox_new(sys_mbox_t *mbox, int size) {
    *mbox = xQueueCreate(size, sizeof(void *));
    return (*mbox != NULL) ? ERR_OK : ERR_MEM;
}
void sys_mbox_post(sys_mbox_t *mbox, void *msg) {
    while (xQueueSend(*mbox, &msg, portMAX_DELAY) != pdTRUE);
}
err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg) {
    return (xQueueSend(*mbox, &msg, 0) == pdTRUE) ? ERR_OK : ERR_MEM;
}
u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout) {
    void *temp;
    if (!msg) msg = &temp;
    TickType_t start = xTaskGetTickCount();
    if (xQueueReceive(*mbox, msg, timeout / portTICK_PERIOD_MS) == pdTRUE)
        return (xTaskGetTickCount() - start) * portTICK_PERIOD_MS;
    return SYS_ARCH_TIMEOUT;
}
u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg) {
    void *temp;
    if (!msg) msg = &temp;
    return (xQueueReceive(*mbox, msg, 0) == pdTRUE) ? 0 : SYS_MBOX_EMPTY;
}
void sys_mbox_free(sys_mbox_t *mbox) { vQueueDelete(*mbox); }

/* ---------- Threads ---------- */
sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread,
                            void *arg, int stacksize, int prio) {
    TaskHandle_t h;
    if (xTaskCreate(thread, name, stacksize, arg, prio, &h) == pdPASS)
        return h;
    else
        return NULL;
}
