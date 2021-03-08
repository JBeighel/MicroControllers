/**	@defgroup	gpionucleo
	@brief		Implementation of the GPIO General Interface for STM32 Nucleo32 L412KB
	@details	v0.2
		All of the GPIO pins are configured by the ST Cube MX tool.  This will
		define their functionality.  This interface will allow interactions
		with all of the IO pins regardless of this configuration.  It is left
		to other portions of the application to account for the ST Cube MX pin
		configurations.

		NucleoL412KB
		With the PH3 pin set to output I could not get it to change state.
			Unsure if this is a settings issue or code problem.

		The watchdog interface is initialized and started during the boot up
		functions the Cube generates.  The only action that can be taken is to
		refresh the watchdog to prevent restarts.

		In the STM Cube IDE the watchdog will cause a restart when a breakpoint
		is hit.  It must be disabled during debugging sessions.

		The interrupt function replaces the _weak function declared to handle it
		by the STM Cube.  The Cube generated code clears the interrupt so only
		the application code needs included.

		Macros are provided to start and stop the interrupt from being sent:
		HAL_NVIC_DisableIRQ(EXTI3_IRQn);
		HAL_NVIC_EnableIRQ(EXTI3_IRQn);
		These shut down all interrupts in the EXTI3 section.

	#File Information
		File:	GPIO_NucleoL412KB.h
		Author:	J. Beighel
		Date:	2021-03-07
*/

#ifndef __GPIONUCLEO
	#define __GPIONUCLEO

/***** Includes		*****/
	/**	@brief		Setting to use FreeRTOS as the basis of time functions
	 *	@ingroup	gpionucleo
	 */
	#define NUCLEO_TIMESRC_RTOS	1

	/**	@brief		Setting to use the HAL as the basis of time functions
	 *	@ingroup	gpionucleo
	 */
	#define NUCLEO_TIMESRC_HAL	2

	/**	@brief		Defines which timer source will be used
	 *	@ingroup	gpionucleo
	 */
	#define NUCLEO_TIMESRC		NUCLEO_TIMESRC_RTOS

	#include "CommonUtils.h"
	#include "GPIOGeneralInterface.h"
	#include "TimeGeneralInterface.h"

	#include "main.h"
	#include "stm32l4xx.h"
	#include "stm32l4xx_it.h"
	#include "i2c.h"
	#include "usart.h"
	#include "iwdg.h"
	#include "tim.h"

	#if NUCLEO_TIMESRC == NUCLEO_TIMESRC_RTOS
		#include "FreeRTOS.h"
		#include "task.h"
	#endif

/***** Constants	*****/
	/**	@brief		Specifies the capabilities provided by this implementation of the GPIO interface
	 *	@ingroup	gpionucleo
	 */
	#define GPIO_CAPS		(GPIOCap_DigitalWrite | GPIOCap_DigitalRead | GPIOCap_SetInterrupt | TimeCap_WatchdogRefresh)

	/**	@brief		Function to call to initialize this implementation of the GPIO interface
	 *	@ingroup	gpionucleo
	 */
	#define GPIO_INIT		NucleoGPIOPortInitialize

	/**	@brief		Hawrdware information for GPIO Port A
	 *	@ingroup	gpionucleo
	 */
	#define GPIO_A_HWINFO	((void *)&gGPIOPortA)

	/**	@brief		Hawrdware information for GPIO Port B
	 *	@ingroup	gpionucleo
	 */
	#define GPIO_B_HWINFO	((void *)&gGPIOPortB)

	/**	@brief		Hawrdware information for GPIO Port H
	 *	@ingroup	gpionucleo
	 */
	#define GPIO_H_HWINFO	((void *)&gGPIOPortH)

	/**	@brief		Number of GPIO in each port
	 *	@ingroup	gpionucleo
	 */
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

	/**	@brief		Function to call to initialize this implementation of the time interface
	 *	@ingroup	gpionucleo
	 */
	#define TIME_INIT			NucleoTimeInitialize

	/**	@brief		Specifies the capabilities provided by this implementation of the time interface
	 *	@ingroup	gpionucleo
	 */
	#define TIME_CAPS			(TimeCap_GetTicks | TimeCap_DelaySec | TimeCap_DelayMilliSec | TimeCap_WatchdogRefresh)

/***** Definitions	*****/
	/**	@brief		Hardware information for a GPIO interrupt
	 *	@ingroup	gpionucleo
	 */
	typedef struct sNucleoGPIOIntInfo_t {
		bool bIntEnable;
		pfGPIOInterrupt_t pfIntFunc;
		void *pParam;
	} sNucleoGPIOIntInfo_t;

	/**	@brief		Hardware information for a GPIO port
	 *	@ingroup	gpionucleo
	 */
	typedef struct sNucleoGPIOPortInfo_t {
		GPIO_TypeDef *pPort;		/**< GPIO Port on the processor */
		sNucleoGPIOIntInfo_t aIntInfo[NUCLEO_GPIOCNT];	/**< Interrupt information for each GPIO pin */
		sGPIOIface_t *pIface;		/**< Copy of interface object pointer for use in interrupt handler */
	} sNucleoGPIOPortInfo_t;

	typedef struct sNucleoTimerInfo_t {
		TIM_HandleTypeDef *pHWTimer;
		uint32_t nChannel;
	} sNucleoTimerInfo_t;

/***** Globals		*****/
	extern sNucleoGPIOPortInfo_t gGPIOPortA;
	extern sNucleoGPIOPortInfo_t gGPIOPortB;
	extern sNucleoGPIOPortInfo_t gGPIOPortH;

	extern const sNucleoTimerInfo_t gTimer2Ch1;

/***** Prototypes 	*****/
	eGPIOReturn_t NucleoGPIOPortInitialize(sGPIOIface_t *pIface, void *pHWInfo);
	
	eGPIOReturn_t NucleoGPIOSetModeByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, eGPIOModes_t eMode);

	eGPIOReturn_t NucleoGPIODigitalWriteByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, bool bState);
	
	eGPIOReturn_t NucleoGPIODigitalReadByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, bool *bState);

	eGPIOReturn_t NucleoGPIOSetInterrupt(sGPIOIface_t *pIface, GPIOID_t nGPIOPin, pfGPIOInterrupt_t pHandler, bool bEnable, void *pParam);

	eReturn_t NucleoTimeInitialize(sTimeIface_t *pIface);

	uint32_t NucleoGetCurrentTicks(void);

	eReturn_t NucleoTimeDelaySeconds(uint32_t nDelayAmount);

	eReturn_t NucleoTimeDelayMilliSeconds(uint32_t nDelayAmount);

	eReturn_t NucleoWatchdogRefresh(void);

/***** Functions	*****/

#endif
