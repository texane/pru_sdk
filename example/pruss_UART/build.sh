#!/usr/bin/env sh


# build host program
make pru
make clean
make -f ../../build/top.mk START_ADDR=`./get_start_addr.sh build/pru_main.elf`