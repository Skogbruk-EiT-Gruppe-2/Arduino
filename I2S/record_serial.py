import serial
import time

ser = serial.Serial('COM3', 9600)  # Adjust COM port and baud rate
time.sleep(2)  # Wait for the serial connection to initialize

with open("arduino_output.txt", "w") as f:
    while True:
        if ser.in_waiting:
            line = ser.readline().decode().strip()
            print(line)
            f.write(line + "\n")
