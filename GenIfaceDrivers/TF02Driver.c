/*
	File:	TF02Driver.c
	Author:	J. Beighel
	Date:	09-15-2020
*/


/*****	Includes	*****/
	#include "TF02Driver.h"

/*****	Constants	*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/


/*****	Functions	*****/

eTF02Returns_t TF02Initialize(sTF02Device_t *pTF02Obj, sUARTIface_t *pUART) {
	pTF02Obj->pUART = pUART;
	pTF02Obj->nDataIdx = 0; 
	
	return TF02_Success;
}

eTF02Returns_t TF02GetReading(sTF02Device_t *pTF02Obj, uint16_t *pnDistCM, uint16_t *pnSigStr) {
	uint16_t nBytesAvail, nBytesRead, nCtr;
	eUARTReturns_t eReturn;
	
	(*pnDistCM) = 0;
	(*pnSigStr) = 0;
	
	eReturn = pTF02Obj->pUART->pfUARTDataAvailable(pTF02Obj->pUART, &nBytesAvail);
	
	if (eReturn != UART_Success) {
		return TF02Fail_UART;
	}
	
	while ((pTF02Obj->nDataIdx < 9) && (nBytesAvail > 0)) {
		if (pTF02Obj->nDataIdx == 0) { //Waiting for the header byte to arrive
			pTF02Obj->pUART->pfUARTReadData(pTF02Obj->pUART, 1, &(pTF02Obj->uData.aRaw[0]), & nBytesRead);
			
			if (pTF02Obj->uData.aRaw[0] == TF02_HEADERBYTE) {
				pTF02Obj->nDataIdx += 1;
			}
		} else if (pTF02Obj->nDataIdx == 1) { //Waiting for second header byte to arrive
			pTF02Obj->pUART->pfUARTReadData(pTF02Obj->pUART, 1, &(pTF02Obj->uData.aRaw[1]), &nBytesRead);
			
			if (pTF02Obj->uData.aRaw[0] == TF02_HEADERBYTE) {
				pTF02Obj->nDataIdx += 1;
			} else { //Invalid frame start, scrap and start over
				pTF02Obj->nDataIdx = 0;
			}
		} else { //Reading the 7 data bytes
			pTF02Obj->pUART->pfUARTReadData(pTF02Obj->pUART, 1, &(pTF02Obj->uData.aRaw[pTF02Obj->nDataIdx]), &nBytesRead);
			pTF02Obj->nDataIdx += 1;
		}

		//See if more data has arrived
		eReturn = pTF02Obj->pUART->pfUARTDataAvailable(pTF02Obj->pUART, &nBytesAvail);
	
		if (eReturn != UART_Success) {
			return TF02Fail_UART;
		}
	}
	
	if (pTF02Obj->nDataIdx < 9) { //Didn't receive a full frame
		return TF02Fail_NoData;
	}
	
	pTF02Obj->nDataIdx = 0; //Reset for the next data read
	
	//Do the checksum test
	nBytesRead = 0;
	for (nCtr = 0; nCtr < TF02_FRAMESIZE - 1; nCtr++) {
		nBytesRead += pTF02Obj->uData.aRaw[nCtr];
	}
	nBytesRead &= 0x00FF; //Only the low bytes is used in the checksum
	if (pTF02Obj->uData.sParse.nChecksum != nBytesRead) {
		return TF02Fail_Checksum;
	}
	
	//Return the results
	(*pnDistCM) = pTF02Obj->uData.sParse.nDistLow;
	(*pnDistCM) |= pTF02Obj->uData.sParse.nDistHigh << 8;
	
	(*pnSigStr) = pTF02Obj->uData.sParse.nSigStrLow;
	(*pnSigStr) |= pTF02Obj->uData.sParse.nSigStrHigh << 8;
	
	if ((pTF02Obj->uData.sParse.nReliability == 7) || (pTF02Obj->uData.sParse.nReliability == 8)) {
		return TF02_Success;
	} else {
		return TF02Warn_Unreliable;
	}
}