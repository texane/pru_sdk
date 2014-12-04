#!/usr/bin/env sh

export PRU_SDK_DIR=/segfs/linux/pru_sdk
export PRU_CGT_DIR=$PRU_SDK_DIR/pru_2.0.0B2


# compile support library without optimization
# enabled to keep argument passing convention

$PRU_CGT_DIR/bin/clpru \
--silicon_version=2 \
--hardware_mac=on \
-i$PRU_CGT_DIR/include \
-i$PRU_CGT_DIR/lib \
-c \
-s \
pru_hal.c


$PRU_CGT_DIR/bin/clpru \
--silicon_version=2 \
--hardware_mac=on \
-i$PRU_CGT_DIR/include \
-i$PRU_CGT_DIR/lib \
-O3 \
-c \
-s \
pru_main.c


# compile and link
# optimizations allowed

$PRU_CGT_DIR/bin/clpru \
--silicon_version=2 \
--hardware_mac=on \
-i$PRU_CGT_DIR/include \
-i$PRU_CGT_DIR/lib \
-z \
pru_main.obj pru_hal.obj -llibc.a \
-m pru_main.map \
-o pru_main.elf \
$PRU_CGT_DIR/example/AM3359_PRU.cmd


# convert ELF to binary file pru_main.bin

$PRU_CGT_DIR/bin/hexpru \
$PRU_CGT_DIR/bin.cmd \
./pru_main.elf


# build host program

make clean
make START_ADDR=`./get_start_addr.sh ./pru_main.elf`
