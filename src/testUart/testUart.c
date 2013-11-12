#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "timer.h"
#include "i2c.h"
#include "pin.h"
#include "uart.h"
#include <stdio.h>


int16_t main(void) {
    //initialize modules
    init_clock();
    init_timer();
    init_uart();

    timer_setPeriod(&timer1,1e-2);//100 Hz
    timer_lower(&timer1);
    timer_start(&timer1);

    printf("Begin");

    while (1) {
        if (timer_flag(&timer1)){
            printf("GO");
            timer_lower(&timer1);
        }
    }
}