/**	File:	RasPiBase.c
	Author:	J. Beighel
	Date:	12-16-2020
*/

/*****	Includes	*****/
	#include "CommonUtils.h"
	#include "TimeGeneralInterface.h"
	#include "GPIOGeneralInterface.h"
	#include "I2CGeneralInterface.h"
	#include "SPIGeneralInterface.h"
	#include "UARTGeneralInterface.h"
	
	#include "GPIO_RaspberryPi.h"
	#include "I2C_RaspberryPi.h"
	#include "SPI_RaspberryPi.h"
	#include "UART_RaspberryPi.h"

/*****	Defines		*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/
	sTimeIface_t gTime;
	sGPIOIface_t gGPIO;
	sI2CIface_t gI2C;
	sSPIIface_t gSPI;
	sUARTIface_t gUART;

/*****	Prototypes 	*****/


/*****	Functions	*****/
int main(int nArgCnt, char **aArgVals) {
	
	TIME_INIT(&gTime);
	GPIO_INIT(&gGPIO, GPIO_HWINFO);
	I2C_1_PORTINIT(&gI2C, true, 100000, I2C_1_HWINFO);
	SPI_1_PORTINIT(&gSPI, SPI_1_HWINFO, 5000000, SPI_MSBFirst, SPI_Mode0);
	UART_PORTINITIALIZE(&gUART, 9600, UART_8None1, UART_1_HWINFO);
	
	return 0;
}