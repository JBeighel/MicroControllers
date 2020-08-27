/**	@defgroup	gpionucleo
	@brief		Implementation of the GPIO General Interface for STM32 Nucleo32 L412KB
	@details	v0.1
		
*/

#ifndef __GPIONUCLEO
	#define __GPIONUCLEO

/***** Includes		*****/
	#include "CommonUtils.h"
	#include "GPIOGeneralInterface.h"

	#include "main.h"
	#include "stm32l4xx.h"


/***** Definitions	*****/
	#define NUCLEO_PININFOTYPE	uint16_t

/***** Constants	*****/
	/**	@brief		Default GPIO hardware information object
		@ingroup	gpionucleo
	*/
	#define GPIO_B_HWINFO		((void *)GPIOB)
	
	#define GPIO_INIT		NucleoGPIOPortInitialize
	
	#define DELAYMILLISEC		osDelay
	
	//#define DELAYMICROSEC		delayMicroseconds

	/**	@brief		Count of available GPIO pins
		@ingroup	gpionucleo
	*/
	#define NUCLEO_GPIOCNT		(sizeof(gNucleoGPIOList) / sizeof(NUCLEO_PININFOTYPE))

	#define NUCLEO_DACCNT		(sizeof(gNucleoDACList) / sizeof(NUCLEO_PININFOTYPE))

	#define NUCLEO_ADCCNT		(sizeof(gNucleoADCList) / sizeof(NUCLEO_PININFOTYPE))

	#define NUCLEO_PWMCNT		(sizeof(gNucleoPWMList) / sizeof(NUCLEO_PININFOTYPE))

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
