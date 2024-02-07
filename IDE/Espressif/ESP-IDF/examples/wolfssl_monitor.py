#!/usr/bin/env python
#
# needs pyserial and argparse
# pip install pyserial
#
import serial # pip install pyserial
import argparse
import sys
import re

parser = argparse.ArgumentParser(description="Serial communication script")
parser.add_argument("-p", "--port", type=str, default="/dev/ttyS9", help="Serial port")
parser.add_argument("-b", "--baudrate", type=int, default=115200, help="Baud rate")
parser.add_argument("-L", "--logfile", type=str, default="ESP32.log", help="Log file name" )
args = parser.parse_args()

file_path = args.logfile
print("Saving log to " + file_path)

print("Connecting to " + args.port)
ser = serial.Serial(args.port, args.baudrate)

print("Monitoring until complete: " + args.port)
with open(file_path, "w") as file:
    while True:
        line = ser.readline().decode('utf-8').strip()

        # show the text on the console
        print(line)

        # Remove ANSI escape codes using a regular expression
        line_without_ansi = re.sub(r'\x1B\[[0-?]*[ -/]*[@-~]', '', line)

        # save the text to the log file.
        file.write(line_without_ansi + "\n")
        if "WOLFSSL_COMPLETE" in line:
            # no exact match as ESP_LOG colorizes output.
            print("Example completed. Exiting monitor.")
            break

        if line == "WOLFSSL_FAIL":
            print("Application Failed. Exiting monitor.")
            ser.close()
            sys.exit(1)  # Exit with a non-zero error code

ser.close()
