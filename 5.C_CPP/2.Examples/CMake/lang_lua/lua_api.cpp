#include "lua_api.h"

#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/gpio.h>

static int l_pinMode(lua_State* L)
{
    int n = lua_gettop(L);

    if (n == 2) {
        uint pin = luaL_checknumber(L, 1);
        uint dir = luaL_checknumber(L, 2);
        gpio_init(pin);
        gpio_set_dir(pin, dir);
    }

    return 0;
}

static int l_digitalWrite(lua_State* L)
{
    int n = lua_gettop(L);

    if (n == 2) {
        uint pin   = luaL_checknumber(L, 1);
        int  value = luaL_checknumber(L, 2);
        gpio_put(pin, value);
    }

    return 0;
}

static int l_delayUS(lua_State* L)
{
    int n = lua_gettop(L);

    if (n == 1) {
        uint32_t t = luaL_checknumber(L, 1);
        sleep_us(t);
    }

    return 0;
}

static int l_delayMS(lua_State* L)
{
    int n = lua_gettop(L);

    if (n == 1) {
        uint32_t t = luaL_checknumber(L, 1);
        sleep_ms(t);
    }

    return 0;
}

static luaL_Reg mylib[] = {
    {"pinMode",      l_pinMode     },
    {"digitalWrite", l_digitalWrite},
    {"delayUS",      l_delayUS     },
    {"delayMS",      l_delayMS     },
    {NULL,           NULL          }
};

const char lua_test[] = {
    "PIN_LED = 25\n"
    "GPIO_IN = 0\n"
    "GPIO_OUT = 1\n"
    "pinMode(PIN_LED,GPIO_OUT)\n"
    "while 1 do\n"
    " digitalWrite(PIN_LED,0)\n"
    " delayMS(500)\n"
    " digitalWrite(PIN_LED,1)\n"
    " delayMS(500)\n"
    "end"};

void lua_run(void)
{
    lua_State* L;
    L = luaL_newstate();
    if (L == NULL) {
        printf("ERR");
        while (1) {};
    }
    luaopen_base(L);
    luaL_setfuncs(L, mylib, 0);
    luaL_dostring(L, lua_test);
    lua_close(L);
}