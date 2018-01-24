import serial
import time
import struct

ser = serial.Serial('COM5', timeout=0)
print(ser.name)         # check which port was really used
while True :
    # Get frequency
    ser.readlines()
    ser.write(b'c')
    time.sleep(1)
    ser.write(b'gf')
    time.sleep(1)
    freq = ser.readline()
    x = struct.unpack('>f', freq[0:4])
    #print(freq)
    print(x)
    time.sleep(2)

    # Set frequency to 500 then get frequency
    ser.write(b'sf' + struct.pack('>f', 500))
    time.sleep(.5)
    ser.write(b'gf')
    time.sleep(1)
    freq = ser.readline()
    x = struct.unpack('>f', freq[0:4])
    #print(freq)
    print(x)
    time.sleep(2)

    # Set frequency out of range then get error and frequency
    ser.write(b'sf' + struct.pack('>f', 5000))
    time.sleep(1)
    ser.write(b'ge')
    time.sleep(1)
    err = ser.readline()
    print(err[0])
    ser.write(b'gf')
    time.sleep(2)
    freq = ser.readline()
    x = struct.unpack('>f', freq[0:4])
    #print(freq)
    print(x)
    
    # Reset frequency to 5
    ser.write(b'sf' + struct.pack('>f', 5))
    time.sleep(.5)

    # Enter Game mode
    ser.write(b'sm' + struct.pack('B', 1)) # turn on game mode
    ser.write(b'a') #acquisition mode
    time.sleep(.5)
    for i in range(0,2) :
        result = ser.readlines()
        print(result)
        time.sleep(1)
    # Re-enter Config mode
    ser.write(b'c') # turn on config mode
    time.sleep(1)
    
ser.close() 
