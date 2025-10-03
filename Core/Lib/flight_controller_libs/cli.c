#include "cli.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

/* ===================== USER PARAMETERS ====================== */
/* Example parameter backing variables (put them in a config module if you like) */
typedef struct {
    float max_angle_rad;       // radians
    float max_rate_rads;       // rad/s
    float pid_roll_kp, pid_roll_ki, pid_roll_kd;
    float pid_pitch_kp, pid_pitch_ki, pid_pitch_kd;
    float pid_yaw_kp, pid_yaw_ki, pid_yaw_kd;
    int32_t rc_mid;            // 1500 µs typical
    int32_t rc_deadband;       // µs
    bool   motor_reverse;      // example bool
    char   profile_name[32];   // example string
} app_params_t;

static app_params_t P;

/* Parameter table: add/edit to suit your stack */
param_desc_t g_params[] = {
    {"max_angle",   P_F32, &P.max_angle_rad,   0.0349f, 1.3963f, 0, "Max attitude angle [rad] (2..80 deg)"},
    {"max_rate",    P_F32, &P.max_rate_rads,   1.0f,   20.0f,    0, "Max body rate [rad/s] (≈60..1145 deg/s)"},
    {"roll.kp",     P_F32, &P.pid_roll_kp,     0.0f,   10.0f,    0, "Roll PID Kp"},
    {"roll.ki",     P_F32, &P.pid_roll_ki,     0.0f,   10.0f,    0, "Roll PID Ki"},
    {"roll.kd",     P_F32, &P.pid_roll_kd,     0.0f,   10.0f,    0, "Roll PID Kd"},
    {"pitch.kp",    P_F32, &P.pid_pitch_kp,    0.0f,   10.0f,    0, "Pitch PID Kp"},
    {"pitch.ki",    P_F32, &P.pid_pitch_ki,    0.0f,   10.0f,    0, "Pitch PID Ki"},
    {"pitch.kd",    P_F32, &P.pid_pitch_kd,    0.0f,   10.0f,    0, "Pitch PID Kd"},
    {"yaw.kp",      P_F32, &P.pid_yaw_kp,      0.0f,   10.0f,    0, "Yaw PID Kp"},
    {"yaw.ki",      P_F32, &P.pid_yaw_ki,      0.0f,   10.0f,    0, "Yaw PID Ki"},
    {"yaw.kd",      P_F32, &P.pid_yaw_kd,      0.0f,   10.0f,    0, "Yaw PID Kd"},
    {"rc.mid",      P_I32, &P.rc_mid,          800,    2200,     0, "RC mid [µs]"},
    {"rc.db",       P_I32, &P.rc_deadband,     0,      200,      0, "RC deadband [µs]"},
    {"motor.rev",   P_BOOL,&P.motor_reverse,   0,      1,        0, "Reverse motor direction logic"},
    {"profile",     P_STR,  P.profile_name,    0,      0,       sizeof(P.profile_name), "Profile name"},
};
const size_t g_params_count = sizeof(g_params)/sizeof(g_params[0]);

/* Defaults */
void params_load_defaults(void) {
    P.max_angle_rad   = 0.87266f; // 50 deg
    P.max_rate_rads   = 10.0f;    // ~572 deg/s
    P.pid_roll_kp = 0.12f; P.pid_roll_ki = 0.06f; P.pid_roll_kd = 0.002f;
    P.pid_pitch_kp= 0.12f; P.pid_pitch_ki= 0.06f; P.pid_pitch_kd= 0.002f;
    P.pid_yaw_kp  = 0.20f; P.pid_yaw_ki  = 0.10f; P.pid_yaw_kd  = 0.000f;
    P.rc_mid = 1500; P.rc_deadband = 6;
    P.motor_reverse = false;
    strncpy(P.profile_name, "default", sizeof(P.profile_name));
}

/* ===================== LOCKS (stub, wire to your RTOS) ====================== */
void params_lock(void)   {}
void params_unlock(void) {}

/* ===================== UTILS ====================== */
static inline float fclamp(float v, float a, float b){ return (v<a)?a:((v>b)?b:v); }
static inline int32_t iclamp(int32_t v, int32_t a, int32_t b){ return (v<a)?a:((v>b)?b:v); }

static param_desc_t* find_param(const char *name) {
    for (size_t i=0;i<g_params_count;i++){
        if (strcmp(g_params[i].name, name)==0) return &g_params[i];
    }
    return NULL;
}

bool params_set_f(const char *name, float v){
    param_desc_t *p = find_param(name);
    if (!p || p->type!=P_F32) return false;
    params_lock();
    *(float*)p->ptr = fclamp(v, p->f_min, p->f_max);
    params_unlock();
    return true;
}
bool params_set_i(const char *name, int32_t v){
    param_desc_t *p = find_param(name);
    if (!p || p->type!=P_I32) return false;
    params_lock();
    *(int32_t*)p->ptr = iclamp(v, (int32_t)p->f_min, (int32_t)p->f_max);
    params_unlock();
    return true;
}
bool params_set_b(const char *name, bool v){
    param_desc_t *p = find_param(name);
    if (!p || p->type!=P_BOOL) return false;
    params_lock();
    *(bool*)p->ptr = v;
    params_unlock();
    return true;
}
bool params_get_str(const char *name, char *out, size_t cap){
    param_desc_t *p = find_param(name);
    if (!p || p->type!=P_STR || !out || cap==0) return false;
    params_lock();
    strncpy(out, (const char*)p->ptr, cap);
    out[cap-1]='\0';
    params_unlock();
    return true;
}

/* ===================== PERSISTENCE (simple CRC32 blob) ====================== */
/* If you want to wire to STM32F405 internal flash, implement the 3 weak functions.
   By default, we just return false so `save`/`load` will say "not supported". */

__attribute__((weak)) bool storage_save_blob(const void *data, size_t len, uint32_t crc32) { (void)data;(void)len;(void)crc32; return false; }
__attribute__((weak)) bool storage_load_blob(void *data, size_t len, uint32_t *out_crc32){ (void)data;(void)len;(void)out_crc32; return false; }

static uint32_t crc32(const void *data, size_t len){
    const uint8_t *p = (const uint8_t*)data;
    uint32_t crc = 0xFFFFFFFFu;
    for (size_t i=0;i<len;i++){
        crc ^= p[i];
        for (int b=0;b<8;b++){
            uint32_t m = -(crc & 1u);
            crc = (crc >> 1) ^ (0xEDB88320u & m);
        }
    }
    return ~crc;
}

typedef struct __attribute__((packed)) {
    uint32_t magic;   // 'DPAR'
    uint32_t version; // bump on struct changes
    app_params_t payload;
    uint32_t crc32;   // over magic|version|payload
} persist_image_t;

#define PERSIST_MAGIC 0x52504144u /* 'DPAR' */
#define PERSIST_VERSION 1u

bool params_save_to_flash(void){
    persist_image_t img = {0};
    img.magic = PERSIST_MAGIC;
    img.version = PERSIST_VERSION;
    img.payload = P;
    img.crc32 = crc32(&img, sizeof(img)-sizeof(img.crc32));
    return storage_save_blob(&img, sizeof(img), img.crc32);
}
bool params_load_from_flash(void){
    persist_image_t img = {0};
    uint32_t got_crc = 0;
    if (!storage_load_blob(&img, sizeof(img), &got_crc)) return false;
    if (img.magic!=PERSIST_MAGIC || img.version!=PERSIST_VERSION) return false;
    uint32_t exp = crc32(&img, sizeof(img)-sizeof(img.crc32));
    if (exp!=img.crc32) return false;
    P = img.payload;
    return true;
}

/* ===================== CLI CORE ====================== */
#define CLI_MAX_LINE 256
#define CLI_MAX_TOK  8

static char linebuf[CLI_MAX_LINE];
static size_t linelen = 0;

static int tokenize(char *s, char *argv[], int maxv){
    int argc=0;
    while (*s && argc<maxv){
        while (isspace((unsigned char)*s)) s++;
        if (!*s) break;
        if (*s=='"'){ // quoted
            s++;
            argv[argc++] = s;
            while (*s && *s!='"') s++;
            if (*s=='"') *s++ = 0;
        } else {
            argv[argc++] = s;
            while (*s && !isspace((unsigned char)*s)) s++;
            if (*s) *s++ = 0;
        }
    }
    return argc;
}

static void print_param(const param_desc_t *d){
    switch (d->type){
        case P_BOOL: cli_writef("%-12s = %s\t# %s\n", d->name, (*(bool*)d->ptr)?"true":"false", d->help); break;
        case P_I32:  cli_writef("%-12s = %ld\t# [%ld..%ld] %s\n", d->name, (long)*(int32_t*)d->ptr, (long)(int32_t)d->f_min, (long)(int32_t)d->f_max, d->help); break;
        case P_F32:  cli_writef("%-12s = %.6f\t# [%.3f..%.3f] %s\n", d->name, *(float*)d->ptr, d->f_min, d->f_max, d->help); break;
        case P_STR:  cli_writef("%-12s = \"%s\"\t# cap=%u %s\n", d->name, (char*)d->ptr, d->str_cap, d->help); break;
    }
}

/* Commands */
static void cmd_help(int argc, char **argv);
static void cmd_list(int argc, char **argv);
static void cmd_get(int argc, char **argv);
static void cmd_set(int argc, char **argv);
static void cmd_dump(int argc, char **argv);
static void cmd_defaults(int argc, char **argv);
static void cmd_save(int argc, char **argv);
static void cmd_load(int argc, char **argv);
static void cmd_reboot(int argc, char **argv);
static void cmd_info(int argc, char **argv);

typedef struct { const char *name; void(*fn)(int,char**); const char *help; } cmd_t;
static const cmd_t CMDS[] = {
    {"help",     cmd_help,     "Show commands or help <cmd>"},
    {"list",     cmd_list,     "List parameters (optionally filter prefix)"},
    {"get",      cmd_get,      "get <name>"},
    {"set",      cmd_set,      "set <name> <value>"},
    {"dump",     cmd_dump,     "Dump all parameters as JSON"},
    {"defaults", cmd_defaults, "Restore factory defaults (not saved yet)"},
    {"save",     cmd_save,     "Save parameters to flash (if enabled)"},
    {"load",     cmd_load,     "Load parameters from flash (if enabled)"},
    {"reboot",   cmd_reboot,   "Reboot MCU (unsafe if motors armed!)"},
    {"info",     cmd_info,     "Firmware/target info"},
};

static void prompt(void){ cli_writef("> "); }

void cli_init(void){
    params_load_defaults();  // start from defaults; you may call cli_try_autoload()
    linelen = 0;
    prompt();
}

void cli_try_autoload(void){
    if (params_load_from_flash()){
        cli_writef("[CLI] params loaded from flash.\n");
    } else {
        cli_writef("[CLI] no saved params or CRC mismatch; using defaults.\n");
    }
}

static void exec_line(char *line){
    char *argv[CLI_MAX_TOK] = {0};
    int argc = tokenize(line, argv, CLI_MAX_TOK);
    if (argc==0) return;
    for (size_t i=0;i<sizeof(CMDS)/sizeof(CMDS[0]);i++){
        if (strcmp(argv[0], CMDS[i].name)==0){
            CMDS[i].fn(argc, argv);
            return;
        }
    }
    cli_writef("Unknown command '%s'. Try 'help'.\n", argv[0]);
}

void cli_feed(const uint8_t *bytes, size_t len){
    for (size_t i=0;i<len;i++){
        uint8_t c = bytes[i];
        if (c=='\r') continue;
        if (c=='\n'){
            linebuf[linelen] = 0;
            exec_line(linebuf);
            linelen = 0;
            prompt();
            continue;
        }
        if (c==0x7F || c==0x08){ // backspace
            if (linelen>0) linelen--;
            continue;
        }
        if (linelen < (CLI_MAX_LINE-1)){
            linebuf[linelen++] = (char)c;
        }
    }
}

/* ===================== COMMAND IMPL ====================== */
static void cmd_help(int argc, char **argv){
    if (argc==1){
        cli_writef("Commands:\n");
        for (size_t i=0;i<sizeof(CMDS)/sizeof(CMDS[0]);i++)
            cli_writef("  %-8s - %s\n", CMDS[i].name, CMDS[i].help);
        cli_writef("Tips: use quotes for strings. Example: set profile \"race_1\"\n");
        return;
    }
    // per-command (simple)
    for (size_t i=0;i<sizeof(CMDS)/sizeof(CMDS[0]);i++)
        if (strcmp(argv[1], CMDS[i].name)==0){
            cli_writef("%s: %s\n", CMDS[i].name, CMDS[i].help);
            return;
        }
    cli_writef("No help for '%s'\n", argv[1]);
}

static void cmd_list(int argc, char **argv){
    const char *pref = (argc>=2)? argv[1] : NULL;
    for (size_t i=0;i<g_params_count;i++){
        if (pref && strncmp(g_params[i].name, pref, strlen(pref))!=0) continue;
        print_param(&g_params[i]);
    }
}

static void cmd_get(int argc, char **argv){
    if (argc<2){ cli_writef("Usage: get <name>\n"); return; }
    param_desc_t *d = find_param(argv[1]);
    if (!d){ cli_writef("Param '%s' not found.\n", argv[1]); return; }
    print_param(d);
}

static void cmd_set(int argc, char **argv){
    if (argc<3){ cli_writef("Usage: set <name> <value>\n"); return; }
    param_desc_t *d = find_param(argv[1]);
    if (!d){ cli_writef("Param '%s' not found.\n", argv[1]); return; }

    params_lock();
    switch (d->type){
        case P_BOOL:{
            int v = (!strcmp(argv[2],"1")||!strcmp(argv[2],"true")||!strcmp(argv[2],"on"));
            *(bool*)d->ptr = v?true:false;
        } break;
        case P_I32:{
            long v = strtol(argv[2], NULL, 0);
            *(int32_t*)d->ptr = iclamp((int32_t)v, (int32_t)d->f_min, (int32_t)d->f_max);
        } break;
        case P_F32:{
            float v = strtof(argv[2], NULL);
            *(float*)d->ptr = fclamp(v, d->f_min, d->f_max);
        } break;
        case P_STR:{
            size_t cap = d->str_cap ? d->str_cap : 1;
            strncpy((char*)d->ptr, argv[2], cap);
            ((char*)d->ptr)[cap-1] = '\0';
        } break;
    }
    params_unlock();
    print_param(d);
}

static void cmd_dump(int argc, char **argv){
    (void)argc; (void)argv;
    cli_writef("{\n");
    for (size_t i=0;i<g_params_count;i++){
        const param_desc_t *d = &g_params[i];
        switch (d->type){
            case P_BOOL: cli_writef("  \"%s\": %s", d->name, (*(bool*)d->ptr)?"true":"false"); break;
            case P_I32:  cli_writef("  \"%s\": %ld", d->name, (long)*(int32_t*)d->ptr); break;
            case P_F32:  cli_writef("  \"%s\": %.6f", d->name, *(float*)d->ptr); break;
            case P_STR:  cli_writef("  \"%s\": \"%s\"", d->name, (char*)d->ptr); break;
        }
        if (i+1<g_params_count) cli_writef(",\n"); else cli_writef("\n");
    }
    cli_writef("}\n");
}

static void cmd_defaults(int argc, char **argv){
    (void)argc;(void)argv;
    params_load_defaults();
    cli_writef("Defaults loaded (not saved yet). Use 'save' to persist.\n");
}

static void cmd_save(int argc, char **argv){
    (void)argc;(void)argv;
    if (params_save_to_flash()) cli_writef("Saved to flash.\n");
    else cli_writef("Save not supported (no storage backend wired).\n");
}

static void cmd_load(int argc, char **argv){
    (void)argc;(void)argv;
    if (params_load_from_flash()) cli_writef("Loaded from flash.\n");
    else cli_writef("Load failed or not supported.\n");
}

/* weak reboot hook; override with NVIC_SystemReset if desired */
__attribute__((weak)) void cli_do_reboot(void) {}

static void cmd_reboot(int argc, char **argv){
    (void)argc;(void)argv;
    cli_writef("Rebooting...\n");
    cli_do_reboot();
}

static void cmd_info(int argc, char **argv){
    (void)argc;(void)argv;
    cli_writef("FW: %s  Target: %s\n", FW_VERSION, FW_TARGET);
    cli_writef("Params: %u  LineMax: %u  JSON: yes  CRC: yes\n",
               (unsigned)g_params_count, (unsigned)CLI_MAX_LINE);
}
