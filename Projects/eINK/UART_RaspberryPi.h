/**	@defgroup	uartraspi
	@brief		Raspberry Pi implementation of the UART General Interface
	@details	v0.2
	#Description
	
	#File Information
		File:	UART_RaspberryPi.h
		Author:	J. Beighel
		Date:	12-11-2020
*/

#ifndef __UARTRASPI
	#define __UARTRASPI

/*****	Includes	*****/
	#include <stdio.h>
	
	#include <unistd.h>
	#include <fcntl.h>
	#include <sys/ioctl.h>
	#include <termios.h>
	#include <errno.h>
	
	#include "UARTGeneralInterface.h"

/*****	Defines		*****/
	#ifdef UART_PORTINITIALIZE
		#undef UART_PORTINITIALIZE
	#endif
	
	#define UART_PORTINITIALIZE		RasPiUARTPortInit

	#define UART_1_HWINFO			((void *)&(gUARTHWInfo[0]))
	
	#define UART_1_CAPS				(UART_Configure | UART_Shutdown | UART_ReadData | UART_WriteData | UART_DataAvailable)

/*****	Definitions	*****/
	/**	@brief		Structure holding information on the UART Hardware
		@ingroup	uartraspi
	*/
	typedef struct sRasPiUARTHWInfo_t {
		const char *pcFilePath;	/**< Path to the filesystem object for this bus */
		int32_t UARTFile;		/**< File handle to use when interacting with this bus */
		int32_t nLastErr;		/**< Last error reported by the OS UART function calls */
	} sRasPiUARTHWInfo_t;

/*****	Constants	*****/
	extern sRasPiUARTHWInfo_t gUARTHWInfo[];


/*****	Globals		*****/


/*****	Prototypes 	*****/
	eUARTReturn_t RasPiUARTPortInit(sUARTIface_t *pUARTIface, uint32_t nBaudRate, eUARTModes_t eMode, void *pHWInfo);

/*****	Functions	*****/


#endif

