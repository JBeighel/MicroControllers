/**	File:	DNPBase.c
	Author:	J. Beighel
	Date:	2021-03-15
*/

/*****	Includes	*****/
	#include "DNPBase.h"

/*****	Defines		*****/


/*****	Definitions	*****/


/*****	Constants	*****/
	/**	@brief		Constant array holding the size in bits of recognized DNP data objects
	 *	@ingroup	dnp
	 */
	const sDNPDataObjBitSize_t gDNPObjSize[] = {
		{ .eGroup = DNPGrp_BinaryInput,			.nVariation = 1,	.nBits = 1, },	//Packed format
		{ .eGroup = DNPGrp_BinaryInput,			.nVariation = 2,	.nBits = 8, },	//With flags

		{ .eGroup = DNPGrp_BinaryInputEvent,	.nVariation = 1,	.nBits = 8, },	//Without time
		{ .eGroup = DNPGrp_BinaryInputEvent,	.nVariation = 2,	.nBits = 56, },	//Absolute time
		{ .eGroup = DNPGrp_BinaryInputEvent,	.nVariation = 3,	.nBits = 24, },	//Relative time

		{ .eGroup = DNPGrp_BinaryOutput,		.nVariation = 1,	.nBits = 1, },	//Packed format
		{ .eGroup = DNPGrp_BinaryOutput,		.nVariation = 2,	.nBits = 8, },	//With flags

		{ .eGroup = DNPGrp_BinaryOutputCmd,		.nVariation = 1,	.nBits = 88, },	//Control relay output block (CROB)
		{ .eGroup = DNPGrp_BinaryOutputCmd,		.nVariation = 2,	.nBits = 88, },	//Pattern control block (PCB)
	};

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

	//Clear out the data object space
	pMsg->sDataObj.eQualifier = DNPQual_IndexPrefixNone | DNPQual_CodeNoRange;
	pMsg->sDataObj.eGroup = DNPGrp_Unknown;
	pMsg->sDataObj.nVariation = 0;
	pMsg->sDataObj.nAddressStart = 0;
	pMsg->sDataObj.nAddressEnd = 0;
	pMsg->sDataObj.nDataBytes = 0;
	pMsg->sDataObj.nIdxStart = 0;
	pMsg->sDataObj.nTotalBytes = 0;

	return Success;
}

uint16_t DNPGetDataObjectBitSize(eDNPGroup_t eGroup, uint8_t nVariation) {
	uint32_t nCtr;

	for (nCtr = 0; nCtr < sizeof(gDNPObjSize) / sizeof(sDNPDataObjBitSize_t); nCtr++) {
		if ((gDNPObjSize[nCtr].eGroup == eGroup) && (gDNPObjSize[nCtr].nVariation == nVariation)) {
			//Found the requested data object
			return gDNPObjSize[nCtr].nBits;
		}
	}

	return 0; //Couldn't find the requested object type
}
