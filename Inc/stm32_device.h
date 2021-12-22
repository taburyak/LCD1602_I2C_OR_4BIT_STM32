/*
 * stm32_device.h
 *
 *  Created on: Oct 13, 2021
 *      Author: honcharenkoa
 */

#ifndef _STM32_DEVICE_H_
#define _STM32_DEVICE_H_

#include "main.h"

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
		.bl_pin = (0 << BACKLIGHT),
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
	data_arr[0] = up|flags|lcdConfig.bl_pin|lcdConfig.en_pin;
	data_arr[1] = up|flags|lcdConfig.bl_pin;
	data_arr[2] = lo|flags|lcdConfig.bl_pin|lcdConfig.en_pin;
	data_arr[3] = lo|flags|lcdConfig.bl_pin;

	res = HAL_I2C_Master_Transmit(&LCD_I2C_PORT, lcdConfig.i2c_address, data_arr, sizeof(data_arr), HAL_MAX_DELAY);
	HAL_Delay(1);
	return res;
}

#else

//#define GET_MCU_FREQ()	(HAL_RCC_GetHCLKFreq())

void WriteEN(bool state) { HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin, state); }
void WriteRS(bool state) { HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, state); }
void WriteD7(bool state) { HAL_GPIO_WritePin(LCD_D7_GPIO_Port, LCD_D7_Pin, state); }
void WriteD6(bool state) { HAL_GPIO_WritePin(LCD_D6_GPIO_Port, LCD_D6_Pin, state); }
void WriteD5(bool state) { HAL_GPIO_WritePin(LCD_D5_GPIO_Port, LCD_D5_Pin, state); }
void WriteD4(bool state) { HAL_GPIO_WritePin(LCD_D4_GPIO_Port, LCD_D4_Pin, state); }

fourBit_ConfigStruct lcdConfig = {
		.en_pin = WriteEN,
		.rs_pin = WriteRS,
		.d7_pin = WriteD7,
		.d6_pin = WriteD6,
		.d5_pin = WriteD5,
		.d4_pin = WriteD4,
		.mcuFreq = 64000000,
};

#endif /* USE_I2C_BUS */

#endif /* _STM32_DEVICE_H_ */
