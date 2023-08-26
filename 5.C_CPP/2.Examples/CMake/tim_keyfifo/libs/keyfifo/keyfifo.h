#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define KEY_FIFO_SIZE 10

typedef struct
{
    uint8_t buff[KEY_FIFO_SIZE]; /* 键值缓冲区 */
    uint8_t Read;                /* 缓冲区读指针 */
    uint8_t Write;               /* 缓冲区写指针 */
} KEY_FIFO_T;

// 按下,弹起,长按
#define KEY_STATE_DECLARE(n)

#define KEY_STATE_COUNT 3

typedef enum {
    KEY_EVENT_PRESS = 0,
    KEY_EVENT_RELEASE,
    KEY_EVENT_LONG_PRESS,
} key_event_e;

typedef enum {
    KEY_NONE = 0,
    // declare key event
    KEY_STATE_DECLARE(0),
    KEY_STATE_DECLARE(1),
    KEY_STATE_DECLARE(2),
} key_state_e;

typedef enum {
    KID_K0 = 0,
    KID_K1,
    KID_K2,
} key_event_e;

/*
    按键滤波时间50ms, 单位10ms。
    只有连续检测到50ms状态不变才认为有效，包括弹起和按下两种事件
    即使按键电路不做硬件滤波，该滤波机制也可以保证可靠地检测到按键事件
*/
#define KEY_FILTER_TIME 5
#define KEY_LONG_TIME   100 /* 单位10ms， 持续1秒，认为长按事件 */

/*
    每个按键对应1个全局的结构体变量。
*/
typedef struct
{
    uint8_t  Count;       /* 滤波器计数器 */
    uint16_t LongCount;   /* 长按计数器 */
    uint16_t LongTime;    /* 按键按下持续时间, 0表示不检测长按 */
    uint8_t  State;       /* 按键当前状态（按下还是弹起） */
    uint8_t  RepeatSpeed; /* 连续按键周期 */
    uint8_t  RepeatCount; /* 连续按键计数器 */
} KEY_T;

// 函数声明
void    InitKey(void);
void    KeyScan10ms(void);
void    PutKey(uint8_t _KeyCode);
uint8_t GetKey(void);
uint8_t GetKey2(void);
uint8_t GetKeyState(KEY_ID_E _ucKeyID);
void    SetKeyParam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t _RepeatSpeed);
void    ClearKey(void);
void    handler(void);

#ifdef __cplusplus
}
#endif
