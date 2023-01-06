import serial
import time


if __name__ == '__main__':
    ser = serial.Serial('COM10', 115200, timeout=1)
    ser.reset_input_buffer()

    while True:
        ser.write(b"Hey\n")
        line = ser.readline().decode('utf-8').rstrip()
        print(line)
        time.sleep(1)



# ser = serial.Serial('COM10', 9600)

# while(True):
#     # time.sleep(1)
#     ser.write(bytes('H', 'utf-8'))
#     time.sleep(1)
#     ser.write(bytes('L', 'utf-8'))
#     time.sleep(1)