#!/usr/bin/env sh
echo pwm_P8_36 > /sys/devices/bone_capemgr.9/slots
echo pru_enable > /sys/devices/bone_capemgr.9/slots
./main
