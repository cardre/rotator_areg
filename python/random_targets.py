#!/bin/python
#
# Simple and dumb random target generator
#
# Outputs a bunch of random targets for the rotator to point at
# adjust serial_path to point to the USB device for the rotator_areg device
# VK5CD

import serial, time, random

# Config these as required
serial_path = '/dev/tty.usbmodem14121'
serial_speed = 115200
# min_az_degrees = -170
# max_az_degrees = 170
# min_el_degrees = -20
# max_el_degrees = 85
min_az_degrees = -130
max_az_degrees = -100
min_el_degrees = 00
max_el_degrees = 25

ser = serial.Serial(port=serial_path, timeout=0.1, baudrate=serial_speed)

print "Random target generator for rotator_areg"

def display_incoming_serial():
    while ( ser.inWaiting() > 0 ):
        print ser.readline()

# output current orientation after delay
time.sleep(1)
display_incoming_serial()
time.sleep(1)
display_incoming_serial()
print "get current position"
ser.write('g\n')
time.sleep(1)
display_incoming_serial()

while True:
    display_incoming_serial()
    time.sleep(4)

    next_az = random.randint(min_az_degrees, max_az_degrees)
    next_el = random.randint(min_el_degrees, max_el_degrees)
    ser.write('t'+str(next_az)+','+str(next_el)+'\n')
