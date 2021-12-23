/*------------------------------------------------------*/
/* File:       Library for HD44780 compatible displays  */
/* Version:	   v3.00  						 			*/
/* Author:     GrAnd/www.MakeSystem.net					*/
/* 				https://stm32withoutfear.blogspot.com	*/
/* Tested on:  AVR, STM32F10X, STM32F4XX			 	*/
/* License:	   GNU LGPLv2.1		 		 	 			*/
/*------------------------------------------------------*/
/* Copyright (C)2021 And Hon All right reserved			*/
/*------------------------------------------------------*/
#include "hd44780.h"
#include "stddef.h"

/*!	\brief	Macro-definitions. */
#if (USE_PROGRESS_BAR)
/*!	\brief	Progress bar definitions. */
#define CGROM_PROGRESS_BAR_SIZE		6u
#define FULL_LOAD					5u
#define EMPTY_LOAD					0u
#define EMPTY_ROW					0x00u /* xxx00000 */

static const uint8_t progress_bar[CGROM_PROGRESS_BAR_SIZE] = {0x00u,0x10u,0x18u,0x1Cu,0x1Eu,0x1Fu};
static uint8_t current_bar_pixel;
static uint8_t current_cell_load;
static void lcdInitBar(void);
#endif

#ifdef USE_I2C_BUS
static lcdI2C_ConfigStruct* i2cConfig;
static uint8_t sendInternal(uint8_t data, uint8_t flags);
#else

#define DATA_7_MASK		0x80u
#define DATA_6_MASK		0x40u
#define DATA_5_MASK		0x20u
#define DATA_4_MASK		0x10u
#define DATA_3_MASK		0x08u
#define DATA_2_MASK		0x04u
#define DATA_1_MASK		0x02u
#define DATA_0_MASK		0x01u

#define HIGH	1u
#define LOW		0u

#define ENABLE_CYCLE_TIME	1u	/* Minimal value ~ 1us */
#define AC_UPDATE_TIME		4u	/* Minimal value ~ 4us */

static fourBit_ConfigStruct* fourBitConfig;

#define SET_EN()	(fourBitConfig->en_pin(HIGH))
#define CLR_EN()	(fourBitConfig->en_pin(LOW))
#define SET_RS()	(fourBitConfig->rs_pin(HIGH))
#define CLR_RS()	(fourBitConfig->rs_pin(LOW))
#define SET_D7()	(fourBitConfig->d7_pin(HIGH))
#define CLR_D7()	(fourBitConfig->d7_pin(LOW))
#define SET_D6()	(fourBitConfig->d6_pin(HIGH))
#define CLR_D6()	(fourBitConfig->d6_pin(LOW))
#define SET_D5()	(fourBitConfig->d5_pin(HIGH))
#define CLR_D5()	(fourBitConfig->d5_pin(LOW))
#define SET_D4()	(fourBitConfig->d4_pin(HIGH))
#define CLR_D4()	(fourBitConfig->d4_pin(LOW))
#define MCU_FREQ_VALUE (fourBitConfig->mcuFreq / 1000000U)

static void lcdWrite(uint8_t data);
static void lcdStrobe(void);
static void lcdHigh(uint8_t data);
static void lcdLow(uint8_t data);
static void lcd10usDelay(volatile uint32_t us);

#endif /* USE_I2C_BUS */

/*!	\brief	Low-level functions. */
static void lcdConfig(uint8_t param);
static uint32_t lcdPow10(uint8_t n);

#ifdef USE_I2C_BUS
static uint8_t sendInternal(uint8_t data, uint8_t flags)
{
	if(i2cConfig->SendData != NULL)
	{
		return i2cConfig->SendData(data, flags);
	}
	else
	{
		return 0;
	}
}
#else

/*!	\brief	Creates delay multiples of 10us. */
static void lcd10usDelay(volatile uint32_t us)
{
	/* Conversion to us */
	us *= MCU_FREQ_VALUE;
	/* Wait */
	while (us > 0u)
	{
		us--;
	}
}

/*!	\brief	Send data/commands to the display. */
static void lcdWrite(uint8_t data)
{/* Low level function. */
	lcdHigh(data);
	lcdStrobe();
	lcdLow(data);
	lcdStrobe();
	/* The busy flag must be checked after the 4-bit data has been transferred twice. */
	lcd10usDelay(BUSY_CYCLE_TIME);
}

/*!	\brief	Initiate the transfer of data/commands to LCD. */
static void lcdStrobe(void)
{/* Low level function. */
	SET_EN();
	lcd10usDelay(ENABLE_CYCLE_TIME);
	CLR_EN();
}

/*!	\brief	Send the msb nibble of the data / command to LCD. */
static void lcdHigh(uint8_t data)
{/* Low level function. */
	if(data & DATA_7_MASK) SET_D7(); else CLR_D7();
	if(data & DATA_6_MASK) SET_D6(); else CLR_D6();
	if(data & DATA_5_MASK) SET_D5(); else CLR_D5();
	if(data & DATA_4_MASK) SET_D4(); else CLR_D4();
}

/*!	\brief	Send the lsb nibble of the data / command to LCD. */
static void lcdLow(uint8_t data)
{/* Low level function. */
	if(data & DATA_3_MASK) SET_D7(); else CLR_D7();
	if(data & DATA_2_MASK) SET_D6(); else CLR_D6();
	if(data & DATA_1_MASK) SET_D5(); else CLR_D5();
	if(data & DATA_0_MASK) SET_D4(); else CLR_D4();
}
#endif

/*!	\brief	Initializing by instruction. 4-bit or i2c interface initialization. */
static void lcdConfig(uint8_t param)
{
/* Low level function. */
#ifdef USE_I2C_BUS
	sendInternal(param, 0);
#else
	/* Send commands to LCD. */
	CLR_RS();

	lcdHigh(param);
	lcdStrobe();		// Change 8-bit interface to 4-bit interface
	lcd10usDelay(BUSY_CYCLE_TIME);
	lcdStrobe();		/* DB7 to DB4 of the "Function set" instruction is written twice. */
	lcd10usDelay(BUSY_CYCLE_TIME);
	lcdLow(param);
	lcdStrobe();		// 4-bit, two lines, 5x8 pixel
	lcd10usDelay(BUSY_CYCLE_TIME);
	/* Note: The number of display lines and character font cannot be changed after this point. */
#endif
}

							//-----------------------------//
							/*         LCDlib API          */
							//-----------------------------//

/*!	\details	Clear display writes space code 20H into all DDRAM addresses.
 * 				It then sets DDRAM address 0 into the address counter,
 * 				and returns the display to its original status if it was shifted.
 * 				In other words, the display disappears and the cursor
 * 				or blinking goes to the left edge of the display (in the first line if 2 lines are displayed).
 * 				It also sets I/D to 1 (increment mode) in entry mode (S of entry mode does not change). */
void lcdClrScr(void)
{
	/* Send a command to LCD. */
#ifdef USE_I2C_BUS
	sendInternal(0x01u, 0);
#else
	CLR_RS();
	/* Clear screen */
	lcdWrite(0x01u);
	/* Busy delay */
	lcd10usDelay(CLRSCR_CYCLE_TIME);
#endif
}

/*!	\details	"Return home" sets DDRAM address 0 into the address counter,
 * 				and returns the display to its original status if it was shifted.
 * 				The DDRAM contents do not change.
 * 				The cursor or blinking go to the left edge of the display
 * 				(in the first line if 2 lines are displayed). */
void lcdReturn(void)
{
	/* Send a command to LCD. */
#ifdef USE_I2C_BUS
	sendInternal(0x02u, 0);
#else
	CLR_RS();
	/* Return home */
	lcdWrite(0x02u);
	/* Busy delay */
	lcd10usDelay(RETHOME_CYCLE_TIME);
#endif
}

/*!	\details	lcdScroll shifts the display to the right or left without writing or reading display data.
 * 				This function is used to correct or search the display.
 *	\note		The first and second line displays will shift at the same time. */
void lcdScroll(uint8_t direction)
{
	/* Send a command to LCD. */
#ifdef USE_I2C_BUS
	switch (direction)
	{
	/* To left */
		case LEFT  :
			sendInternal(0x18u, 0);
			break;

		/* To right */
		case RIGHT :
			sendInternal(0x1Cu, 0);
			break;

		default:
			/* Ignore this command */
			break;
	}
#else
	CLR_RS();
	/* Scroll display */
	switch (direction)
	{
	/* To left */
		case LEFT  :
			lcdWrite(0x18u);
			break;

		/* To right */
		case RIGHT :
			lcdWrite(0x1Cu);
			break;

		default:
			/* Ignore this command */
			break;
	}
#endif
}

/*!	\details	"Cursor shift" shifts the cursor position to the right or left,
 * 				without writing or reading display data.
 * 				This function is used to correct or search the display.
 * 				In a 2-line display, the cursor moves to the second line
 * 				when it passes the 40th digit of the first line. */
void cursorShift(uint8_t direction)
{
	/* Send a command to LCD. */
#ifdef USE_I2C_BUS

	switch (direction)
	{
		/* To left */
		case LEFT  :
			sendInternal(0x10u, 0);
			break;

		/* To right */
		case RIGHT :
			sendInternal(0x14u, 0);
			break;

		default:
			/* Ignore this command */
			break;
	}

#else
	CLR_RS();
	/* Shift cursor */
	switch (direction)
	{
		/* To left */
		case LEFT  :
			lcdWrite(0x10u);
			break;

		/* To right */
		case RIGHT :
			lcdWrite(0x14u);
			break;

		default:
			/* Ignore this command */
			break;
	}
#endif
}

/*!	\details	Go to the specified (DDRAM/CGRAM) memory address.*/
void lcdGoto(uint8_t line, uint8_t address)
{
	/* Send a command to LCD. */
#ifdef USE_I2C_BUS
	switch (line)
	{
		/* Set DDRAM address. */
		case LCD_1st_LINE: sendInternal(0x80u | START_ADDRESS_1st_LINE | address, 0); break;
		case LCD_2nd_LINE: sendInternal(0x80u | START_ADDRESS_2nd_LINE | address, 0); break;
		case LCD_3rd_LINE: sendInternal(0x80u | START_ADDRESS_3rd_LINE | address, 0); break;
		case LCD_4th_LINE: sendInternal(0x80u | START_ADDRESS_4th_LINE | address, 0); break;

		/* Set CGRAM address. */
		case CGRAM : sendInternal(0x40u | address, 0); break;

		default:
			/* Ignore this command */
			break;
	}
#else
	CLR_RS();
	/* Set DDRAM/CGRAM address. */
	switch (line)
	{
		/* Set DDRAM address. */
		case LCD_1st_LINE: lcdWrite(0x80u | START_ADDRESS_1st_LINE | address); break;
		case LCD_2nd_LINE: lcdWrite(0x80u | START_ADDRESS_2nd_LINE | address); break;
		case LCD_3rd_LINE: lcdWrite(0x80u | START_ADDRESS_3rd_LINE | address); break;
		case LCD_4th_LINE: lcdWrite(0x80u | START_ADDRESS_4th_LINE | address); break;
		/* Set CGRAM address. */
		case CGRAM : lcdWrite(0x40u | address); break;

		default:
			/* Ignore this command */
			break;
	}
#endif
}

/*!	\details	Change LCD settings. */
void lcdSetMode(uint8_t param)
{
	/* Send a command to LCD. */
#ifdef USE_I2C_BUS
	sendInternal(param, 0);
#else
	CLR_RS();
	lcdWrite(param);
#endif
}

/*!	\details	Write a single char to the current memory space (DDRAM/CGRAM). */
void lcdPutc(uint8_t data)
{
	/* Send data to LCD. */
#ifdef USE_I2C_BUS
	sendInternal(data, i2cConfig->rs_pin);
#else
	SET_RS();
	lcdWrite(data);
	/* Note:
	 * After execution of the CGRAM/DDRAM data write/read instruction, the RAM address counter is incremented
	 * or decremented by 1. The RAM address counter is updated after the busy flag turns off.
	 * tADD is the time elapsed after the busy flag turns off until the address counter is updated. */
	lcd10usDelay(AC_UPDATE_TIME);	/* Update RAM address counter delay. */
#endif
}

/*!	\details	Writes ANSI-C string to LCD (DDRAM memory space). */
//void lcdPuts(const uint8_t *str)
void lcdPuts(const char *str)
{
	/* Send a ANSI-C string to LCD. */
	while ('\0' != *str)
	{
#if ( USE_FORMATTED_OUTPUT )
		if(('\n' == *str))
		{/*New line */
			lcdGoto(LCD_2nd_LINE, 0u);
		}
		else if(('\r' == *str))
		{/* Return home */
			lcdReturn();
		}
		else if(('\t' == *str))
		{/* Tab space */
			uint8_t i;

			for(i=0u; i<TAB_SPACE; i++)
			{/* Shift cursor to the right. */
				cursorShift(RIGHT);
			}
		}
		else
#endif
		{
			/* Display a symbol. */
			lcdPutc(*str);
		}
		/* Get the next symbol. */
		str++;
	}
}

/*!	\details	Load the user-defined symbol into the CGRAM memory. */
void lcdLoadChar(uint8_t* vector, uint8_t position)
{
	uint8_t i;
	/* Go to the CGRAM memory space: 0 to 7 */
	lcdGoto(CGRAM, (position * FONT_HEIGHT));

	for(i = 0u; i < FONT_HEIGHT; i++)
	{/* Load one row of pixels into the CGRAM register. */
		lcdPutc(vector[i]);
	}

	/* Return to the DDRAM memory space. */
	lcdGoto(LCD_1st_LINE, 0u);
}

/*!	\details	Load and display the user-defined symbol. */
void lcdDrawChar( uint8_t* vector,
			   	  uint8_t position,
			   	  uint8_t line,
			   	  uint8_t address )
{
	/* Load the user-defined symbol into the CGRAM memory. */
	lcdLoadChar(vector, position);
	/* Select LCD position. */
	lcdGoto(line, address);
	/* Display the user-defined symbol. */
	lcdPutc(position);
}

/*!	\details	Erase a symbol from the left of the cursor. */
void lcdBackSpace(void)
{
	cursorShift(LEFT);
	lcdPutc(' ');
	cursorShift(LEFT);
}

/*!	\brief	Returns 10^n value. */
static uint32_t lcdPow10(uint8_t n)
{
	uint32_t retval = 1u;

	while (n > 0u)
	{
		retval *= 10u;
		n--;
	}

	return retval;
}

/*!	\brief	Display a integer number: +/- 2147483647. */
void lcdItos(int32_t value)
{
	int32_t i;

	if (value < 0)
	{
		lcdPutc('-');
		value = -value;
	}

	i = 1;
	while ((value / i) > 9)
	{
		i *= 10;
	}

	lcdPutc(value/i + '0');	/* Display at least one symbol */
	i /= 10;

	while (i > 0)
	{
		lcdPutc('0' + ((value % (i*10)) / i));
		i /= 10;
	}
}

/*!	\brief	Display a floating point number. */
void lcdFtos(float value, uint8_t n)
{
	if (value < 0.0)
	{
		lcdPutc('-');
		value = -value;
	}

	lcdItos((int32_t)value);

	if (n > 0u)
	{
		lcdPutc('.');

		lcdNtos((uint32_t)(value * (float)lcdPow10(n)), n);
	}
}

/*!	\brief	Display "n" right digits of "value". */
void lcdNtos(uint32_t value, uint8_t n)
{
	if (n > 0u)
	{
		uint32_t i = lcdPow10(n - 1u);

		while (i > 0u)	/* Display at least one symbol */
		{
			lcdPutc('0' + ((value/i) % 10u));

			i /= 10u;
		}
	}
}

#if ( USE_PROGRESS_BAR )
/*!	\brief	Initialize the progress bar
 * 			(i.e. preload elements of the progress bar into CGRAM and reset all variables). */
static void lcdInitBar(void)
{
	uint8_t i, j;

	for (i = 0u; i < CGROM_PROGRESS_BAR_SIZE; i++)
	{
		lcdGoto(CGRAM, (i * FONT_HEIGHT));

		for (j = 0u; j < FONT_HEIGHT; j++)
		{
			if (j < PROGRESS_BAR_HEIGHT)
			{
				lcdPutc(progress_bar[i]);
			}
			else
			{/* Load an empty row of pixels in CGRAM. */
				lcdPutc(EMPTY_ROW);
			}
		}
	}

	/* Clear the entire bar and initialize all variables. */
	lcdClrBar();
}

/*!	\brief	Draw progress bar. */
void lcdDrawBar(uint8_t next_bar_pixel)
{
	/* Go to the current cell position in the progress bar. */
	lcdGoto(PROGRESS_BAR_LINE, (current_bar_pixel / FONT_WIDTH));

	if (next_bar_pixel > current_bar_pixel)
	{
		/* Increment LCD cursor */
		lcdSetMode(ENTRY_MODE_INC_NO_SHIFT);

		/* Prevent the progress bar overflow */
		if (next_bar_pixel > PROGRESS_BAR_MAX_LOAD)
		{
			next_bar_pixel = PROGRESS_BAR_MAX_LOAD;
		}

		while (current_bar_pixel != next_bar_pixel)
		{
			/* Go to the next pixel. */
			current_bar_pixel++;
			current_cell_load++;
			/* Display the load of the current cell. */
			lcdPutc(current_cell_load);

			if (current_cell_load < FULL_LOAD)
			{/* Return the cursor to the current cell. */
				cursorShift(LEFT);
			}
			else
			{/* Go to the next cell. */
				current_cell_load = EMPTY_LOAD;
			}
		}
	 }
#if (USE_REGRESS_BAR)
	 else if (next_bar_pixel < current_bar_pixel)
	 {
		 /* Decrement LCD cursor */
		lcdSetMode(ENTRY_MODE_DEC_NO_SHIFT);

		do
		{
			if (EMPTY_LOAD == current_cell_load)
			{/* Go to the next cell. */
				cursorShift(LEFT);
				current_cell_load = FULL_LOAD;
			}
			/* Go to the next pixel. */
			current_bar_pixel--;
			current_cell_load--;
			/* Display the load of the current cell. */
			lcdPutc(current_cell_load);
			/* Return the cursor to the current cell. */
			cursorShift(RIGHT);
		}
		while (current_bar_pixel != next_bar_pixel);
	 }
#endif /* USE_REGRESS_BAR */
	 else
	 {
		 /* Nothing to do. */
	 }

	/* Restore the default entry mode. */
	lcdSetMode(DEFAULT_ENTRY_MODE);
	/* Return home. */
	lcdGoto(LCD_1st_LINE, 0u);
}

/*!	\brief	Clear the entire progress bar. */
void lcdClrBar(void)
{
	uint8_t i;
	/* Go to the last cell in the progress bar. */
	lcdGoto(PROGRESS_BAR_LINE, (PROGRESS_BAR_WIDTH - 1u));
	/* Set the decrement mode. */
	lcdSetMode(ENTRY_MODE_DEC_NO_SHIFT);

	for(i = 0u; i < PROGRESS_BAR_WIDTH; i++)
	{/* Display the "empty cell" symbol (i.e. clear the LCD cell). */
		lcdPutc(EMPTY_LOAD);
	}

	/* Reset the progress bar variables. */
	current_bar_pixel = 0u;
	current_cell_load = EMPTY_LOAD;

	/* Restore the default entry mode. */
	lcdSetMode(DEFAULT_ENTRY_MODE);
	/* Return home. */
	lcdGoto(LCD_1st_LINE, 0u);
}
#endif
/*!	\brief	Initialize the LCD.
 * 	\note	This library use the I2C interface. */
#ifdef USE_I2C_BUS
void lcdInit(void* config)
{
	i2cConfig = (lcdI2C_ConfigStruct*) config;
	if(i2cConfig->InitPeriph != NULL)
	{
		i2cConfig->InitPeriph();
	}
#ifdef USE_LCD2004

#endif /* USE_LCD2004 */
	lcdConfig(DEFAULT_DISPLAY_CONFIG);
	lcdSetMode(DEFAULT_VIEW_MODE);
	lcdSetMode(DEFAULT_ENTRY_MODE);
	lcdClrScr();
	lcdReturn();
	#if (USE_PROGRESS_BAR)
		lcdInitBar();
	#endif
}

void lcdBackLightOn(void)
{
	i2cConfig->bl_pin = (1 << 3);
	sendInternal(0x0Fu, 0);
}

void lcdBackLightOff(void)
{
	i2cConfig->bl_pin = (0 << 3);
	sendInternal(0x0Fu, 0);
}
#else /* USE_I2C_BUS */
/*!	\brief	Initialize the LCD.
 * 	\note	This library use the 4-bit interface. */
void lcdInit(void* config)
{
	/* Peripheral initialization. */
	fourBitConfig = (fourBit_ConfigStruct*) config;

	if(fourBitConfig->InitPeriph != NULL)
	{
		fourBitConfig->InitPeriph();
	}
	/* LCD initialization. */
	lcdWrite(0x30);
	lcd10usDelay(INIT_CYCLE_TIME);
	lcdWrite(0x30);
	lcd10usDelay(INIT_CYCLE_TIME);
#ifdef USE_LCD2004
	lcdWrite(0x03);
	lcd10usDelay(INIT_CYCLE_TIME);
	lcdWrite(0x03);
	lcd10usDelay(INIT_CYCLE_TIME);
	lcdWrite(0x03);
	lcd10usDelay(INIT_CYCLE_TIME);
	lcdWrite(0x02);
	lcd10usDelay(INIT_CYCLE_TIME);
#endif
	lcdConfig(DEFAULT_DISPLAY_CONFIG);
	lcdSetMode(DEFAULT_VIEW_MODE);
	lcdSetMode(DEFAULT_ENTRY_MODE);
	lcdClrScr();
	lcdReturn();
#if (USE_PROGRESS_BAR)
		lcdInitBar();
#endif
}
#endif /* USE_I2C_BUS */
//-------------------------------
/* END OF FILE */
//-------------------------------
