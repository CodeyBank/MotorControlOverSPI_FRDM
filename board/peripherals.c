/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Peripherals v14.0
processor: MCXN947
package_id: MCXN947VDF
mcu_data: ksdk2_0
processor_version: 15.0.0
board: FRDM-MCXN947
functionalGroups:
- name: BOARD_InitPeripherals
  UUID: e5d1b37d-2343-4b4d-97fe-f554381dd2a6
  selectedCore: cm33_core0
- name: NCN_InitPeripherals
  UUID: adc2bdb1-9329-4a08-b5c0-ffe8a3fe333c
  called_from_default_init: true
  selectedCore: cm33_core0
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/

/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
component:
- type: 'system'
- type_id: 'system_54b53072540eeeb8f8e9343e71f28176'
- global_system_definitions:
  - user_definitions: ''
  - user_includes: ''
  - global_init: ''
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/

/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
component:
- type: 'uart_cmsis_common'
- type_id: 'uart_cmsis_common_9cb8e302497aa696fdbb5a4fd622c2a8'
- global_USART_CMSIS_common:
  - quick_selection: 'default'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/

/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
component:
- type: 'gpio_adapter_common'
- type_id: 'gpio_adapter_common_57579b9ac814fe26bf95df0a384c36b6'
- global_gpio_adapter_common:
  - quick_selection: 'default'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

/***********************************************************************************************************************
 * Included files
 **********************************************************************************************************************/
#include "peripherals.h"

/***********************************************************************************************************************
 * BOARD_InitPeripherals functional group
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * NVIC initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'NVIC'
- type: 'nvic'
- mode: 'general'
- custom_name_enabled: 'false'
- type_id: 'nvic_57b5eef3774cc60acaede6f5b8bddc67'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'NVIC'
- config_sets:
  - nvic:
    - interrupt_table:
      - 0: []
      - 1: []
    - interrupts: []
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

/* Empty initialization function (commented out)
static void NVIC_init(void) {
} */

/***********************************************************************************************************************
 * NCN_InitPeripherals functional group
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * NVIC_2 initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'NVIC_2'
- type: 'nvic'
- mode: 'general'
- custom_name_enabled: 'false'
- type_id: 'nvic_57b5eef3774cc60acaede6f5b8bddc67'
- functional_group: 'NCN_InitPeripherals'
- peripheral: 'NVIC'
- config_sets:
  - nvic:
    - interrupt_table:
      - 0: []
      - 1: []
    - interrupts: []
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

/* Empty initialization function (commented out)
static void NVIC_2_init(void) {
} */

/***********************************************************************************************************************
 * TimeBase initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'TimeBase'
- type: 'ctimer'
- mode: 'Capture_Match'
- custom_name_enabled: 'true'
- type_id: 'ctimer_44573e4bbd77c18d33bceb2e7900a074'
- functional_group: 'NCN_InitPeripherals'
- peripheral: 'CTIMER4'
- config_sets:
  - fsl_ctimer:
    - ctimerConfig:
      - mode: 'kCTIMER_TimerMode'
      - clockSource: 'FunctionClock'
      - clockSourceFreq: 'BOARD_BootClockPLL150M'
      - timerPrescaler: '150'
    - EnableTimerInInit: 'true'
    - matchChannels:
      - 0:
        - matchChannelPrefixId: 'Match_0'
        - matchChannel: 'kCTIMER_Match_0'
        - matchValueStr: '1000'
        - enableCounterReset: 'true'
        - enableCounterStop: 'false'
        - outControl: 'kCTIMER_Output_NoAction'
        - outPinInitValue: 'low'
        - enableInterrupt: 'true'
    - captureChannels: []
    - interruptCallbackConfig:
      - interrupt:
        - IRQn: 'CTIMER4_IRQn'
        - enable_priority: 'false'
        - priority: '0'
      - callback: 'kCTIMER_SingleCallback'
      - singleCallback: 'NCN_Timebase_Callback'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */
const ctimer_config_t TimeBase_config = {
  .mode = kCTIMER_TimerMode,
  .input = kCTIMER_Capture_0,
  .prescale = 149
};
const ctimer_match_config_t TimeBase_Match_0_config = {
  .matchValue = 999,
  .enableCounterReset = true,
  .enableCounterStop = false,
  .outControl = kCTIMER_Output_NoAction,
  .outPinInitState = false,
  .enableInterrupt = true
};
/* Single callback functions definition */
ctimer_callback_t TimeBase_callback[] = {NCN_Timebase_Callback};

static void TimeBase_init(void) {
  /* CTIMER4 peripheral initialization */
  CTIMER_Init(TIMEBASE_PERIPHERAL, &TimeBase_config);
  /* Match channel 0 of CTIMER4 peripheral initialization */
  CTIMER_SetupMatch(TIMEBASE_PERIPHERAL, TIMEBASE_MATCH_0_CHANNEL, &TimeBase_Match_0_config);
  CTIMER_RegisterCallBack(TIMEBASE_PERIPHERAL, TimeBase_callback, kCTIMER_SingleCallback);
  /* Start the timer */
  CTIMER_StartTimer(TIMEBASE_PERIPHERAL);
}

/***********************************************************************************************************************
 * NCN_IRQ_Interrupt initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'NCN_IRQ_Interrupt'
- type: 'gpio'
- mode: 'GPIO'
- custom_name_enabled: 'true'
- type_id: 'gpio_bdab6a68c04595b6073dff48897d210f'
- functional_group: 'NCN_InitPeripherals'
- peripheral: 'GPIO0'
- config_sets:
  - fsl_gpio:
    - enable_irq: 'true'
    - port_interrupt:
      - IRQn: 'GPIO00_IRQn'
      - enable_interrrupt: 'enabled'
      - enable_priority: 'true'
      - priority: '2'
      - enable_custom_name: 'true'
      - handler_custom_name: 'NCN_IRQ_Handler'
    - enable_irq_1: 'false'
    - gpio_interrupt_1:
      - IRQn: 'GPIO01_IRQn'
      - enable_interrrupt: 'enabled'
      - enable_priority: 'false'
      - priority: '1'
      - enable_custom_name: 'false'
    - enable_irq_EFT: 'false'
    - port_interrupt_EFT:
      - IRQn: 'PORT_EFT_IRQn'
      - enable_interrrupt: 'enabled'
      - enable_priority: 'false'
      - priority: '2'
      - enable_custom_name: 'false'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

static void NCN_IRQ_Interrupt_init(void) {
  /* Make sure, the clock gate for port 0 is enabled (e. g. in pin_mux.c) */
  /* Interrupt vector GPIO00_IRQn priority settings in the NVIC. */
  NVIC_SetPriority(NCN_IRQ_INTERRUPT_INT_0_IRQN, NCN_IRQ_INTERRUPT_INT_0_IRQ_PRIORITY);
  /* Enable interrupt GPIO00_IRQn request in the NVIC. */
  EnableIRQ(NCN_IRQ_INTERRUPT_INT_0_IRQN);
}

/***********************************************************************************************************************
 * NCN_SPI initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'NCN_SPI'
- type: 'lpflexcomm_lpspi'
- mode: 'polling'
- custom_name_enabled: 'true'
- type_id: 'lpflexcomm_lpspi_dcd1cd00db41d7f526ea586b769eb79b'
- functional_group: 'NCN_InitPeripherals'
- peripheral: 'LP_FLEXCOMM0'
- config_sets:
  - main:
    - mode: 'kLPSPI_Master'
    - clockSource: 'clk_src0'
    - clockSourceFreq: 'BOARD_BootClockPLL150M'
    - master:
      - baudRateBps: '12000000'
      - bitsPerFrame: '8'
      - cpol: 'kLPSPI_ClockPolarityActiveHigh'
      - cpha: 'kLPSPI_ClockPhaseFirstEdge'
      - direction: 'kLPSPI_MsbFirst'
      - pcsToSckDelayInNanoSec: '1000'
      - lastSckToPcsDelayInNanoSec: '1000'
      - betweenTransferDelayInNanoSec: '1000'
      - whichPcs: 'kLPSPI_Pcs0'
      - pcsActiveHighOrLow: 'kLPSPI_PcsActiveLow'
      - pinCfg: 'kLPSPI_SdiInSdoOut'
      - pcsFunc: 'kLPSPI_PcsAsCs'
      - dataOutConfig: 'kLpspiDataOutRetained'
      - enableInputDelay: 'false'
    - set_FifoWaterMarks: 'false'
    - fifoWaterMarks:
      - txWatermark: '0'
      - rxWatermark: '0'
    - allPcsPolarityEnable: 'false'
    - allPcsPolarity:
      - kLPSPI_Pcs1Active: 'kLPSPI_PcsActiveLow'
      - kLPSPI_Pcs2Active: 'kLPSPI_PcsActiveLow'
      - kLPSPI_Pcs3Active: 'kLPSPI_PcsActiveLow'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */
const lpspi_master_config_t NCN_SPI_config = {
  .baudRate = 12000000UL,
  .bitsPerFrame = 8UL,
  .cpol = kLPSPI_ClockPolarityActiveHigh,
  .cpha = kLPSPI_ClockPhaseFirstEdge,
  .direction = kLPSPI_MsbFirst,
  .pcsToSckDelayInNanoSec = 1000UL,
  .lastSckToPcsDelayInNanoSec = 1000UL,
  .betweenTransferDelayInNanoSec = 1000UL,
  .whichPcs = kLPSPI_Pcs0,
  .pcsActiveHighOrLow = kLPSPI_PcsActiveLow,
  .pinCfg = kLPSPI_SdiInSdoOut,
  .pcsFunc = kLPSPI_PcsAsCs,
  .dataOutConfig = kLpspiDataOutRetained,
  .enableInputDelay = false
};

static void NCN_SPI_init(void) {
  LPSPI_MasterInit(NCN_SPI_PERIPHERAL, &NCN_SPI_config, NCN_SPI_CLOCK_FREQ);
}

/***********************************************************************************************************************
 * Initialization functions
 **********************************************************************************************************************/
void BOARD_InitPeripherals(void)
{
  /* Initialize components */
}

void NCN_InitPeripherals(void)
{
  /* Initialize components */
  TimeBase_init();
  NCN_IRQ_Interrupt_init();
  NCN_SPI_init();
}

/***********************************************************************************************************************
 * BOARD_InitBootPeripherals function
 **********************************************************************************************************************/
void BOARD_InitBootPeripherals(void)
{
  NCN_InitPeripherals();
}
