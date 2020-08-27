/***** Includes		*****/
	#include "GPIO_NucleoL412KB.h"

/***** Constants	*****/


/***** Definitions	*****/


/***** Constants	*****/
	/** @brief		Constant array of all pin numbers to use for GPIO
		@details	There are 22 potential GPIO pins, owever 7 are used by the peripheral buses
			Pin 0 and 1 are used for the UART.
			Pin 11 is MOSI, Pin 12 is MIS and Pin 13 are SCK; reserved for SPI
			Pin 18/A4 is SDA and Pin 19/A5 is SCL; reserved for I2C
		@ingroup	gpionucleo
	*/
	const NUCLEO_PININFOTYPE gNucleoGPIOList[] = { LD3_Pin, D4_Pin,	D5_Pin }; //All in GPIOB

	/** @brief		Constant array of all pin numbers that support Analog Input
		@ingroup	gpionucleo
	*/
	const NUCLEO_PININFOTYPE gNucleoADCList[] = { };

	/** @brief		Constant array of all pin numbers that support Analog Ouput
		@ingroup	gpionucleo
	*/
	const NUCLEO_PININFOTYPE gNucleoDACList[] = { };

	/** @brief		Constant array of all pin numbers that support PWM Output
		@ingroup	gpionucleo
	*/
	const NUCLEO_PININFOTYPE gNucleoPWMList[] = { };

/***** Globals		*****/


/***** Prototypes 	*****/


/***** Functions	*****/

eGPIOReturn_t NucleoGPIOPortInitialize(sGPIOIface_t *pIface, void *pHWInfo) {
	uint32_t nCtr;

	//All of this is handled by the STM Cube built project, os just fill out the object

	GPIOInterfaceInitialize(pIface); //Set a base sanity for the interface
	pIface->pHWInfo = pHWInfo;

	//Set pointers for all supported functions
	pIface->pfPortInit = &NucleoGPIOPortInitialize;
	pIface->pfSetModeByPin = &NucleoGPIOSetModeByPin;
	pIface->pfDigitalWriteByPin = &NucleoGPIODigitalWriteByPin;
	pIface->pfDigitalReadByPin = &NucleoGPIODigitalReadByPin;

	//Provide the capabilities of the available GPIO
	pIface->nPWMBitDepth = NUCLEO_PWMBITDEPTH;
	pIface->nAnaInBitDepth = NUCLEO_ADCBITDEPTH;
	pIface->nAnaOutBitDepth = NUCLEO_DACBITDEPTH;

	if (GPIO_IOCNT < NUCLEO_GPIOCNT) { //Can't provide more IO than the interface allows
		pIface->nGPIOCnt = GPIO_IOCNT;
	} else {
		pIface->nGPIOCnt = NUCLEO_GPIOCNT;
	}

	pIface->pHWInfo = pHWInfo;

	//Set the capabilities of all the IO points
	for (nCtr = 0; nCtr < pIface->nGPIOCnt; nCtr++) {
		pIface->aGPIO[nCtr].eCapabilities = (eGPIOModes_t)(GPIO_DigitalInput | GPIO_DigitalOutput | GPIO_InputPullup);

		if (NumberInArray_u16(gNucleoGPIOList[nCtr], gNucleoPWMList, NUCLEO_PWMCNT) == true) {
			pIface->aGPIO[nCtr].eCapabilities = (eGPIOModes_t)(pIface->aGPIO[nCtr].eCapabilities | GPIO_OutputPWM);
		}

		if (NumberInArray_u16(gNucleoGPIOList[nCtr], gNucleoADCList, NUCLEO_ADCCNT) == true) {
			pIface->aGPIO[nCtr].eCapabilities = (eGPIOModes_t)(pIface->aGPIO[nCtr].eCapabilities | GPIO_AnalogInput);
		}

		if (NumberInArray_u16(gNucleoGPIOList[nCtr], gNucleoDACList, NUCLEO_DACCNT) == true) {
			pIface->aGPIO[nCtr].eCapabilities = (eGPIOModes_t)(pIface->aGPIO[nCtr].eCapabilities | GPIO_AnalogOutput);
		}

		pIface->aGPIO[nCtr].eMode = GPIO_None;
		pIface->aGPIO[nCtr].pHWInfo = (void *)((uint32_t)gNucleoGPIOList[nCtr]);
	}

	return GPIO_Success;
}
	
eGPIOReturn_t NucleoGPIOSetModeByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, eGPIOModes_t eMode) {
	//Mode is set by the cube, not doing it here
	return GPIO_Success;
}
	
eGPIOReturn_t NucleoGPIODigitalWriteByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, bool bState) {
	uint32_t nIdx = IndexInArray_u16(nGPIOPin, gNucleoGPIOList, NUCLEO_GPIOCNT);
	GPIO_PinState eState;
	
	if (nIdx >= NUCLEO_GPIOCNT) {
		return GPIOFail_InvalidPin;
	}

	//Should check that its an output first
	
	if (bState == true) {
		eState = GPIO_PIN_SET;
	} else {
		eState = GPIO_PIN_RESET;
	}

	HAL_GPIO_WritePin(pIface->pHWInfo, nGPIOPin, eState);
	
	return GPIO_Success;
}

eGPIOReturn_t NucleoGPIODigitalReadByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, bool *bState) {
	if (HAL_GPIO_ReadPin(pIface->pHWInfo, nGPIOPin) == GPIO_PIN_SET) {
		*bState = true;
	} else {
		*bState = false;
	}
	
	return GPIO_Success;
}

