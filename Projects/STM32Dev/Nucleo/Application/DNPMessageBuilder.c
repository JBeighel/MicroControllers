/**	File:	DNPMessageBuilder.c
	Author:	J. Beighel
	Date:	2021-03-15
*/

/*****	Includes	*****/
	#include "DNPMessageBuilder.h"

/*****	Defines		*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	eReturn_t DNPBuilderAddByte(sDNPMsgBuffer_t *pMsg, uint8_t nData);
	eReturn_t DNPBuilderAddData(sDNPMsgBuffer_t *pMsg, uint8_t *pData, uint32_t nDataLen);

/*****	Functions	*****/

eReturn_t DNPBuilderAddByte(sDNPMsgBuffer_t *pMsg, uint8_t nData) {
	if (pMsg->nUserDataLen + 1 >= DNP_USERDATAMAX) {
		return Fail_BufferSize;
	}

	pMsg->aUserData[pMsg->nUserDataLen] = nData;
	pMsg->nUserDataLen += 1;

	return Success;
}

eReturn_t DNPBuilderAddData(sDNPMsgBuffer_t *pMsg, uint8_t *pData, uint32_t nDataLen) {
	uint32_t nCtr;

	if (pMsg->nUserDataLen + nDataLen >= DNP_USERDATAMAX) {
		return Fail_BufferSize;
	}

	for (nCtr = 0; nCtr < nDataLen; nCtr++) {
		pMsg->aUserData[pMsg->nUserDataLen] = pData[nCtr];
		pMsg->nUserDataLen += 1;
	}

	return Success;
}

eReturn_t DNPBuilderGenerateDNP(sDNPMsgBuffer_t *pMsg) {
	uint32_t nUserDataCnt, nFragDataCnt, nChunkCnt, nFragCnt, nChunkStartIdx;
	uint32_t nMsgSizeIndex, nMsgHdrStartIdx;
	crc16_t nCRCVal;

	//Start at the beginning
	pMsg->nDNPMsgLen = 0;	//Start with first byte in message buffer
	nUserDataCnt = 0;	//Count of User data bytes in the message
	nFragDataCnt = 0;	//Count of data bytes in the current fragment
	nChunkCnt = 0;		//Count of data bytes in current 16-byte CRC chunk
	nFragCnt = 0;		//Count of message fragments in this message

	//Build the message header
	while (nUserDataCnt < pMsg->nUserDataLen) {
		if (pMsg->nDNPMsgLen + 10 >= DNP_MESSAGESIZEMAX) { //Not enough space to fit the header for this fragment
			return Fail_BufferSize;
		}

		nMsgHdrStartIdx = pMsg->nDNPMsgLen;

		//Add in headers and framing for Transport and application layers
		if (nFragDataCnt == 0) { //Starting a new fragment
			pMsg->aDNPMessage[pMsg->nDNPMsgLen + DNPHdrIdx_DataControl] = pMsg->eDataControl;

			UInt16ToBytes(DNP_MSGSTARTBYTES, false, pMsg->aDNPMessage, pMsg->nDNPMsgLen + DNPHdrIdx_StartBytes);
			UInt16ToBytes(pMsg->nDestAddr, true, pMsg->aDNPMessage, pMsg->nDNPMsgLen + DNPHdrIdx_DestAddr);
			UInt16ToBytes(pMsg->nSourceAddr, true, pMsg->aDNPMessage, pMsg->nDNPMsgLen + DNPHdrIdx_SourceAddr);

			nMsgSizeIndex = pMsg->nDNPMsgLen + DNPHdrIdx_DataLength; //Once this data size of this fragment is know, put it here

			nFragDataCnt = 5; //The header always uses 5 bytes
			pMsg->nDNPMsgLen += 10; //Header uses up 10 bytes
		}

		nChunkStartIdx = pMsg->nDNPMsgLen; //CRC for 16 bytes of data starts here

		if (nFragCnt == 0) { //First fragment, application and transport headers needed
			//Transport header
			pMsg->aDNPMessage[pMsg->nDNPMsgLen] = DNPTransHdr_FirstMsg | (pMsg->nTransportSequence & DNPTransHdr_SequenceMask);
			if (pMsg->nUserDataLen - nUserDataCnt < DNP_MAXDATABYTES) { //The remaining data fits in this fragment
				pMsg->aDNPMessage[pMsg->nDNPMsgLen] |= DNPTransHdr_LastMsg;
			}

			pMsg->nDNPMsgLen += 1;
			nFragDataCnt += 1;
			nChunkCnt += 1;

			//Application header
			pMsg->aDNPMessage[pMsg->nDNPMsgLen] = DNPAppHdr_FirstMsg | DNPAppHdr_LastMsg | (DNPAppHdr_SequenceMask & pMsg->nApplicationSequence);
			if (pMsg->eControlCode != DNPCtrl_Response) { //Responses don't require a confirmation response
				pMsg->aDNPMessage[pMsg->nDNPMsgLen] |= DNPAppHdr_ConfirmExpect;
			}

			pMsg->nDNPMsgLen += 1;
			nFragDataCnt += 1;
			nChunkCnt += 1;

			//Control type (part of application layer)
			pMsg->aDNPMessage[pMsg->nDNPMsgLen] = pMsg->eControlCode;
			pMsg->nDNPMsgLen += 1;
			nFragDataCnt += 1;
			nChunkCnt += 1;

			if (pMsg->eControlCode == DNPCtrl_Response) { //Responses must include internal indicators
				UInt16ToBytes(pMsg->eIntIndicators, true, pMsg->aDNPMessage, pMsg->nDNPMsgLen + DNPHdrIdx_DestAddr);

				pMsg->nDNPMsgLen += 2;
				nFragDataCnt += 2;
				nChunkCnt += 2;
			}
		} else { //Subsequent fragment, transport header only
			//Transport header
			pMsg->aDNPMessage[pMsg->nDNPMsgLen] = (pMsg->nTransportSequence & DNPTransHdr_SequenceMask);
			if (pMsg->nUserDataLen - nUserDataCnt < DNP_MAXDATABYTES) { //The remaining data fits in this fragment
				pMsg->aDNPMessage[pMsg->nDNPMsgLen] |= DNPTransHdr_LastMsg;
			}

			pMsg->nDNPMsgLen += 1;
			nFragDataCnt += 1;
			nChunkCnt += 1;
		}

		//All the formalities are over, start stuffing in the actual data
		while ((nFragDataCnt < DNP_MAXDATABYTES) && (nUserDataCnt < pMsg->nUserDataLen)) {
			while ((nChunkCnt < DNP_DATACRCCHUNKSIZE) && (nUserDataCnt < pMsg->nUserDataLen)) {
				pMsg->aDNPMessage[pMsg->nDNPMsgLen] = pMsg->aUserData[nUserDataCnt];

				pMsg->nDNPMsgLen += 1;
				nFragDataCnt += 1;
				nChunkCnt += 1;
				nUserDataCnt += 1;

				if (pMsg->nDNPMsgLen >= DNP_MESSAGESIZEMAX) { //That byte filled the buffer
					return Fail_BufferSize;
				}
			}

			//Finished a chunk, give it a CRC
			if (pMsg->nDNPMsgLen + 2 >= DNP_MESSAGESIZEMAX) { //There isn't room for the CRC bytes
				return Fail_BufferSize;
			}

			nCRCVal = CalculateCRC16(CRC_DNP, &(pMsg->aDNPMessage[nChunkStartIdx]), nChunkCnt);
			UInt16ToBytes(nCRCVal, false, pMsg->aDNPMessage, pMsg->nDNPMsgLen);
			pMsg->nDNPMsgLen += 2;

			//Update counters for the next chunk
			nChunkStartIdx = pMsg->nDNPMsgLen;
			nChunkCnt = 0;
		}

		//Finished a fragment, set the size and header CRC
		pMsg->aDNPMessage[nMsgSizeIndex] = nFragDataCnt;

		if (pMsg->nDNPMsgLen + 2 >= DNP_MESSAGESIZEMAX) { //There isn't room for the CRC bytes
			return Fail_BufferSize;
		}

		//CRC header up to where the CRC bytes go
		nCRCVal = CalculateCRC16(CRC_DNP, &(pMsg->aDNPMessage[nMsgHdrStartIdx]), DNPHdrIdx_CRC);
		UInt16ToBytes(nCRCVal, false, pMsg->aDNPMessage, nMsgHdrStartIdx + DNPHdrIdx_CRC);

		//Update the counters for the next fragment
		nFragDataCnt = 0;
		nChunkCnt = 0;
		nFragCnt += 1;
	}

	//All message fragments are prepared and ready
	return Success;
}

eReturn_t DNPBuilderAddDataObjectRequest(sDNPMsgBuffer_t *pMsg, eDNPGroup_t eGroup, uint8_t nVariation, uint16_t nCountStart, uint16_t nCountStop) {
	unsigned char aCntBytes[4];
	eDNPQualifier_t eQualifier;

	if((nCountStart != 0) || (nCountStop != 0)){
		eQualifier = (eDNPQualifier_t)(DNPQual_IndexPrefixNone | DNPQual_CodeCountStopAndStart2Bytes);

		UInt16ToBytes(nCountStart, true, aCntBytes, 0);
		UInt16ToBytes(nCountStop, true, aCntBytes, 2);

		DNPBuilderAddByte(pMsg, eGroup);
		DNPBuilderAddByte(pMsg, nVariation);
		DNPBuilderAddByte(pMsg, eQualifier);
		DNPBuilderAddData(pMsg, aCntBytes, 2);
	} else {
		eQualifier = (eDNPQualifier_t)(DNPQual_IndexPrefixNone | DNPQual_CodeNoRange);

		DNPBuilderAddByte(pMsg, eGroup);
		DNPBuilderAddByte(pMsg, nVariation);
		DNPBuilderAddByte(pMsg, eQualifier);
	}

	return Success;
}

eReturn_t DNPBuilderAddBinaryOutputCommandDataObject(sDNPMsgBuffer_t *pMsg, uint8_t nVariation, uint16_t nPrefixIdx, eDNPBinOutControlCode_t eCtrlCode, uint8_t nOpCount, uint32_t nOnTime, uint32_t nOffTime, uint8_t nStatus) {
	unsigned char aBytes[4];

	switch (nVariation) {
		case 1: //Control relay output block
		case 2: //pattern control block
			DNPBuilderAddByte(pMsg, DNPGrp_BinaryOutputCmd); //Group
			DNPBuilderAddByte(pMsg, nVariation);
			DNPBuilderAddByte(pMsg, DNPQual_IndexPrefix2Bytes | DNPQual_CodeSingleVal2Bytes); //Qualifier
			UInt16ToBytes(0x0001, true, aBytes, 0); //Only controlling 1 point
			DNPBuilderAddData(pMsg, aBytes, 2);// 2 byte quantity

			UInt16ToBytes(nPrefixIdx, true, aBytes, 0);
			DNPBuilderAddData(pMsg, aBytes, 2); // 2 byte prefix

			//Fill in the Control Relay Block (CROB)
			DNPBuilderAddByte(pMsg, eCtrlCode);
			DNPBuilderAddByte(pMsg, nOpCount);
			UInt32ToBytes(nOnTime, true, aBytes, 0);
			DNPBuilderAddData(pMsg, aBytes, 4);
			UInt32ToBytes(nOffTime, true, aBytes, 0);
			DNPBuilderAddData(pMsg, aBytes, 4);
			DNPBuilderAddByte(pMsg, nStatus);

			break;
		default:
			return Fail_Invalid;
	}

	return Success;
}
