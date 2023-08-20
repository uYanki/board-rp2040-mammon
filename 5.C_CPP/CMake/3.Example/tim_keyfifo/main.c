

#include <stdio.h>
#include "pico/stdlib.h"

bool repeating_timer_callback(struct repeating_timer* t)
{
    printf("Repeat at %lld\n", time_us_64());
    return true;
}

int main()
{
    stdio_init_all();
    printf("Hello Timer!\n");

    // sleep_ms(3000);

    struct repeating_timer timer;
    add_repeating_timer_ms(500, repeating_timer_callback, NULL, &timer);
    add_repeating_timer_ms(-500, repeating_timer_callback, NULL, &timer);

    // negative period: timer start a new period after callback ending
    // positive period: timer start a new period before callback beginning

    return 0;
}
/// \end::timer_example[]
