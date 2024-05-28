/*
 * onsemi_hardware.c
 *
 *  Created on: Feb 9, 2024
 *      Author: Stanley.Ezeh
 */

#include "onsemi_hardware.h"
#include "pin_mux.h"
#include "fsl_gpio.h"
#include "fsl_lpspi.h"
#include "peripherals.h"
#include "T1S_Hardware.h"

static ISR_t dataReadyCallback = NULL;

// -> Global variables for GPIO  ... DO not modify .struct here. Do so using config tools
NCN_PinDef_t pcs_pin = { .base = NCN_PERIPHERAL_INIT_IRQ_Pin_GPIO, .pin =
		NCN_PERIPHERAL_INIT_PCS_Pin_PIN, .mask =
		NCN_PERIPHERAL_INIT_PCS_Pin_PIN_MASK };

NCN_PinDef_t reset_pin = { .base = NCN_PERIPHERAL_INIT_RESET_Pin_GPIO, .pin =
		NCN_PERIPHERAL_INIT_RESET_Pin_PIN, .mask =
		NCN_PERIPHERAL_INIT_RESET_Pin_PIN_MASK };

// -> Modify GPIO State
void NCN_SetPinState(NCN_PinDef_t pindef, NCN_PinState_t state) {
	GPIO_PinWrite(pindef.base, pindef.pin, (uint8_t) state);
}

void NCN_TogglePinState(NCN_PinDef_t pindef) {
	GPIO_PortToggle(pindef.base, pindef.mask);
}

// -> CTIMER4 Timebase  definitions
volatile uint32_t g_uwtick = 20U;

static void IncTick(void) {
	g_uwtick++;
}

void SetTick(uint32_t tick){
	g_uwtick=tick;
}

//#define SE_DEBUG
void NCN_Timebase_Callback(uint32_t flags) {
	IncTick();
#ifdef SE_DEBUG
	if (g_uwtick % 5000 == 0)
		PRINTF("HTime : %4d s \n", g_uwtick / 1000);
#endif //SE_DEBUG
}

inline uint32_t GetTick(void) {
	return g_uwtick;
}

void DelayMs(uint32_t ms) {
	uint32_t delay = ms + GetTick();
	while (delay > GetTick())
		;
}

/* GPIO00_IRQn interrupt handler */
void NCN_IRQ_Handler(void) {
	/* Get pin flags 0 */
	uint32_t pin_flags0 = GPIO_GpioGetInterruptChannelFlags(GPIO0, 0U);

	/* Place your interrupt code here */

	/* Clear pin flags 0 */
	GPIO_GpioClearInterruptChannelFlags(GPIO0, pin_flags0, 0U);
//	PRINTF("IRQ called at: %6d \n", GetTick());
	if (dataReadyCallback != NULL )
	{
		dataReadyCallback(0); // note tick is not implemented
	}
	/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F
	 Store immediate overlapping exception return operation might vector to incorrect interrupt. */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
  #endif
    SDK_ISR_EXIT_BARRIER;
}

// -> SPI Communication
lpspi_transfer_t transferInfo;  // transfer structure
void NCN_SPI_Transfer(uint8_t *rx_buffer, uint8_t *tx_buffer, uint16_t num_bytes_to_transfer) {
	uint16_t data_left = num_bytes_to_transfer;
	uint32_t fifo_size = LPSPI_GetRxFifoSize(NCN_SPI_PERIPHERAL);

	// first assert the PCS PIN
	NCN_SetPinState(pcs_pin, OFF);
	do {

		// Data length estimation
		num_bytes_to_transfer = data_left;
		if (num_bytes_to_transfer > (fifo_size))
			num_bytes_to_transfer = (fifo_size);

		// Update transfer structure
		transferInfo.txData = tx_buffer;
		transferInfo.rxData = rx_buffer;
		transferInfo.dataSize = num_bytes_to_transfer;

		// Transfer data part
		LPSPI_MasterTransferBlocking(NCN_SPI_PERIPHERAL, &transferInfo);
		// Data pointers correction
		tx_buffer += num_bytes_to_transfer;
		rx_buffer += num_bytes_to_transfer;

		// Data length correction
		data_left -= num_bytes_to_transfer;
	} while (data_left);

	// de-assert the PCS PIN
	NCN_SetPinState(pcs_pin, ON);
}


// -> NCN Specific

uint32_t SPI_Init(){

	return SUCCESS;
}


uint32_t SetDataReadyISR(ISR_t callback)
{
    if (callback != NULL){
    	dataReadyCallback = callback;
    }
	return SUCCESS;
}


uint32_t SPI_Cleanup()
{
	return SUCCESS;
}


uint32_t SleepUntilISR()
{
	return SUCCESS;
}

uint32_t SPI_Transfer(uint8_t* rx_buffer, uint8_t* tx_buffer, uint16_t num_bytes_to_transfer){
	NCN_SPI_Transfer(rx_buffer, tx_buffer, num_bytes_to_transfer);
	return SUCCESS;
}
