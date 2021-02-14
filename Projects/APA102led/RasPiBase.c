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
const uint32_t anPattern[] = {
	0x00220000,
	0x00220000,
	0x00221100,
	0x00221100,
	0x00222200,
	0x00222200,
	0x00112200,
	0x00112200,
	0x00002200,
	0x00002200,
	0x00002211,
	0x00002211,
	0x00002222,
	0x00002222,
	0x00001122,
	0x00001122,
	0x00000022,
	0x00000022,
	0x00110022,
	0x00110022,
	0x00220022,
	0x00220022,
	0x00220011,
	0x00220011,
};

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
	
	APA102SetLightColor(&gLed, 0, 30, 30, 30);
	APA102SetLightColor(&gLed, 1, 0, 0, 30);
	APA102SetLightColor(&gLed, 2, 0, 15, 30);
	APA102SetLightColor(&gLed, 3, 0, 30, 30);
	APA102SetLightColor(&gLed, 4, 0, 30, 15);
	APA102SetLightColor(&gLed, 5, 0, 30, 0);
	APA102SetLightColor(&gLed, 6, 15, 30, 0);
	APA102SetLightColor(&gLed, 7, 30, 30, 0);
	APA102SetLightColor(&gLed, 8, 30, 15, 0);
	APA102SetLightColor(&gLed, 9, 30, 0, 0);
	APA102SetLightColor(&gLed, 10, 30, 0, 15);
	APA102UpdateLights(&gLed);
	
	return 0;
}