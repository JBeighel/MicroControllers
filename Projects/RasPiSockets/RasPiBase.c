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
	#include "NetworkGeneralInterface.h"

	#include "GPIO_RaspberryPi.h"
	#include "I2C_RaspberryPi.h"
	#include "SPI_RaspberryPi.h"
	#include "UART_RaspberryPi.h"
	#include "Network_RaspberryPi.h"
	
	#include "Terminal.h"
	
	//Driver libraries
	
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
	sTCPClient_t gTCPClient;
	sUDPServ_t gUDPServ;
	sUDPClient_t gUDPClient;
	
	bool bStillConnected;
	
/*****	Prototypes 	*****/
	eReturn_t TelnetCmdHandler(sTerminal_t *pTerminal, const char *pCmd);

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
	
	eResult = I2C_INIT(&gI2C, true, 100000, I2C_1_HWINFO);
	if (eResult != GPIO_Success) {
		return Fail_Unknown;
	}
	
	eResult = SPI_INIT(&gSPI, SPI_1_HWINFO, 5000000, SPI_MSBFirst, SPI_Mode0);
	if (eResult != SPI_Success) {
		return Fail_Unknown;
	}
	
	eResult = UART_INIT(&gUART, 9600, UART_8None1, UART_1_HWINFO);
	if (eResult != UART_Success) {
		return Fail_Unknown;
	}
	
	TCPSERV_INIT(&gTCPServ);
	TCPCLIENT_INIT(&gTCPClient);
	UDPSERV_INIT(&gUDPServ);
	UDPCLIENT_INIT(&gUDPClient);
	
	//Init peripherals (board support work)
	
	
	return Success;
}

int main(int nArgCnt, char **aArgVals) {
	if (BoardInit() != Success) {
		printf("Board initialization failed.\r\n");
		return 1;
	}
	
	sTerminal_t Terminal;
	sIOConnect_t IOObj;
	sConnInfo_t Conn;
	
	Conn.Addr.Octets.b0 = 192;
	Conn.Addr.Octets.b1 = 168;
	Conn.Addr.Octets.b2 = 1;
	Conn.Addr.Octets.b3 = 200;
	Conn.Port = 23;
	
	gTCPServ.pfBind(&gTCPServ, &Conn);
	IOCnctCreateFromTCPServ(&gTCPServ, &IOObj);
	TerminalInitialize(&Terminal, &IOObj);
	Terminal.pfAddCmdHandler(&Terminal, &TelnetCmdHandler);
	
	Terminal.pfWriteTextLine(&Terminal, "Welcome!");
	bStillConnected = true;
	
	while (bStillConnected == true) {
		//Read input goes into an infinite loop pulling in data
		//It doesn't have any response that no data is waiting
		//So the terminal doesn't know to stop looking for more
		if (Terminal.pfReadInput(&Terminal) < Success) {
			//Failed to read input?
			printf("Terminal Input Read Failed!");
			bStillConnected = false;
		}
		
		//Need a way to tell if the socket was closed
	}
	
	return 0;
}

eReturn_t TelnetCmdHandler(sTerminal_t *pTerminal, const char *pCmd) {
	pTerminal->pfWriteTextLine(pTerminal, "Command>");
	pTerminal->pfWriteTextLine(pTerminal, pCmd);
	pTerminal->pfWriteTextLine(pTerminal, "<End");
	
	return Fail_Unknown;
}
