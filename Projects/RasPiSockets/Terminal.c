/**	File:	Terminal.c
	Author:	J. Beighel
	Date:	2021-04-18
*/

/*****	Includes	*****/
	#include "Terminal.h"

/*****	Defines		*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
eReturn_t IOCnctReadByteNoImplement(sIOConnect_t *pIOObj, uint8_t *pnByte);
eReturn_t IOCnctWriteByteNoImplement(sIOConnect_t *pIOObj, uint8_t nByte);
eReturn_t IOCnctWriteTextNoImplement(sIOConnect_t *pIOObj, char *pText);
eReturn_t IOCnctReadDataNoImplement(sIOConnect_t *pIOObj, uint8_t *pnDataBuff, uint32_t nBuffSize, uint32_t *pnReadSize);
eReturn_t IOCnctWriteDataNoImplement(sIOConnect_t *pIOObj, uint8_t *pnData, uint32_t nDataLen);

eReturn_t IOCnctReadByteUART(sIOConnect_t *pIOObj, uint8_t *pnByte);
eReturn_t IOCnctWriteByteUART(sIOConnect_t *pIOObj, uint8_t nByte);
eReturn_t IOCnctReadDataUART(sIOConnect_t *pIOObj, uint8_t *pnDataBuff, uint32_t nBuffSize, uint32_t *pnReadSize);
eReturn_t IOCnctWriteDataUART(sIOConnect_t *pIOObj, uint8_t *pnData, uint32_t nDataLen);

eReturn_t TerminalWriteTextLine(sTerminal_t *pTerminal, const char *pText);
eReturn_t TerminalReadInput(sTerminal_t *pTerminal);

eReturn_t TerminalProcessCommand(sTerminal_t *pTerminal, uint32_t nCmdLen);

eReturn_t TerminalAddSetHandler(sTerminal_t *pTerminal, pfTerminalSetHandler_t pFunc);
eReturn_t TerminalAddGetHandler(sTerminal_t *pTerminal, pfTerminalGetHandler_t pFunc);
eReturn_t TerminalAddCmdHandler(sTerminal_t *pTerminal, pfTerminalCommandHandler_t pFunc);

eReturn_t TerminalDelSetHandler(sTerminal_t *pTerminal, pfTerminalSetHandler_t pFunc);
eReturn_t TerminalDelGetHandler(sTerminal_t *pTerminal, pfTerminalGetHandler_t pFunc);
eReturn_t TerminalDelCmdHandler(sTerminal_t *pTerminal, pfTerminalCommandHandler_t pFunc);

uint32_t CountStringWhiteSpace(char *aText, uint32_t nStartIdx, uint32_t nStringLen);
uint32_t CountStringNonWhiteSpace(char *aText, uint32_t nStartIdx, uint32_t nStringLen);

eReturn_t IOCnctReadByteTCPServ(sIOConnect_t *pIOObj, uint8_t *pnByte);
eReturn_t IOCnctWriteByteTCPServ(sIOConnect_t *pIOObj, uint8_t nByte);

/*****	Functions	*****/
eReturn_t IOCnctObjectInitialize(sIOConnect_t *pIOObj) {
	pIOObj->pfReadByte = &IOCnctReadByteNoImplement;
	pIOObj->pfWriteByte = &IOCnctWriteByteNoImplement;
	pIOObj->pfReadData = &IOCnctReadDataNoImplement;
	pIOObj->pfWriteData = &IOCnctWriteDataNoImplement;
	
	pIOObj->pHWInfo = NULL;
	
	return Success;
}

eReturn_t IOCnctReadByteNoImplement(sIOConnect_t *pIOObj, uint8_t *pnByte) {
	return Fail_NotImplem;
}

eReturn_t IOCnctWriteByteNoImplement(sIOConnect_t *pIOObj, uint8_t nByte)  {
	return Fail_NotImplem;
}

eReturn_t IOCnctReadDataNoImplement(sIOConnect_t *pIOObj, uint8_t *pnDataBuff, uint32_t nBuffSize, uint32_t *pnReadSize) {
	eReturn_t eResult;
	uint32_t nCtr;

	*pnReadSize = 0; //Set initial value

	for (nCtr = 0; nCtr < nBuffSize; nCtr++) {
		eResult = pIOObj->pfReadByte(pIOObj, &(pnDataBuff[nCtr]));

		if (eResult == Warn_EndOfData) {
			return Warn_EndOfData;
		} else if (eResult != Success) {
			return eResult;
		}

		*pnReadSize += 1;
	}

	return Success;
}

eReturn_t IOCnctWriteDataNoImplement(sIOConnect_t *pIOObj, uint8_t *pnData, uint32_t nDataLen) {
	eReturn_t eResult;
	uint32_t nCtr;

	for (nCtr = 0; nCtr < nDataLen; nCtr++) {
		eResult = pIOObj->pfWriteByte(pIOObj, pnData[nCtr]);

		if (eResult != Success) {
			return eResult;
		}
	}

	return Success;
}

eReturn_t IOCnctCreateFromUART(sUARTIface_t *pUARTIface, sIOConnect_t *pIOObj) {
	IOCnctObjectInitialize(pIOObj);

	pIOObj->pfReadByte = &IOCnctReadByteUART;
	pIOObj->pfWriteByte = &IOCnctWriteByteUART;
	pIOObj->pfReadData = &IOCnctReadDataUART;
	pIOObj->pfWriteData = &IOCnctWriteDataUART;

	pIOObj->pHWInfo = pUARTIface;

	return Success;
}

eReturn_t IOCnctReadByteUART(sIOConnect_t *pIOObj, uint8_t *pnByte) {
	sUARTIface_t *pUART = (sUARTIface_t *)(pIOObj->pHWInfo);
	uint16_t nReadBytes;
	eUARTReturn_t eResult;

	eResult = pUART->pfUARTReadData(pUART, 1, pnByte, &nReadBytes);

	if (eResult < UART_Success) {
		return Fail_CommError;
	} else if ((eResult == UART_Warn_Timeout) || (nReadBytes < 1)) {
		return Warn_EndOfData;
	}

	return Success;
}

eReturn_t IOCnctWriteByteUART(sIOConnect_t *pIOObj, uint8_t nByte) {
	sUARTIface_t *pUART = (sUARTIface_t *)(pIOObj->pHWInfo);
	eUARTReturn_t eResult;

	eResult = pUART->pfUARTWriteData(pUART, 1, &nByte);

	if (eResult != UART_Success) {
		return Fail_CommError;
	}

	return Success;
}

eReturn_t IOCnctReadDataUART(sIOConnect_t *pIOObj, uint8_t *pnDataBuff, uint32_t nBuffSize, uint32_t *pnReadSize) {
	sUARTIface_t *pUART = (sUARTIface_t *)(pIOObj->pHWInfo);
	uint16_t nReadBytes, nDataLen;
	eUARTReturn_t eResult;

	while (nBuffSize > 0) {
		if (nBuffSize > UINT16_MAXVALUE) {
			nDataLen = UINT16_MAXVALUE;
		} else {
			nDataLen = nBuffSize;
		}

		eResult = pUART->pfUARTReadData(pUART, nDataLen, pnDataBuff, &nReadBytes);

		if (eResult < UART_Success) {
			return Fail_CommError;
		} else if ((eResult == UART_Warn_Timeout) || (nReadBytes < nDataLen)) {
			return Warn_EndOfData;
		}

		//REad successful, try to get more
		nBuffSize -= nDataLen;
		pnDataBuff = &(pnDataBuff[nDataLen]);
	}

	return Success;
}

eReturn_t IOCnctWriteDataUART(sIOConnect_t *pIOObj, uint8_t *pnData, uint32_t nDataLen) {
	sUARTIface_t *pUART = (sUARTIface_t *)(pIOObj->pHWInfo);
	eUARTReturn_t eResult;
	uint16_t nCurrWriteLen;

	while (nDataLen > 0) {
		if (nDataLen > UINT16_MAXVALUE) {
			nCurrWriteLen = UINT16_MAXVALUE;
		} else {
			nCurrWriteLen = nDataLen;
		}
		eResult = pUART->pfUARTWriteData(pUART, nCurrWriteLen, &pnData);

		if (eResult != UART_Success) {
			return Fail_CommError;
		}

		//Write successful, try to send more
		nDataLen -= nCurrWriteLen;
		pnData = &(pnData[nCurrWriteLen]);
	}

	return Success;
}

eReturn_t TerminalInitialize(sTerminal_t *pTerm, sIOConnect_t *pIOObj) {
	uint32_t nCtr;

	pTerm->pfWriteTextLine = &TerminalWriteTextLine;
	pTerm->pfReadInput = &TerminalReadInput;

	pTerm->pfAddSetHandler = &TerminalAddSetHandler;
	pTerm->pfAddGetHandler = &TerminalAddGetHandler;
	pTerm->pfAddCmdHandler = &TerminalAddCmdHandler;

	pTerm->pfRemoveSetHandler = &TerminalDelSetHandler;
	pTerm->pfRemoveGetHandler = &TerminalDelGetHandler;
	pTerm->pfRemoveCmdHandler = &TerminalDelCmdHandler;

	pTerm->pIOObj = pIOObj;
	pTerm->nBufferUsed = 0;

	for (nCtr = 0; nCtr < TERMINAL_MAXHANDLERS; nCtr++) {
		pTerm->pafSetHandlers[nCtr] = NULL;
		pTerm->pafGetHandlers[nCtr] = NULL;
		pTerm->pafCmdHandlers[nCtr] = NULL;
	}

	return Success;
}

eReturn_t TerminalWriteTextLine(sTerminal_t *pTerminal, const char *pText) {
	uint32_t nTextLen;
	eReturn_t eResult;

	nTextLen = strlen(pText);

	//Write the requested data
	eResult = pTerminal->pIOObj->pfWriteData(pTerminal->pIOObj, (void *)pText, nTextLen);
	if (eResult < Success) {
		return eResult;
	}

	//Add end of line characters
	eResult = pTerminal->pIOObj->pfWriteData(pTerminal->pIOObj, (void *)"\r\n", 2);

	return eResult;
}

eReturn_t TerminalReadInput(sTerminal_t *pTerminal) {
	eReturn_t eResult;
	uint32_t nReadBytes, nCtr, nCutCtr;

	//Read data from the IOConnect to fill the buffer
	eResult = pTerminal->pIOObj->pfReadData(pTerminal->pIOObj, (void *)&(pTerminal->aInputBuffer[pTerminal->nBufferUsed]), TERMINAL_BUFFERSIZE - pTerminal->nBufferUsed, &nReadBytes);

	if (eResult < Success) {
		return Fail_CommError;
	}

	pTerminal->nBufferUsed += nReadBytes; //Update how much of the buffer is used

	//Look through the buffer for new line characters
	for (nCtr = 0; nCtr < pTerminal->nBufferUsed; nCtr++) {
		if ((pTerminal->aInputBuffer[nCtr] == '\n') || (pTerminal->aInputBuffer[nCtr] == '\r')) {
			eResult = TerminalProcessCommand(pTerminal, nCtr);

			if (eResult == Warn_Incomplete) { //Command spans multiple lines?
				continue; //Continue to find another new line
			} else { //Dump this command to look for another one
				nCtr += 1; //1 character more to cut out the new line

				//Copy unprocessed characters to the beginning of the buffer
				for (nCutCtr = 0; nCutCtr + nCtr < pTerminal->nBufferUsed; nCutCtr++) {
					pTerminal->aInputBuffer[nCutCtr] = pTerminal->aInputBuffer[nCutCtr + nCtr];
				}
				pTerminal->aInputBuffer[nCtr] = '\0'; //Probably not needed, but won't hurt

				//Reduce the used count
				pTerminal->nBufferUsed -= nCtr;

				nCtr = UINT32_MAXVALUE; //It should increment and be at zero resetting the loop
			}
		}
	}

	return Success;
}

eReturn_t TerminalProcessCommand(sTerminal_t *pTerminal, uint32_t nCmdLen) {
	eReturn_t eResult;
	uint32_t nKeyIdx, nValIdx, nCtr;
	sRegExResult_t RXResult;
	bool bFoundHandler = false; //Will be set true if a handler accepts the command

	StrRegEx(pTerminal->aInputBuffer, "\\s*get\\s", RX_IgnoreCase, &RXResult);
	if (RXResult.eResult == RX_Success) { //Get command
		//Find the key
		nKeyIdx = RXResult.nLength;
		StrRegEx(&(pTerminal->aInputBuffer[nKeyIdx]), "\\s*(\\w+)\\s*$", RX_IgnoreCase, &RXResult);

		if (RXResult.eResult == RX_WarnNoMatch) {
			pTerminal->pfWriteTextLine(pTerminal, "Get command uses the format");
			pTerminal->pfWriteTextLine(pTerminal, "GET <Key>");
			pTerminal->pfWriteTextLine(pTerminal, "Where <Key> is the value to look up");
			return Fail_Invalid;
		}

		nKeyIdx += RXResult.aGroups[0][0];
		pTerminal->aInputBuffer[nKeyIdx + RXResult.aGroups[0][1]] = '\0'; //End the key string

		for (nCtr = 0; nCtr < TERMINAL_MAXHANDLERS; nCtr++) {
			if (pTerminal->pafGetHandlers[nCtr] != NULL) {
				eResult = pTerminal->pafGetHandlers[nCtr](pTerminal, &(pTerminal->aInputBuffer[nKeyIdx]));

				if (eResult == Success) {
					bFoundHandler = true;
				}
			}
		}

		if (bFoundHandler == true) { //Only end here if something accepted the command
			return Success;
		}else {
			pTerminal->pfWriteTextLine(pTerminal, "Get Value Unrecognized: ");
			pTerminal->pfWriteTextLine(pTerminal, &(pTerminal->aInputBuffer[nKeyIdx]));

			return Fail_Invalid;
		}
	}

	StrRegEx(pTerminal->aInputBuffer, "\\s*set\\s", RX_IgnoreCase, &RXResult);
	if (RXResult.eResult == RX_Success) { //Set command
		nKeyIdx = RXResult.nLength; //Don't forget this offset from the first match
		nValIdx = RXResult.nLength;
		StrRegEx(&(pTerminal->aInputBuffer[nKeyIdx]), "\\s*(\\w+)\\s+(\\S+)\\s*$", RX_IgnoreCase, &RXResult);

		if (RXResult.eResult == RX_WarnNoMatch) {
			pTerminal->pfWriteTextLine(pTerminal, "Set command uses the format");
			pTerminal->pfWriteTextLine(pTerminal, "SET <Key> <Value>");
			pTerminal->pfWriteTextLine(pTerminal, "Where <Key> specifies what to change and <Value> is the value to set");
			return Fail_Invalid;
		}

		nKeyIdx += RXResult.aGroups[0][0];
		pTerminal->aInputBuffer[nKeyIdx + RXResult.aGroups[0][1]] = '\0'; //End the key string

		nValIdx += RXResult.aGroups[1][0];
		pTerminal->aInputBuffer[nValIdx + RXResult.aGroups[1][1]] = '\0'; //End the value string

		for (nCtr = 0; nCtr < TERMINAL_MAXHANDLERS; nCtr++) {
			if (pTerminal->pafSetHandlers[nCtr] != NULL) {
				eResult = pTerminal->pafSetHandlers[nCtr](pTerminal, &(pTerminal->aInputBuffer[nKeyIdx]), &(pTerminal->aInputBuffer[nValIdx]));

				if (eResult == Success) {
					bFoundHandler = true;
				}
			}
		}

		if (bFoundHandler == true) { //Only end here if something accepted the command
			return Success;
		}else {
			pTerminal->pfWriteTextLine(pTerminal, "Set Value Unrecognized: ");
			pTerminal->pfWriteTextLine(pTerminal, &(pTerminal->aInputBuffer[nKeyIdx]));

			return Fail_Invalid;
		}
	}

	//All other commands go to generic handlers
	for (nCtr = 0; nCtr < TERMINAL_MAXHANDLERS; nCtr++) {
		if (pTerminal->pafCmdHandlers[nCtr] != NULL) {
			eResult = pTerminal->pafCmdHandlers[nCtr](pTerminal, pTerminal->aInputBuffer);

			if (eResult == Success) {
				bFoundHandler = true;
			}
		}
	}

	if (bFoundHandler == true) {
		return Success;
	} else { //Nothing accepted this command, give generic error
		pTerminal->pfWriteTextLine(pTerminal, "Command Unrecognized: ");
		pTerminal->pfWriteTextLine(pTerminal, pTerminal->aInputBuffer);
		return Fail_Invalid;
	}
}

eReturn_t TerminalAddSetHandler(sTerminal_t *pTerminal, pfTerminalSetHandler_t pFunc) {
	uint32_t nCtr;

	for (nCtr = 0; nCtr < TERMINAL_MAXHANDLERS; nCtr++) {
		if (pTerminal->pafSetHandlers[nCtr] == NULL) {
			pTerminal->pafSetHandlers[nCtr] = pFunc;
			return Success;
		}
	}

	return Fail_BufferSize;
}

eReturn_t TerminalAddGetHandler(sTerminal_t *pTerminal, pfTerminalGetHandler_t pFunc) {
	uint32_t nCtr;

		for (nCtr = 0; nCtr < TERMINAL_MAXHANDLERS; nCtr++) {
			if (pTerminal->pafGetHandlers[nCtr] == NULL) {
				pTerminal->pafGetHandlers[nCtr] = pFunc;
				return Success;
			}
		}

		return Fail_BufferSize;
}

eReturn_t TerminalAddCmdHandler(sTerminal_t *pTerminal, pfTerminalCommandHandler_t pFunc) {
	uint32_t nCtr;

		for (nCtr = 0; nCtr < TERMINAL_MAXHANDLERS; nCtr++) {
			if (pTerminal->pafCmdHandlers[nCtr] == NULL) {
				pTerminal->pafCmdHandlers[nCtr] = pFunc;
				return Success;
			}
		}

		return Fail_BufferSize;
}

eReturn_t TerminalDelSetHandler(sTerminal_t *pTerminal, pfTerminalSetHandler_t pFunc) {
	uint32_t nCtr;

		for (nCtr = 0; nCtr < TERMINAL_MAXHANDLERS; nCtr++) {
			if (pTerminal->pafSetHandlers[nCtr] == pFunc) {
				pTerminal->pafSetHandlers[nCtr] = NULL;
			}
		}

		return Success;
}

eReturn_t TerminalDelGetHandler(sTerminal_t *pTerminal, pfTerminalGetHandler_t pFunc)  {
	uint32_t nCtr;

		for (nCtr = 0; nCtr < TERMINAL_MAXHANDLERS; nCtr++) {
			if (pTerminal->pafGetHandlers[nCtr] == pFunc) {
				pTerminal->pafGetHandlers[nCtr] = NULL;
				return Success;
			}
		}

		return Fail_BufferSize;
}

eReturn_t TerminalDelCmdHandler(sTerminal_t *pTerminal, pfTerminalCommandHandler_t pFunc)  {
	uint32_t nCtr;

		for (nCtr = 0; nCtr < TERMINAL_MAXHANDLERS; nCtr++) {
			if (pTerminal->pafCmdHandlers[nCtr] == pFunc) {
				pTerminal->pafCmdHandlers[nCtr] = NULL;
				return Success;
			}
		}

		return Fail_BufferSize;
}

uint32_t CountStringWhiteSpace(char *aText, uint32_t nStartIdx, uint32_t nStringLen) {
	uint32_t nCtr;

	for (nCtr = nStartIdx; nCtr < nStringLen; nCtr++) {
		//Count any leading white space
		if (aText[nCtr] == ' ') {
			continue;
		}

		if (aText[nCtr] == '\t') {
			continue;
		}

		if (aText[nCtr] == '\r') {
			continue;
		}

		if (aText[nCtr] == '\n') {
			continue;
		}

		//On non-white space stop counting
		break;
	}

	return nCtr;
}

uint32_t CountStringNonWhiteSpace(char *aText, uint32_t nStartIdx, uint32_t nStringLen) {
	uint32_t nCtr;

	for (nCtr = nStartIdx; nCtr < nStringLen; nCtr++) {
		//Found white space, stop counting
		if (aText[nCtr] == ' ') {
			break;
		}

		if (aText[nCtr] == '\t') {
			break;
		}

		if (aText[nCtr] == '\r') {
			continue;
		}

		if (aText[nCtr] == '\n') {
			continue;
		}
	}

	return nCtr;
}

eReturn_t IOCnctCreateFromTCPServ(sTCPServ_t *pTCPIface, sIOConnect_t *pIOObj) {
	sSocket_t sckClient;
	eNetReturn_t eResult;
	
	IOCnctObjectInitialize(pIOObj);

	pIOObj->pfReadByte = &IOCnctReadByteTCPServ;
	pIOObj->pfWriteByte = &IOCnctWriteByteTCPServ;
	pIOObj->pfReadData = &IOCnctReadDataNoImplement;
	pIOObj->pfWriteData = &IOCnctWriteDataNoImplement;

	pIOObj->pHWInfo = pTCPIface;
	pIOObj->pClient = NULL;
	
	//Now wait on a connection to arrive, then create the terminal on that socket
	eResult = pTCPIface->pfAcceptClient(pTCPIface, &sckClient);
	if (eResult != Net_Success) {
		return Fail_CommError;
	}
	
	//Set a short timeout so reads don't block for long
	pTCPIface->pfSetRecvTimeout(pTCPIface, &sckClient, 1);
	
	pIOObj->pClient = (void *)sckClient.nSocket; //All we need is the socket number to comm through it

	return Success;
}

eReturn_t IOCnctReadByteTCPServ(sIOConnect_t *pIOObj, uint8_t *pnByte) {
	sSocket_t sckClient;
	uint32_t nBytesRcv;
	eNetReturn_t eResult;
	
	//Extract TCP objects
	sTCPServ_t *pServ = (sTCPServ_t *)pIOObj->pHWInfo;
	sckClient.nSocket = (int32_t)pIOObj->pClient;
	
	//Read some data
	eResult = pServ->pfReceive(pServ, &sckClient, 1, pnByte, &nBytesRcv);
	
	if (eResult == Net_Success) {
		return Success;
	} else if (eResult == NetWarn_EndOfData) {
		return Warn_EndOfData;
	} else {
		return Fail_CommError;
	}
}

eReturn_t IOCnctWriteByteTCPServ(sIOConnect_t *pIOObj, uint8_t nByte) {
	sSocket_t sckClient;
	eNetReturn_t eResult;
	
	//Extract TCP objects
	sTCPServ_t *pServ = (sTCPServ_t *)pIOObj->pHWInfo;
	sckClient.nSocket = (int32_t)pIOObj->pClient;
	
	//Read some data
	eResult = pServ->pfSend(pServ, &sckClient, 1, &nByte);
	
	if (eResult != Net_Success) {
		return Fail_CommError;
	}
	
	return Success;
}

