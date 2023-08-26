#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"

const struct {
    uint8_t  id;
    uint32_t pins;
    uint32_t active;
} g_key_list[] = {
    {1, 0},
    {2, 0},
    {3, 0},
    {4, 1},
};

int main()
{
    printf("%d", g_key_list[0].pins[1]);
}