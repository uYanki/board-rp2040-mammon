
#include <stdio.h>
#include <stdlib.h>

#include <hardware/irq.h>
#include <hardware/structs/scb.h>
#include <hardware/uart.h>
#include <hardware/watchdog.h>
#include <pico/bootrom.h>
#include <pico/stdlib.h>

#include "bsp/board.h"

#include "tusb.h"

#include "para_table.h"
#include "para_group.h"

#include "mbmst.h"
#include "mbslv.h"

static para_tbl_t m_paratbl;

static mbslv_context_t m_mbslv_ctx;
static mbmst_context_t m_mbmst_ctx;
static uint16_t        system_error = 0;

static void mbmst_tx(uint8_t* data, size_t size)
{
    gpio_put(MB_DE_PIN, 1);

    uart_write_blocking(MB_UART, data, size);

    // Wait until fifo is drained so we now when to turn off the driver enable pin.
    uart_tx_wait_blocking(MB_UART);
    gpio_put(MB_DE_PIN, 0);
}

static void on_mb_rx(void)  // Interrupt
{
    mbmst_rx(&m_mbmst_ctx, uart_getc(MB_UART));
}

static uint32_t mb_get_tick_ms(void)
{
    return time_us_64() / 1000;
}

static void mbslv_tx(uint8_t* data, size_t size)
{
    if (tud_cdc_n_connected(USB_ITF_MB)) {
        tud_cdc_n_write(USB_ITF_MB, data, size);
        tud_cdc_n_write_flush(USB_ITF_MB);
    }
}

void tud_cdc_rx_cb(uint8_t i)  // Interrupt
{
    char c;
    if (USB_ITF_MB == i) {
        while (tud_cdc_n_available(i)) {
            if (tud_cdc_n_read(i, &c, i)) {
                mbslv_rx(&m_mbslv_ctx, c);
            }
        }
    }
}

static void mbmst_status(uint8_t address, uint8_t function, uint8_t error_code)
{
    (void)address;
    (void)function;
    system_error |= error_code & 0xFF;
}

static void sys_reset(void)
{
    scb_hw->aircr |= M0PLUS_AIRCR_SYSRESETREQ_BITS;
    for (;;) {}
}

static mb_result_e mbslv_write_single_holding_register(uint16_t reg, uint16_t value)
{
    switch (reg) {
        case 0 ... sizeof(paragrp_t) - 1:
            *(((uint16_t*)(&g_paragrp)) + reg) = value;
            return MB_NO_ERROR;
        default: {
            return MB_ERROR_ILLEGAL_DATA_ADDRESS;
        }
    }
}

static mb_result_e mbslv_write_holding_registers(uint16_t start, uint16_t* data, uint16_t count)
{
    mb_result_e res;

    for (int i = 0; i < count; i++) {
        res = mbslv_write_single_holding_register(start + i, data[i]);
        if (res != MB_NO_ERROR) {
            return res;
        }
    }

    return MB_NO_ERROR;
}

static mb_result_e mbslv_read_single_holding_register(uint16_t reg, uint16_t* value)
{
    switch (reg) {
        case 0 ... sizeof(paragrp_t) - 1:
            *value = *(((uint16_t*)(&g_paragrp)) + reg);
            return MB_NO_ERROR;
        default:
            return MB_ERROR_ILLEGAL_DATA_ADDRESS;
    }
}

static mb_result_e mbslv_read_holding_registers(uint16_t start, uint16_t count)
{
    uint16_t val;
    for (int i = 0; i < count; i++) {
        if (mbslv_read_single_holding_register(start + i, &val) == MB_NO_ERROR) {
            mbslv_add_response(&m_mbslv_ctx, val);
        } else {
            return MB_ERROR_ILLEGAL_DATA_ADDRESS;
        }
    }
    return MB_NO_ERROR;
}

static void mbmst_tx_request(uint8_t* data, size_t size)
{
    mbmst_send_raw(&m_mbmst_ctx, data, size);
}

void pid_task(void)
{
#if CONFIG_PID_MODULE

    static PID_t pid = {
        .Kp    = &(g_paragrp.PID.Kp),
        .Ki    = &(g_paragrp.PID.Ki),
        .Kd    = &(g_paragrp.PID.Kd),
        .ref   = &(g_paragrp.PID.ref),
        .fbk   = &(g_paragrp.PID.fbk),
        .ramp  = &(g_paragrp.PID.ramp),
        .lower = &(g_paragrp.PID.lower),
        .upper = &(g_paragrp.PID.upper),
        .Ts    = 1,
        .cb    = &PID_Handler_Basic,
    };

    static absolute_time_t tmr;

    if (absolute_time_diff_us(tmr, get_absolute_time()) > 0) {
        tmr               = make_timeout_time_ms(g_paragrp.PID.Ts * 1000);
        g_paragrp.PID.fbk = g_paragrp.PID.out;
        g_paragrp.PID.out = pid.cb(&pid);  // PID process
    }

#endif
}

void led_task(void)
{
#if CONFIG_LED_MODULE

    static bool            on = false;
    static absolute_time_t tmr;

    if (g_paragrp.LED.period) {
        if (absolute_time_diff_us(tmr, get_absolute_time()) > 0) {
            tmr = make_timeout_time_ms(g_paragrp.LED.period);
            (on = !on) ? board_led_on() : board_led_off();
            printf("led tgl\n");
        }
    }

#endif
}

void cmdword_task()
{
#if CONFIG_USING_CMDWORD
    if (g_paragrp.cmdword.paratbl_reload) {
        paratbl_load(&m_paratbl);
    }
    if (g_paragrp.cmdword.paratbl_save) {
        paratbl_save(&m_paratbl);
    }
    if (g_paragrp.cmdword.paratbl_reset) {
        paragrp_reset();
    }
    // sys rst at last
    if (g_paragrp.cmdword.system_reset) {
        sys_reset();
    }
    g_paragrp.cmdword.value = 0;  // clear
#endif
}

static void setup_uarts(void)
{
    gpio_init(MB_DE_PIN);
    gpio_set_dir(MB_DE_PIN, GPIO_OUT);
    gpio_put(MB_DE_PIN, 0);

    uart_init(MB_UART, MB_UART_BAUD);

    gpio_set_function(MB_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(MB_RX_PIN, GPIO_FUNC_UART);

    gpio_pull_down(MB_RX_PIN);

    uart_set_fifo_enabled(MB_UART, false);

    irq_set_exclusive_handler(MB_UART_IRQ, on_mb_rx);
    irq_set_enabled(MB_UART_IRQ, true);

    uart_set_irq_enables(MB_UART, true, false);
}

int main(void)
{
    board_init();
    tud_init(TUD_OPT_RHPORT);
    stdio_usb_init();
    // stdio_init_all();

    // gpio_init(LED_PIN);
    // gpio_set_dir(LED_PIN, GPIO_OUT);

    watchdog_enable(100, 1);

    setup_uarts();

    paratbl_config(&m_paratbl, &g_paragrp, 0, offsetof(paragrp_t, cmdword));
    paratbl_load(&m_paratbl);

    if (g_paragrp.salveID == 0) {
        paragrp_reset();
    }

    mbslv_cb_t mbslv = {
        .get_tick_ms              = mb_get_tick_ms,
        .write_single_register    = mbslv_write_single_holding_register,
        .read_holding_registers   = mbslv_read_holding_registers,
        .write_multiple_registers = mbslv_write_holding_registers,
        .tx                       = mbslv_tx,
        .rx                       = mbmst_tx_request,
    };
    mbslv_init(&m_mbslv_ctx, g_paragrp.salveID, &mbslv);

    mbmst_cb_t mbmst = {
        .get_tick_ms = mb_get_tick_ms,
        .status      = mbmst_status,
        .tx          = mbmst_tx,
        .rx          = mbslv_tx,
    };
    mbmst_init(&m_mbmst_ctx, &mbmst);

    for (;;) {
        // virtual serial port (usb)
        tud_task();

        // modbus
        mbslv_task(&m_mbslv_ctx);
        // mbmst_task(&m_mbmst_ctx);

        // add tasks at here
        led_task();
        pid_task();
        cmdword_task();

        watchdog_update();
    }
}
