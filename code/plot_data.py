import serial
import matplotlib.pyplot as plt
import numpy as np

plt.ion()
fig = plt.figure()

i=0
x=[]
y=[]

ser = serial.Serial('COM3', 9600)
ser.close()
ser.open()

while True:
    data = ser.readline().decode()
    print(data)
    x.append(i)
    y.append(data)

    plt.scatter(i, float(data))

    i+=1
    plt.show()
    plt.pause(0.00001)