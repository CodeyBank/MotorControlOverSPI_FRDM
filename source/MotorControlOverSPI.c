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
#include "freemaster_integration.h"
#include "udpecho.h"

/* TODO: insert other definitions and declarations here. */

extern NCN_PinDef_t pcs_pin;
extern NCN_PinDef_t reset_pin;
//#define SE_DEBUG

#ifdef SE_DEBUG
#define SIZE 12
	uint8_t tx_buffer[SIZE] ={0};
	uint8_t rx_buffer[SIZE] ={0};
#endif //SE_DEBUG



	// freemaster integrations
#include "freemaster.h"
#include "freemaster_example.h"
#include "freemaster_net.h"

/* Stack size of the temporary lwIP initialization thread. */
#define EXAMPLE_THREAD_STACKSIZE 1024

/* Priority of the temporary lwIP initialization thread. */
#define EXAMPLE_FMSTR_THREAD_PRIO 3

/* Priority of the temporary lwIP initialization thread. */
#define EXAMPLE_APP_THREAD_PRIO 7

static FMSTR_BOOL fmstr_initialized = FMSTR_FALSE;

static void fmstr_task(void *arg);
static void example_task(void *arg);
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
    FMSTR_NET_IF_CAPS caps;
    memset(&caps, 0, sizeof(caps));
//    start_task();
//    vTaskCreateUDPServer( configMINIMAL_STACK_SIZE*6,tskIDLE_PRIORITY );

    /* FreeMaster task */
	if(xTaskCreate(fmstr_task, "fmstr_task", EXAMPLE_THREAD_STACKSIZE, NULL, EXAMPLE_FMSTR_THREAD_PRIO, NULL) == pdFAIL)
		PRINTF("fmstr_task: Task creation failed.");
//
//	/* Example application task */
//	if(xTaskCreate(example_task, "example_task", EXAMPLE_THREAD_STACKSIZE -512, NULL, EXAMPLE_APP_THREAD_PRIO, NULL) == pdFAIL)
//		PRINTF("example_task: Task creation failed.");

	FMSTR_ASSERT_RETURN(FMSTR_NET_DRV.GetCaps != NULL, 0);
	FMSTR_NET_DRV.GetCaps(&caps);

	PRINTF("\n\nFreeMaster %s %s Example\n\n",
		   ((caps.flags & FMSTR_NET_IF_CAPS_FLAG_UDP) != 0U ? "UDP" : "TCP"),
		   (FMSTR_NET_BLOCKING_TIMEOUT == 0 ? "Non-Blocking" : "Blocking"));


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



static void example_task(void *arg)
{
    while(!fmstr_initialized)
    {
        vTaskDelay(10);
    };

    /* Generic example initialization code */
    FMSTR_Example_Init_Ex(FMSTR_FALSE);
    PRINTF("Example task initialzied \n");

    while(1)
    {
        /* Increment test variables periodically, use the
           FreeMASTER PC Host tool to visualize the variables */
        FMSTR_Example_Poll_Ex(FMSTR_FALSE);

        /* Check the network connection and DHCP status periodically */
//        Network_Poll();
    }
    vTaskDelay(40);
}

/*
 * FreeMASTER task.
 *
 * Network communication takes place here. This task sleeps when waiting
 * for a communication and lets the other example tasks to run.
 */
static void fmstr_task(void *arg)
{

   /* FreeMASTER driver initialization */
    FMSTR_Init();

    fmstr_initialized = FMSTR_TRUE;

    while(1)
    {
        /* The FreeMASTER poll handles the communication interface and protocol
           processing. This call will block the task execution when no communication
           takes place (also see FMSTR_NET_BLOCKING_TIMEOUT option) */
        FMSTR_Poll();

        /* When no blocking timeout is specified, the FMSTR_Poll() returns
           immediately without any blocking. We need to sleep to let other
           tasks to run. */
#if FMSTR_NET_BLOCKING_TIMEOUT == 0
        vTaskDelay(1);
#endif
        vTaskDelay(10);
    }
}


