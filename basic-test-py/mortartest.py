# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2022 Adrian Przekwas <adrian.v.przekwas@gmail.com>

# usage:
# sudo pigpiod
# python
# import mortartest
# mortartest.move_stepper_to(50, 0) # move first (0) stepper 50 steps forward, ALT stepper is 0, AZI is 1

import time
import math
import pigpio
pi = pigpio.pi()
if not pi.connected:
    print ("pigpio not connected, check if pigpiod daemon is running")
    exit()

powerLimit = 0.2 # 0.0-1.0
decPin = 22 # GPIO 22, physical pin 15
xI0Pin = 17 # GPIO17, physical pin 11, current regulation
xI1Pin = 27 # GPIO27, physical pin 13
# xI1 xI0 Current
# 1    1    0%
# 1    0    38%
# 0    1    71%
# 0    0    100%*/
pi.set_mode(decPin, pigpio.OUTPUT)
pi.set_mode(xI0Pin, pigpio.OUTPUT)
pi.set_mode(xI1Pin, pigpio.OUTPUT)
pi.write(decPin, 0)
pi.write(xI0Pin, 0)
pi.write(xI1Pin, 0)

aEnblPinPWMAlt = 4 # GPIO 4, physical pin 7
aPhPinAlt = 5 # GPIO 5, physical pin 29
bEnblPinPWMAlt = 18 # GPIO 18, physical pin 12
bPhPinAlt = 6 # GPIO 6, physical pin 31
pi.set_mode(aPhPinAlt, pigpio.OUTPUT)
pi.set_mode(bPhPinAlt, pigpio.OUTPUT)
pi.set_PWM_frequency(aEnblPinPWMAlt, 40000)
pi.set_PWM_frequency(bEnblPinPWMAlt, 40000)

aEnblPinPWMAzi = 20 # GPIO 20, physical pin 38
aPhPinAzi = 12  # GPIO 12, physical pin 32
bEnblPinPWMAzi = 21 # GPIO 21 physical pin 21
bPhPinAzi = 26 # GPIO 26, physical pin 37
pi.set_mode(aPhPinAzi, pigpio.OUTPUT)
pi.set_mode(bPhPinAzi, pigpio.OUTPUT)
pi.set_PWM_frequency(aEnblPinPWMAzi, 40000)
pi.set_PWM_frequency(bEnblPinPWMAzi, 40000)

def calc_and_set_phases(pos, a_enbl_pin, a_ph_pin, b_enbl_pin, b_ph_pin):
    step = 0.0
    if pos >= 0:
        step = math.fmod(pos, 4.0)
    else:
        step = 4.0 - math.fmod(abs(pos), 4.0)
    a_pwm = abs(math.sin((math.pi * step) / 2)) * powerLimit
    b_pwm = abs(math.cos((math.pi * step) / 2)) * powerLimit
    if step < 1:
        a_phase = 0
        b_phase = 1
    if step >= 1 and step < 2:
        a_phase = 0
        b_phase = 0
    if step >= 2 and step < 3:
        a_phase = 1
        b_phase = 0
    if step >= 3:
        a_phase = 1
        b_phase = 1
    print('Pos: ',f'{pos:.2f}', 'Step:',f'{step:.2f}', 'APHASE:',f'{a_phase:.2f}', 'BPHASE: ',f'{b_phase:.2f}', 'AENBL: ',f'{a_pwm:.2f}', 'BENBL: ',f'{b_pwm:.2f}')
    pi.write(a_ph_pin, a_phase)
    pi.write(b_ph_pin, b_phase)
    pi.set_PWM_dutycycle(a_enbl_pin, round(a_pwm * 255))
    pi.set_PWM_dutycycle(b_enbl_pin, round(b_pwm * 255))

def move_stepper_to(target, stepper_id = 0):
    if stepper_id == 0:
        a_enbl_pin = aEnblPinPWMAlt
        a_ph_pin = aPhPinAlt
        b_enbl_pin = bEnblPinPWMAlt
        b_ph_pin = bPhPinAlt
    elif stepper_id == 1:
        a_enbl_pin = aEnblPinPWMAzi
        a_ph_pin = aPhPinAzi
        b_enbl_pin = bEnblPinPWMAzi
        b_ph_pin = bPhPinAzi
    else:
        print ("Wrong motor id")
        exit()
    microstep_size = 0.1
    sleep_time = 0.01
    base = 0.0
    next_pos =  base
    if target > base:
        while next_pos < target:
            next_pos += microstep_size
            calc_and_set_phases (next_pos, a_enbl_pin, a_ph_pin, b_enbl_pin, b_ph_pin)
            time.sleep(sleep_time)
    else:
        while next_pos > target:
            next_pos -= microstep_size
            calc_and_set_phases (next_pos, a_enbl_pin, a_ph_pin, b_enbl_pin, b_ph_pin)
            time.sleep(sleep_time)
    pi.set_PWM_dutycycle(aEnblPinPWMAlt, 0)
    pi.set_PWM_dutycycle(bEnblPinPWMAlt, 0)
    pi.set_PWM_dutycycle(aEnblPinPWMAzi, 0)
    pi.set_PWM_dutycycle(bEnblPinPWMAzi, 0)

if __name__ == '__main__':
    print ("Stepper 0, 50 steps forward")
    move_stepper_to(50, 0)
    print ("Stepper 1, 50 steps forward")
    move_stepper_to(50, 1)
    print ("Stepper 0, 50 steps backward")
    move_stepper_to(-50, 0)
    print ("Stepper 1, 50 steps backward")
    move_stepper_to(-50, 1)
    pi.stop()
