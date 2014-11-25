C_FILES := host_main.c ../common/mio.c
DTS_FILES := pru_enable-00A0.dts
C_FLAGS := -DSTART_ADDR=$(START_ADDR)

#PRU_SDK_DIR ?= /home/longqi/pru_sdk
PRU_SDK_DIR ?= $(shell pwd)/../..

include $(PRU_SDK_DIR)/build/top.mk
