/*------------------------------------------------------*/
/* File:       Library for HD44780 compatible displays  */
/* Version:	   v4.15  						 			*/
/* Author:     Andrii Honcharenko						*/
/* 			   https://stm32withoutfear.blogspot.com	*/
/* Tested on:  STM32 HAL library, PSoC4 PDL2 library	*/
/* License:	   GNU LGPLv2.1		 		 	 			*/
/*------------------------------------------------------*/
/* Copyright (C)2021 AH All right reserved				*/
/*------------------------------------------------------*/

#ifndef PSOC4_DEVICE_H
#define PSOC4_DEVICE_H

//-------------------------------
/* SET LCD<->MCU CONNECTIONS */
//-------------------------------
#ifdef USE_I2C_BUS

//#define LCD_I2C_ADDRESS		(0x27U) //I2S address for LCD2004
#define LCD_I2C_ADDRESS		(0x3FU) //I2S address for LCD1602

#define LCD_I2C_ADDRESS_7B	(LCD_I2C_ADDRESS << 0)
#define PIN_RS    			(1 << 0) //The bit that matches the pin_rs on PCF8574
#define PIN_EN    			(1 << 2) //The bit that matches the pin_en on PCF8574
#define BACKLIGHT 			(1 << 3) //The bit that matches the pin_led on PCF8574

cy_stc_scb_i2c_context_t LCD_I2C_context; //I2C context structure

void LcdInit_I2C(void); //Prototype of I2C peripheral initialization function
uint8_t SendInternal_psoc4(uint8_t data, uint8_t flags); //Prototype of the function of sending data via I2C

//Structure for work on the I2C protocol
lcdI2C_ConfigStruct lcdConfig = {
		.InitPeriph = LcdInit_I2C,
		.SendData = SendInternal_psoc4,
		.bl_pin = (0 << BACKLIGHT),
		.rs_pin = PIN_RS,
		.en_pin = PIN_EN,
		.i2c_address = LCD_I2C_ADDRESS_7B
};

//I2C peripheral initialization function
void LcdInit_I2C(void)
{
	Cy_SCB_I2C_Init(LCD_I2C_HW, &LCD_I2C_config, &LCD_I2C_context);
	Cy_SCB_I2C_Enable(LCD_I2C_HW, &LCD_I2C_context);
}

//The function of sending data via I2C
uint8_t SendInternal_psoc4(uint8_t data, uint8_t flags)
{
	cy_en_scb_i2c_status_t res;

	uint8_t up = data & 0xF0;
	uint8_t lo = (data << 4) & 0xF0;

	uint8_t data_arr[4];
	data_arr[0] = up|flags|lcdConfig.bl_pin|lcdConfig.en_pin;
	data_arr[1] = up|flags|lcdConfig.bl_pin;
	data_arr[2] = lo|flags|lcdConfig.bl_pin|lcdConfig.en_pin;
	data_arr[3] = lo|flags|lcdConfig.bl_pin;

	Cy_SCB_I2C_MasterSendStart(LCD_I2C_HW, lcdConfig.i2c_address, CY_SCB_I2C_WRITE_XFER, BUSY_CYCLE_TIME, &LCD_I2C_context);

	for (int i = 0; i < 4; ++i)
	{
		res = Cy_SCB_I2C_MasterWriteByte(LCD_I2C_HW, data_arr[i], BUSY_CYCLE_TIME, &LCD_I2C_context);
	}

	Cy_SCB_I2C_MasterSendStop(LCD_I2C_HW, BUSY_CYCLE_TIME, &LCD_I2C_context);

	Cy_SysLib_Delay(BUSY_CYCLE_TIME);
	return res;
}

#else

void WriteEN(bool state) { Cy_GPIO_Write(LCD_E_PORT, LCD_E_PIN, state); }   // Set/Clr EN signal
void WriteRS(bool state) { Cy_GPIO_Write(LCD_RS_PORT, LCD_RS_PIN, state); } // Set/Clr RS signal
void WriteD7(bool state) { Cy_GPIO_Write(LCD_D7_PORT, LCD_D7_PIN, state); } // Set/Clr D7 signal
void WriteD6(bool state) { Cy_GPIO_Write(LCD_D6_PORT, LCD_D6_PIN, state); } // Set/Clr D6 signal
void WriteD5(bool state) { Cy_GPIO_Write(LCD_D5_PORT, LCD_D5_PIN, state); } // Set/Clr D5 signal
void WriteD4(bool state) { Cy_GPIO_Write(LCD_D4_PORT, LCD_D4_PIN, state); } // Set/Clr D4 signal

//Structure for work on the GPIO 4-bit protocol
fourBit_ConfigStruct lcdConfig = {
		.en_pin = WriteEN,
		.rs_pin = WriteRS,
		.d7_pin = WriteD7,
		.d6_pin = WriteD6,
		.d5_pin = WriteD5,
		.d4_pin = WriteD4,
		.mcuFreq = 48000000,
};

#endif /* USE_I2C_BUS */

#endif /* PSOC4_DEVICE_H */
