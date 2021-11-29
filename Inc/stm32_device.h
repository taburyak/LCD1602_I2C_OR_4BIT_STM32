/*
 * stm32_device.h
 *
 *  Created on: Oct 13, 2021
 *      Author: honcharenkoa
 */

#ifndef _STM32_DEVICE_H_
#define _STM32_DEVICE_H_

#include "main.h"
#include "lcd_cfg.h"

#define GET_MCU_FREQ()	(HAL_RCC_GetHCLKFreq())

#ifdef USE_I2C_BUS



#else

#define SET(x,n)		(HAL_GPIO_WritePin(x, n, GPIO_PIN_SET))
#define CLR(x,n)		(HAL_GPIO_WritePin(x, n, GPIO_PIN_RESET))
#define GET(x,n)		(HAL_GPIO_ReadPin(x, n))

#endif /* USE_I2C_BUS */

#endif /* _STM32_DEVICE_H_ */
