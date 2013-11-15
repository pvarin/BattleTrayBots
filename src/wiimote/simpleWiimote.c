#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "usb.h"
#include "ui.h"
#include "pin.h"
#include "i2c.h"
#include "timer.h"
#include "stdio.h"


#define True 1
void init_wiimote(void);
void read_wiimote(void);

int16_t main(void) {
    //initialize modules
    init_clock();
    init_pin();
    init_timer();
    init_uart();
    init_i2c();

    //initialize the wiimote
    init_wiimote();
    
    //setup the main loop to execute at 100Hz
    timer_setPeriod(&timer1,1e-2);
    timer_lower(&timer1);
    timer_start(&timer1);

    while (True) {
        if (timer_flag(&timer1)){
            //read from the wiimote camera
            read_wiimote();
            timer_lower(&timer1);
        }
    }
}

void read_wiimote(void){
    uint8_t i;
    uint8_t buf[12];
    
    i2c_start(&i2c1);
    i2c_putc(&i2c1, 0xB0); i2c_idle(&i2c1);
    i2c_putc(&i2c1, 0x37); i2c_idle(&i2c1);
    i2c_stop(&i2c1);

    //TODO wait
    timer_setPeriod(&timer5, 25e-6); timer_start(&timer5);
    while(!timer_flag(&timer5)){};
    timer_lower(&timer5); timer_stop(&timer5);

    printf("Collecting Data\n");
    i2c_start(&i2c1);
    i2c_putc(&i2c1, 0xB1); i2c_idle(&i2c1);
    for (i=0; i<8; i++){
        buf[i] = i2c_getc(&i2c1);
        i2c_ack(&i2c1); i2c_idle(&i2c1);
        printf("\tData: %i\n",buf[i]);
    }
    i2c_stop(&i2c1);
    i2c_idle(&i2c1);

    //TODO wait
    timer_setPeriod(&timer5, 380e-6); timer_start(&timer5);
    while(!timer_flag(&timer5)){};
    timer_lower(&timer5); timer_stop(&timer5);

    i2c_start(&i2c1);
    i2c_putc(&i2c1, 0xB1); i2c_idle(&i2c1);
    for (i=8; i<12; i++){
        buf[i] = i2c_getc(&i2c1);
        i2c_ack(&i2c1); i2c_idle(&i2c1);
        printf("\tData: %i\n",buf[i]);
    }
    i2c_stop(&i2c1);
    i2c_idle(&i2c1);    
}

void init_wiimote(void){

    //initialize the wiimote
    printf("Initializing the WiiMote camera\n");
    i2c_open(&i2c1, 400e3);
    i2c_idle(&i2c1);

    i2c_start(&i2c1);
    i2c_putc(&i2c1, 0xB0); i2c_idle(&i2c1);
    i2c_putc(&i2c1, 0x30); i2c_idle(&i2c1);
    i2c_putc(&i2c1, 0x01); i2c_idle(&i2c1);
    i2c_stop(&i2c1);
    
    //wait 100ms
    timer_setPeriod(&timer5, 0.1); timer_start(&timer5);
    while(!timer_flag(&timer5)){};
    timer_lower(&timer5); timer_stop(&timer5);

    i2c_start(&i2c1);
    i2c_putc(&i2c1, 0xB0); i2c_idle(&i2c1);
    i2c_putc(&i2c1, 0x00); i2c_idle(&i2c1);
    i2c_putc(&i2c1, 0x00); i2c_idle(&i2c1);
    i2c_putc(&i2c1, 0x00); i2c_idle(&i2c1);
    i2c_putc(&i2c1, 0x00); i2c_idle(&i2c1);
    i2c_putc(&i2c1, 0x00); i2c_idle(&i2c1);
    i2c_putc(&i2c1, 0x00); i2c_idle(&i2c1);
    i2c_putc(&i2c1, 0x00); i2c_idle(&i2c1);
    i2c_putc(&i2c1, 0x90); i2c_idle(&i2c1);
    i2c_stop(&i2c1);
    
    //wait 100ms
    timer_setPeriod(&timer5, 0.1); timer_start(&timer5);
    while(!timer_flag(&timer5)){};
    timer_lower(&timer5); timer_stop(&timer5);

    i2c_start(&i2c1);
    i2c_putc(&i2c1, 0xB0); i2c_idle(&i2c1);
    i2c_putc(&i2c1, 0x07); i2c_idle(&i2c1);
    i2c_putc(&i2c1, 0x00); i2c_idle(&i2c1);
    i2c_putc(&i2c1, 0x41); i2c_idle(&i2c1);
    i2c_stop(&i2c1);
    
    //wait 100ms
    timer_setPeriod(&timer5, 0.1); timer_start(&timer5);
    while(!timer_flag(&timer5)){};
    timer_lower(&timer5); timer_stop(&timer5);

    i2c_start(&i2c1);
    i2c_putc(&i2c1, 0xB0); i2c_idle(&i2c1);
    i2c_putc(&i2c1, 0x1A); i2c_idle(&i2c1);
    i2c_putc(&i2c1, 0x40); i2c_idle(&i2c1);
    i2c_putc(&i2c1, 0x00); i2c_idle(&i2c1);
    i2c_stop(&i2c1);
    
    //wait 100ms
    timer_setPeriod(&timer5, 0.1); timer_start(&timer5);
    while(!timer_flag(&timer5)){};
    timer_lower(&timer5); timer_stop(&timer5);

    i2c_start(&i2c1);
    i2c_putc(&i2c1, 0xB0); i2c_idle(&i2c1);
    i2c_putc(&i2c1, 0x30); i2c_idle(&i2c1);
    i2c_putc(&i2c1, 0x08); i2c_idle(&i2c1);
    i2c_stop(&i2c1);
    
    printf("Successfully initialized the WiiMote camera\n");
}