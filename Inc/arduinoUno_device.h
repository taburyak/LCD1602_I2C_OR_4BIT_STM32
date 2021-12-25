#ifndef _ARDUINOUNO_DEVICE_H_
#define _ARDUINOUNO_DEVICE_H_

#include "Arduino.h"
#include "hd44780.h"

//-------------------------------
/* SET LCD<->MCU CONNECTIONS */
//-------------------------------
#ifdef USE_I2C_BUS

#include "Wire.h"

//#define LCD_I2C_ADDRESS		(0x27U) //I2S address for LCD2004
#define LCD_I2C_ADDRESS		(0x3FU) //I2S address for LCD1602

#define LCD_I2C_ADDRESS_7B	(LCD_I2C_ADDRESS << 0) //Without conversion, needed a 7-bit address
#define PIN_RS    			(1 << 0) //The bit that matches the pin_rs on PCF8574
#define PIN_EN    			(1 << 2) //The bit that matches the pin_en on PCF8574
#define BACKLIGHT 			(1 << 3) //The bit that matches the pin_led on PCF8574

void LcdInit_I2C(void); //Prototype of I2C peripheral initialization function
uint8_t SendInternal_arduino(uint8_t data, uint8_t flags); //Prototype of the function of sending data via I2C

//Structure for work on the I2C protocol
lcdI2C_ConfigStruct lcdConfig = {
		.en_pin = PIN_EN,
        .rs_pin = PIN_RS,
        .bl_pin = (0 << BACKLIGHT),
        .i2c_address = LCD_I2C_ADDRESS_7B,
        .SendData = SendInternal_arduino,
		.InitPeriph = LcdInit_I2C,
};

//I2C peripheral initialization function
void LcdInit_I2C(void)
{
	Wire.begin();
}

//The function of sending data via I2C
uint8_t SendInternal_arduino(uint8_t data, uint8_t flags)
{
	uint8_t res = 0;

	uint8_t up = data & 0xF0;
	uint8_t lo = (data << 4) & 0xF0;

	uint8_t data_arr[4];
	data_arr[0] = up|flags|lcdConfig.bl_pin|lcdConfig.en_pin;
	data_arr[1] = up|flags|lcdConfig.bl_pin;
	data_arr[2] = lo|flags|lcdConfig.bl_pin|lcdConfig.en_pin;
	data_arr[3] = lo|flags|lcdConfig.bl_pin;

	Wire.beginTransmission((int) lcdConfig.i2c_address);

	for (int i = 0; i < 4; ++i)
	{
		res = Wire.write(data_arr[i]);
	}

	Wire.endTransmission();

	delay(BUSY_CYCLE_TIME);
	return res;
}

#else

#define LCD_EN  9
#define LCD_RS  8
#define LCD_D7  7
#define LCD_D6  6
#define LCD_D5  5
#define LCD_D4  4

void initPin(void)
{
  pinMode(LCD_EN, OUTPUT);
  pinMode(LCD_RS, OUTPUT);
  pinMode(LCD_D7, OUTPUT);
  pinMode(LCD_D6, OUTPUT);
  pinMode(LCD_D5, OUTPUT);
  pinMode(LCD_D4, OUTPUT);
}

void WriteEN(bool state) { digitalWrite(LCD_EN, state); } // Set/Clr EN signal
void WriteRS(bool state) { digitalWrite(LCD_RS, state); } // Set/Clr RS signal
void WriteD7(bool state) { digitalWrite(LCD_D7, state); } // Set/Clr D7 signal
void WriteD6(bool state) { digitalWrite(LCD_D6, state); } // Set/Clr D6 signal
void WriteD5(bool state) { digitalWrite(LCD_D5, state); } // Set/Clr D5 signal
void WriteD4(bool state) { digitalWrite(LCD_D4, state); } // Set/Clr D4 signal

fourBit_ConfigStruct lcdConfig = {
  .en_pin = WriteEN,
  .rs_pin = WriteRS,
  .d7_pin = WriteD7,
  .d6_pin = WriteD6,
  .d5_pin = WriteD5,
  .d4_pin = WriteD4,
  .mcuFreq = 8000000,
  .InitPeriph = initPin,
};

#endif /* USE_I2C_BUS */

#endif /*  _ARDUINOUNO_DEVICE_H_ */
