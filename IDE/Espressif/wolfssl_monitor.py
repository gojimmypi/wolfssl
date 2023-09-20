#!/usr/bin/env python
#
# needs pyserial and argparse
# pip install pyserial
#
import serial # pip install pyserial
import argparse
import sys

parser = argparse.ArgumentParser(description="Serial communication script")
parser.add_argument("-p", "--port", type=str, default="/dev/ttyS9", help="Serial port")
parser.add_argument("-b", "--baudrate", type=int, default=115200, help="Baud rate")
args = parser.parse_args()

print("Connecting to " + args.port)
ser = serial.Serial(args.port, args.baudrate)

print("Monitoring until complete: " + args.port)
while True:
    line = ser.readline().decode('utf-8').strip()
    print(line)
    if "WOLFSSL_COMPLETE" in line:
        # no exact match as ESP_LOG colorizes output.
        print("Example completed. Exiting monitor.")
        break

    if line == "WOLFSSL_FAIL":
        print("Application Failed. Exiting monitor.")
        ser.close()
        sys.exit(1)  # Exit with a non-zero error code

ser.close()
