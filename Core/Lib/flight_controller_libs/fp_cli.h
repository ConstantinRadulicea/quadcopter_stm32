#ifndef FP_CLI_H
#define FP_CLI_H

#include "lwshell/lwshell.h"
#include "fp_cli_server.h"

#ifdef __cplusplus
extern "C" {
#endif

//#define OUT_PRINTF(...) usb_printf(__VA_ARGS__)
#define OUT_PRINTF(...) fprintfsock(g_fp_cli_server_socket, __VA_ARGS__)

extern lwshell_t lwshell_cli;
extern volatile int start_telemetry;

void fp_cli_example_minimal_init(void);


#ifdef __cplusplus
}
#endif


#endif
