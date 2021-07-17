/**	File:	RasPiBase.c
	Author:	J. Beighel
	Date:	12-17-2020
*/

/*****	Includes	*****/
	//Genereral use libraries
	#include "CommonUtils.h"
	#include "GPIOGeneralInterface.h"
	#include "I2CGeneralInterface.h"
	#include "SPIGeneralInterface.h"

	#include "GPIO_RaspberryPi.h"
	#include "I2C_RaspberryPi.h"
	#include "SPI_RaspberryPi.h"
	
	//Driver libraries
	#include "RC522Driver.h"
	
/*****	Defines		*****/


/*****	Definitions	*****/
	#define RC500_CSPIN		18
	#define RC500_RSTPIN	17

/*****	Constants	*****/


/*****	Globals		*****/
	sGPIOIface_t gGPIO;
	sI2CIface_t gI2C;
	sSPIIface_t gSPI;
	
	sRC522Obj_t gRC522;
	
/*****	Prototypes 	*****/


/*****	Functions	*****/
eReturn_t BoardInit(void) {
	int eResult;
	
	//Init processor (pin support work)
	eResult = GPIO_INIT(&gGPIO, GPIO_HWINFO);
	if (eResult != Success) {
		return Fail_Unknown;
	}
	
	eResult = I2C_INIT(&gI2C, true, 100000, I2C_1_HWINFO);
	if (eResult != GPIO_Success) {
		return Fail_Unknown;
	}
	
	eResult = SPI_INIT(&gSPI, SPI_1_HWINFO, 5000000, SPI_MSBFirst, SPI_Mode0);
	if (eResult != SPI_Success) {
		return Fail_Unknown;
	}
	
	//Init peripherals (board support work)
	eResult = RC522InitializeSPI(&gRC522, &gSPI, &gGPIO, RC500_CSPIN);
	
	//eResult = RC522InitializeI2C(&gRC522, &gI2C, RC522AddrBase);
	
	if (eResult != RC522_Success) {
		printf("RC522 Initialization Fail %d\n", eResult);
		return Fail_Unknown;
	}
	
	return Success;
}

int main(int nArgCnt, char **aArgVals) {
	
	if (BoardInit() != Success) {
		printf("Board initialization failed.\r\n");
		return 1;
	}
	
	return 0;
}