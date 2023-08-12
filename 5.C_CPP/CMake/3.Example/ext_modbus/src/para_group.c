#include "para_group.h"

paragrp_t g_paragrp;

// param table default value
void paragrp_reset(void)
{
    g_paragrp.salveID = 10;

#if CONFIG_LED_MODULE
    g_paragrp.LED.period = 100;
#endif

#if CONFIG_PID_MODULE
    g_paragrp.PID.Kp    = 0.2f;
    g_paragrp.PID.Ki    = 0.1f;
    g_paragrp.PID.Kd    = 0.0f;
    g_paragrp.PID.ref   = 0.0f;
    g_paragrp.PID.fbk   = 0.0f;
    g_paragrp.PID.ramp  = 0.0f;
    g_paragrp.PID.lower = FLT_MIN;
    g_paragrp.PID.upper = FLT_MAX;
    g_paragrp.PID.out   = 0.0f;
    g_paragrp.PID.Ts    = 0.1f;  // 100ms
#endif

    g_paragrp.cmdword.value = 0;  // 命令字
}
