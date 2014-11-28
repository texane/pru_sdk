#!/usr/bin/env sh

export PRU_SDK_DIR=`pwd`/../..
export PRU_CGT_DIR=/opt/PRU/ti-cgt-pru_2.1.0


# compile support library without optimization
# enabled to keep argument passing convention


$PRU_CGT_DIR/bin/clpru \
--silicon_version=2 \
--hardware_mac=on \
-i$PRU_CGT_DIR/include \
-i$PRU_CGT_DIR/lib \
-c \
dcan.c

$PRU_CGT_DIR/bin/clpru \
--silicon_version=2 \
--hardware_mac=on \
-i$PRU_CGT_DIR/include \
-i$PRU_CGT_DIR/lib \
-c \
dcan_frame.c

$PRU_CGT_DIR/bin/clpru \
--silicon_version=2 \
--hardware_mac=on \
-i$PRU_CGT_DIR/include \
-i$PRU_CGT_DIR/lib \
-c \
pru_hal.c


$PRU_CGT_DIR/bin/clpru \
--silicon_version=2 \
--hardware_mac=on \
-i$PRU_CGT_DIR/include \
-i$PRU_CGT_DIR/lib \
-O3 \
-c \
pru_main.c


# compile and link
# optimizations allowed

$PRU_CGT_DIR/bin/clpru \
--silicon_version=2 \
--hardware_mac=on \
-i$PRU_CGT_DIR/include \
-i$PRU_CGT_DIR/lib \
-z \
pru_main.obj dcan.obj dcan_frame.obj pru_hal.obj -llibc.a \
-m pru_main.map \
-o pru_main.elf \
$PRU_CGT_DIR/example/AM3359_PRU.cmd


# convert ELF to binary file pru_main.bin

$PRU_CGT_DIR/bin/hexpru \
$PRU_CGT_DIR/bin.cmd \
./pru_main.elf


# build host program

make clean
make  START_ADDR=`./get_start_addr.sh ./pru_main.elf`
