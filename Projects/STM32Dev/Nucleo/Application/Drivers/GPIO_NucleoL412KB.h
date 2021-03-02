/**	@defgroup	gpionucleo
	@brief		Implementation of the GPIO General Interface for STM32 Nucleo32 L412KB
	@details	v0.1
		All of the GPIO pins are configured by the ST Cube MX tool.  This will
		define their functionality.  This interface will allow interactions
		with all of the IO pins regardless of this configuration.  It is left
		to other portions of the application to account for the ST Cube MX pin
		configurations.

		NucleoL412KB
		With the PH3 pin set to output I could not get it to change state.
			Unsure if this is a settings issue or code problem.

	#File Information
		File:	GPIO_NucleoL412KB.h
		Author:	J. Beighel
		Date:	2021-03-01
*/

#ifndef __GPIONUCLEO
	#define __GPIONUCLEO

/***** Includes		*****/
	#define NUCLEO_TIMESRC_RTOS	1
	#define NUCLEO_TIMESRC_HAL	2

	#define NUCLEO_TIMESRC		NUCLEO_TIMESRC_RTOS

	#include "CommonUtils.h"
	#include "GPIOGeneralInterface.h"
	#include "TimeGeneralInterface.h"

	#include "main.h"
	#include "stm32l4xx.h"
	#include "i2c.h"
	#include "usart.h"

	#if NUCLEO_TIMESRC == NUCLEO_TIMESRC_RTOS
		#include "FreeRTOS.h"
		#include "task.h"
	#endif

/***** Definitions	*****/


/***** Constants	*****/
	#define GPIO_CAPS		((eGPIOCapabilities_t)GPIOCap_DigitalWrite | GPIOCap_DigitalRead)

	#define GPIO_INIT		NucleoGPIOPortInitialize

	#define GPIO_A_HWINFO	((void *)GPIOA)
	#define GPIO_B_HWINFO	((void *)GPIOB)
	#define GPIO_H_HWINFO	((void *)GPIOH)

	#define NUCLEO_GPIOCNT		16

	/**	@brief		Available bit depth for Analog Inputs
		@ingroup	gpionucleo
	*/
	#define NUCLEO_ADCBITDEPTH	12

	/**	@brief		Available bit depth for Analog Outputs
		@ingroup	gpionucleo
	*/
	#define NUCLEO_DACBITDEPTH	10

	/**	@brief		Available bit depth for PWM Outputs
		@ingroup	gpionucleo
	*/
	#define NUCLEO_PWMBITDEPTH	10

	#define TIME_INIT			NucleoTimeInitialize

	#define TIME_CAPS			((eTimeCapabilities_t)TimeCap_GetTicks | TimeCap_DelaySec | TimeCap_DelayMilliSec)

/***** Constants	*****/


/***** Globals		*****/


/***** Prototypes 	*****/
	eGPIOReturn_t NucleoGPIOPortInitialize(sGPIOIface_t *pIface, void *pHWInfo);
	
	eGPIOReturn_t NucleoGPIOSetModeByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, eGPIOModes_t eMode);

	eGPIOReturn_t NucleoGPIODigitalWriteByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, bool bState);
	
	eGPIOReturn_t NucleoGPIODigitalReadByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, bool *bState);

	eReturn_t NucleoTimeInitialize(sTimeIface_t *pIface);

	uint32_t NucleoGetCurrentTicks(void);

	eReturn_t NucleoTimeDelaySeconds(uint32_t nDelayAmount);

	eReturn_t NucleoTimeDelayMilliSeconds(uint32_t nDelayAmount);

/***** Functions	*****/

#endif
