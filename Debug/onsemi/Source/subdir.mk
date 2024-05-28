################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../onsemi/Source/NCN26010.c \
../onsemi/Source/T1S_FreeRTOS_OS.c \
../onsemi/Source/T1S_FreeRTOS_TCP-IP.c \
../onsemi/Source/onsemi_hardware.c \
../onsemi/Source/se_communication.c \
../onsemi/Source/se_udp_communication.c 

C_DEPS += \
./onsemi/Source/NCN26010.d \
./onsemi/Source/T1S_FreeRTOS_OS.d \
./onsemi/Source/T1S_FreeRTOS_TCP-IP.d \
./onsemi/Source/onsemi_hardware.d \
./onsemi/Source/se_communication.d \
./onsemi/Source/se_udp_communication.d 

OBJS += \
./onsemi/Source/NCN26010.o \
./onsemi/Source/T1S_FreeRTOS_OS.o \
./onsemi/Source/T1S_FreeRTOS_TCP-IP.o \
./onsemi/Source/onsemi_hardware.o \
./onsemi/Source/se_communication.o \
./onsemi/Source/se_udp_communication.o 


# Each subdirectory must supply rules for building sources it contributes
onsemi/Source/%.o: ../onsemi/Source/%.c onsemi/Source/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MCXN947VDF -DCPU_MCXN947VDF_cm33 -DCPU_MCXN947VDF_cm33_core0 -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -DPMSM_SNSLESS -DPMSM_SNSLESS_ENC -DBLDC_SNSLESS -DMID_EN -DMC_EXAMPLE -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\board" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\drivers" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\device" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\utilities" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\component\uart" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\component\serial_manager" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\component\lists" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\CMSIS" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\startup" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\freemaster" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\freemaster\platforms" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\freemaster\drivers" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\source" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\rtcesl\CM33F_RTCESL_4.7.1_MCUX\AMCLIB\Include" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\rtcesl\CM33F_RTCESL_4.7.1_MCUX\GDFLIB\Include" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\rtcesl\CM33F_RTCESL_4.7.1_MCUX\GFLIB\Include" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\rtcesl\CM33F_RTCESL_4.7.1_MCUX\GMCLIB\Include" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\rtcesl\CM33F_RTCESL_4.7.1_MCUX\MLIB\Include" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\motor_control\pmsm\pmsm_float\mc_algorithms" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\motor_control\pmsm\pmsm_float\state_machine" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\motor_control\pmsm\pmsm_float\mc_state_machine" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\motor_control\freemaster" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\motor_control\docs" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\motor_control\pmsm\pmsm_float" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\motor_control\bldc\mc_algorithms" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\motor_control\bldc\state_machine" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\motor_control\bldc\mc_state_machine" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\motor_control\pmsm\pmsm_float\mc_identification" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\motor_control\pmsm\pmsm_float\mc_algorithms\mcaa_lib\cm33f_mcux\Include" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\onsemi\Include" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\freertos\freertos-kernel\portable\GCC\ARM_CM33_NTZ" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\freertos\freertos-kernel\include" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\onsemi\FreeRTOS-Plus-TCP\include" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\freertos\freertos-kernel\portable\GCC\ARM_CM33_NTZ\non_secure" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\example_data" -I"C:\Users\Stanley.Ezeh\Documents\09.MCXN Motor Control\Software\MCUXPresso Projects\MotorControlOverSPI\motor_control\pmsm\pmsm_float\mc_drivers" -O0 -fno-common -g3 -gdwarf-4 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-onsemi-2f-Source

clean-onsemi-2f-Source:
	-$(RM) ./onsemi/Source/NCN26010.d ./onsemi/Source/NCN26010.o ./onsemi/Source/T1S_FreeRTOS_OS.d ./onsemi/Source/T1S_FreeRTOS_OS.o ./onsemi/Source/T1S_FreeRTOS_TCP-IP.d ./onsemi/Source/T1S_FreeRTOS_TCP-IP.o ./onsemi/Source/onsemi_hardware.d ./onsemi/Source/onsemi_hardware.o ./onsemi/Source/se_communication.d ./onsemi/Source/se_communication.o ./onsemi/Source/se_udp_communication.d ./onsemi/Source/se_udp_communication.o

.PHONY: clean-onsemi-2f-Source

