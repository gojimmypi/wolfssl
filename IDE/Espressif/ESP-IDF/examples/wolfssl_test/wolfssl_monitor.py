#!/usr/bin/env python
#
# wolfssl_monitor [--port | -p] [--baudrate | -b] [--logfile | -L]
#
# Monitors the serial port, echoing to console and optionally saving
# to specified logfile. Monitoring continues until keywords are found:
#
# WOLFSSL_COMPLETE - normal completion
# WOLFSSL_FAIL     - error detected, return exit code = 1
#
# needs pyserial and argparse
# pip install pyserial
#
import serial # pip install pyserial
import argparse
import sys
import re

# Parse command-line arguments
parser = argparse.ArgumentParser(description="Serial communication script")
parser.add_argument("-p", "--port", type=str, default="/dev/ttyS9", help="Serial port")
parser.add_argument("-b", "--baudrate", type=int, default=115200, help="Baud rate")
parser.add_argument("-L", "--logfile", type=str, default="ESP32.log", help="Log file name" )
args = parser.parse_args()

# Optionally log to file, if specified
if args.logfile is None or args.logfile == '':
    print("No log file path provided.")
else:
    file_path = args.logfile
    print(f"Saving log to " + file_path)

print("Connecting to " + args.port)
ser = serial.Serial(args.port, args.baudrate)

print("Monitoring until complete: " + args.port)
with open(file_path, "w") as file:
    while True:
        line = ser.readline().decode('utf-8', errors='replace').strip()

        # show the text on the console
        print(line)

        # Only save to log if a log file argument was found
        if not(args.logfile is None or args.logfile == ''):
            # Remove ANSI escape codes using a regular expression
            line_without_ansi = re.sub(r'\x1B\[[0-?]*[ -/]*[@-~]', '', line)
            # save the text to the log file.
            file.write(line_without_ansi + "\n")

        if "WOLFSSL_COMPLETE" in line:
            # no exact match as ESP_LOG colorizes output.
            print("Example completed. Exiting monitor.")
            break

        if "A fatal error occurred" in line:
            print("A fatal error occurred. Exiting monitor.")
            ser.close()
            sys.exit(1)  # Exit with a non-zero error code

        if line == "WOLFSSL_FAIL":
            print("Application Failed. Exiting monitor.")
            ser.close()
            sys.exit(1)  # Exit with a non-zero error code

ser.close()
