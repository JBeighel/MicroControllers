/***** Includes		*****/
	#include "GPIOGeneralInterface.h"
/***** Definitions	*****/


/***** Constants	*****/


/***** Globals		*****/


/***** Prototypes 	*****/


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

	memset(pIface->aGPIO, 0, sizeof(sGPIOInfo_t) * GPIO_IOCNT);

	pIface->nPWMBitDepth = 0;
	pIface->nAnaInBitDepth = 0;
	pIface->nAnaOutBitDepth = 0;
	pIface->nGPIOCnt = 0;

	pIface->pHWInfo = NULL;

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

bool TimeInterfaceInitialize(sTimeIface_t *pIface) {
	pIface->pfGetTicks = &TimeCurrentTicks;
	pIface->pfDelaySeconds = &TimeDelay;
	pIface->pfDelayMilliSeconds = &TimeDelay;
	pIface->pfDelayMicroSeconds = &TimeDelay;
}
	
uint32_t TimeCurrentTicks(void) {
	return 0;
}

bool TimeDelay(uint32_t nDelayAmount) {
	return false;
}