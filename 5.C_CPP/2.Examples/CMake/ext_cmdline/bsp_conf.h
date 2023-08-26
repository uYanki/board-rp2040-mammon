#ifndef __BSP_CONF_H__
#define __BSP_CONF_H__

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"

#include "version.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VERSION_MAJOR 1
#define VERSION_MINOR 2

#define BEEP_PIN      2
#define LED_PIN       4

#ifdef __cplusplus
}
#endif

#endif  // __BSP_CONF_H__
