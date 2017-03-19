################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
PRU_MAC_Multiply_Accum.obj: ../PRU_MAC_Multiply_Accum.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: PRU Compiler'
	"/opt/ti/ga/ccsv6/tools/compiler/pru_2.0.0B2/bin/clpru" -v3 --include_path="/opt/ti/ga/ccsv6/tools/compiler/pru_2.0.0B2/include" --include_path="../../../../include" -g --define=am3359 --define=pru0 --diag_warning=225 --display_error_number --diag_wrap=off --endian=little --hardware_mac=on --preproc_with_compile --preproc_dependency="PRU_MAC_Multiply_Accum.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '


