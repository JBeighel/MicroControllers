/**	File:	RasPiBase.c
	Author:	J. Beighel
	Date:	12-17-2020
*/

/*****	Includes	*****/
	//Genereral use libraries
	#include "CommonUtils.h"
	#include "TimeGeneralInterface.h"
	#include "GPIOGeneralInterface.h"
	#include "I2CGeneralInterface.h"
	#include "SPIGeneralInterface.h"
	#include "UARTGeneralInterface.h"
	
	//Pin Support, implementation libraries
	#include "GPIO_RaspberryPi.h"
	#include "I2C_RaspberryPi.h"
	#include "SPI_RaspberryPi.h"
	#include "UART_RaspberryPi.h"
	
	//Driver libraries
	#include "RasPi-Network.h"

/*****	Defines		*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/
	sTimeIface_t gTime;
	sGPIOIface_t gGPIO;
	sI2CIface_t gI2C;
	sSPIIface_t gSPI;
	sUARTIface_t gUART;
	
	sTCPServ_t gTCPServ;
	
/*****	Prototypes 	*****/


/*****	Functions	*****/
eReturn_t BoardInit(void) {
	int eResult;
	
	//Init processor (pin support work)
	eResult = TIME_INIT(&gTime);
	if (eResult != Success) {
		return Fail_Unknown;
	}
	
	eResult = GPIO_INIT(&gGPIO, GPIO_HWINFO);
	if (eResult != Success) {
		return Fail_Unknown;
	}
	
	eResult = I2C_1_PORTINIT(&gI2C, true, 100000, I2C_1_HWINFO);
	if (eResult != GPIO_Success) {
		return Fail_Unknown;
	}
	
	eResult = SPI_1_PORTINIT(&gSPI, SPI_1_HWINFO, 5000000, SPI_MSBFirst, SPI_Mode0);
	if (eResult != SPI_Success) {
		return Fail_Unknown;
	}
	
	eResult = UART_PORTINITIALIZE(&gUART, 9600, UART_8None1, UART_1_HWINFO);
	if (eResult != UART_Success) {
		return Fail_Unknown;
	}
	
	//Init peripherals (board support work)
	
	
	return Success;
}

int main(int nArgCnt, char **aArgVals) {
	eNetReturn_t eResult;
	sConnInfo_t sConn;
	Socket_t ClientSck;
	
	if (BoardInit() != Success) {
		printf("Board initialization failed.\r\n");
		return 1;
	}
	
	RasPiTCPServInitialize(&gTCPServ);
	
	sConn.Port = 20000;
	sConn.Addr.Octets.b0 = 127;
	sConn.Addr.Octets.b1 = 0;
	sConn.Addr.Octets.b2 = 0;
	sConn.Addr.Octets.b3 = 1;
	
	eResult = gTCPServ.pfBind(&gTCPServ, &sConn);
	if (eResult != Net_Success) {
		printf("Failed to bind %d\r\n", eResult);
		return 1;
	}
	
	eResult = gTCPServ.pfAcceptClient(&gTCPServ, &ClientSck);
	if (eResult != Net_Success) {
		printf("Failed to accept %d\r\n", eResult);
		return 1;
	}
	
	return 0;
}