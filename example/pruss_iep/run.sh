#!/usr/bin/env sh

umount /lib/firmware
mount /dev/mmcblk0p3 /lib/firmware
cp /tmp/pru_enable-00A0.dtbo /lib/firmware/

echo pru_enable > /sys/devices/bone_capemgr.9/slots

modprobe uio_pruss

/tmp/main
