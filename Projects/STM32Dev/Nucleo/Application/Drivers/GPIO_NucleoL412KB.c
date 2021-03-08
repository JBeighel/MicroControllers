/* #File Information
		File:	GPIO_NucleoL412KB.c
		Author:	J. Beighel
		Date:	2021-03-07
*/

/***** Includes		*****/
	#include "GPIO_NucleoL412KB.h"

/***** Constants	*****/


/***** Definitions	*****/


/***** Constants	*****/


/***** Globals		*****/
	sNucleoGPIOPortInfo_t gGPIOPortA = { .pPort = GPIOA };
	sNucleoGPIOPortInfo_t gGPIOPortB = { .pPort = GPIOB };
	sNucleoGPIOPortInfo_t gGPIOPortH = { .pPort = GPIOH };

	const sNucleoTimerInfo_t gTimer2Ch1 = { .pHWTimer = &htim2, .nChannel = TIM_CHANNEL_1};

/***** Prototypes 	*****/
	void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

/***** Functions	*****/

eGPIOReturn_t NucleoGPIOPortInitialize(sGPIOIface_t *pIface, void *pHWInfo) {
	uint32_t nCtr;
	sNucleoGPIOPortInfo_t *pCurrPort = (sNucleoGPIOPortInfo_t *)pHWInfo;

	//All of this is handled by the STM Cube built project, just fill out the object here

	GPIOInterfaceInitialize(pIface); //Set a sane base for the interface
	pIface->pHWInfo = pHWInfo; //HW info is the port the pin is on

	//Set pointers for all supported functions
	pIface->pfPortInit = &NucleoGPIOPortInitialize;
	pIface->pfSetModeByPin = &NucleoGPIOSetModeByPin;
	pIface->pfDigitalWriteByPin = &NucleoGPIODigitalWriteByPin;
	pIface->pfDigitalReadByPin = &NucleoGPIODigitalReadByPin;
	pIface->pfSetInterrupt = &NucleoGPIOSetInterrupt;

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

	//Set the interrupts to disabled for all pins
	pCurrPort->pIface = pIface;

	for (nCtr = 0; nCtr < NUCLEO_GPIOCNT; nCtr++) {
		pCurrPort->aIntInfo[nCtr].bIntEnable = false;
		pCurrPort->aIntInfo[nCtr].pParam = NULL;
		pCurrPort->aIntInfo[nCtr].pfIntFunc = NULL;
	}

	return GPIO_Success;
}
	
eGPIOReturn_t NucleoGPIOSetModeByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, eGPIOModes_t eMode) {
	//Mode is set by the cube, not doing it here
	return GPIO_Success;
}
	
eGPIOReturn_t NucleoGPIODigitalWriteByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, bool bState) {
	sNucleoGPIOPortInfo_t *pCurrPort = (sNucleoGPIOPortInfo_t *)(pIface->pHWInfo);
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

	HAL_GPIO_WritePin(pCurrPort->pPort, nGPIOPin, eState);
	
	return GPIO_Success;
}

eGPIOReturn_t NucleoGPIODigitalReadByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, bool *bState) {
	sNucleoGPIOPortInfo_t *pCurrPort = (sNucleoGPIOPortInfo_t *)(pIface->pHWInfo);

	if (HAL_GPIO_ReadPin(pCurrPort->pPort, nGPIOPin) == GPIO_PIN_SET) {
		*bState = true;
	} else {
		*bState = false;
	}
	
	return GPIO_Success;
}

eGPIOReturn_t NucleoGPIOSetInterrupt(sGPIOIface_t *pIface, GPIOID_t nGPIOPin, pfGPIOInterrupt_t pHandler, bool bEnable, void *pParam) {
	uint16_t nIdx;
	sNucleoGPIOPortInfo_t *pCurrPort = (sNucleoGPIOPortInfo_t *)(pIface->pHWInfo);

	//Determine the index of the pin interrupt information
	switch (nGPIOPin) {
		case (GPIO_PIN_0) :
			nIdx = 0;
			break;
		case (GPIO_PIN_1) :
			nIdx = 1;
			break;
		case (GPIO_PIN_2) :
			nIdx = 2;
			break;
		case (GPIO_PIN_3) :
			nIdx = 3;
			break;
		case (GPIO_PIN_4) :
			nIdx = 4;
			break;
		case (GPIO_PIN_5) :
			nIdx = 5;
			break;
		case (GPIO_PIN_6) :
			nIdx = 6;
			break;
		case (GPIO_PIN_7) :
			nIdx = 7;
			break;
		case (GPIO_PIN_8) :
			nIdx = 8;
			break;
		case (GPIO_PIN_9) :
			nIdx = 9;
			break;
		case (GPIO_PIN_10) :
			nIdx = 10;
			break;
		case (GPIO_PIN_11) :
			nIdx = 11;
			break;
		case (GPIO_PIN_12) :
			nIdx = 12;
			break;
		case (GPIO_PIN_13) :
			nIdx = 13;
			break;
		case (GPIO_PIN_14) :
			nIdx = 14;
			break;
		case (GPIO_PIN_15) :
			nIdx = 15;
			break;
		default:
			return GPIOFail_InvalidPin;
	}

	//Setup the interrupt details for this pin
	pCurrPort->aIntInfo[nIdx].bIntEnable = bEnable;
	pCurrPort->aIntInfo[nIdx].pfIntFunc = pHandler;
	pCurrPort->aIntInfo[nIdx].pParam = pParam;

	return GPIO_Success;
}

eReturn_t NucleoTimeInitialize(sTimeIface_t *pIface) {
	TimeInterfaceInitialize(pIface);

	pIface->pfGetTicks = &NucleoGetCurrentTicks;
	pIface->pfDelaySeconds = &NucleoTimeDelaySeconds;
	pIface->pfDelayMilliSeconds = &NucleoTimeDelayMilliSeconds;
	pIface->pfWatchdogRefresh = &NucleoWatchdogRefresh;

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

eReturn_t NucleoWatchdogRefresh(void) {
	HAL_IWDG_Refresh(&hiwdg);
	return Success;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	uint16_t nIdx;

	//Determine the index of the pin interrupt information
	switch (GPIO_Pin) {
		case (GPIO_PIN_0) :
			nIdx = 0;
			break;
		case (GPIO_PIN_1) :
			nIdx = 1;
			break;
		case (GPIO_PIN_2) :
			nIdx = 2;
			break;
		case (GPIO_PIN_3) :
			nIdx = 3;
			break;
		case (GPIO_PIN_4) :
			nIdx = 4;
			break;
		case (GPIO_PIN_5) :
			nIdx = 5;
			break;
		case (GPIO_PIN_6) :
			nIdx = 6;
			break;
		case (GPIO_PIN_7) :
			nIdx = 7;
			break;
		case (GPIO_PIN_8) :
			nIdx = 8;
			break;
		case (GPIO_PIN_9) :
			nIdx = 9;
			break;
		case (GPIO_PIN_10) :
			nIdx = 10;
			break;
		case (GPIO_PIN_11) :
			nIdx = 11;
			break;
		case (GPIO_PIN_12) :
			nIdx = 12;
			break;
		case (GPIO_PIN_13) :
			nIdx = 13;
			break;
		case (GPIO_PIN_14) :
			nIdx = 14;
			break;
		case (GPIO_PIN_15) :
			nIdx = 15;
			break;
		default:
			return;
	}

	//Look for the GPIO that caused the interrupt
	if ((gGPIOPortA.aIntInfo[nIdx].bIntEnable == true) && (gGPIOPortA.aIntInfo[nIdx].pfIntFunc != NULL)) {
		gGPIOPortA.aIntInfo[nIdx].pfIntFunc(gGPIOPortA.pIface, GPIO_Pin, gGPIOPortA.aIntInfo[nIdx].pParam);
		return;
	}

	if ((gGPIOPortB.aIntInfo[nIdx].bIntEnable == true) && (gGPIOPortB.aIntInfo[nIdx].pfIntFunc != NULL)) {
		gGPIOPortB.aIntInfo[nIdx].pfIntFunc(gGPIOPortB.pIface, GPIO_Pin, gGPIOPortB.aIntInfo[nIdx].pParam);
		return;
	}

	if ((gGPIOPortH.aIntInfo[nIdx].bIntEnable == true) && (gGPIOPortH.aIntInfo[nIdx].pfIntFunc != NULL)) {
		gGPIOPortH.aIntInfo[nIdx].pfIntFunc(gGPIOPortH.pIface, GPIO_Pin, gGPIOPortH.aIntInfo[nIdx].pParam);
		return;
	}

	return;
}
