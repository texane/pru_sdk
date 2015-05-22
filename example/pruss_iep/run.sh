#!/usr/bin/env sh

umount /lib/firmware
mount /dev/mmcblk0p3 /lib/firmware
cp /tmp/gpio_p8_11-00A0.dtbo /lib/firmware/

echo gpio_p8_11 > /sys/devices/bone_capemgr.9/slots

modprobe uio_pruss

/tmp/main
