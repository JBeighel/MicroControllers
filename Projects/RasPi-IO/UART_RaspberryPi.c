/**	File:	UART_RaspberryPi.c
	Author:	J. Beighel
	Date:	12-11-2020
*/

/*****	Includes	*****/
	#include "UART_RaspberryPi.h"

/*****	Defines		*****/


/*****	Definitions	*****/


/*****	Constants	*****/
	const char cgUART1File[] = "/dev/serial0";
	
	const char cgUART2File[] = "/dev/serial1";

/*****	Globals		*****/
	sRasPiSPIHWInfo_t gUARTHWInfo [] = {
		{ .pcFilePath = cgUART1File, .UARTFile = -1, },
		{ .pcFilePath = cgUART2File, .UARTFile = -1, },
	};

/*****	Prototypes 	*****/
	eUARTReturns_t	RasPiUARTShutdown(sUARTIface_t *pUARTIface);
		
	eUARTReturns_t	RasPiUARTUARTReadData(sUARTIface_t *pUARTIface, uint16_t nBuffSize, void *pDataBuff, uint16_t *pnBytesRead);
	
	eUARTReturns_t	RasPiUARTUARTWriteData(sUARTIface_t *pUARTIface, uint16_t nBuffSize, const void *pDataBuff);

/*****	Functions	*****/
eUARTReturns_t RasPiUARTPortInit(sUARTIface_t *pUARTIface, uint32_t nBaudRate, eUARTModes_t eMode, void *pHWInfo) {
	sRasPiSPIHWInfo_t *pUART = (sRasPiSPIHWInfo_t *)pHWInfo;
	struct termios UARTOpts;
	
	//Setup interface structure
	UARTInterfaceInitialize(pUARTIface);
	
	pUARTIface->nBaudRate = nBaudRate;
	pUARTIface->eMode = eMode;
	pUARTIface->pHWInfo = pHWInfo;
	
	pUARTIface->pfPortInitialize = &RasPiUARTPortInit;
	pUARTIface->pfShutdown = &RasPiUARTShutdown;
	pUARTIface->pfUARTReadData = &RasPiUARTUARTReadData;
	pUARTIface->pfUARTWriteData = &RasPiUARTUARTWriteData;
	
	//Setup the hardware
	UARTOpts.c_cflag = CLOCAL | CREAD; //Ignore momem status lines, and enable receive
	UARTOpts.c_iflag = 0;
	UARTOpts.c_oflag = 0;
	UARTOpts.c_lflag = 0;
	
	switch (nBaudRate) {
		case 1200:
			UARTOpts.c_cflag |= B1200;
			break;
		case 2400:
			UARTOpts.c_cflag |= B2400;
			break;
		case 1200:
			UARTOpts.c_cflag |= B1200;
			break;
		case 4800:
			UARTOpts.c_cflag |= B4800;
			break;
		case 9600:
			UARTOpts.c_cflag |= B9600;
			break;
		case 19200:
			UARTOpts.c_cflag |= B19200;
			break;
		case 38400:
			UARTOpts.c_cflag |= B38400;
			break;
		case 57600:
			UARTOpts.c_cflag |= B57600;
			break;
		case 115200:
			UARTOpts.c_cflag |= B115200;
			break;
		case 230400:
			UARTOpts.c_cflag |= B230400;
			break;
		case 460800:
			UARTOpts.c_cflag |= B460800;
			break;
		case 500000:
			UARTOpts.c_cflag |= B500000;
			break;
		case 576000:
			UARTOpts.c_cflag |= B576000;
			break;
		case 921600:
			UARTOpts.c_cflag |= B921600;
			break;
		case 1000000:
			UARTOpts.c_cflag |= B1000000;
			break;
		case 1152000:
			UARTOpts.c_cflag |= B1152000;
			break;
		case 2000000:
			UARTOpts.c_cflag |= B2000000;
			break;
		case 2500000:
			UARTOpts.c_cflag |= B2500000;
			break;
		case 3000000:
			UARTOpts.c_cflag |= B3000000;
			break;
		case 3500000:
			UARTOpts.c_cflag |= B3500000;
			break;
		case 4000000:
			UARTOpts.c_cflag |= B4000000;
			break;
		default:
			return UART_Fail_Unsupported;
	}
	
	switch (eMode) {
		case UART_5None1:
			UARTOpts.c_cflag |= C5; //5 data lines, 1 stop bit
			UARTOpts.c_iflag = IGNPAR; //ignore parity
			break;
		case UART_6None1:
			UARTOpts.c_cflag |= C6; //6 data lines, 1 stop bit
			UARTOpts.c_iflag = IGNPAR; //ignore parity
			break;
		case UART_7None1:
			UARTOpts.c_cflag |= C7; //7 data lines, 1 stop bit
			UARTOpts.c_iflag = IGNPAR; //ignore parity
			break;
		case UART_8None1:
			UARTOpts.c_cflag |= C8; //8 data lines, 1 stop bit
			UARTOpts.c_iflag = IGNPAR; //ignore parity
			break;
		case UART_5None2:
			UARTOpts.c_cflag |= C5 | CSTOPB; //5 data lines, 2 stop bit
			UARTOpts.c_iflag = IGNPAR; //ignore parity
			break;
		case UART_6None2:
			UARTOpts.c_cflag |= C6 | CSTOPB; //6 data lines, 2 stop bit
			UARTOpts.c_iflag = IGNPAR; //ignore parity
			break;
		case UART_7None2:
			UARTOpts.c_cflag |= C7 | CSTOPB; //8 data lines, 2 stop bit
			UARTOpts.c_iflag = IGNPAR; //ignore parity
			break;
		case UART_8None2:
			UARTOpts.c_cflag |= C7 | CSTOPB; //8 data lines, 2 stop bit
			UARTOpts.c_iflag = IGNPAR; //ignore parity
			break;
		case UART_5Even1:
			UARTOpts.c_cflag |= C5; //5 data lines, 1 stop bit, even parity
			UARTOpts.c_iflag = INPCK; //Enable parity
			break;
		case UART_6Even1:
		case UART_7Even1:
		case UART_8Even1:
		case UART_5Even2:
		case UART_6Even2:
		case UART_7Even2:
		case UART_8Even2:
		case UART_5Odd1:
			UARTOpts.c_cflag |= C5 | PARODD; //5 data lines, 1 stop bit, odd parity
			UARTOpts.c_iflag = INPCK; //Enable parity
			break;
		case UART_6Odd1:
		case UART_7Odd1:
		case UART_8Odd1:
		case UART_5Odd2:
		case UART_6Odd2:
		case UART_7Odd2:
		case UART_8Odd2: 
		default: 
			return UART_Fail_Unsupported;
	}
	
}

eUARTReturns_t	RasPiUARTShutdown(sUARTIface_t *pUARTIface) {
	
}
		
eUARTReturns_t	RasPiUARTUARTReadData(sUARTIface_t *pUARTIface, uint16_t nBuffSize, void *pDataBuff, uint16_t *pnBytesRead) {
	
}
	
eUARTReturns_t	RasPiUARTUARTWriteData(sUARTIface_t *pUARTIface, uint16_t nBuffSize, const void *pDataBuff) {
	
}
