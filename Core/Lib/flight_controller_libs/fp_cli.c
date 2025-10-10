#include "fp_cli.h"

#include <string.h>
#include "lwshell/lwshell.h"


lwshell_t lwshell_cli;

/* Command to get called */
int32_t mycmd_fn(int32_t argc, char** argv) {
    printf("mycmd_fn called. Number of argv: %d\r\n", (int)argc);
    for (int32_t i = 0; i < argc; ++i) {
        printf("ARG[%d]: %s\r\n", (int)i, argv[i]);
    }

    /* Successful execution */
    return 0;
}

/* Example code */
void fp_cli_example_minimal_init(void) {
    const char* input_str = "mycmd param1 \"param 2 with space\"\r\n";

    /* Init library */
    //lwshell_init();
    lwshell_init_ex(&lwshell_cli);

    /* Define shell commands */
    //lwshell_register_cmd("mycmd", mycmd_fn, "Adds 2 integer numbers and prints them");
    lwshell_register_cmd_ex(&lwshell_cli, "mycmd", mycmd_fn, "Adds 2 integer numbers and prints them");

    /* User input to process every character */

    /* Now insert input */
    //lwshell_input(input_str, strlen(input_str));
}
