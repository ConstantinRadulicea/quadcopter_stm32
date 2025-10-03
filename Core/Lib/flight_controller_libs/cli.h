#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* ===== Transport glue =====
   You feed raw RX bytes into cli_feed().
   Implement cli_writef() to transmit text back (UART/USB-CDC). */

int cli_writef(const char *fmt, ...);  // YOU implement (printf->UART)

/* ===== Parameter system ===== */
typedef enum {
    P_BOOL, P_I32, P_F32, P_STR
} param_type_t;

typedef struct {
    const char *name;
    param_type_t type;
    void *ptr;             // points to your variable in RAM
    float f_min, f_max;    // used for P_F32 and P_I32 (casted)
    uint16_t str_cap;      // capacity for P_STR buffer (including '\0')
    const char *help;
} param_desc_t;

/* Register your parameters here (extern from .c) */
extern param_desc_t g_params[];
extern const size_t g_params_count;

/* Thread-safety hooks (optional but recommended) */
void params_lock(void);
void params_unlock(void);

/* Defaults */
void params_load_defaults(void);

/* Persistence (optional; noop if you don't call flash init/save/load) */
bool params_save_to_flash(void);
bool params_load_from_flash(void);

/* ===== CLI ===== */
void cli_init(void);
void cli_feed(const uint8_t *bytes, size_t len);

/* Call once at boot if you want auto-load */
void cli_try_autoload(void);

/* ===== Helper: clamp/set by name programmatically ===== */
bool params_set_f(const char *name, float v);
bool params_set_i(const char *name, int32_t v);
bool params_set_b(const char *name, bool v);
bool params_get_str(const char *name, char *out, size_t out_cap);

/* Build info (printed by `info`) — set these at link or compile time */
#ifndef FW_VERSION
#define FW_VERSION "v0.1.0"
#endif
#ifndef FW_TARGET
#define FW_TARGET  "STM32F405"
#endif
