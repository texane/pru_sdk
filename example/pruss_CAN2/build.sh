#!/usr/bin/env sh

export PRU_SDK_DIR=`pwd`/../..
export PRU_CGT_DIR=/opt/PRU/ti-cgt-pru_2.1.0
export START_WARE_headers_1=$PRU_SDK_DIR/include
export START_WARE_headers_2=$PRU_SDK_DIR/include/include/hw
export START_WARE_headers_3=$PRU_SDK_DIR/include/include/armv7a
export START_WARE_headers_4=$PRU_SDK_DIR/include/include/armv7a/am335x

export Headers="-i$PRU_CGT_DIR/include \
-i$PRU_CGT_DIR/lib \
-i$START_WARE_headers_1 \
-i$START_WARE_headers_2 \
-i$START_WARE_headers_3 \
-i$START_WARE_headers_4 "


# compile support library without optimization
# enabled to keep argument passing convention


$PRU_CGT_DIR/bin/clpru \
--silicon_version=2 \
--hardware_mac=on \
$Headers \
-c \
dcan.c

$PRU_CGT_DIR/bin/clpru \
--silicon_version=2 \
--hardware_mac=on \
$Headers \
-c \
dcan_frame.c

$PRU_CGT_DIR/bin/clpru \
--silicon_version=2 \
--hardware_mac=on \
$Headers \
-c \
pru_hal.c


$PRU_CGT_DIR/bin/clpru \
--silicon_version=2 \
--hardware_mac=on \
$Headers \
-O3 \
-c \
pru_main.c


# compile and link
# optimizations allowed

$PRU_CGT_DIR/bin/clpru \
--silicon_version=2 \
--hardware_mac=on \
$Headers \
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
