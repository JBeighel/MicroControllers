/**	@defgroup	terminal	Generic terminal interfaces
	@brief		Generic terminal user interface
	@details	v0.2
	#Description
	
	#File Information
		File:	Terminal.h
		Author:	J. Beighel
		Date:	2021-04-18
*/

#ifndef __TERMINAL_H
	#define __TERMINAL_H

/*****	Includes	*****/
	#include <string.h>

	#include "CommonUtils.h"
	#include "UARTGeneralInterface.h"
	#include "NetworkGeneralInterface.h"
	#include "StringTools.h"

/*****	Defines		*****/
	#define TERMINAL_BUFFERSIZE		35

	#define TERMINAL_MAXHANDLERS	5

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
		
		void *pHWInfo;						/**< Pointer to connection interface to use */
		void *pClient;						/**< Pointer to client information to use */
	} sIOConnect_t;

	typedef eReturn_t (*pfTerminalWriteTextLine_t)(sTerminal_t *pTerminal, const char *pText);
	typedef eReturn_t (*pfTerminalReadInput_t)(sTerminal_t *pTerminal);
	typedef eReturn_t (*pfTerminalSetHandler_t)(sTerminal_t *pTerminal, const char *pKey, const char *pValue);
	typedef eReturn_t (*pfTerminalGetHandler_t)(sTerminal_t *pTerminal, const char *pKey);
	typedef eReturn_t (*pfTerminalCommandHandler_t)(sTerminal_t *pTerminal, const char *pCmd);
	typedef eReturn_t (*pfTerminalUpdateSetHandler_t)(sTerminal_t *pTerminal, pfTerminalSetHandler_t pFunc);
	typedef eReturn_t (*pfTerminalUpdateGetHandler_t)(sTerminal_t *pTerminal, pfTerminalGetHandler_t pFunc);
	typedef eReturn_t (*pfTerminalUpdateCmdHandler_t)(sTerminal_t *pTerminal, pfTerminalCommandHandler_t pFunc);

	typedef struct sTerminal_t {
		pfTerminalWriteTextLine_t pfWriteTextLine;
		pfTerminalReadInput_t pfReadInput;

		pfTerminalUpdateSetHandler_t pfAddSetHandler;
		pfTerminalUpdateSetHandler_t pfRemoveSetHandler;

		pfTerminalUpdateGetHandler_t pfAddGetHandler;
		pfTerminalUpdateGetHandler_t pfRemoveGetHandler;

		pfTerminalUpdateCmdHandler_t pfAddCmdHandler;
		pfTerminalUpdateCmdHandler_t pfRemoveCmdHandler;

		pfTerminalSetHandler_t pafSetHandlers[TERMINAL_MAXHANDLERS];
		pfTerminalGetHandler_t pafGetHandlers[TERMINAL_MAXHANDLERS];
		pfTerminalCommandHandler_t pafCmdHandlers[TERMINAL_MAXHANDLERS];

		char aInputBuffer[TERMINAL_BUFFERSIZE];
		uint32_t nBufferUsed;
		sIOConnect_t *pIOObj;
	} sTerminal_t;

/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	eReturn_t IOCnctObjectInitialize(sIOConnect_t *pIOObj);

	eReturn_t IOCnctCreateFromUART(sUARTIface_t *pUARTIface, sIOConnect_t *pIOObj);
	
	eReturn_t IOCnctCreateFromTCPServ(sTCPServ_t *pTCPIface, sIOConnect_t *pIOObj);

	eReturn_t TerminalInitialize(sTerminal_t *pTerm, sIOConnect_t *pIOObj);

/*****	Functions	*****/


#endif

