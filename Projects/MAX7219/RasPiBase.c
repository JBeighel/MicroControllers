/**	File:	RasPiBase.c
	Author:	J. Beighel
	Date:	12-17-2020
*/

/*****	Includes	*****/
	//Genereral use libraries
	#include "CommonUtils.h"
	#include "GPIOGeneralInterface.h"
	#include "SPIGeneralInterface.h"
	
	//Pin Support, implementation libraries
	#include "GPIO_RaspberryPi.h"
	#include "SPI_RaspberryPi.h"
	
	//Driver libraries
	#include "MAX7219Driver.h"

/*****	Defines		*****/
	#define LEDS_CHIPSEL	8

/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/
	sGPIOIface_t gGPIO;
	sSPIIface_t gSPI;
	
	sMAX7219Info_t gLeds;
	
/*****	Prototypes 	*****/


/*****	Functions	*****/
eReturn_t BoardInit(void) {
	int eResult;
	
	//Init processor (pin support work)
	eResult = GPIO_INIT(&gGPIO, GPIO_HWINFO);
	if (eResult != Success) {
		return Fail_Unknown;
	}
	
	eResult = SPI_INIT(&gSPI, SPI_1_HWINFO, 5000000, SPI_MSBFirst, SPI_Mode0);
	if (eResult != SPI_Success) {
		return Fail_Unknown;
	}
	
	//Init peripherals (board support work)
	MAX7219Initialize(&gLeds, 8, LEDS_CHIPSEL, &gGPIO, &gSPI);
	
	return Success;
}

int main(int nArgCnt, char **aArgVals) {
	
	if (BoardInit() != Success) {
		printf("Board initialization failed.\r\n");
		return 1;
	}
	
	MAX7219SetDigitLEDs(&gLeds, 0, MAX7219Led_0 | MAX7219Led_2);
	
	return 0;
}