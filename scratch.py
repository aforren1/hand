import serial
import struct

ser = serial.Serial('COM4', 9600, timeout=0)
ser.close()
ser.open()
ser.readlines()
ser.write(b'c')

ser.write(b'sf' + struct.pack('>f', 500))
ser.write(b'gf')
xx = ser.readlines()
struct.unpack('>f', xx[0][0:4])
