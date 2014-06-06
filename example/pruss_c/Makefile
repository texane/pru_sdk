C_FILES := host_main.c ../common/mio.c
DTS_FILES := pru_enable-00A0.dts
C_FLAGS := -DMAIN_ADDR=$(MAIN_ADDR)

PRU_SDK_DIR ?= /segfs/linux/pru_sdk
include $(PRU_SDK_DIR)/build/top.mk
