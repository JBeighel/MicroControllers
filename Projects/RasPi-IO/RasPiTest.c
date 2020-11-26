/*****	Includes	*****/
	//Pin supported (micro hardware and buses)
	#include "CommonUtils.h"
	#include "GPIOGeneralInterface.h"
	#include "GPIO_RaspberryPi.h"
	
	//Board support (peripherals)

/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/
sGPIOIface_t gGPIO;

/*****	Prototypes 	*****/


/*****	Functions	*****/
int32_t main(int32_t nArgCnt, char *aArgVals) {
	eGPIOReturn_t eResult;
	eGPIOModes_t ePinMode;
	bool bPinState;
	uint16_t nCtr;
	
	eResult = GPIO_INIT(&gGPIO, GPIO_HWINFO);
	
	if (eResult != GPIO_Success) {
		printf("GPIO Init failed %d\r\n", eResult);
		return 0;
	} else {
		printf("GPIO Init succeeded\r\n");
	}
	
	for (nCtr = 0; nCtr < 10; nCtr++) {
		eResult = gGPIO.pfSetModeByPin(&gGPIO, nCtr, GPIO_DigitalOutput);
		if (eResult != GPIO_Success) {
			printf("Failed to set mode for pin %d: %d\r\n", nCtr, eResult);
		}
		
		eResult = gGPIO.pfDigitalWriteByPin(&gGPIO, nCtr, false);
		if (eResult != GPIO_Success) {
			printf("Failed to output set state for pin %d: %d\r\n", nCtr, eResult);
		}
	}
	
	for (nCtr = 10; nCtr < 20; nCtr++) {
		eResult = gGPIO.pfSetModeByPin(&gGPIO, nCtr, GPIO_DigitalInput);
		
		if (eResult != GPIO_Success) {
			printf("Failed to set mode for pin %d: %d\r\n", nCtr, eResult);
		}
	}
	
	for (nCtr = 0; nCtr < 20; nCtr++) {
		gGPIO.pfReadModeByPin(&gGPIO, nCtr, &ePinMode);
		
		switch (ePinMode) {
			case (GPIO_DigitalInput):
				gGPIO.pfDigitalReadByPin(&gGPIO, nCtr, &bPinState);
				printf("Pin %d is Input, value %d\r\n", nCtr, bPinState);
				break;
			case (GPIO_DigitalOutput):
				printf("Pin %d is Output\r\n", nCtr);
				break;
			default:
				printf("Pin %d is unknown %d\r\n", nCtr, ePinMode);
				break;
		}
	}
	
	return 0;
}
