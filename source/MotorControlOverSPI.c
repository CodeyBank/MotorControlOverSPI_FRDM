/*
 * Copyright 2016-2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file    MotorControlOverSPI.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MCXN947_cm33_core0.h"
#include "fsl_debug_console.h"
#include "onsemi_hardware.h"
/* TODO: insert other include files here. */

// FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

// NCN includes

#include "ncn26010.h"
#include "T1S_OS.h"
#include "T1S_TCP-IP.h"


/* TODO: insert other definitions and declarations here. */

extern NCN_PinDef_t pcs_pin;
extern NCN_PinDef_t reset_pin;
//#define SE_DEBUG

#ifdef SE_DEBUG
#define SIZE 12
	uint8_t tx_buffer[SIZE] ={0};
	uint8_t rx_buffer[SIZE] ={0};
#endif //SE_DEBUG

/*
 * @brief   Application entry point.
 */
int main(void) {

    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

#ifdef SE_DEBUG
    // Fill the buffer with random data
    for(int i=0; i<SIZE; i++){
    	tx_buffer[i] = i;
    }
#endif //SE_DEBUG
    NCN26010_Init();
    OS_Start();

    while(1) {

#ifdef SE_DEBUG
    	PRINTF("Sending data ...\n");
    	NCN_SPI_Transfer(rx_buffer, tx_buffer, SIZE);
    	DelayMs(2000);
    }
#endif //SE_DEBUG
	}
    return 0 ;
}
