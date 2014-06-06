#!/usr/bin/env sh


# compile support library without optimization
# enabled to keep argument passing convention

/segfs/linux/pru_sdk/pru_2.0.0B2/bin/clpru \
--silicon_version=2 \
--hardware_mac=on \
-i/segfs/linux/pru_sdk/pru_2.0.0B2/include \
-i/segfs/linux/pru_sdk/pru_2.0.0B2/lib \
-c \
pru_sdk.c


/segfs/linux/pru_sdk/pru_2.0.0B2/bin/clpru \
--silicon_version=2 \
--hardware_mac=on \
-i/segfs/linux/pru_sdk/pru_2.0.0B2/include \
-i/segfs/linux/pru_sdk/pru_2.0.0B2/lib \
-O3 \
-c \
pru_main.c


# compile and link
# optimizations allowed

/segfs/linux/pru_sdk/pru_2.0.0B2/bin/clpru \
--silicon_version=2 \
--hardware_mac=on \
-i/segfs/linux/pru_sdk/pru_2.0.0B2/include \
-i/segfs/linux/pru_sdk/pru_2.0.0B2/lib \
-z \
pru_main.obj pru_sdk.obj -llibc.a \
am335x.cmd


# convert to binary file

/segfs/linux/pru_sdk/pru_2.0.0B2/bin/hexpru -i -b ./pru_main.elf -o=pru_main.bin


# retrieve main address

export MAIN_ADDR=0x`/segfs/linux/pru_sdk/pru_2.0.0B2/bin/dispru pru_main.elf | grep main: | cut -f1 -d\  `
make clean ; make
