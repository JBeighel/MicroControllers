/**	File:	GPIO_RaspberryPi.c
	Author:	J. Beighel
	Created:11-24-2020
*/

/*****	Includes	*****/
	#include "GPIO_RaspberryPi.h"

/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	//Removes the compiler warning
	int nanosleep(const struct timespec *req, struct timespec *rem);

	eGPIOReturn_t RasPiGPIOReadModeByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, eGPIOModes_t *eMode);
	
	eGPIOReturn_t RasPiGPIOSetModeByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, eGPIOModes_t eMode);
	
	eGPIOReturn_t RasPiGPIODigitalReadByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, bool *bState);
	
	eGPIOReturn_t RasPiGPIODigitalWriteByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, bool bState);
	
	uint32_t RasPiGetCurrentTicks(void);
	
	eReturn_t RasPiDelaySeconds(uint32_t nDelayAmount);
	
	eReturn_t RasPiDelayMilliSeconds(uint32_t nDelayAmount);
	
	eReturn_t RasPiDelayMicroSeconds(uint32_t nDelayAmount);

/*****	Functions	*****/
eGPIOReturn_t RasPiGPIOPortInitialize(sGPIOIface_t *pIface, void *pHWInfo) {
	pRasPiRegType_t pGPIOReg = (pRasPiRegType_t)(pHWInfo + RASPI_GPIOOFFSET);
	int32_t nMemFile;
	uint16_t nCtr;
	void *pGpioMap;
	
	GPIOInterfaceInitialize(pIface);
	
	//Setup the hardware
	//Open the file system object giving memory access
	nMemFile = open("/dev/mem", O_RDWR | O_SYNC);
	if (nMemFile < 0) {
		return GPIOFail_Unknown;
	}
	
	//Map the GPIO memory block
	//Enable reading and writing and share with other processes
	pGpioMap = mmap(NULL, RASPI_BLOCKSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, nMemFile, (off_t)pGPIOReg);
	
	close(nMemFile);
	
	if (pGpioMap == MAP_FAILED) {
		return GPIOFail_Unknown;
	}
	
	//Setup the interface object
	//Only add function pointers to the supported features
	pIface->pfPortInit = &RasPiGPIOPortInitialize;
	pIface->pfReadModeByPin = &RasPiGPIOReadModeByPin;
	pIface->pfSetModeByPin = &RasPiGPIOSetModeByPin;
	pIface->pfDigitalReadByPin = &RasPiGPIODigitalReadByPin;
	pIface->pfDigitalWriteByPin = &RasPiGPIODigitalWriteByPin;
	
	pIface->nGPIOCnt = 28;
	pIface->pHWInfo = pGpioMap;
	
	//Set values for all of the IO pins
	for (nCtr = 0; nCtr < RASPI_PINCOUNT; nCtr++) {
		//All supported capabilities
		pIface->aGPIO[nCtr].eCapabilities = GPIO_DigitalInput | GPIO_DigitalOutput;
		
		pIface->aGPIO[nCtr].eMode = GPIO_DigitalInput; //this is the default mode
		
		//Try to read the current mode
		RasPiGPIOReadModeByPin(pIface, nCtr, &(pIface->aGPIO[nCtr].eMode));
	}
	
	return GPIO_Success;
}

eGPIOReturn_t RasPiGPIOReadModeByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, eGPIOModes_t *eMode) {
	pRasPiRegType_t pGPIOReg = (pIface->pHWInfo);
	uint32_t nMode;
	
	if (nGPIOPin >= RASPI_PINCOUNT) {
		return GPIOFail_InvalidPin;
	}
	
	//Read the pin settings register
	nMode = *(pGPIOReg + RASPI_GPIOMODE + (nGPIOPin / RasPi_GPIORegPins));
	
	//Shift and mask to get just the requested pin
	nMode >>= RasPi_GPIOPinShift * (nGPIOPin % RasPi_GPIORegPins);
	nMode &= RasPi_GPIOPinMask;
	
	//Convert to the interface format
	if (nMode == RasPi_GPIOInput) {
		(*eMode) = GPIO_DigitalInput;
	} else {
		(*eMode) = GPIO_DigitalOutput;
	}
	
	return GPIO_Success;
}

eGPIOReturn_t RasPiGPIOSetModeByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, eGPIOModes_t eMode) {
	pRasPiRegType_t pGPIOReg = (pIface->pHWInfo);
	uint32_t nRegVal, nCurrMode;
	
	if (nGPIOPin >= RASPI_PINCOUNT) {
		return GPIOFail_InvalidPin;
	}
	
	if (CheckAllBitsInMask(pIface->aGPIO[nGPIOPin].eCapabilities, eMode) == false) {
		return GPIOFail_Unsupported;
	}
	
	if (CheckAllBitsInMask(eMode, GPIO_DigitalInput | GPIO_DigitalOutput) == true) {
		return GPIOFail_InvalidMode;
	}
	
	if (CheckAnyBitsInMask(eMode, GPIO_DigitalInput | GPIO_DigitalOutput) == false) {
		return GPIOFail_InvalidMode;
	}
	
	//save off the mode register for this pin
	pGPIOReg += RASPI_GPIOMODE + (nGPIOPin / RasPi_GPIORegPins);
	
	//Get the current pin settings
	nRegVal = *pGPIOReg;
	
	//Clear the settings for this pin
	nRegVal &= ~(RasPi_GPIOPinMask << ((nGPIOPin % RasPi_GPIORegPins) * RasPi_GPIOPinShift));
	
	//Set the requested mode
	if (CheckAllBitsInMask(eMode, GPIO_DigitalInput) == true) {
		nRegVal |= RasPi_GPIOInput << ((nGPIOPin % RasPi_GPIORegPins) * RasPi_GPIOPinShift);
	} else if (CheckAllBitsInMask(eMode, GPIO_DigitalOutput) == true) {
		nRegVal |= RasPi_GPIOOutput << ((nGPIOPin % RasPi_GPIORegPins) * RasPi_GPIOPinShift);
	}
	
	if (CheckAllBitsInMask(eMode, GPIO_InputPullup) == true) {
		
	}
	
	//Update the mode register
	*pGPIOReg = nRegVal;
	
	return GPIO_Success;
}

eGPIOReturn_t RasPiGPIODigitalReadByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, bool *bState) {
	pRasPiRegType_t pGPIOReg = (pIface->pHWInfo);
	uint32_t nRegVal;
	
	if (nGPIOPin >= RASPI_PINCOUNT) {
		return GPIOFail_InvalidPin;
	}
	
	//Read the values from the pins
	nRegVal = *(pGPIOReg + RASPI_GPIOREAD);
	
	//Mask out the value for this pin
	nRegVal &= 1 << nGPIOPin;
	
	if (nRegVal == 0) {
		*bState = false;
	} else {
		*bState = true;
	}
	
	return GPIO_Success;
}

eGPIOReturn_t RasPiGPIODigitalWriteByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, bool bState) {
	pRasPiRegType_t pGPIOReg = (pIface->pHWInfo);
	uint32_t nRegVal;
	
	if (nGPIOPin >= RASPI_PINCOUNT) {
		return GPIOFail_InvalidPin;
	}
	
	if (bState == true) {
		pGPIOReg += RASPI_GPIOSET;
	} else {
		pGPIOReg += RASPI_GPIOCLEAR;
	}
	
	//Set the flag for the requested pin and issue command
	nRegVal = 1 << nGPIOPin;
	
	*pGPIOReg = nRegVal;
	
	return GPIO_Success;
}

eReturn_t RasPiDelaySeconds(uint32_t nDelayAmount) {
	sleep(nDelayAmount);
	
	return Success;
}

eReturn_t RasPiDelayMilliSeconds(uint32_t nDelayAmount) {
	struct timespec tReq, tRemain;
	
	//Pull out the number of seconds
	tReq.tv_sec = nDelayAmount / 1000;
	
	//Figure out the mSec remaining
	tReq.tv_nsec = nDelayAmount - (tReq.tv_sec * 1000);
	
	//Convert to nano seconds
	tReq.tv_nsec *= 1000000;
	
	nanosleep(&tReq, &tRemain);
	
	return Success;
}

eReturn_t RasPiDelayMicroSeconds(uint32_t nDelayAmount) {
	struct timespec tReq, tRemain;
	
	//Pull out the number of seconds
	tReq.tv_sec = nDelayAmount / 1000000;
	
	//Figure out the mSec remaining
	tReq.tv_nsec = nDelayAmount - (tReq.tv_sec * 1000000);
	
	//Convert to nano seconds
	tReq.tv_nsec *= 1000;
	
	nanosleep(&tReq, &tRemain);
	
	return Success;
}

eReturn_t RasPiTimeInit(sTimeIface_t *pTime) {
	TimeInterfaceInitialize(pTime);
	
	pTime->pfGetTicks = &RasPiGetCurrentTicks;
	pTime->pfDelaySeconds = &RasPiDelaySeconds;
	pTime->pfDelayMilliSeconds = &RasPiDelayMilliSeconds;
	pTime->pfDelayMicroSeconds = &RasPiDelayMicroSeconds;
	
	return Success;
}

uint32_t RasPiGetCurrentTicks(void) {
	uint32_t tTime;
	struct timespec TimeInfo;

    clock_gettime(CLOCK_REALTIME, &TimeInfo);
	
	tTime = (uint32_t)(TimeInfo.tv_nsec / 1000000);
	tTime += (uint32_t)(TimeInfo.tv_sec * 1000);
	
	return tTime;
}
