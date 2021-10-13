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

#ifndef HD44780_H
#define HD44780_H

#include "lcd_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

//-------------------------------
// CONSTANTS
//-------------------------------
#define CGRAM	 	   	  		 		5u	/* used in "lcdGoto(CGRAM,address)" function */
#define RIGHT   	  					'>'
#define LEFT       			  			'<'
#define LCD_1st_LINE					1u
#define LCD_2nd_LINE					2u
#define LCD_3rd_LINE					3u
#define LCD_4th_LINE					4u
/* 5x8 pixel font only */
#define FONT_HEIGHT    					8u	/* 8 pixels */
#define FONT_WIDTH						5u	/* 5 pixels */
#define PROGRESS_BAR_MAX_LOAD			(PROGRESS_BAR_WIDTH * FONT_WIDTH) /* 100% full */

//-------------------------------
// DISPLAY CONFIG MACROS
//-------------------------------
#define DISPLAY_CONFIG_4bit_2L_5x8		0x28u /* Use 4-bit interface, 2 or 4 Lines, 5x8 pixel resolution */
#define DISPLAY_CONFIG_4bit_1L_5x8		0x20u /* Use 4-bit interface, 1 Line, 5x8 pixel resolution */

//-------------------------------
// ENTRY MODE MACROS
//-------------------------------
#define ENTRY_MODE_DEC_NO_SHIFT			0x04u
#define ENTRY_MODE_DEC_WITH_SHIFT     	0x05u
#define ENTRY_MODE_INC_NO_SHIFT	      	0x06u
#define ENTRY_MODE_INC_WITH_SHIFT     	0x07u

//-------------------------------
// VIEW_MODE MACROS
//-------------------------------
#define VIEW_MODE_DispOn_BlkOn_CrsOn	0x0Fu /* Display On, Blink On, Cursor On */
#define VIEW_MODE_DispOn_BlkOff_CrsOn	0x0Eu /* Display On, Blink Off, Cursor On */
#define VIEW_MODE_DispOn_BlkOn_CrsOff	0x0Du /* Display On, Blink On, Cursor Off */
#define VIEW_MODE_DispOn_BlkOff_CrsOff	0x0Cu /* Display On, Blink Off, Cursor Off */
#define VIEW_MODE_DispOff_BlkOff_CrsOff	0x08u /* Display Off, Blink Off, Cursor Off */

//-------------------------------
// LCDlib API
//-------------------------------
extern void lcdClrScr(void);
extern void lcdReturn(void);
extern void lcdSetMode(uint8_t param);
extern void lcdGoto(uint8_t line, uint8_t address);
extern void lcdPuts(const char *str);
extern void lcdPutc(uint8_t data);
extern void lcdLoadChar(uint8_t *vector, uint8_t position);
extern void lcdDrawChar(uint8_t *vector, uint8_t position, uint8_t line, uint8_t address);
extern void lcdBackSpace(void);
extern void lcdScroll(uint8_t direction);
extern void cursorShift(uint8_t direction);
extern void lcdItos(int32_t value);
extern void lcdFtos(float value, uint8_t n);
extern void lcdNtos(uint32_t value, uint8_t n);
extern void lcdDrawBar(uint8_t data);
extern void lcdClrBar(void);
extern void lcdInit(void);

#ifdef USE_I2C_BUS
extern void lcdBackLightOn(void);
extern void lcdBackLightOff(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* HD44780_H */

//-------------------------------
/* END OF FILE */
//-------------------------------
