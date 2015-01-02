################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
button_led_1.obj: ../button_led_1.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: PRU Compiler'
	"/home/sitara/ti-cgt-pru_2.1.0/bin/clpru" -v3 --include_path="/home/sitara/ti-cgt-pru_2.1.0/include" --include_path="/home/sitara/ti-sdk-am335x-evm-07.00.00.00/example-applications/pru/include" -g --define=am3359 --define=pru0 --diag_warning=225 --display_error_number --diag_wrap=off --endian=little --preproc_with_compile --preproc_dependency="button_led_1.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '


