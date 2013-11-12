#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "usb.h"
#include "ui.h"
#include "pin.h"
#include "i2c.h"

#define READ_NUNCHUCK       1
#define TOGGLE_LED1         2
#define TOGGLE_LED2         3
#define TOGGLE_LED3         4
#define READ_SW1            5
#define READ_SW2            6
#define READ_SW3            7
#define ALT_READ_SW1        8

//void ClassRequests(void) {
//    switch (USB_setup.bRequest) {
//        default:
//            USB_error_flags |= 0x01;                    // set Request Error Flag
//    }
//}

void VendorRequests(void) {
    uint16_t i;

    switch (USB_setup.bRequest) {
        case READ_NUNCHUCK:
            i2c_start(&i2c1);
            i2c_putc(&i2c1, 0xA4);
            i2c_idle(&i2c1);
            i2c_putc(&i2c1, 0);
            i2c_idle(&i2c1);
            i2c_stop(&i2c1);
            for (i = 600; i; i--) {}
            i2c_idle(&i2c1);
            i2c_start(&i2c1);
            i2c_putc(&i2c1, 0xA5);
            i2c_idle(&i2c1);
            for (i = 0; i<6; i++) {
                BD[EP0IN].address[i] = (i2c_getc(&i2c1)^0x17)+0x17;
                if (i<5)
                    i2c_ack(&i2c1);
                else
                    i2c_nak(&i2c1);
                i2c_idle(&i2c1);
            }
            i2c_stop(&i2c1);
            i2c_idle(&i2c1);
            BD[EP0IN].bytecount = 6;    // set EP0 IN byte count to 6
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        case TOGGLE_LED1:
            led_toggle(&led1);
            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        case TOGGLE_LED2:
            led_toggle(&led2);
            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        case TOGGLE_LED3:
            led_toggle(&led3);
            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        case READ_SW1:
            BD[EP0IN].address[0] = sw_read(&sw1);
            BD[EP0IN].bytecount = 1;    // set EP0 IN byte count to 1
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        case READ_SW2:
            BD[EP0IN].address[0] = sw_read(&sw2);
            BD[EP0IN].bytecount = 1;    // set EP0 IN byte count to 1
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        case READ_SW3:
            BD[EP0IN].address[0] = sw_read(&sw3);
            BD[EP0IN].bytecount = 1;    // set EP0 IN byte count to 1
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        case ALT_READ_SW1:
            BD[EP0IN].address[0] = sw_read(&sw1);
            BD[EP0IN].bytecount = 1;    // set EP0 IN byte count to 1
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

int16_t main(void) {
    init_clock();
    init_ui();
    init_pin();
    init_i2c();

    i2c_open(&i2c1, 100e3);
    i2c_idle(&i2c1);

    i2c_start(&i2c1);
    i2c_putc(&i2c1, 0xA4);
    i2c_idle(&i2c1);
    i2c_putc(&i2c1, 0x40);
    i2c_idle(&i2c1);
    i2c_putc(&i2c1, 0);
    i2c_idle(&i2c1);
    i2c_stop(&i2c1);
    i2c_idle(&i2c1);

    InitUSB();                              // initialize the USB registers and serial interface engine
    while (USB_USWSTAT!=CONFIG_STATE) {     // while the peripheral is not configured...
        ServiceUSB();                       // ...service USB requests
    }
    while (1) {
        ServiceUSB();                       // service any pending USB requests
    }
}

