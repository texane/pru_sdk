#!/usr/bin/env sh

scp \
gpio_p8_11-00A0.dtbo \
spwm.bin \
main \
run.sh \
root@192.168.0.44:/lib/firmware/spwm/
