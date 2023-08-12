

#include <stdio.h>
#include <stdlib.h>

#include "bsp/board.h"
#include "pico/stdlib.h"
#include "hardware/flash.h"

#define LED_PIN             4

// We're going to erase and reprogram a region 256k from the start of flash.
// Once done, we can access this at XIP_BASE + 256k.
#define FLASH_TARGET_OFFSET (256 * 1024)

const uint8_t* flash_target_contents = (const uint8_t*)(XIP_BASE + FLASH_TARGET_OFFSET);

void print_buf(const uint8_t* buf, size_t len)
{
    for (size_t i = 0; i < len; ++i) {
        printf("%02x", buf[i]);
        if (i & 0xFF == 15) {
            printf("\n");
        } else {
            printf(" ");
        }
    }
}

int main()
{
    stdio_init_all();
    sleep_ms(3000);

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);

    uint8_t random_data[FLASH_PAGE_SIZE];
    for (int i = 0; i < FLASH_PAGE_SIZE; ++i) {
        random_data[i] = rand() >> 16;
    }

    printf("Generated random data:\n");
    print_buf(random_data, FLASH_PAGE_SIZE);

    uint32_t ints;

    // Note that a whole number of sectors must be erased at a time.
    printf("\nErasing target region...\n");
    {
        // must disable interrupts !!!
        ints = save_and_disable_interrupts();
        flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
        restore_interrupts(ints);
    }
    printf("Done. Read back target region:\n");
    print_buf(flash_target_contents, FLASH_PAGE_SIZE);

    printf("\nProgramming target region...\n");
    {
        ints = save_and_disable_interrupts();
        flash_range_program(FLASH_TARGET_OFFSET, random_data, FLASH_PAGE_SIZE);
        restore_interrupts(ints);
    }
    printf("Done. Read back target region:\n");
    print_buf(flash_target_contents, FLASH_PAGE_SIZE);

    bool mismatch = false;
    for (int i = 0; i < FLASH_PAGE_SIZE; ++i) {
        if (random_data[i] != flash_target_contents[i]) {
            mismatch = true;
            break;
        }
    }

    if (mismatch) {
        printf("\nProgramming failed!\n");
    } else {
        printf("\nProgramming successful!\n");
    }

    while (1) {
        gpio_put(LED_PIN, 0);  // on
        sleep_ms(1000);
        gpio_put(LED_PIN, 1);  // off
        sleep_ms(1000);
    }
}
