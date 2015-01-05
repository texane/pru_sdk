################### PRU building #######################
PRU_CGT_DIR = /opt/PRU/ti-cgt-pru_2.1.0

CC = ${PRU_CGT_DIR}/bin/clpru
#DEVICE=am335x
AM335X_StarterWare_PATH = /opt/AM335X_StarterWare_02_00_01_01
#CFLAGS = --silicon_version=3 -o1 -D$(DEVICE)
CFLAGS = --silicon_version=2 --hardware_mac=on -O3
HEXBIN = ${PRU_CGT_DIR}/bin/hexpru


PRU_C_DIR=-I${PRU_CGT_DIR}/lib \
	    -I${PRU_CGT_DIR}/include \
	    -I${AM335X_StarterWare_PATH}/include \
	    -I${AM335X_StarterWare_PATH}/include/hw \
	    -I${AM335X_StarterWare_PATH}/include/arm7a \
	    -I${AM335X_StarterWare_PATH}/include/arm7a/am335x \
	    -I${AM335X_StarterWare_PATH}/platform/beaglebone

DRIVER_DIR = ${AM335X_StarterWare_PATH}/drivers
#SRC = $(wildcard ${DRIVER_DIR}/*.c) pru_main.c
SRC = pru*.c consoleUtils.c uart.c uart_irda_cir.c uartStdio.c uartConsole.c
OUT = pru_main.elf
CMD = ${PRU_CGT_DIR}/example/AM3359_PRU.cmd
BUILD_DIR = build

pru:
	mkdir -p $(BUILD_DIR)
	
	$(CC) $(CFLAGS) $(PRU_C_DIR) $(SRC) -z $(CMD) -o $(BUILD_DIR)/pru_main.elf -m $(OUT).map
	rm -rf *.out *.map *.obj
	$(HEXBIN) $(PRU_CGT_DIR)/bin.cmd $(BUILD_DIR)/pru_main.elf
	cp *.bin $(BUILD_DIR)
	rm *.bin
