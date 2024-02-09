/*
 * onsemi_hardware.h
 *
 *  Created on: Feb 9, 2024
 *      Author: Stanley.Ezeh
 */

#ifndef ONSEMI_HARDWARE_H_
#define ONSEMI_HARDWARE_H_

#include "fsl_debug_console.h"
#include "MCXN947_cm33_core0.h"

typedef struct{
	GPIO_Type *base;
	uint32_t pin;
	uint32_t mask;
}NCN_PinDef_t;

typedef enum{
	OFF, ON
}NCN_PinState_t;

// custom error codes
enum NCNERROR {
	SUCCESS = 0U,
	FAIL
};


// -> GPIO Modifications
void NCN_SetPinState(NCN_PinDef_t pindef, NCN_PinState_t state);
void NCN_TogglePinState(NCN_PinDef_t pindef);

// -> SPI Transfer
void NCN_SPI_Transfer(uint8_t* rx_buffer, uint8_t* tx_buffer, uint16_t num_bytes_to_transfer);

// ->Timer
void DelayMs(uint32_t ms);

#endif /* ONSEMI_HARDWARE_H_ */
