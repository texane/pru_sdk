C_FILES := host_main.c ../common/mio.c
DTS_FILES := pwm_P9_21-00A0.dts pwm_P9_22-00A0.dts pwm_P8_34-00A0.dts pwm_P8_36-00A0.dts pwm_P8_45-00A0.dts pwm_P8_46-00A0.dts pru_enable-00A0.dts

C_FLAGS := -DSTART_ADDR=$(START_ADDR)

PRU_SDK_DIR ?= /segfs/linux/pru_sdk
include $(PRU_SDK_DIR)/build/top.mk
