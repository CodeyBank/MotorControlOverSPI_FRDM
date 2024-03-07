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
#include "m1_sm_snsless_enc.h"

	// Motor control
#include "mid_sm_states.h"
#include "mc_periph_init.h"
#include "fsl_gpio.h"
#include "se_communication.h"

/* Stack size of the temporary lwIP initialization thread. */
#define EXAMPLE_THREAD_STACKSIZE 1024 * 2

/* Priority of the temporary lwIP initialization thread. */
#define EXAMPLE_FMSTR_THREAD_PRIO tskIDLE_PRIORITY+3

/* Priority of the temporary lwIP initialization thread. */
#define EXAMPLE_APP_THREAD_PRIO tskIDLE_PRIORITY

static FMSTR_BOOL fmstr_initialized = FMSTR_FALSE;

static void fmstr_task(void *arg);
static void motorcontrol_task(void *arg);


// Motor control variables
/* Example's feature set in form of bits inside ui16featureSet.
   This feature set is expected to be growing over time.
   ... | FEATURE_S_RAMP | FEATURE_FIELD_WEAKENING | FEATURE_ENC
*/
#define FEATURE_ENC (1)               /* Encoder feature flag */
#define FEATURE_FIELD_WEAKENING (0)   /* Field weakening feature flag */
#define FEATURE_S_RAMP (0)            /* S-ramp feature flag */

#define FEATURE_SET (FEATURE_ENC << (0) | \
                     FEATURE_FIELD_WEAKENING << (1) | \
                     FEATURE_S_RAMP << (2))

#define MCRSP_VER "2.0.0" /* motor control package version */

#define BOARD_USER_BUTTON_PRIORITY 4

/* Three instruction added after interrupt flag clearing as required */
#define M1_END_OF_ISR \
    {                 \
        __DSB();      \
        __ISB();      \
    }

/* CPU load measurement SysTick START / STOP macros */
#define SYSTICK_START_COUNT() (SysTick->VAL = SysTick->LOAD)
#define SYSTICK_STOP_COUNT(par1)   \
    uint32_t val  = SysTick->VAL;  \
    uint32_t load = SysTick->LOAD; \
    par1          = load - val

static void BOARD_Init(void);
static void BOARD_InitSysTick(void);
static void DemoSpeedStimulator(void);
static void DemoPositionStimulator(void);
static void Application_Control_BL(void);
static void BOARD_InitGPIO(void);

uint32_t g_ui32NumberOfCycles=0U;
uint32_t g_ui32MaxNumberOfCycles = 0U;
bool_t bDemoModeSpeed = FALSE;
bool_t bDemoModePosition = FALSE;

/* Used for demo mode */
static uint32_t ui32SpeedStimulatorCnt    = 0U;
static uint32_t ui32PositionStimulatorCnt = 0U;

/* Structure used in FM to get required ID's */
app_ver_t g_sAppIdFM = {
    "frdmmcxn947", /* board id */
    "pmsm_enc", /* example id */
    MCRSP_VER,      /* sw version */
    FEATURE_SET,    /* example's feature-set */
};

mid_app_cmd_t g_eMidCmd;                  /* Start/Stop MID command */
ctrl_m1_mid_t g_sSpinMidSwitch;           /* Control Spin/MID switching */


//uint8_t test_buf[MAX_TXRX_SIZE + 20] = {0};
uint8_t t_buff[20];

//**
/*
 * @brief   Application entry point.
 */
int main(void) {

    /*Accessing ID structure to prevent optimization*/
    g_sAppIdFM.ui16FeatureSet = FEATURE_SET;

    uint32_t ui32PrimaskReg;

    /* Disable all interrupts before peripherals are initialized */
    ui32PrimaskReg = DisableGlobalIRQ();

    /* Disable demo mode after reset */
    bDemoModeSpeed    = FALSE;
    bDemoModePosition = FALSE;
    ui32SpeedStimulatorCnt = 0U;

    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();

    // MC
    BOARD_Init();
    //end of MC Specific
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



/* Initialize peripheral motor control driver for motor M1 */
	MCDRV_Init_M1();

	/* Initialize RTCESL PQ */
	RTCESL_PQ_Init();

	/* Turn off application */
	M1_SetAppSwitch(FALSE);

	/* Init application state - Spin or MID */
	g_sSpinMidSwitch.eAppState = kAppStateMID;

	if(g_sSpinMidSwitch.eAppState == kAppStateMID)
	{
	  MID_Init_AR();
	}

	/* Spin state machine is default */
	g_sSpinMidSwitch.eAppState = kAppStateSpin;

	/* Enable interrupts  */
	EnableGlobalIRQ(ui32PrimaskReg);

	/* Start PWM */
	g_sM1Pwm3ph.pui32PwmBaseAddress->MCTRL |= PWM_MCTRL_RUN(0xF);


#ifdef interface_test
//    start_task();
//    vTaskCreateUDPServer( configMINIMAL_STACK_SIZE*6,tskIDLE_PRIORITY );
#endif // interface_test

    /* FreeMaster task */
	if(xTaskCreate(fmstr_task, "fmstr_task", EXAMPLE_THREAD_STACKSIZE, NULL, EXAMPLE_FMSTR_THREAD_PRIO, NULL) == pdFAIL)
		PRINTF("fmstr_task: Task creation failed.");

	/* Example application task */
	if(xTaskCreate(motorcontrol_task, "motorcontrol_task", EXAMPLE_THREAD_STACKSIZE, NULL, EXAMPLE_APP_THREAD_PRIO, NULL) == pdFAIL)
		PRINTF("motorcontrol_task: Task creation failed.");

	FMSTR_ASSERT_RETURN(FMSTR_NET_DRV.GetCaps != NULL, 0);
	FMSTR_NET_DRV.GetCaps(&caps);

#ifdef interface_test
	PRINTF("\n\nFreeMaster %s %s Example\n\n",
		   ((caps.flags & FMSTR_NET_IF_CAPS_FLAG_UDP) != 0U ? "UDP" : "TCP"),
		   (FMSTR_NET_BLOCKING_TIMEOUT == 0 ? "Non-Blocking" : "Blocking"));
#endif

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



extern size_t g_table_size;
static void motorcontrol_task(void *arg)
{
    while(!fmstr_initialized)
    {
        vTaskDelay(50);
    };

    /* Generic example initialization code */
//    FMSTR_Example_Init_Ex(FMSTR_FALSE);
//    PRINTF("Example task initialzied \n");

    // three variables
    tU32 variable1 = 1223442334;
    tU16 var2 = 35000;
    tU8 var3 = 250;

    MEMORY_TABLE_DECL_START()
    SE_MEM_REC_CREATE(variable1, sU32, RO)
	SE_MEM_REC_CREATE(var2, sU16, WO)
	SE_MEM_REC_CREATE(var3, sU8, RW)
    MEMORY_TABLE_DECL_CLOSE()

	g_table_size = sizeof(MEM_TABLE) / sizeof(MEM_TABLE[0]);

//	for(int i=0; i<g_table_size; i++){
//
//		PRINTF("variable name: %s address: 0x%x value: %d \n", MEM_TABLE[i].name, MEM_TABLE[i].address, *(uint16_t *)MEM_TABLE[i].address );
//	}
    commandheader_t cmd_header = {.command_type=load, .transaction_id=23342};
	command_t cmd1 = {
			.address = &var3, //4
			.header = cmd_header, //5
			.txrx_size = 1, //1
			.data[0] = 30 //1
	};

	memory_table_init();
	parse_command(&cmd1, MEM_TABLE);
//
//	uint8_t cmd1_sz = get_required_buffsize_from_cmdsize(cmd1);
//	command_t cmd2 = {};
//
//
//
//	serialize_data(&cmd1, test_buf);
//	deserialize_data(test_buf, &cmd2, cmd1_sz);

//    serialize_mem_record(&MEM_TABLE, t_buff);



    while(1)
    {
        /* Increment test variables periodically, use the
           FreeMASTER PC Host tool to visualize the variables */
//        FMSTR_Example_Poll_Ex(FMSTR_FALSE);
//    	Application_Control_BL();

    	// try to read
//    	commandheader_t cmd_header = {.command_type=write, .transaction_id=23342};
//    	command_t cmd1 = {
//    			.address = &var3,
//    			.header = cmd_header,
//				.txrx_size = 1,
//				.data[0] = 30
//    	};
////
//    	parse_command(&cmd1, MEM_TABLE);
//    	PRINTF("Stored data: %d \n", cmd1.data[0]);
//    	command_t *cmd = {{.address=&var1, .header={.transcation}}, {}, {}};


        /* Check the network connection and DHCP status periodically */
//        Network_Poll();
    }
    vTaskDelay(2000);
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
//        vTaskDelay(5);
    }
}



/*!
 * @brief   ADC ISR called with 100us period processes
 *           - motor M1 fast application machine function
 *           - demo mode if enabled
 *
 * @param   void
 *
 * @return  none
 */


void ADC1_IRQHandler(void)
{
//    uint32_t ui32PrimaskReg;
    uint32_t starttime=0;
    /* Disable all interrupts before peripherals are initialized */
//    ui32PrimaskReg = DisableGlobalIRQ();

    /* Start CPU tick number couting */
//    SYSTICK_START_COUNT();
    SetTick(starttime);

    switch(g_sSpinMidSwitch.eAppState)
    {
    case kAppStateSpin:
        /* M1 state machine */
        SM_StateMachineFast(&g_sM1Ctrl);
      break;
    default:
        /* MID state machine */
        MID_ProcessFast_FL();
      break;
    }

    /* Stop CPU tick number couting and store actual and maximum ticks */
    g_ui32NumberOfCycles = GetTick();
//    SYSTICK_STOP_COUNT(g_ui32NumberOfCycles);
    g_ui32MaxNumberOfCycles =
        g_ui32NumberOfCycles > g_ui32MaxNumberOfCycles ? g_ui32NumberOfCycles : g_ui32MaxNumberOfCycles;

    /* Enable interrupts  */
//    EnableGlobalIRQ(ui32PrimaskReg);

    /* Call FreeMASTER recorder */
//    FMSTR_Recorder(0);

    /* Clear the TCOMP INT flag */
    ADC1->STAT |= (uint32_t)(1U << 9);

    /* Add empty instructions for correct interrupt flag clearing */
    M1_END_OF_ISR;
}


/*!
 * @brief   Slow loop interrupt handler (1ms period)
 *           - motor M1 slow application machine function
 *
 * @param   void
 *
 * @return  none
 */
void CTIMER0_IRQHandler(void)
{
    static int16_t ui16i = 0;

    /* M1 Slow StateMachine call */
    SM_StateMachineSlow(&g_sM1Ctrl);

    /* If in STOP state turn on RED */
    if (M1_GetAppState() == 2)
    {
        LED_RED_ON();
        LED_GREEN_OFF();
    }

    /* If in FAULT state RED blinking*/
    else if (M1_GetAppState() == 0)
    {
        if (ui16i-- < 0)
        {
            LED_RED_TOGGLE();
            bDemoModeSpeed = FALSE;
            bDemoModePosition = FALSE;
            ui16i = 125;
        }
        LED_GREEN_OFF();
    }

    /* If in RUN or INIT state turn on green */
    else
    {
        LED_RED_OFF();
        LED_GREEN_ON();
    }

    /* Demo speed stimulator */
    DemoSpeedStimulator();

    /* Demo position stimulator */
    DemoPositionStimulator();

    /* Call FreeMASTER recorder */
    FMSTR_Recorder(0);

    /* Clear the match interrupt flag. */
    CTIMER0->IR |= CTIMER_IR_MR0INT(1U);

    /* Add empty instructions for correct interrupt flag clearing */
    M1_END_OF_ISR;
}


/*!
 * @brief   DemoSpeedStimulator
 *           - When demo mode is enabled it changes the required speed according
 *             to predefined profile
 *
 * @param   void
 *
 * @return  none
 */
static void DemoSpeedStimulator(void)
{
    if (bDemoModeSpeed)
    {
        ui32SpeedStimulatorCnt++;
        switch (ui32SpeedStimulatorCnt)
        {
            case 10:
                g_sM1Drive.eControl                  = kControlMode_SpeedFOC;
                g_sM1Drive.sMCATctrl.ui16PospeSensor = MCAT_SENSORLESS_CTRL;
                break;
            case 100:
                M1_SetSpeed(1000.0F);
                break;
            case 3000:
                M1_SetSpeed(2000.0F);
                break;
            case 6000:
                M1_SetSpeed(4000.0F);
                break;
            case 9000:
                M1_SetSpeed(-1000.0F);
                break;
            case 12000:
                M1_SetSpeed(-2000.0F);
                break;
            case 15000:
                M1_SetSpeed(-4000.0F);
                break;
            case 18000:
                M1_SetSpeed(800.0F);
                ui32SpeedStimulatorCnt = 0;
                break;
            default:
            	;
                break;
        }
    }
    else
    {
        ui32SpeedStimulatorCnt = 0U;
    }
}

/*!
 * @brief   DemoPositionStimulator
 *           - When demo mode is enabled it changes the required position according
 *             to predefined profile
 *
 * @param   void
 *
 * @return  none
 */
static void DemoPositionStimulator(void)
{
    if (bDemoModePosition)
    {
        ui32PositionStimulatorCnt++;
        switch (ui32PositionStimulatorCnt)
        {
            case 2:
                g_sM1Drive.eControl                  = kControlMode_PositionFOC;
                g_sM1Drive.sMCATctrl.ui16PospeSensor = MCAT_ENC_CTRL;
                break;
            case 10:
                M1_SetPosition(ACC32(20));
                break;
            case 3000:
                M1_SetPosition(ACC32(10));
                break;
            case 6000:
                M1_SetPosition(ACC32(20));
                break;
            case 9000:
                M1_SetPosition(ACC32(0));
                break;
            case 12000:
                M1_SetPosition(ACC32(20));
                break;
            case 15000:
                M1_SetPosition(ACC32(0));
                break;
            case 18000:
                ui32PositionStimulatorCnt = 0;
                break;
            default:
			    ;
            	break;
        }
    }
}

/*!
 * @brief   Application_Control_BL
 *           - Control switching between Spin and MID
 *
 * @param   void
 *
 * @return  none
 */
static void Application_Control_BL(void)
{
  switch(g_sSpinMidSwitch.eAppState)
  {
    case kAppStateSpin:
        /* M1 state machine */
        if(g_sSpinMidSwitch.bCmdRunMid == TRUE)
        {
          if((kSM_AppStop == M1_GetAppState()) && (FALSE == M1_GetAppSwitch()) )
          {
            MID_Init_AR();
            g_sSpinMidSwitch.sFaultCtrlM1_Mid &= ~(FAULT_APP_SPIN);
            g_eMidCmd = kMID_Cmd_Stop;                          /* Reset MID control command */
            g_sSpinMidSwitch.eAppState = kAppStateMID;          /* MID routines will be processed */
          }
          else
            g_sSpinMidSwitch.sFaultCtrlM1_Mid |= FAULT_APP_SPIN;

          g_sSpinMidSwitch.bCmdRunMid = FALSE;                  /* Always clear request */
        }

        g_sSpinMidSwitch.bCmdRunM1 = FALSE;
        break;
    default:
        /* MID state machine */
        if(g_sSpinMidSwitch.bCmdRunM1 == TRUE)
        {
          if((g_eMidCmd == kMID_Cmd_Stop) && (kMID_Stop == MID_GetActualState()))
          {
            g_sSpinMidSwitch.sFaultCtrlM1_Mid &= ~(FAULT_APP_MID);
            g_sM1Ctrl.eState = kSM_AppInit;                      /* Set Init state for M1 state machine */
            g_sSpinMidSwitch.eAppState = kAppStateSpin;          /* Switch application state to Spin */
          }
          else
            g_sSpinMidSwitch.sFaultCtrlM1_Mid |= FAULT_APP_MID;

           /* Always clear request */
          g_sSpinMidSwitch.bCmdRunM1 = FALSE;
          g_sSpinMidSwitch.bCmdRunMid = FALSE;
          break;
        }

        g_sSpinMidSwitch.bCmdRunMid = FALSE;
        MID_Process_BL(&g_eMidCmd);
        break;
  }
}

/*!
 *@brief      SysTick initialization for CPU cycle measurement
 */
static void BOARD_InitSysTick(void)
{
    /* Initialize SysTick core timer to run free */
//    /* Set period to maximum value 2^24*/
//    SysTick->LOAD = 0xFFFFFF;
//
//    /*Clock source - System Clock*/
//    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
//
//    /*Start Sys Timer*/
//    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

/*!
 * @brief LPUART Module initialization (LPUART is a the standard block included e.g. in K66F)
 */

/*!
 * @brief Interrupt service fuction of switch SW3.
 */
void BOARD_SW3_IRQ_HANDLER(void)
{

    /* Speed demo */
    if (bDemoModeSpeed)
    {
        /* Stop application */
        M1_SetSpeed(0);
        M1_SetAppSwitch(0);
        bDemoModeSpeed = FALSE;
    }
    else
    {
        /* Start application */
        M1_SetAppSwitch(1);
        bDemoModeSpeed         = TRUE;
        ui32SpeedStimulatorCnt = 0;
    }

    /* Clear external interrupt flag. */
    GPIO_GpioClearInterruptFlags(BOARD_SW3_GPIO, 1U << BOARD_SW3_GPIO_PIN);

    SDK_ISR_EXIT_BARRIER;
}


/*!
 *@brief      Initialization of the Clocks and Pins
 *
 *@param      none
 *
 *@return     none
 */
static void BOARD_Init(void)
{

    /* Init pins set in pin_mux file */
    BOARD_InitBootPins();
    /* Init GPIO pins */
    BOARD_InitGPIO();

}


/*!
 * @brief   static void BOARD_InitGPIO(void)
 *           - Initialization of the GPIO peripherals
 *
 * @param   void
 *
 * @return  none
 */
static void BOARD_InitGPIO(void)
{
//    /* Define the init structure for the input switch pin */
//    gpio_pin_config_t sw_config = {
//        kGPIO_DigitalInput,
//        0,
//    };
//
//    /* Define the init structure for the output LED pin */
//    gpio_pin_config_t led_config = {
//        kGPIO_DigitalOutput,
//        0,
//    };
//
//    /* enable clock for GPIO*/
//    CLOCK_EnableClock(kCLOCK_Gpio0);
//    CLOCK_EnableClock(kCLOCK_Gpio3);
//
//    /* Init input switch GPIO. */
//    GPIO_SetPinInterruptConfig(BOARD_SW3_GPIO, BOARD_SW3_GPIO_PIN, kGPIO_InterruptFallingEdge);
//    GPIO_PinInit(BOARD_SW3_GPIO, BOARD_SW3_GPIO_PIN, &sw_config);
//
//    /* Init output LED GPIO. */
//    GPIO_PinInit(BOARD_LED_RED_GPIO, BOARD_LED_RED_GPIO_PIN, &led_config);
//    GPIO_PinInit(BOARD_LED_BLUE_GPIO, BOARD_LED_BLUE_GPIO_PIN, &led_config);
//    GPIO_PinInit(BOARD_LED_GREEN_GPIO, BOARD_LED_GREEN_GPIO_PIN, &led_config);
//
//    /* Enable GPIO pin interrupt for SW3 button */
//    EnableIRQ(BOARD_SW3_IRQ);
//    NVIC_SetPriority(BOARD_SW3_IRQ, BOARD_USER_BUTTON_PRIORITY);

}




