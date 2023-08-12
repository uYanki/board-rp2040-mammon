#pragma once

#include "config.h"
#if CONFIG_PID_MODULE
#include "pid.h"
#endif

///< @note mbpoll -> foarmat > 32-bit float > little-endian byte swap

typedef struct {
    // modbus slave id
    u16 salveID;

#if CONFIG_LED_MODULE
    // blink
    struct {
        // toggle period
        u16 period;
    } LED;
#endif

#if CONFIG_PID_MODULE
    // pid
    struct {
        f32 Kp;
        f32 Ki;
        f32 Kd;
        f32 ref;
        f32 fbk;
        f32 ramp;
        f32 lower;
        f32 upper;
        f32 out;
        f32 Ts;  // unit: second
    } PID;
#endif

    // command word, 命令字 (unsaved)
    union {
        uint16_t value : 16;
        struct {
            // system
            bool system_reset : 1;
            // params table
            bool paratbl_reset  : 1;
            bool paratbl_save   : 1;
            bool paratbl_reload : 1;
        };
    } cmdword;

} paragrp_t;

extern paragrp_t g_paragrp;

void paragrp_reset(void);
