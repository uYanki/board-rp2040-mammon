#include "keyfifo.h"

#define HARD_KEY_NUM 4                  /* 实体按键个数 */
#define KEY_COUNT    (HARD_KEY_NUM + 0) /* 3个独立建 + 2个组合按键 */

#define pin(n)       (1 << (n))

typedef struct {
    uint8_t pin;
    uint8_t lvl;  // active level
} key_t;

void key_add(key_t key[], uint8_t cnt)
{
    while (cnt) {
    }
}

const struct {
    uint8_t  id;
    uint32_t pins;
    uint32_t level;  // active level

    key_state_e cur_state;
} g_key_list[] = {
    {.id = 1, .pins = bv(1), .level = 0b00},
    {.id = 2, .pins = bv(1), .level = 0b00},
    {.id = 3, .pins = bv(1), .level = 0b00},
    {.id = 4, .pins = bv(1), .level = 0b01},
};

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(*arr))

#define KEY_CNT         ARRAY_SIZE(g_key_list)

static KEY_FIFO_T s_key_fifo;
static KEY_T      s_tBtn[KEY_COUNT] = {0};

void key_put(uint8_t _KeyCode)
{
    s_key_fifo.buff[s_key_fifo.Write] = _KeyCode;
    if (++s_key_fifo.Write >= KEY_FIFO_SIZE) {
        s_key_fifo.Write = 0;
    }
}

uint8_t key_get(void)
{
    uint8_t ret;

    if (s_key_fifo.Read == s_key_fifo.Write) {
        return KEY_NONE;
    } else {
        ret = s_key_fifo.buff[s_key_fifo.Read];
        if (++s_key_fifo.Read >= KEY_FIFO_SIZE) {
            s_key_fifo.Read = 0;
        }
        return ret;
    }
}

static void InitKeyVar(void)
{
    uint8_t i;

    /* 对按键FIFO读写指针清零 */
    s_key_fifo.Read  = 0;
    s_key_fifo.Write = 0;

    /* 给每个按键结构体成员变量赋一组缺省值 */
    for (i = 0; i < KEY_COUNT; i++) {
        s_tBtn[i].LongTime    = KEY_LONG_TIME;       /* 长按时间 0 表示不检测长按键事件 */
        s_tBtn[i].Count       = KEY_FILTER_TIME / 2; /* 计数器设置为滤波时间的一半 */
        s_tBtn[i].State       = 0;                   /* 按键缺省状态，0为未按下 */
        s_tBtn[i].RepeatSpeed = 0;                   /* 按键连发的速度，0表示不支持连发 */
        s_tBtn[i].RepeatCount = 0;                   /* 连发计数器 */
    }

    //	/* 支持长按1秒后，自动连发 */
    //	SetKeyParam(KID_JOY_U, 100, 6);
}

void InitKey(void)
{
    InitKeyVar(); /* 初始化按键变量 */
}

#include <hardware/gpio.h>

/**
 * @brief
 *
 * @param id
 * @return true:  key press
 * @return false: key release
 */
static uint8_t key_is_active(uint8_t id)
{
    return gpio_get_all() == g_key_list[id].level;
}

static uint8_t IsKeyDownFunc(uint8_t _id)
{
}

static void DetectKey(uint8_t i)
{
    KEY_T* pBtn;  //

    pBtn = &s_tBtn[i];
    if (IsKeyDownFunc(i)) {
        if (pBtn->Count < KEY_FILTER_TIME) {
            pBtn->Count = KEY_FILTER_TIME;
        } else if (pBtn->Count < 2 * KEY_FILTER_TIME) {
            pBtn->Count++;
        } else {
            if (pBtn->State == 0) {
                pBtn->State = 1;

                /* 发送按钮按下的消息 */
                key_put((uint8_t)(3 * i + 1));
            }

            if (pBtn->LongTime > 0) {
                if (pBtn->LongCount < pBtn->LongTime) {
                    /* 发送按钮持续按下的消息 */
                    if (++pBtn->LongCount == pBtn->LongTime) {
                        /* 键值放入按键FIFO */
                        key_put((uint8_t)(3 * i + 3));
                    }
                } else {
                    if (pBtn->RepeatSpeed > 0) {
                        if (++pBtn->RepeatCount >= pBtn->RepeatSpeed) {
                            pBtn->RepeatCount = 0;
                            /* 常按键后，每隔10ms发送1个按键 */
                            key_put((uint8_t)(3 * i + 1));
                        }
                    }
                }
            }
        }
    } else {
        if (pBtn->Count > KEY_FILTER_TIME) {
            pBtn->Count = KEY_FILTER_TIME;
        } else if (pBtn->Count != 0) {
            pBtn->Count--;
        } else {
            if (pBtn->State == 1) {
                pBtn->State = 0;

                /* 发送按钮弹起的消息 */
                key_put((uint8_t)(3 * i + 2));
            }
        }

        pBtn->LongCount   = 0;
        pBtn->RepeatCount = 0;
    }
}

void key_scan(void)
{
    uint8_t i;

    for (i = 0; i < KEY_COUNT; i++) {
        DetectKey(i);
    }
}

void key_handler()
{
    switch (GetKey()) {
        case KEY0_DOWN:
            printf("0");
            break;
        case KEY_NONE:
        default:
            break;
    }
}
