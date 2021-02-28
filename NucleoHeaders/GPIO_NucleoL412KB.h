/**	@defgroup	gpionucleo
	@brief		Implementation of the GPIO General Interface for STM32 Nucleo32 L412KB
	@details	v0.1
		All of the GPIO pins are configured by the ST Cube MX tool.  This will
		define their functionality.  This interface will allow interactions
		with all of the IO pins regardless of this configuration.  It is left
		to other portions of the application to account for the ST Cube MX pin
		configurations.
*/

#ifndef __GPIONUCLEO
	#define __GPIONUCLEO

/***** Includes		*****/
	#include "CommonUtils.h"
	#include "GPIOGeneralInterface.h"

	#include "main.h"
	#include "stm32l4xx.h"


/***** Definitions	*****/


/***** Constants	*****/
	#define GPIO_INIT			NucleoGPIOPortInitialize

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

/***** Constants	*****/


/***** Globals		*****/


/***** Prototypes 	*****/
	eGPIOReturn_t NucleoGPIOPortInitialize(sGPIOIface_t *pIface, void *pHWInfo);
	
	eGPIOReturn_t NucleoGPIOSetModeByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, eGPIOModes_t eMode);

	eGPIOReturn_t NucleoGPIODigitalWriteByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, bool bState);
	
	eGPIOReturn_t NucleoGPIODigitalReadByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, bool *bState);

/***** Functions	*****/

#endif
