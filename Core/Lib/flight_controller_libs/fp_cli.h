#ifndef FP_CLI_H
#define FP_CLI_H

#include "lwshell/lwshell.h"

#ifdef __cplusplus
extern "C" {
#endif

extern lwshell_t lwshell_cli;
extern volatile int start_telemetry;

void fp_cli_example_minimal_init(void);


#ifdef __cplusplus
}
#endif


#endif
