/*
 * stm32_device.h
 *
 *  Created on: Oct 13, 2021
 *      Author: honcharenkoa
 */

#ifndef _STM32_DEVICE_H_
#define _STM32_DEVICE_H_

#include "main.h"

#define GET_MCU_FREQ()	(HAL_RCC_GetHCLKFreq())

//-------------------------------
/* SET LCD<->MCU CONNECTIONS */
//-------------------------------
#ifdef USE_I2C_BUS

#define LCD_I2C_PORT		hi2c1
//#define LCD_I2C_ADDRESS		(0x27U)
#define LCD_I2C_ADDRESS		(0x3FU)

#define LCD_I2C_ADDRESS_8B	(LCD_I2C_ADDRESS << 1)
#define PIN_RS    			(1 << 0)
#define PIN_EN    			(1 << 2)
#define BACKLIGHT 			(1 << 3)

extern I2C_HandleTypeDef LCD_I2C_PORT;

uint8_t sendInternal_stm32(uint8_t data, uint8_t flags);

lcdI2C_ConfigStruct lcdConfig = {
		.SendData = sendInternal_stm32,
		.backlight = (0 << BACKLIGHT),
		.rs_pin = PIN_RS,
		.en_pin = PIN_EN,
		.i2c_address = LCD_I2C_ADDRESS_8B
};

uint8_t sendInternal_stm32(uint8_t data, uint8_t flags)
{
	HAL_StatusTypeDef res;
	for(;;)
	{
		res = HAL_I2C_IsDeviceReady(&LCD_I2C_PORT, lcdConfig.i2c_address, 1, HAL_MAX_DELAY);
		if(res == HAL_OK)
			break;
	}

	uint8_t up = data & 0xF0;
	uint8_t lo = (data << 4) & 0xF0;

	uint8_t data_arr[4];
	data_arr[0] = up|flags|lcdConfig.backlight|lcdConfig.en_pin;
	data_arr[1] = up|flags|lcdConfig.backlight;
	data_arr[2] = lo|flags|lcdConfig.backlight|lcdConfig.en_pin;
	data_arr[3] = lo|flags|lcdConfig.backlight;

	res = HAL_I2C_Master_Transmit(&LCD_I2C_PORT, lcdConfig.i2c_address, data_arr, sizeof(data_arr), HAL_MAX_DELAY);
	HAL_Delay(1);
	return res;
}

#else

/* E - Starts data read/write. */
#define HD44780_E_OUT				(LCD_E_GPIO_Port)	/* Output register */
#define HD44780_E 	 				(LCD_E_Pin)			/* Pin number */

/* RS - Selects registers.
 * 0:	Instruction register (for write)
 * 0:	Busy flag + address counter (for read)
 * 1:	Data register (for write and read) */
#define HD44780_RS_OUT				(LCD_RS_GPIO_Port)	/* Output register */
#define HD44780_RS 					(LCD_RS_Pin)		/* Pin number */

/* R/W - Selects read or write.
 * 0:	Write
 * 1:	Read */
#define HD44780_RW_OUT				(LCD_RW_GPIO_Port)	/* Output register */
#define HD44780_RW					(LCD_RW_Pin)		/* Pin number */

/* DB4 to DB7 - Four high order bidirectional tristate data bus pins.
 * Used for data transfer and receive between the MPU and the HD44780U.
 * DB7 can be used as a busy flag. */
#define HD44780_D7_OUT				(LCD_D7_GPIO_Port)	/* Output register */
#define HD44780_D7 	 				(LCD_D7_Pin)		/* Pin number */

#define HD44780_D6_OUT				(LCD_D6_GPIO_Port)	/* Output register */
#define HD44780_D6					(LCD_D6_Pin)		/* Pin number */

#define HD44780_D5_OUT				(LCD_D5_GPIO_Port)	/* Output register */
#define HD44780_D5					(LCD_D5_Pin)		/* Pin number */

#define HD44780_D4_OUT				(LCD_D4_GPIO_Port)	/* Output register */
#define HD44780_D4					(LCD_D4_Pin)		/* Pin number */

#define SET(x,n)		(HAL_GPIO_WritePin(x, n, GPIO_PIN_SET))
#define CLR(x,n)		(HAL_GPIO_WritePin(x, n, GPIO_PIN_RESET))

#endif /* USE_I2C_BUS */

#endif /* _STM32_DEVICE_H_ */
