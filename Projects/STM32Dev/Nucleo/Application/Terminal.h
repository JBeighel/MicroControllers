/**	@defgroup	terminal	Generic terminal interfaces
	@brief		Generic terminal user interface
	@details	v0.1
	#Description
	
	#File Information
		File:	Terminal.h
		Author:	J. Beighel
		Date:	2021-03-17
*/

#ifndef __TERMINAL_H
	#define __TERMINAL_H

/*****	Includes	*****/
	#include <string.h>

	#include "CommonUtils.h"
	#include "UARTGeneralInterface.h"

/*****	Defines		*****/
#define TERMINAL_BUFFERSIZE	512

/*****	Definitions	*****/
	typedef struct sIOConnect_t sIOConnect_t;
	
	typedef struct sTerminal_t sTerminal_t;

	typedef enum eIOConnectCapabilities_t {
		IOCnctCap_None		= 0x0000,
		IOCnctCap_Read		= 0x0001,
		IOCnctCap_Write		= 0x0002,
	} eIOConnectCapabilities_t;

	typedef eReturn_t (*pfIOCnctReadByte_t)(sIOConnect_t *pIOObj, uint8_t *pnByte);
	typedef eReturn_t (*pfIOCnctWriteByte_t)(sIOConnect_t *pIOObj, uint8_t nByte);
	typedef eReturn_t (*pfIOCnctReadData_t)(sIOConnect_t *pIOObj, uint8_t *pnDataBuff, uint32_t nBuffSize, uint32_t *pnReadSize);
	typedef eReturn_t (*pfIOCnctWriteData_t)(sIOConnect_t *pIOObj, uint8_t *pnData, uint32_t nDataLen);

	typedef struct sIOConnect_t {
		pfIOCnctReadByte_t pfReadByte;
		pfIOCnctWriteByte_t pfWriteByte;
		pfIOCnctReadData_t pfReadData;
		pfIOCnctWriteData_t pfWriteData;
		
		void *pHWInfo;
	} sIOConnect_t;

	typedef eReturn_t (*pfTerminalWriteTextLine_t)(sTerminal_t *pTerminal, char *pText);

	typedef eReturn_t (*pfTerminalReadInput_t)(sTerminal_t *pTerminal);

	typedef struct sTerminal_t {
		pfTerminalWriteTextLine_t pfWriteTextLine;
		pfTerminalReadInput_t pfReadInput;

		char aInputBuffer[TERMINAL_BUFFERSIZE];
		uint32_t nBufferUsed;
		sIOConnect_t *pIOObj;
	} sTerminal_t;

/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	eReturn_t IOCnctObjectInitialize(sIOConnect_t *pIOObj);

	eReturn_t IOCnctCreateFromUART(sUARTIface_t *pUARTIface, sIOConnect_t *pIOObj);

	eReturn_t TerminalInitialize(sTerminal_t *pTerm, sIOConnect_t *pIOObj);

/*****	Functions	*****/


#endif

