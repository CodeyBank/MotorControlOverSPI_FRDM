################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/MotorControlOverSPI.c \
../source/freemaster_integration.c \
../source/mc_periph_init.c \
../source/semihost_hardfault.c 

C_DEPS += \
./source/MotorControlOverSPI.d \
./source/freemaster_integration.d \
./source/mc_periph_init.d \
./source/semihost_hardfault.d 

OBJS += \
./source/MotorControlOverSPI.o \
./source/freemaster_integration.o \
./source/mc_periph_init.o \
./source/semihost_hardfault.o 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c source/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MCXN947VDF -DCPU_MCXN947VDF_cm33 -DCPU_MCXN947VDF_cm33_core0 -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -DPMSM_SNSLESS -DPMSM_SNSLESS_ENC -DBLDC_SNSLESS -DMID_EN -DMC_EXAMPLE -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\board" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\drivers" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\device" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\utilities" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\component\uart" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\component\serial_manager" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\component\lists" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\CMSIS" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\startup" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\freemaster" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\freemaster\platforms" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\freemaster\drivers" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\source" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\rtcesl\CM33F_RTCESL_4.7.1_MCUX\AMCLIB\Include" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\rtcesl\CM33F_RTCESL_4.7.1_MCUX\GDFLIB\Include" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\rtcesl\CM33F_RTCESL_4.7.1_MCUX\GFLIB\Include" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\rtcesl\CM33F_RTCESL_4.7.1_MCUX\GMCLIB\Include" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\rtcesl\CM33F_RTCESL_4.7.1_MCUX\MLIB\Include" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\motor_control\pmsm\pmsm_float\mc_algorithms" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\motor_control\pmsm\pmsm_float\state_machine" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\motor_control\pmsm\pmsm_float\mc_state_machine" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\motor_control\freemaster" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\motor_control\docs" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\motor_control\pmsm\pmsm_float" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\motor_control\bldc\mc_algorithms" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\motor_control\bldc\state_machine" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\motor_control\bldc\mc_state_machine" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\motor_control\pmsm\pmsm_float\mc_identification" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\motor_control\pmsm\pmsm_float\mc_algorithms\mcaa_lib\cm33f_mcux\Include" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\onsemi\Include" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\freertos\freertos-kernel\portable\GCC\ARM_CM33_NTZ" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\freertos\freertos-kernel\include" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\onsemi\FreeRTOS-Plus-TCP\include" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\freertos\freertos-kernel\portable\GCC\ARM_CM33_NTZ\non_secure" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\example_data" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\motor_control\pmsm\pmsm_float\mc_drivers" -O0 -fno-common -g3 -gdwarf-4 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source

clean-source:
	-$(RM) ./source/MotorControlOverSPI.d ./source/MotorControlOverSPI.o ./source/freemaster_integration.d ./source/freemaster_integration.o ./source/mc_periph_init.d ./source/mc_periph_init.o ./source/semihost_hardfault.d ./source/semihost_hardfault.o

.PHONY: clean-source

