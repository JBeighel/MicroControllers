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

	typedef struct sNucleoGPIOHWInfo_t {
		GPIO_TypeDef *pGPIOPort;
		const NUCLEO_PININFOTYPE *pIOPinList;
		const NUCLEO_PININFOTYPE *pPWMPinList;
		const NUCLEO_PININFOTYPE *pDACPinList;
		const NUCLEO_PININFOTYPE *pADCPinList;
		uint16_t nIOCount;
		uint16_t nPWMCount;
		uint16_t nDACCount;
		uint16_t nADCCount;
	} sNucleoGPIOHWInfo_t;

/***** Constants	*****/
	#define GPIO_INIT			NucleoGPIOPortInitialize

	/** @brief		Constant array of all I/O pin numbers to use for GPIO on Port A
		@ingroup	gpionucleo
	*/
	const NUCLEO_PININFOTYPE gNucleoGPIOAList[] = { GPI_A03_1_Pin, GPI_A04_2_Pin, GPI_A05_3_Pin, GPI_A08_4_Pin, GPI_A11_5_Pin, GPI_A12_6_Pin };

	/** @brief		Constant array of all I/O pin numbers to use for GPIO on Port B
		@ingroup	gpionucleo
	*/
	const NUCLEO_PININFOTYPE gNucleoGPIOBList[] = { GPI_B00_0_Pin, GPI_B01_1_Pin, GPI_B03_2_Pin };

	/** @brief		Constant array of all I/O pin numbers to use for GPIO on Port H
		@ingroup	gpionucleo
	*/
	const NUCLEO_PININFOTYPE gNucleoGPIOHList[] = { GPI_H03_5_Pin };
	
	/**	@brief		Count of available GPIO pins
		@ingroup	gpionucleo
	*/
	#define NUCLEO_GPIOACNT
	#define NUCLEO_GPIOBCNT		(sizeof(gNucleoGPIOBList) / sizeof(NUCLEO_PININFOTYPE))
	#define NUCLEO_GPIOHCNT		(sizeof(gNucleoGPIOHList) / sizeof(NUCLEO_PININFOTYPE))

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

	const sNucleoGPIOHWInfo_t GPIO_A_HWINFO	= {
		.pGPIOPort = GPIOA,
		.pIOPinList = gNucleoGPIOAList,
		.pPWMPinList = NULL,
		.pDACPinList = NULL,
		.pADCPinList = NULL,
		.nIOCount = (sizeof(gNucleoGPIOAList) / sizeof(NUCLEO_PININFOTYPE)),
		.nPWMCount = 0,
		.nDACCount = 0,
		.nADCCount = 0,
	};
	
	const sNucleoGPIOHWInfo_t GPIO_B_HWINFO	= {
		.pGPIOPort = GPIOB,
		.pIOPinList = gNucleoGPIOBList,
		.pPWMPinList = NULL,
		.pDACPinList = NULL,
		.pADCPinList = NULL,
		.nIOCount = (sizeof(gNucleoGPIOBList) / sizeof(NUCLEO_PININFOTYPE)),
		.nPWMCount = 0,
		.nDACCount = 0,
		.nADCCount = 0,
	};

	const sNucleoGPIOHWInfo_t GPIO_H_HWINFO	= {
		.pGPIOPort = GPIOH,
		.pIOPinList = gNucleoGPIOBList,
		.pPWMPinList = NULL,
		.pDACPinList = NULL,
		.pADCPinList = NULL,
		.nIOCount = (sizeof(gNucleoGPIOHList) / sizeof(NUCLEO_PININFOTYPE)),
		.nPWMCount = 0,
		.nDACCount = 0,
		.nADCCount = 0,
	};

/***** Constants	*****/


/***** Globals		*****/


/***** Prototypes 	*****/
	eGPIOReturn_t NucleoGPIOPortInitialize(sGPIOIface_t *pIface, void *pHWInfo);
	
	eGPIOReturn_t NucleoGPIOSetModeByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, eGPIOModes_t eMode);

	eGPIOReturn_t NucleoGPIODigitalWriteByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, bool bState);
	
	eGPIOReturn_t NucleoGPIODigitalReadByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, bool *bState);

/***** Functions	*****/

#endif
