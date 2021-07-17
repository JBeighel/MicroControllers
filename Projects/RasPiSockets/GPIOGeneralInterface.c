/*	File:	GPIOGeneralInterface.c
	Author:	J. Beighel
	Date:	2021-03-04
*/

/***** Includes		*****/
	#include "GPIOGeneralInterface.h"
	
/***** Definitions	*****/


/***** Constants	*****/


/***** Globals		*****/


/***** Prototypes 	*****/
	eGPIOReturn_t GPIOPortInitialize(sGPIOIface_t *pIface, void *pHWInfo);
	
	eGPIOReturn_t GPIOSetModeByPin(sGPIOIface_t *pIface, GPIOID_t nGPIOPin, eGPIOModes_t eMode);
	
	eGPIOReturn_t GPIOReadModeByPin(sGPIOIface_t *pIface, GPIOID_t nGPIOPin, eGPIOModes_t *eMode);
	
	eGPIOReturn_t GPIODigitalWriteByPin(sGPIOIface_t *pIface, GPIOID_t nGPIOPin, bool bState);
	
	eGPIOReturn_t GPIODigitalReadByPin(sGPIOIface_t *pIface, GPIOID_t nGPIOPin, bool *bState);
	
	eGPIOReturn_t GPIOPWMWriteByPin(sGPIOIface_t *pIface, GPIOID_t nGPIOPin, uint32_t nPWMValue);
	
	eGPIOReturn_t GPIOAnalogWriteByPin(sGPIOIface_t *pIface, GPIOID_t nGPIOPin, uint32_t nAnaValue);
	
	eGPIOReturn_t GPIOAnalogReadByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, uint32_t *nAnaValue);

	eGPIOReturn_t GPIOSetInterruptNotImplem(sGPIOIface_t *pIface, GPIOID_t nGPIOPin, pfGPIOInterrupt_t pHandler, bool bEnable, void *pParam);

/***** Functions	*****/

eGPIOReturn_t GPIOInterfaceInitialize(sGPIOIface_t *pIface) {
	pIface->pfPortInit = &GPIOPortInitialize;
	pIface->pfSetModeByPin = &GPIOSetModeByPin;
	pIface->pfReadModeByPin = &GPIOReadModeByPin;
	pIface->pfDigitalWriteByPin = &GPIODigitalWriteByPin;
	pIface->pfDigitalReadByPin = &GPIODigitalReadByPin;
	pIface->pfPWMWriteByPin = &GPIOPWMWriteByPin;
	pIface->pfAnalogWriteByPin = &GPIOAnalogWriteByPin;
	pIface->pfAnalogReadByPin = &GPIOAnalogReadByPin;
	pIface->pfSetInterrupt = &GPIOSetInterruptNotImplem;

	memset(pIface->aGPIO, 0, sizeof(sGPIOInfo_t) * GPIO_IOCNT);

	pIface->nPWMBitDepth = 0;
	pIface->nAnaInBitDepth = 0;
	pIface->nAnaOutBitDepth = 0;
	pIface->nGPIOCnt = 0;

	pIface->pHWInfo = NULL;

	pIface->ePortCapabilities = GPIOCap_None;

	return GPIO_Success;
}

eGPIOReturn_t GPIOPortInitialize(sGPIOIface_t *pIface, void *pHWInfo) {
	return GPIOFail_Unsupported;
}

eGPIOReturn_t GPIOSetModeByPin(sGPIOIface_t *pIface, GPIOID_t nGPIOPin, eGPIOModes_t eMode) {
	return GPIOFail_Unsupported;
}

eGPIOReturn_t GPIOReadModeByPin(sGPIOIface_t *pIface, GPIOID_t nGPIOPin, eGPIOModes_t *eMode) {
	return GPIOFail_Unsupported;
}

eGPIOReturn_t GPIODigitalWriteByPin(sGPIOIface_t *pIface, GPIOID_t nGPIOPin, bool bState) {
	return GPIOFail_Unsupported;
}

eGPIOReturn_t GPIODigitalReadByPin(sGPIOIface_t *pIface, GPIOID_t nGPIOPin, bool *bState) {
	return GPIOFail_Unsupported;
}

eGPIOReturn_t GPIOPWMWriteByPin(sGPIOIface_t *pIface, GPIOID_t nGPIOPin, uint32_t nPWMValue) {
	return GPIOFail_Unsupported;
}

eGPIOReturn_t GPIOAnalogWriteByPin(sGPIOIface_t *pIface, GPIOID_t nGPIOPin, uint32_t nAnaValue) {
	return GPIOFail_Unsupported;
}

eGPIOReturn_t GPIOAnalogReadByPin(sGPIOIface_t *pIface, GPIOID_t nGPIOPin, uint32_t *nAnaValue) {
	return GPIOFail_Unsupported;
}

eGPIOReturn_t GPIOSetInterruptNotImplem(sGPIOIface_t *pIface, GPIOID_t nGPIOPin, pfGPIOInterrupt_t pHandler, bool bEnable, void *pParam) {
	return GPIOFail_Unsupported;
}
