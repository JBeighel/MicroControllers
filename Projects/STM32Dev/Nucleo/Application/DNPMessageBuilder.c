/**	File:	DNPMessageBuilder.c
	Author:	J. Beighel
	Date:	2021-03-11
*/

/*****	Includes	*****/
	#include "DNPMessageBuilder.h"

/*****	Defines		*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	eReturn_t DNPBuilderAddByte(sDNPMsgBuild_t *pMsg, uint8_t nData);
	eReturn_t DNPBuilderAddData(sDNPMsgBuild_t *pMsg, uint8_t *pData, uint32_t nDataLen);

/*****	Functions	*****/

eReturn_t DNPBuilderAddByte(sDNPMsgBuild_t *pMsg, uint8_t nData) {
	if (pMsg->nUserDataLen + 1 >= DNP_USERDATAMAX) {
		return Fail_Unknown;
	}

	pMsg->aUserData[pMsg->nUserDataLen] = nData;
	pMsg->nUserDataLen += 1;

	return Success;
}

eReturn_t DNPBuilderAddData(sDNPMsgBuild_t *pMsg, uint8_t *pData, uint32_t nDataLen) {
	uint32_t nCtr;

	if (pMsg->nUserDataLen + nDataLen >= DNP_USERDATAMAX) {
		return Fail_Unknown;
	}

	for (nCtr = 0; nCtr < nDataLen; nCtr++) {
		pMsg->aUserData[pMsg->nUserDataLen] = pData[nCtr];
		pMsg->nUserDataLen += 1;
	}

	return Success;
}

eReturn_t DNPBuilderNewMessage(sDNPMsgBuild_t *pMsg) {
	pMsg->nDNPMsgLen = 0;
	pMsg->nUserDataLen = 0;
	pMsg->nDestAddr = 0;
	pMsg->nSourceAddr = 0;
	pMsg->eIntIndicators = 0;
	pMsg->eControlCode = 0;

	return Success;
}

eReturn_t DNPBuilderGenerateDNP(sDNPMsgBuild_t *pMsg) {

}
