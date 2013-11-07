#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "timer.h"
#include "i2c.h"
#include <stdio.h>

#define True 1
#define Fale 0

/***********************************
**** Additional Timer Functions ****
***********************************/

/* wait - takes a reference to a timer and a time to wait and stalls the program until the time has passed
 * inputs:
 *      _TIMER* timer:  a pointer to the timer to control the wait time
 *      float waitTime: the time to wait in seconds
 */
void wait(_TIMER* timer, float waitTime){
    timer_setPeriod(timer, waitTime);
    timer_lower(timer);
    timer_start(timer);
    while(!timer_flag(timer)){}
    timer_stop(timer);
}

/*********************************
**** Additional I2C Functions ****
*********************************/

/* readI2C - reads a string of data from the I2C line blindly and confirms reciept each byte
 * inputs:
 *      _I2C self*:     a pointer to the _I2C object to recieve data from
 *      uint8_t data*:  a pointer to the preallocated memory
 *      uint16_t len:   the number of bytes to recieve
 */
void readI2C(_I2C *self, uint8_t *data, uint16_t len){
    uint16_t i;
    for (i=0; i<len; i++){
        data[0] = i2c_getc(self);
        i2c_ack(self);  //acknowledge reciept
        i2c_idle(self); //wait until the line goes quiet
    }
}

/* writeI2C - writes a string of data to the I2C line and confirms transmission
 * inputs:
 *      _I2C* self:     a pointer to the _I2C object to send data
 *      uint8_t* data:  the byte array to send
 *      uint16_t len:   the number of bytes to send
 * output:
 *      returns 0 upon failure and 1 upon success
 */
uint8_t writeI2C(_I2C* self, uint8_t* data, uint16_t len){
    i2c_start(self);
    uint16_t i;
    for (i=0; i<len; i++){        
        if (i2c_putc(self, data[i]))
            return 0;
        i2c_idle(self);
    }
    i2c_stop(self);
    i2c_idle(self);
    return 1;
}

/**************************
**** Wiimote Functions ****
**************************/

typedef struct{
    uint16_t x, y;
    uint8_t  size;
} WiiData_t;

struct Wiimote_t{
    _I2C* i2c;          // a pointer to the i2c object
    uint8_t addr;       // address of the Wiimote on the I2C line
    WiiData_t data[4];    // data read from the Wiimote
    _TIMER* timer;      // timer used to time the communication
} Wiimote;

/* initWiimote - initializes communication with the IR Wiimote camera on i2c1 and timing management on timer5
 *  precondition: i2c_init() has been previously called
 *      Note that timer5 should not be relied on if communication with the Wiimote is in progress
 */
void initWiimote(){
    // initialize the Wiimote struct with default values
    Wiimote.i2c = &i2c1;
    Wiimote.data;
    Wiimote.addr = 0xB0;
    Wiimote.timer = &timer5;

    //open the comms and wait to be initialized
    i2c_open(Wiimote.i2c, 400e3);
    i2c_idle(Wiimote.i2c);
}

/* startWiimote - sends the start data (sensitivity and mode) to the Wiimote
 *  precondition: timer_init() has previously been called
 */
void startWiimote(){
    //start the timer
    timer_setPeriod(Wiimote.timer, 0.1);
    timer_start(Wiimote.timer);
    uint8_t cmd1 [3] = {Wiimote.addr, 0x30, 0x01};
    uint8_t cmd2 [9] = {Wiimote.addr, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90};
    uint8_t cmd3 [4] = {Wiimote.addr, 0x07, 0x00, 0x41};
    uint8_t cmd4 [4] = {Wiimote.addr, 0x1A, 0x40, 0x00};
    uint8_t cmd5 [3] = {Wiimote.addr, 0x33, 0x03};
    uint8_t cmd6 [3] = {Wiimote.addr, 0x30, 0x08};

    //begin
    writeI2C(Wiimote.i2c, cmd1, 3);
    while(!timer_flag(Wiimote.timer)){};//wait 100ms
    timer_lower(Wiimote.timer);
    //sensitivity part1
    writeI2C(Wiimote.i2c, cmd2, 9);
    while(!timer_flag(Wiimote.timer)){};//wait 100ms
    timer_lower(Wiimote.timer);
    //sensitivity part2
    writeI2C(Wiimote.i2c, cmd3, 4);
    while(!timer_flag(Wiimote.timer)){};//wait 100ms
    timer_lower(Wiimote.timer);
    //sensitivity part3
    writeI2C(Wiimote.i2c, cmd4, 4);
    while(!timer_flag(Wiimote.timer)){};//wait 100ms
    timer_lower(Wiimote.timer);
    //mode
    writeI2C(Wiimote.i2c, cmd5, 3);
    while(!timer_flag(Wiimote.timer)){};//wait 100ms
    timer_lower(Wiimote.timer);
    //something else
    writeI2C(Wiimote.i2c, cmd6, 3);
    while(!timer_flag(Wiimote.timer)){};//wait 100ms
    timer_lower(Wiimote.timer);
    timer_stop(Wiimote.timer);
}

/* extractWiimoteData - takes raw data from the Wiimote and formats it into a WiiData_t struct
 * inputs:
 *      *WiiData_t:     a pointer to the WiiData to modify
 *      *data:          the 3-byte array of data containing x(10-bits), y(10-bits), size(4-bits)
 */
void extractWiimoteData(WiiData_t *self, uint8_t *data){
    uint16_t x = data[0] + ((uint16_t)data[2] << 2) & 0b11 << 8;
    uint16_t y = data[1] + ((uint16_t)data[2] << 4) & 0b11 << 8;
    uint8_t size = data[2] & 0xF;
    self->x = x;
    self->y = y;
    self->size = size;
}

/* updateWiimote - sends read request, manages comms, and parses the returned data
 *  precondition: startWiimote() has previously been called and timer5 is not currently being used
 */
void updateWiimote(){
    //TODO setup timer to delay at 25us and 380us
    uint8_t cmd1[2] = {Wiimote.addr, 0x37};
    uint8_t cmd2[1] = {0xB1};
    uint8_t readBuf[8];

    //prepare to read
    writeI2C(Wiimote.i2c, cmd1, 2);
    //wait 25us
    wait(Wiimote.timer,25e-6);
    //read request
    writeI2C(Wiimote.i2c, cmd2, 1);
    readI2C(Wiimote.i2c, readBuf, 8);
    extractWiimoteData(Wiimote.data,readBuf);
    extractWiimoteData(Wiimote.data,&(readBuf[4]));
    //wait 380us
    wait(Wiimote.timer,380e-6);
    //read more data
    writeI2C(Wiimote.i2c, cmd2, 1);
    readI2C(Wiimote.i2c, readBuf, 4);
    extractWiimoteData(Wiimote.data,readBuf);
}

int16_t main(void) {
    //initialize modules
    init_clock();
    init_timer();

    //setup the i2c line at 9600 baud
    initWiimote();
    startWiimote();

    timer_setPeriod(&timer1,1e-2);//100 Hz
    timer_lower(&timer1);
    timer_start(&timer1);

    while (True) {
        if (timer_flag(&timer1)){
            updateWiimote();
            timer_lower(&timer1);
        }
    }
}