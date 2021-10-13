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

//#include "stm32f1xx.h"
#include "main.h"

//#define USE_LCD2004
//#define USE_I2C_BUS

//-------------------------------
/* SET LCD<->MCU CONNECTIONS */
//-------------------------------
#ifdef USE_I2C_BUS

#define LCD_I2C_PORT		hi2c2
//#define LCD_I2C_ADDRESS		0x27
#define LCD_I2C_ADDRESS		0x3F

#define LCD_I2C_ADDRESS_8B	(LCD_I2C_ADDRESS << 1)
#define PIN_RS    			(1 << 0)
#define PIN_EN    			(1 << 2)
#define BACKLIGHT 			(1 << 3)

extern I2C_HandleTypeDef LCD_I2C_PORT;

#else

/* E - Starts data read/write. */
#define LCD_E_OUT					(LCD_E_GPIO_Port)	/* Output register */
#define LCD_E 	 					(LCD_E_Pin)			/* Pin number */

/* RS - Selects registers.
 * 0:	Instruction register (for write)
 * 0:	Busy flag + address counter (for read)
 * 1:	Data register (for write and read) */
#define LCD_RS_OUT					(LCD_RS_GPIO_Port)		/* Output register */
#define LCD_RS 						(LCD_RS_Pin)			/* Pin number */

/* R/W - Selects read or write.
 * 0:	Write
 * 1:	Read */
#define LCD_RW_OUT					LCD_RW_GPIO_Port->ODR	/* Output register */
#define LCD_RW						RW_PIN_NUMBER			/* Pin number */

/* DB4 to DB7 - Four high order bidirectional tristate data bus pins.
 * Used for data transfer and receive between the MPU and the HD44780U.
 * DB7 can be used as a busy flag. */
#define LCD_D7_OUT					(LCD_D7_GPIO_Port)		/* Output register */
#define LCD_D7_IN					(LCD_D7_GPIO_Port)		/* Input register */
#define LCD_D7 	 					(LCD_D7_Pin)			/* Pin number */

#define LCD_D6_OUT					(LCD_D6_GPIO_Port)		/* Output register */
#define LCD_D6						(LCD_D6_Pin)			/* Pin number */

#define LCD_D5_OUT					(LCD_D5_GPIO_Port)		/* Output register */
#define LCD_D5						(LCD_D5_Pin)			/* Pin number */

#define LCD_D4_OUT					(LCD_D4_GPIO_Port)		/* Output register */
#define LCD_D4						(LCD_D4_Pin)			/* Pin number */

#endif
//-------------------------------
// DEFAULT CONFIGURATIONS
//-------------------------------
#define DEFAULT_DISPLAY_CONFIG		DISPLAY_CONFIG_4bit_2L_5x8
#define DEFAULT_ENTRY_MODE			ENTRY_MODE_INC_NO_SHIFT
#define DEFAULT_VIEW_MODE			VIEW_MODE_DispOn_BlkOff_CrsOff
#define USE_BUSY_FLAG				0u	/* 1 (true) or 0 (false) */

//-------------------------------
// SET MCU TIMINGS
//-------------------------------
#define MCU_FREQ_VALUE				SystemCoreClock/1000000	/* MHz. Minimal value = 1 MHz */
#define BUSY_CYCLE_TIME				5u						/* x 10us. See datasheet for minimal value. */
#define CLRSCR_CYCLE_TIME			200u					/* x 10us. See datasheet for minimal value. */
#define RETHOME_CYCLE_TIME			200u					/* x 10us. See datasheet for minimal value. */
#define INIT_CYCLE_TIME				400u
//-------------------------------
// CONFIGURE LCD WITH 4 LINES
//-------------------------------
#define START_ADDRESS_1st_LINE		0x00u
#define START_ADDRESS_2nd_LINE		0x40u
#ifdef USE_LCD2004
#define START_ADDRESS_3rd_LINE		0x14u
#define START_ADDRESS_4th_LINE		0x54u
#else
#define START_ADDRESS_3rd_LINE		0x10u
#define START_ADDRESS_4th_LINE		0x50u
#endif

//-------------------------------
// SET FORMATTED OUTPUT OPTIONS
//-------------------------------
#define USE_FORMATTED_OUTPUT	 	1u	/* 1 (true) or 0 (false) */
#define TAB_SPACE					4u	/* 1 .. 255 */

//-------------------------------
// PROGRESS BAR OPTIONS
//-------------------------------
#define USE_PROGRESS_BAR			0u				/* 1 (true) or 0 (false) */
#define USE_REGRESS_BAR				1u				/* 1 (true) or 0 (false) */
#define PROGRESS_BAR_LINE			LCD_2nd_LINE	/* Select lcd line: 1, 2, 3, 4, ... */
#define PROGRESS_BAR_HEIGHT			5u  			/* in pixel: 1(min), 2, 3, 4, 5, 6, 7, 8(max) */
#define PROGRESS_BAR_WIDTH			10u 			/* Number of chars in lcd line:  1, 2, .. , 8, 16, 20 */

#ifndef USE_I2C_BUS
//-------------------------------
// LCDlib CALLBACKS
//-------------------------------
#if (USE_BUSY_FLAG)

#define Set_D7_as_Input()	\
	GPIOA->CRL &= 0xFFF0FFFFu;	\
	GPIOA->CRL |= 0x00040000u;

#define Set_D7_as_Outut()	\
	GPIOA->CRL &= 0xFFF0FFFFu;	\
	GPIOA->CRL |= 0x00030000u;

#endif /* USE_BUSY_FLAG */

void lcdGpioInit(void);
#define setAllPinsAsOutputs()	lcdGpioInit()
#endif

#endif /* LCD_CFG_H */

//-------------------------------
/* THE END */
//-------------------------------
