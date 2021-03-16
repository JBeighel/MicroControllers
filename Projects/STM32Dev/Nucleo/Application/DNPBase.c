/**	File:	DNPBase.c
	Author:	J. Beighel
	Date:	2021-03-15
*/

/*****	Includes	*****/
	#include "DNPBase.h"

/*****	Defines		*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/


/*****	Functions	*****/
eReturn_t UInt16ToBytes(uint16_t nValue, bool bLSBFirst, uint8_t *pBuffer, uint32_t nBuffOffset) {
	if (bLSBFirst == true) {
		pBuffer[nBuffOffset] = nValue & 0x00FF;
		pBuffer[nBuffOffset + 1] = nValue >> 8;
	} else {
		pBuffer[nBuffOffset] = nValue >> 8;
		pBuffer[nBuffOffset + 1] = nValue & 0x00FF;
	}

	return true;
}

eReturn_t UInt32ToBytes(uint32_t nValue, bool bLSBFirst, uint8_t *pBuffer, uint32_t nBuffOffset) {
	int nCtr;

	if (bLSBFirst == true) {
		for (nCtr = 0; nCtr < 4; nCtr++) {
			pBuffer[nBuffOffset + nCtr] = nValue & 0x000000FF;
			nValue = nValue >> 8;
		}
	} else {
		for (nCtr = 3; nCtr >= 0; nCtr--) {
			pBuffer[nBuffOffset + nCtr] = nValue & 0x000000FF;
			nValue = nValue >> 8;
		}
	}

	return true;
}

uint16_t BytesToUInt16(uint8_t *pBuffer, bool bLSBFirst, uint32_t nBuffOffset) {
	unsigned short nValue;

	if (bLSBFirst == true) {
		nValue = pBuffer[nBuffOffset + 1];
		nValue = nValue << 8;
		nValue += pBuffer[nBuffOffset];
	} else {
		nValue = pBuffer[nBuffOffset];
		nValue = nValue << 8;
		nValue += pBuffer[nBuffOffset + 1];
	}

	return nValue;
}

uint32_t BytesToUInt32(uint8_t *pBuffer, bool bLSBFirst, uint32_t nBuffOffset, uint32_t nLength) {
	unsigned int nCtr, nValue = 0;

	if (nLength > 4) {
		nLength = 4;
	}

	if (bLSBFirst == true) {
		for (nCtr = 0; nCtr < nLength; nCtr++) {
			nValue += pBuffer[nBuffOffset + nCtr] << (nCtr * 8);
		}
	} else {
		for (nCtr = 0; nCtr < nLength; nCtr++) {
			nValue = nValue << 8;
			nValue += pBuffer[nBuffOffset + nCtr];
		}
	}

	return nValue;
}

eReturn_t DNPBufferNewMessage(sDNPMsgBuffer_t *pMsg) {
	pMsg->nDNPMsgLen = 0;
	pMsg->nUserDataLen = 0;
	pMsg->nDestAddr = 0;
	pMsg->nSourceAddr = 0;
	pMsg->nUserDataIdx = 0;

	pMsg->eIntIndicators = DNPIntInd_None;
	pMsg->eControlCode = DNPCtrl_Response;
	pMsg->eDataControl = DNPData_Direction | DNPData_Primary | DNPData_UnconfirmData;

	//Each message should get a new sequence number, where they start isn't important
	pMsg->nTransportSequence += 1;
	pMsg->nApplicationSequence += 1;

	return Success;
}
