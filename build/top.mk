C_FILES := host_main.c ../common/mio.c
DTS_FILES := pru_enable-00A0.dtsi
C_FLAGS := -DSTART_ADDR=$(START_ADDR)

PRU_SDK_DIR ?= $(shell pwd)/../..

CROSS_COMPILE ?= /opt/gcc-linaro-arm-linux-gnueabihf-4.7-2013.03-20130313_linux/bin/arm-linux-gnueabihf-

CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)gcc
STRIP := $(CROSS_COMPILE)strip
PASM := $(PRU_SDK_DIR)/bin/pasm
DTC := $(PRU_SDK_DIR)/bin/dtc

C_FLAGS += -Wall -O2 -mtune=cortex-a8 -march=armv7-a

C_FLAGS += -I$(PRU_SDK_DIR)/include
L_FLAGS += -L$(PRU_SDK_DIR)/lib
L_LIBS += -lprussdrv

BIN_FILES := $(P_FILES:.p=.bin)
O_FILES := $(C_FILES:.c=.o)
DTBO_FILES := $(DTS_FILES:.dtsi=.dtbo)

BUILD_DIR = build

all:	main $(BIN_FILES) $(DTBO_FILES)

main:	$(O_FILES)
	$(LD) -static -o $(BUILD_DIR)/$@ $(O_FILES) $(L_FLAGS) $(L_LIBS)
	$(STRIP) $(BUILD_DIR)/$@
	rm *.o

%.bin : %.p
	$(PASM) -V2 -I$(PRU_SDK_DIR)/include -b $<

%.o : %.c
	$(CC) $(C_FLAGS) -c -o $@ $<

%.dtbo : %.dtsi
	$(DTC) -@ -O dtb -o $@ $<

.PHONY	: clean all
clean	:
	-rm -f $(O_FILES)
	-rm -f $(BIN_FILES)
	-rm -f $(DTBO_FILES)
	-rm -f main
	