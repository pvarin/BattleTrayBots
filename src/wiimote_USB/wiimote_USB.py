from matplotlib import pyplot
from time import sleep
import usb.core

class wiimoteVis:
	def __init__(self):
		# setup matplotlib
		pyplot.ion()
		self.fig = pyplot.figure()
		self.axis = self.fig.gca()

		# setup USB comms
		self.READ_WIIMOTE =1
		self.dev = usb.core.find(idVendor = 0x6666, idProduct = 0x0003)
		# if self.dev is None:
		# 	raise ValueError('no USB device found matching idVendor = 0x6666 and idProduct = 0x0003')
		# self.dev.set_configuration()

	def close(self):
		self.dev = None

	def updateData(self):
		# read data
		center = [(rand(),rand())];
		rad = [rand()];
		try:
			pass
			# ret = self.dev.ctrl_transfer(0xC0, self.READ_WIIMOTE, 0, 0, 6)
		except usb.core.USBError:
			print "Could not send READ_NUNCHUCK vendor request."
		# FIXME parse the data from ret

		# plot data
		self.fig.clf()
		for c,r in zip(center,rad):
			circ = pyplot.Circle(c,r,color='r')
			self.fig.gca().add_artist(circ)
		self.fig.canvas.draw()

	def run(self):
		while(True):
			try:
				self.updateData()
			except(KeyboardInterrupt):
				break
		self.close()

if __name__=="__main__":
	wiiCam = wiimoteVis()
	wiiCam.run()