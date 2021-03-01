/***** Includes		*****/
	#include "GPIO_NucleoL412KB.h"

/***** Constants	*****/


/***** Definitions	*****/


/***** Constants	*****/


/***** Globals		*****/


/***** Prototypes 	*****/


/***** Functions	*****/

eGPIOReturn_t NucleoGPIOPortInitialize(sGPIOIface_t *pIface, void *pHWInfo) {
	uint32_t nCtr;

	//All of this is handled by the STM Cube built project, os just fill out the object

	GPIOInterfaceInitialize(pIface); //Set a sane base for the interface
	pIface->pHWInfo = pHWInfo; //HW info is the port the pin is on

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
	pIface->ePortCapabilities = GPIO_CAPS;

	//Set the capabilities of all the IO points (even if they aren't configured to be IO)
	for (nCtr = 0; nCtr < pIface->nGPIOCnt; nCtr++) {
		pIface->aGPIO[nCtr].eCapabilities = (eGPIOModes_t)(GPIO_DigitalInput | GPIO_DigitalOutput | GPIO_InputPullup);

		pIface->aGPIO[nCtr].eMode = GPIO_None;
		pIface->aGPIO[nCtr].pHWInfo = NULL;
	}

	return GPIO_Success;
}
	
eGPIOReturn_t NucleoGPIOSetModeByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, eGPIOModes_t eMode) {
	//Mode is set by the cube, not doing it here
	return GPIO_Success;
}
	
eGPIOReturn_t NucleoGPIODigitalWriteByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, bool bState) {
	GPIO_PinState eState;
	
	if (nGPIOPin >= NUCLEO_GPIOCNT) {
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

eReturn_t NucleoTimeInitialize(sTimeIface_t *pIface) {
	TimeInterfaceInitialize(pIface);

	pIface->pfGetTicks = &NucleoGetCurrentTicks;
	pIface->pfDelaySeconds = &NucleoTimeDelaySeconds;
	pIface->pfDelayMilliSeconds = &NucleoTimeDelayMilliSeconds;

	pIface->eCapabilities = TIME_CAPS;

	return Success;
}

uint32_t NucleoGetCurrentTicks(void) {
	#if NUCLEO_TIMESRC == NUCLEO_TIMESRC_RTOS
		return xTaskGetTickCount();
	#elif #if NUCLEO_TIMESRC == NUCLEO_TIMESRC_HAL
		return HAL_GetTick();
	#endif
}

eReturn_t NucleoTimeDelaySeconds(uint32_t nDelayAmount) {
	#if NUCLEO_TIMESRC == NUCLEO_TIMESRC_RTOS
		vTaskDelay(nDelayAmount * 1000);
	#elif #if NUCLEO_TIMESRC == NUCLEO_TIMESRC_HAL
		HAL_Delay(nDelayAmount * 1000);
	#endif

	return Success;
}

eReturn_t NucleoTimeDelayMilliSeconds(uint32_t nDelayAmount) {
	#if NUCLEO_TIMESRC == NUCLEO_TIMESRC_RTOS
		vTaskDelay(nDelayAmount);
	#elif #if NUCLEO_TIMESRC == NUCLEO_TIMESRC_HAL
		HAL_Delay(nDelayAmount);
	#endif

	return Success;
}
