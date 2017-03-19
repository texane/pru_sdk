#!/usr/bin/env sh
/sbin/modprobe uio_pruss
echo pru_enable > /sys/devices/bone_capemgr.9/slots
./main
