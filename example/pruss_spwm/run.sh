#!/usr/bin/env sh

# echo 45 > /sys/class/gpio/export
# echo out > /sys/class/gpio/gpio45/direction
# echo 1 > /sys/class/gpio/gpio45/value

umount /lib/firmware
mount /dev/mmcblk0p3 /lib/firmware
cp /tmp/gpio_p8_11-00A0.dtbo /lib/firmware/

echo gpio_p8_11 > /sys/devices/bone_capemgr.9/slots

modprobe uio_pruss

/tmp/main
