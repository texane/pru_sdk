#!/usr/bin/env sh
echo pru_enable > /sys/devices/bone_capemgr.9/slots
./main 10 20
