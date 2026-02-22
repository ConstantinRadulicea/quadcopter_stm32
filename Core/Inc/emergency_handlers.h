#ifndef EMERGENCY_HANDLERS_H
#define EMERGENCY_HANDLERS_H

// emergency_handlers.h

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Put the emergency_shutdown function inside the fault handlers in the following file
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
 */

void emergency_shutdown(void);


#ifdef __cplusplus
}
#endif

#endif
