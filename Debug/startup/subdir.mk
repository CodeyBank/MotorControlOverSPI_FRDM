################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../startup/boot_multicore_slave.c \
../startup/startup_mcxn947_cm33_core0.c 

C_DEPS += \
./startup/boot_multicore_slave.d \
./startup/startup_mcxn947_cm33_core0.d 

OBJS += \
./startup/boot_multicore_slave.o \
./startup/startup_mcxn947_cm33_core0.o 


# Each subdirectory must supply rules for building sources it contributes
startup/%.o: ../startup/%.c startup/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MCXN947VDF -DCPU_MCXN947VDF_cm33 -DCPU_MCXN947VDF_cm33_core0 -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\board" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\drivers" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\device" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\utilities" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\component\uart" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\component\serial_manager" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\component\lists" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\CMSIS" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\startup" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\source" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\onsemi\Include" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\freertos\freertos-kernel\portable\GCC\ARM_CM33_NTZ" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\freertos\freertos-kernel\include" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\onsemi\FreeRTOS-Plus-TCP\include" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\freertos\freertos-kernel\portable\GCC\ARM_CM33_NTZ\non_secure" -O0 -fno-common -g3 -gdwarf-4 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-startup

clean-startup:
	-$(RM) ./startup/boot_multicore_slave.d ./startup/boot_multicore_slave.o ./startup/startup_mcxn947_cm33_core0.d ./startup/startup_mcxn947_cm33_core0.o

.PHONY: clean-startup

