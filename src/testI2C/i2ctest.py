
import usb.core

class i2ctest:

    def __init__(self):
        self.READ_NUNCHUCK = 1
        self.TOGGLE_LED1 = 2
        self.TOGGLE_LED2 = 3
        self.TOGGLE_LED3 = 4
        self.READ_SW1 = 5
        self.READ_SW2 = 6
        self.READ_SW3 = 7
        self.ALT_READ_SW1 = 8
        self.dev = usb.core.find(idVendor = 0x6666, idProduct = 0x0003)
        if self.dev is None:
            raise ValueError('no USB device found matching idVendor = 0x6666 and idProduct = 0x0003')
        self.dev.set_configuration()

    def close(self):
        self.dev = None

    def nunchuck(self):
        ret = self.read_nunchuck()
        print 'Joystick x-axis = {0!s}'.format(ret[0])
        print 'Joystick y-axis = {0!s}'.format(ret[1])
        print 'Accel x-axis = {0!s}'.format((ret[2]<<2)|((ret[5]>>2)&0x03))
        print 'Accel y-axis = {0!s}'.format((ret[3]<<2)|((ret[5]>>4)&0x03))
        print 'Accel z-axis = {0!s}'.format((ret[4]<<2)|((ret[5]>>6)&0x03))
        print 'Z button = {0!s}'.format(ret[5]&0x01)
        print 'C button = {0!s}'.format((ret[5]>>1)&0x01)

    def read_nunchuck(self):
        try:
            ret = self.dev.ctrl_transfer(0xC0, self.READ_NUNCHUCK, 0, 0, 6)
        except usb.core.USBError:
            print "Could not send READ_NUNCHUCK vendor request."
        else:
            return [int(val) for val in ret]

    def toggle_led1(self):
        try:
            self.dev.ctrl_transfer(0x40, self.TOGGLE_LED1)
        except usb.core.USBError:
            print "Could not send TOGGLE_LED1 vendor request."

    def toggle_led2(self):
        try:
            self.dev.ctrl_transfer(0x40, self.TOGGLE_LED2)
        except usb.core.USBError:
            print "Could not send TOGGLE_LED2 vendor request."

    def toggle_led3(self):
        try:
            self.dev.ctrl_transfer(0x40, self.TOGGLE_LED3)
        except usb.core.USBError:
            print "Could not send TOGGLE_LED3 vendor request."

    def read_sw1(self):
        try:
            ret = self.dev.ctrl_transfer(0xC0, self.READ_SW1, 0, 0, 1)
        except usb.core.USBError:
            print "Could not send READ_SW1 vendor request."
        else:
            return int(ret[0])

    def read_sw2(self):
        try:
            ret = self.dev.ctrl_transfer(0xC0, self.READ_SW2, 0, 0, 1)
        except usb.core.USBError:
            print "Could not send READ_SW2 vendor request."
        else:
            return int(ret[0])

    def read_sw3(self):
        try:
            ret = self.dev.ctrl_transfer(0xC0, self.READ_SW3, 0, 0, 1)
        except usb.core.USBError:
            print "Could not send READ_SW3 vendor request."
        else:
            return int(ret[0])

    def alt_read_sw1(self):
        try:
            ret = self.dev.ctrl_transfer(0xC0, self.ALT_READ_SW1, 0, 0, 1)
        except usb.core.USBError:
            print "Could not send ALT_READ_SW1 vendor request."
        else:
            return int(ret[0])
