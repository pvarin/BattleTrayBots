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
#define READ_WIIMOTE 1

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

    InitUSB();
    while (USB_USWSTAT!=CONFIG_STATE) {
        ServiceUSB();
    }
    while (True) {
        ServiceUSB();
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
        if (i==7)
            i2c_nak(&i2c1);
        else
            i2c_ack(&i2c1);
        i2c_idle(&i2c1);
    }

    i2c_stop(&i2c1);
    i2c_idle(&i2c1);
    

    uint16_t x, y, size;
    for (i=0;i<4;i++){
        x = buf[i*3];
        y = buf[i*3+1];
        x += (buf[i*3+2] << 2) & 0x300;
        y += (buf[i*3+2] << 4) & 0xC00;
        size = buf[i*3+2] & 0xF;

        printf("\t[%i] (%i, %i) %i\n",i,x,y,size);
    
    }

    //TODO wait
    timer_setPeriod(&timer5, 380e-6); timer_start(&timer5);
    while(!timer_flag(&timer5)){};
    timer_lower(&timer5); timer_stop(&timer5);

    i2c_start(&i2c1);
    i2c_putc(&i2c1, 0xB1); i2c_idle(&i2c1);
    for (i=8; i<12; i++){
        buf[i] = i2c_getc(&i2c1);
        if (i==11)
            i2c_nak(&i2c1);
        else
            i2c_ack(&i2c1);
        i2c_idle(&i2c1);
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
    i2c_putc(&i2c1, 0x33); i2c_idle(&i2c1);
    i2c_putc(&i2c1, 0x03); i2c_idle(&i2c1);
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

void VendorRequests(void) {
    uint16_t i;

    switch (USB_setup.bRequest) {
        case READ_WIIMOTE:
            read_wiimote();
            for (i=0; i<6; i++)
                BD[EP0IN].address[i] = 0x00;
            BD[EP0IN].bytecount = 6;    // set EP0 IN byte count to 6
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        default:
            USB_error_flags |= 0x01;    // set Request Error Flag
    }
}

void VendorRequestsIn(void) {
    switch (USB_request.setup.bRequest) {
        default:
            USB_error_flags |= 0x01;                    // set Request Error Flag
    }
}

void VendorRequestsOut(void) {
    switch (USB_request.setup.bRequest) {
        default:
            USB_error_flags |= 0x01;                    // set Request Error Flag
    }
}