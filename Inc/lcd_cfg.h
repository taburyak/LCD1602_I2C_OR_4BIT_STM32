/*------------------------------------------------------*/
/* File:       Library for HD44780 compatible displays  */
/* Version:	   v3.00  						 			*/
/* Author:     GrAnd/www.MakeSystem.net					*/
/* 				https://stm32withoutfear.blogspot.com	*/
/* Tested on:  AVR, STM32F10X, STM32F4XX			 	*/
/* License:	   GNU LGPLv2.1		 		 	 			*/
/*------------------------------------------------------*/
/* Copyright (C)2014 GrAnd. All right reserved 			*/
/*------------------------------------------------------*/

#ifndef LCD_CFG_H
#define LCD_CFG_H

#include "stm32_device.h"

//#define USE_LCD2004
//#define USE_I2C_BUS

//-------------------------------
/* SET LCD<->MCU CONNECTIONS */
//-------------------------------
#ifdef USE_I2C_BUS

#define LCD_I2C_PORT		hi2c2
//#define LCD_I2C_ADDRESS		(0x27U)
#define LCD_I2C_ADDRESS		(0x3FU)

#define LCD_I2C_ADDRESS_8B	(LCD_I2C_ADDRESS << 1)
#define PIN_RS    			(1 << 0)
#define PIN_EN    			(1 << 2)
#define BACKLIGHT 			(1 << 3)

extern I2C_HandleTypeDef LCD_I2C_PORT;

extern uint8_t current_status_backlight;

uint8_t SendInternalCallback(uint8_t lcd_addr, uint8_t data, uint8_t flags)
{
    HAL_StatusTypeDef res;
    for(;;)
    {
        res = HAL_I2C_IsDeviceReady(&LCD_I2C_PORT, lcd_addr, 1, HAL_MAX_DELAY);
        if(res == HAL_OK)
            break;
    }

    uint8_t up = data & 0xF0;
    uint8_t lo = (data << 4) & 0xF0;

    uint8_t data_arr[4];
    data_arr[0] = up|flags|current_status_backlight|PIN_EN;
    data_arr[1] = up|flags|current_status_backlight;
    data_arr[2] = lo|flags|current_status_backlight|PIN_EN;
    data_arr[3] = lo|flags|current_status_backlight;

    res = HAL_I2C_Master_Transmit(&LCD_I2C_PORT, lcd_addr, data_arr, sizeof(data_arr), HAL_MAX_DELAY);
    HAL_Delay(BUSY_CYCLE_TIME);
    return res;
}

#else



#endif /* USE_I2C_BUS */

//-------------------------------
// DEFAULT CONFIGURATIONS
//-------------------------------
#define DEFAULT_DISPLAY_CONFIG		DISPLAY_CONFIG_4bit_2L_5x8
#define DEFAULT_ENTRY_MODE			ENTRY_MODE_INC_NO_SHIFT
#define DEFAULT_VIEW_MODE			VIEW_MODE_DispOn_BlkOff_CrsOff

//-------------------------------
// SET MCU TIMINGS
//-------------------------------
#define MCU_FREQ_VALUE				(GET_MCU_FREQ()/1000000U)	/* MHz. Minimal value = 1 MHz */
#define BUSY_CYCLE_TIME				(5u)						/* x 10us. See datasheet for minimal value. */
#define CLRSCR_CYCLE_TIME			(200u)						/* x 10us. See datasheet for minimal value. */
#define RETHOME_CYCLE_TIME			(200u)						/* x 10us. See datasheet for minimal value. */
#define INIT_CYCLE_TIME				(400u)
//-------------------------------
// CONFIGURE LCD WITH 4 LINES
//-------------------------------
#define START_ADDRESS_1st_LINE		(0x00u)
#define START_ADDRESS_2nd_LINE		(0x40u)
#ifdef USE_LCD2004
#define START_ADDRESS_3rd_LINE		(0x14u)
#define START_ADDRESS_4th_LINE		(0x54u)
#else
#define START_ADDRESS_3rd_LINE		(0x10u)
#define START_ADDRESS_4th_LINE		(0x50u)
#endif

//-------------------------------
// SET FORMATTED OUTPUT OPTIONS
//-------------------------------
#define USE_FORMATTED_OUTPUT	 	(1u)	/* 1 (true) or 0 (false) */
#define TAB_SPACE					(4u)	/* 1 .. 255 */

//-------------------------------
// PROGRESS BAR OPTIONS
//-------------------------------
#define USE_PROGRESS_BAR			(0u)			/* 1 (true) or 0 (false) */
#define USE_REGRESS_BAR				(1u)			/* 1 (true) or 0 (false) */
#define PROGRESS_BAR_LINE			LCD_2nd_LINE	/* Select lcd line: 1, 2, 3, 4, ... */
#define PROGRESS_BAR_HEIGHT			(5u)  			/* in pixel: 1(min), 2, 3, 4, 5, 6, 7, 8(max) */
#define PROGRESS_BAR_WIDTH			(10u) 			/* Number of chars in lcd line:  1, 2, .. , 8, 16, 20 */

#endif /* LCD_CFG_H */
//-------------------------------
/* THE END */
//-------------------------------
