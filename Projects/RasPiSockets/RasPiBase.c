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
	
	//Pin Support, implementation libraries
	#include "GPIO_RaspberryPi.h"
	#include "I2C_RaspberryPi.h"
	#include "SPI_RaspberryPi.h"
	#include "UART_RaspberryPi.h"
	#include "Network_RaspberryPi.h"
	
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
	sSocket_t ClientSck;
	bool bServer, bTCP;
	char aData[255];
	uint32_t nBytes;
	
	if (BoardInit() != Success) {
		printf("Board initialization failed.\r\n");
		return 1;
	}
	
	//Arg 0 is the executable
	if ((nArgCnt == 2) && (aArgVals[1][0] == 's')) {
		bServer = true;
	} else {
		bServer = false;
	}
	
	TCPSERV_INIT(&gTCPServ);
	TCPCLIENT_INIT(&gTCPClient);
	UDPSERV_INIT(&gUDPServ);
	UDPCLIENT_INIT(&gUDPClient);
	
	sConn.Port = 20000;
	sConn.Addr.Octets.b0 = 127;
	sConn.Addr.Octets.b1 = 0;
	sConn.Addr.Octets.b2 = 0;
	sConn.Addr.Octets.b3 = 1;
	
	bTCP = false;
	if (bTCP == true) {
		if (bServer == true) {
			printf("TCP Server\r\n");
			
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
			
			eResult = gTCPServ.pfSend(&gTCPServ, &ClientSck, 12, "server send");
			if (eResult != Net_Success) {
				printf("Failed to send %d\r\n", eResult);
				return 1;
			}
			
			eResult = gTCPServ.pfReceive(&gTCPServ, &ClientSck, 255, aData, &nBytes);
			if (eResult != Net_Success) {
				printf("Failed to receive%d\r\n", eResult);
				return 1;
			}
			
			printf("Received %d bytes: %s\r\n", nBytes, aData);
			
			gTCPServ.pfCloseHost(&gTCPServ);
		} else {
			printf("TCP Client\r\n");
			
			eResult = gTCPClient.pfConnect(&gTCPClient, &sConn);
			if (eResult != Net_Success) {
				printf("Failed to connect %d\r\n", eResult);
				return 1;
			}
			
			eResult = gTCPClient.pfReceive(&gTCPClient, 255, aData, &nBytes);
			if (eResult != Net_Success) {
				printf("Failed to receive%d\r\n", eResult);
				return 1;
			}
			
			printf("Received %d bytes: %s\r\n", nBytes, aData);
			
			eResult = gTCPClient.pfSend(&gTCPClient, 12, "client send");
			if (eResult != Net_Success) {
				printf("Failed to send %d\r\n", eResult);
				return 1;
			}
			
			gTCPClient.pfClose(&gTCPClient);
		}
	} else {
		if (bServer == true) {
			printf("UDP Server\r\n");
			
			eResult = gUDPServ.pfBind(&gUDPServ, &sConn);
			if (eResult != Net_Success) {
				printf("Failed to bind %d\r\n", eResult);
				return 1;
			}
			
			eResult = gUDPServ.pfReceive(&gUDPServ, &(ClientSck.Conn), 255, aData, &nBytes);
			if (eResult != Net_Success) {
				printf("Failed to receive%d\r\n", eResult);
				return 1;
			}
			
			printf("Received %d bytes: %s\r\n", nBytes, aData);
			
			eResult = gUDPServ.pfSend(&gUDPServ, &(ClientSck.Conn), 12, "server send");
			if (eResult != Net_Success) {
				printf("Failed to send %d\r\n", eResult);
				return 1;
			}
			
			gUDPServ.pfCloseHost(&gUDPServ);
		} else {
			printf("UDP Client\r\n");
			
			eResult = gUDPClient.pfSetServer(&gUDPClient, &sConn);
			if (eResult != Net_Success) {
				printf("Failed to connect %d\r\n", eResult);
				return 1;
			}
			
			eResult = gUDPClient.pfSend(&gUDPClient, 12, "client send");
			if (eResult != Net_Success) {
				printf("Failed to send %d\r\n", eResult);
				return 1;
			}
			
			eResult = gUDPClient.pfReceive(&gUDPClient, 255, aData, &nBytes);
			if (eResult != Net_Success) {
				printf("Failed to receive%d\r\n", eResult);
				return 1;
			}
			
			printf("Received %d bytes: %s\r\n", nBytes, aData);
		}
	}
	
	return 0;
}