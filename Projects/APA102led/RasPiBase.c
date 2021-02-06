/**	File:	RasPiBase.c
	Author:	J. Beighel
	Date:	12-17-2020
*/

/*****	Includes	*****/
	//Genereral use libraries
	#include "CommonUtils.h"
	#include "SPIGeneralInterface.h"
	
	#include "SPI_RaspberryPi.h"
	
	//Driver libraries
	#include "APA102Driver.h"
	
/*****	Defines		*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/
	sSPIIface_t gSPI;
	
	sAPA102Info_t gLed;
	
/*****	Prototypes 	*****/


/*****	Functions	*****/
eReturn_t BoardInit(void) {
	int eResult;
	
	//Init processor (pin support work)
	eResult = SPI_INIT(&gSPI, SPI_1_HWINFO, 5000000, SPI_MSBFirst, SPI_Mode0);
	if (eResult != SPI_Success) {
		return Fail_Unknown;
	}
	
	//Init peripherals (board support work)
	APA102Initialize(&gLed, &gSPI);
	
	return Success;
}

int main(int nArgCnt, char **aArgVals) {
	
	if (BoardInit() != Success) {
		printf("Board initialization failed.\r\n");
		return 1;
	}
	
	APA102SetLightColor(&gLed, 0, 150, 150, 150);
	APA102SetLightColor(&gLed, 1, 0, 0, 150);
	APA102SetLightColor(&gLed, 2, 0, 150, 0);
	APA102SetLightColor(&gLed, 3, 150, 0, 0);
	APA102SetLightColor(&gLed, 4, 150, 150, 0);
	APA102SetLightColor(&gLed, 5, 150, 0, 150);
	APA102SetLightColor(&gLed, 6, 0, 150, 150);
	APA102UpdateLights(&gLed);
	
	return 0;
}