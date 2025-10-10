#include "fp_cli.h"
#include "usbd_cdc_if.h"
#include <string.h>
#include "lwshell/lwshell.h"


lwshell_t lwshell_cli;

/* Command to get called */
int32_t set_cmd_fn(int32_t argc, char** argv) {
	usb_printf("\r\nNumber of argv: %d\r\n", (int)argc);
    for (int32_t i = 0; i < argc; ++i) {
    	usb_printf("ARG[%d]: %s\r\n", (int)i, argv[i]);
    }

    /* Successful execution */
    return 0;
}

typedef void (*lwshell_output_fn)(const char* str, struct lwshell* lwobj);

void output_fn(const char* str, struct lwshell* lwobj){
	usb_printf("%s", str);
}

/* Example code */
void fp_cli_example_minimal_init(void) {
    //const char* input_str = "mycmd param1 \"param 2 with space\"\r\n";

    /* Init library */
    //lwshell_init();
    lwshell_init_ex(&lwshell_cli);

    lwshellr_t lwshell_set_output_fn_ex(lwshell_t* lwobj, lwshell_output_fn out_fn);
    lwshell_set_output_fn_ex(&lwshell_cli, output_fn);

    /* Define shell commands */
    //lwshell_register_cmd("mycmd", mycmd_fn, "Adds 2 integer numbers and prints them");
    lwshell_register_cmd_ex(&lwshell_cli, "set", set_cmd_fn, "Sets a parameter");

    /* User input to process every character */

    /* Now insert input */
    //lwshell_input(input_str, strlen(input_str));
}
