################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
PRU_audio.obj: ../PRU_audio.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: PRU Compiler'
	"c:/ti/ccsv6/tools/compiler/pru_2.1.0/bin/clpru" -v3 --include_path="c:/ti/ccsv6/tools/compiler/pru_2.1.0/include" --include_path="../../../../include" -g --define=am3359 --define=pru0 --diag_warning=225 --display_error_number --diag_wrap=off --endian=little --hardware_mac=on --preproc_with_compile --preproc_dependency="PRU_audio.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


