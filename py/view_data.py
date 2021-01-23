#!/usr/bin/env python3
import array
import math
import cmath
import sys
import matplotlib.pyplot as plt
from struct import iter_unpack
from scipy.fftpack import fft
import scipy as np
from sys import argv
from scipy import signal
import xml.etree.ElementTree as ET

TIME_STEP = 1/105.46e3

def makeTimeList(readableData, xmin, xstep):
	N = len(readableData)
	return np.linspace(start = xmin, stop = xstep*(N-1), num = N)

def ToIntFrom24bit(d):
	if d < 0x800000:
		return d
	return d-0x1000000

def readDataInt(filename="data.bin"):
	data_x = []
	data_y = []
	f = open(filename, "rb")
	data = f.read()
	f.close()

	for (x,y) in iter_unpack("II", data):
		data_x.append(ToIntFrom24bit(x))
		data_y.append(ToIntFrom24bit(y))
	return (data_x, data_y)


def readData(filename="data.bin"):
	(data_x, data_y) = readDataInt(filename)
	mul = 1./(2**22)
	return (np.asarray(data_x, dtype=np.float32)*mul, np.asarray(data_y, dtype=np.float32)*mul)

def plotXY(data_x, data_y):
	fig, ax = plt.subplots()

	#timeList = makeTimeList(data_x, 0, TIME_STEP)
	timeList = makeTimeList(data_x, 0, 1)

	ax.plot(timeList, data_x, color='red')
	ax.plot(timeList, data_y, color='blue')
	plt.show()

def removeAverage(data):
	x = np.average(data)
	return np.subtract(data, x)


def makeFFT(data):
	N = len(data)
	yf = fft(data)
	yf_abs = 2.0/N * np.absolute(yf[0:N//2])
	yf_dbfs = 20 * np.log10(yf_abs)
	ref_db = 0
	yf_dbfs = np.subtract(yf_dbfs, ref_db)
	xf = np.linspace(0.0, 1.0/(2.0*TIME_STEP), N//2)
	return (xf, yf_dbfs)


def plotFFT(data, color = 'blue'):
	window = signal.blackman(len(data))
	(xf, yf) = makeFFT(data * window)

	fig, ax = plt.subplots()
	ax.set_xlabel('F (Hz)')
	ax.set_ylabel('dB(fft)')
	ax.set_ylim(bottom=-120, top=0)
	#ax.set_ylim(bottom=-120, top=-60)
	ax.plot(xf, yf, color=color)
	plt.grid()
	plt.show()

def plotFFT2(data1, data2):
	(xf1, yf1) = makeFFT(data1)
	(xf2, yf2) = makeFFT(data2)

	fig, ax = plt.subplots()
	ax.set_xlabel('F (Hz)')
	ax.set_ylabel('dB(fft)')
	ax.set_ylim(bottom=-120, top=0)
	ax.plot(xf1, yf1, color='red')
	ax.plot(xf2, yf2, color='blue')
	plt.grid()
	plt.show()

def checkBits(data, text):
	#ищем неизменные биты.
	#если бит не изменяется - это косяк
	#не работает эта проверка, не всегда бит 

	bits_and = 0xFFFF
	bits_or = 0
	for x in data:
		bits_and = bits_and & x
		bits_or = bits_or | x

	for i in range(32):
		if (bits_and & (1<<i)):
			print(text, ": bit", i, " is 1 permanently")
		if (bits_or & (1<<i))==0:
			print(text, ": bit", i, " is 0 permanently")
	return

def ViewDataBin(filename):
	'''
	(data_xi, data_yi) = readDataInt(filename)
	big = 100000
	for i in range(len(data_xi)-100, len(data_xi)):
	#for i in range(len(data_xi)):
		xi = data_xi[i]
		yi =  data_yi[i]
		#if xi<-big or xi>big or yi<-big or yi>big:
		#	print(i, xi, yi)
		print(i, xi, yi)

	
	#checkBits(data_xi, "X")
	#checkBits(data_yi, "Y")
	return
	'''

	(data_x, data_y) = readData(filename)

	print(len(data_x), len(data_y))
	#print(data_x[0], data_y[0])

	#data_x = removeAverage(data_x)
	#data_y = removeAverage(data_y)

	#plotXY(data_x, data_y)
	plotFFT(data_x, color = 'red')
	plotFFT(data_y, color = 'blue')
	#plotFFT2(data_x, data_y)

if len(sys.argv)>1:
	filename = sys.argv[1]
else:
	filename = "data.bin"

ext = filename[-4:]
if ext == ".bin":
	ViewDataBin(filename)
