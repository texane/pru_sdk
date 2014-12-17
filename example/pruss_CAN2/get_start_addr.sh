#!/usr/bin/env sh

# assume PRU_CGT_DIR, the PRU C toolchain directory
# assume 1 argument, the elf filename
export PRU_CGT_DIR=/opt/PRU/ti-cgt-pru_2.1.0
export START_ADDR=0x`$PRU_CGT_DIR/bin/dispru $1 | grep _c_int00 | cut -f1 -d\  `
echo $START_ADDR
