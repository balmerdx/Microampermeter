#!/usr/bin/env python3

import time
import serial
import sys
import struct
import xml.etree.ElementTree as ET

USB_COMMAND_ADC_BUFFER_START = 0x6BDE0001
USB_COMMAND_ADC_BUFFER_GET = 0x6BDE0002
USB_COMMAND_ADC_BUFFER_STATUS = 0x6BDE0003
USB_COMMAND_NANOAMPERS = 0x6BDE0004
USB_COMMAND_ST_INFO = 0x6BDE0005
USB_COMMAND_ST_DATA = 0x6BDE0006


ser = None

def connect():
	global ser
	ser = serial.Serial(
		port='/dev/ttyACM0',
		baudrate=500000,
		parity=serial.PARITY_NONE,
		stopbits=serial.STOPBITS_ONE,
		bytesize=serial.EIGHTBITS,
		timeout = 0.2
	)

	return ser.isOpen()

def close():
	ser.close()

def StartSampling():
	ser.write(struct.pack("I",USB_COMMAND_ADC_BUFFER_START))
	time.sleep(0.1)
	data = ser.read_all()
	if len(data)!=4:
		print("StartSampling timeout")
		return False
	result = struct.unpack("I", data)[0]
	print("res=", result)
	if result==0x12345:
		print("Started succefully")
	else:
		print("StartSampling unexpected error")
		return False


	while True:
		ser.write(struct.pack("I",USB_COMMAND_ADC_BUFFER_STATUS))
		time.sleep(0.1)
		data = ser.read_all()
		if len(data)!=4:
			print("StartSampling status timeout")
			return False

		if(len(data)==4):
			value = struct.unpack("I", data)[0]
			print("status=", hex(value))
			if value:
				return True

		elif(len(data)>256):
			print("len(data)=", len(data))
		else:
			print("value=", data)
		time.sleep(0.1)
	return False


def ReceiveData(filename, command, all_size = 0x8000*4):
	ser.write(struct.pack("I", command))
	print("Data query.")
	f = open(filename, "wb")
	sum_size = 0
	is_empty = False
	while True:
		data = ser.read_all()
		if len(data)==0:
			print(".", end='')
			sys.stdout.flush()
			time.sleep(1)
			continue

		if is_empty:
			print("")
		is_empty = False
		print("len(data)=", len(data))
		f.write(data)
		sum_size += len(data)
		if sum_size>=all_size:
			break
		time.sleep(0.01)
	f.close()
	print("Data received complete. size=", sum_size)
	return


class STInfo:
	def __init__(self, status, tilter_sps, trigger_offset, samples):
		self.status = status
		self.tilter_sps = tilter_sps
		self.trigger_offset = trigger_offset
		self.samples = samples

def ReceiveSTInfo():
	ser.write(struct.pack("I", USB_COMMAND_ST_INFO))
	time.sleep(0.1)
	data = ser.read_all()
	if len(data)!=16:
		print("ReceiveSTInfo bad data size=", len(data))
		return False
	(status, tilter_sps, trigger_offset, samples) = struct.unpack("IfII", data)
	return STInfo(status, tilter_sps, trigger_offset, samples)


def main():
	if not connect():
		print("Cannot connect to serial port")
		exit(1)

	if False:
		filename = "data.bin"
		if len(sys.argv)>2:
			filename = sys.argv[2]
		print("filename=",filename)
		StartSampling()
		ReceiveData(filename=filename, command=USB_COMMAND_ADC_BUFFER_GET)
		return

	if False:	
		filename = "data.na"
		if len(sys.argv)>2:
			filename = sys.argv[2]
		print("filename=",filename)
		ReceiveData(filename=filename, command=USB_COMMAND_NANOAMPERS)
		return
	
	if True:
		info = ReceiveSTInfo()
		print("status =", info.status)
		print("tilter_sps =", info.tilter_sps)
		print("trigger_offset =", info.trigger_offset)
		print("samples =", info.samples)

		if info.status==6 and info.samples>100:
			ReceiveData("out.a", command=USB_COMMAND_ST_DATA, all_size=info.samples*4)
		return

	pass

def help():
	print("Receive data from Microampermeter")

if __name__ == "__main__":
	if len(sys.argv)==1:
		help()
	#print(sys.argv)
	#print(g_standart_freq)
	#print(FreqListX(4))
	main()
