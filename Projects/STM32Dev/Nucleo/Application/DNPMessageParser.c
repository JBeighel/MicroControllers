/**	File:	DNPMessageParser.c
	Author:	J. Beighel
	Date:	2021-03-15
*/

/*****	Includes	*****/
	#include "DNPMessageParser.h"

/*****	Defines		*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/


/*****	Functions	*****/
eReturn_t DNPParserReceivedData(sDNPMsgBuffer_t *pMsg, uint8_t *pData, uint32_t nDataStart, uint32_t nDataLen, uint32_t *pnDataUsed) {
	uint32_t nCtr, nShiftCtr, nChunkStartIdx, nCurrIndex = nDataStart;
	uint16_t nU16Val, nExpMsgSize;
	crc16_t nCRCVal;

	*pnDataUsed = 0; //Set the return in case of failure

	//Check the inputs
	if ((nDataLen == 0) || (pData == NULL) || (nDataStart > nDataLen)) {
		return Fail_Invalid;
	}

	if (pMsg->nDNPMsgLen == 0) { //We're starting a new message, look for start bytes
		for (nCtr = nDataStart + 1; nCtr < nDataLen; nCtr++) {
			nU16Val = pData[nCtr - 1] << 8;
			nU16Val |= pData[nCtr];

			if (nU16Val == DNP_MSGSTARTBYTES) { //Found the start of the message in nCtr - 1
				UInt16ToBytes(DNP_MSGSTARTBYTES, false, pMsg->aDNPMessage, DNPHdrIdx_StartBytes);
				pMsg->nFramgentIdx = pMsg->nDNPMsgLen;
				pMsg->nDNPMsgLen += 2;
				*pnDataUsed += 2;
				nCurrIndex = nCtr + 1; //Set the index to right after the start bytes

				break;
			}
		}

		if (nCurrIndex == 0) { //No DNP message data could be found
			*pnDataUsed = nDataLen; //We went through all the data
			return Warn_Incomplete; //Message is still incomplete
		}
	}

	if ((pMsg->nDNPMsgLen - pMsg->nFramgentIdx < DNP_MSGHEADERLEN) && (nCurrIndex < nDataLen)) { //Need to get the rest of the header
		//Loop until we fill the header or run out of data
		for (nCtr = 0; (pMsg->nDNPMsgLen + nCtr < DNP_MSGHEADERLEN) && (nCurrIndex + nCtr < nDataLen); nCtr++) {
			pMsg->aDNPMessage[nCtr + pMsg->nDNPMsgLen] = pData[nCtr + nCurrIndex];
		}

		//Mark off the data pulled out
		*pnDataUsed += nCtr;
		nCurrIndex += nCtr;
		pMsg->nDNPMsgLen += nCtr;

		//Did we get all of the header?
		if (pMsg->nDNPMsgLen == DNP_MSGHEADERLEN) { //Yes, should be all of the header check its CRC
			nU16Val = BytesToUInt16(pMsg->aDNPMessage, false, DNPHdrIdx_CRC); //CRC in message
			nCRCVal = CalculateCRC16(CRC_DNP, pMsg->aDNPMessage, DNPHdrIdx_CRC); //Calculated CRC

			if (nCRCVal != nU16Val) { //CRC mismatch
				DNPBufferNewMessage(pMsg); //This message is invalid, start over
				return Warn_Incomplete;
			}

			//Header is valid, pull out the values
			pMsg->nDestAddr = BytesToUInt16(pMsg->aDNPMessage, true, DNPHdrIdx_DestAddr);
			pMsg->nSourceAddr = BytesToUInt16(pMsg->aDNPMessage, true, DNPHdrIdx_SourceAddr);

			pMsg->eDataControl = pMsg->aDNPMessage[DNPHdrIdx_DataControl];
		} else if (pMsg->nDNPMsgLen < DNP_MSGHEADERLEN) { //Message is still incomplete
			return Warn_Incomplete; //Message is still incomplete
		} else { //Index is past the header length, something went wrong
			DNPBufferNewMessage(pMsg); //This message is screwed, start over
			return Fail_Unknown;
		}
	}

	//We have the header, figure out how much data we should have
	nExpMsgSize = pMsg->aDNPMessage[DNPHdrIdx_DataLength] - 5; //Take out the header data bytes

	if ((nExpMsgSize % DNP_DATACRCCHUNKSIZE) != 0) {
		nExpMsgSize += (nExpMsgSize / DNP_DATACRCCHUNKSIZE) * 2; //2 CRC bytes for every full data chunk
		nExpMsgSize += 2; //CRC bytes for the partial chunk
	} else {
		nExpMsgSize += (nExpMsgSize / DNP_DATACRCCHUNKSIZE) * 2; //2 CRC bytes for every full data chunk
	}

	nExpMsgSize += 10; //Add the full header size, not just its data, to get a total message size

	//Do we have all of this message?
	if ((pMsg->nDNPMsgLen < nExpMsgSize) && (nCurrIndex < nDataLen)) { //Fill up more of the expected data
		//Figure out where the current data chunk started
		nChunkStartIdx = pMsg->nDNPMsgLen - 10; //Remove the header bytes
		nChunkStartIdx -= nChunkStartIdx % (DNP_DATACRCCHUNKSIZE + sizeof(crc16_t)); //The remainder is the partial chunk
		nChunkStartIdx += 10; //Put the header bytes back in

		//Go through the data adding to the message
		for (nCtr = 0; (pMsg->nDNPMsgLen < nExpMsgSize) && (nCurrIndex + nCtr < nDataLen); nCtr++) {
			pMsg->aDNPMessage[pMsg->nDNPMsgLen] = pData[nCtr + nCurrIndex];
			pMsg->nDNPMsgLen += 1;

			pMsg->aUserData[pMsg->nUserDataLen] = pData[nCtr + nCurrIndex]; //Fill out the user data too
			pMsg->nUserDataLen += 1;

			if (pMsg->nDNPMsgLen - nChunkStartIdx >= DNP_DATACRCCHUNKSIZE + sizeof(crc16_t)) {
				nU16Val = BytesToUInt16(pMsg->aDNPMessage, false, pMsg->nDNPMsgLen - 1); //CRC in message
				nCRCVal = CalculateCRC16(CRC_DNP, &(pMsg->aDNPMessage[nChunkStartIdx]), DNP_DATACRCCHUNKSIZE); //Calculated CRC
				pMsg->nUserDataLen -= 2; //The CRC value was copied into the user data, overwrite it
				nChunkStartIdx = pMsg->nDNPMsgLen; //Begin a new chunk

				if (nCRCVal != nU16Val) { //CRC mismatch
					DNPBufferNewMessage(pMsg); //This message is invalid, start over
					return Warn_Incomplete;
				}

				//First byte of data section is the transport header, don't put that into user data
				if (pMsg->nDNPMsgLen - pMsg->nFramgentIdx == DNP_MSGHEADERLEN + DNP_DATACRCCHUNKSIZE) {
					pMsg->nTransportSequence = pMsg->aUserData[pMsg->nUserDataLen - 16];

					//Shift the chunk bytes down by 1 to squeeze out the transport header
					for (nShiftCtr = pMsg->nUserDataLen - 16; nShiftCtr < pMsg->nUserDataLen; nShiftCtr++) {
						pMsg->aUserData[nShiftCtr] = pMsg->aUserData[nShiftCtr + 1];
					}

					pMsg->nUserDataLen -= 1; //Remove 1 byte for the discard transport header
				}
			}
		}

		//Mark off the data pulled out
		*pnDataUsed += nCtr;
		nCurrIndex += nCtr;

		//Did we get the entire message?
		if (pMsg->nDNPMsgLen == nExpMsgSize) { //Yup, its at least a full fragment
			if (nChunkStartIdx != pMsg->nDNPMsgLen) { //Ended with a partial chunk, check its CRC
				nU16Val = BytesToUInt16(pMsg->aDNPMessage, false, pMsg->nDNPMsgLen - 2); //CRC in message
				nCRCVal = CalculateCRC16(CRC_DNP, &(pMsg->aDNPMessage[nChunkStartIdx]), pMsg->nDNPMsgLen - (nChunkStartIdx + 2)); //Calculated CRC

				if (nCRCVal != nU16Val) { //CRC mismatch
					DNPBufferNewMessage(pMsg); //This message is invalid, start over
					return Warn_Incomplete;
				}

				pMsg->nUserDataLen -= 2; //The CRC value was copied into the user data, overwrite it
			}

			//Transport header indicates if this is the last fragment, make sure we have it
			if (pMsg->nDNPMsgLen - pMsg->nFramgentIdx <= DNP_MSGHEADERLEN + DNP_DATACRCCHUNKSIZE) {
				//Message didn't have a full chunk, so transport header wasn't extracted yet
				pMsg->nTransportSequence = pMsg->aDNPMessage[pMsg->nFramgentIdx + DNPHdrIdx_TransportHdr];

				//Shift the chunk bytes down by 1 to squeeze out the transport header
				nChunkStartIdx = pMsg->nDNPMsgLen - (nChunkStartIdx + 2); //Calculate the bytes in this chunk
				for (nShiftCtr = pMsg->nUserDataLen - nChunkStartIdx; nShiftCtr < pMsg->nUserDataLen; nShiftCtr++) {
					pMsg->aUserData[nShiftCtr] = pMsg->aUserData[nShiftCtr + 1];
				}

				pMsg->nUserDataLen -= 1; //Remove 1 byte for the discard transport header
			}

			if (CheckAllBitsInMask(pMsg->nTransportSequence, DNPTransHdr_LastMsg) == true) {
				pMsg->nTransportSequence &= DNPTransHdr_SequenceMask; //Now filter to just get the sequence

				//This fragment ends the message, get the application sequence
				pMsg->nApplicationSequence = pMsg->aDNPMessage[DNPHdrIdx_AppHdr] & DNPAppHdr_SequenceMask;
				pMsg->eControlCode = pMsg->aDNPMessage[DNPHdrIdx_ControlCode];

				pMsg->nUserDataIdx = 2; //The application header bytes are processed

				//If the message includes internal indicators, pull those out
				if ((pMsg->eControlCode == DNPCtrl_Response) || (pMsg->eControlCode == DNPCtrl_Unsolicited)) {
					pMsg->eIntIndicators = BytesToUInt16(pMsg->aDNPMessage, true, DNPHdrIdx_IntIndicators);
					pMsg->nUserDataIdx += 2; //The internal indicators bytes are processed
				} else {
					pMsg->eIntIndicators = DNPIntInd_None;
				}

				return Success;
			} else { //This fragment is complete, but the message isn't
				return Warn_Incomplete;
			}
		} else if (pMsg->nDNPMsgLen < nExpMsgSize) { //Still missing some
			pMsg->nFramgentIdx = pMsg->nDNPMsgLen + 1; //Next bytes should be a new fragment
			return Warn_Incomplete; //Current fragment is incomplete
		} else { //We have too much data in this message?
			DNPBufferNewMessage(pMsg); //This message is screwed, start over
			return Fail_Unknown;
		}
	}

	//Only should get here if we run out of data in the buffer
	return Warn_Incomplete; //Message is still incomplete and we're out of data
}

eReturn_t DNPParserNextDataObject(sDNPMsgBuffer_t *pMsg) {
	uint8_t nStartBytes, nStopBytes;
	uint32_t nCtr;
	eDNPQualifier_t eQualPart;

	if (pMsg->sDataObj.eGroup != DNPGrp_Unknown) {
		//Advance to the end of the current object
		pMsg->nUserDataIdx = pMsg->sDataObj.nIdxStart + pMsg->sDataObj.nTotalBytes;
	}

	if (pMsg->nUserDataIdx >= pMsg->nUserDataLen) {
		memset(&(pMsg->sDataObj), 0, sizeof(sDNPDataObject_t));
		return Warn_EndOfData;
	}

	//Save off the starting point of the data object
	pMsg->sDataObj.nIdxStart = pMsg->nUserDataIdx;

	//Start pulling out data description values
	pMsg->sDataObj.eGroup = pMsg->aUserData[pMsg->nUserDataIdx];
	pMsg->nUserDataIdx += 1;

	if ((pMsg->sDataObj.eGroup == DNPGrp_VirtualTerminalOut) || (pMsg->sDataObj.eGroup == DNPGrp_VirtualTerminalEvent)) {
		//Virtual terminal stuff doesn't have qualifier or variation, next byte is data length
		pMsg->sDataObj.nVariation = 0;
		pMsg->sDataObj.eQualifier = DNPQual_IndexPrefixNone | DNPQual_CodeSingleVal1Bytes;
	} else {
		//All other types have variation and qualifier bytes
		pMsg->sDataObj.nVariation = pMsg->aUserData[pMsg->nUserDataIdx];
		pMsg->nUserDataIdx += 1;

		pMsg->sDataObj.eQualifier = pMsg->aUserData[pMsg->nUserDataIdx];
		pMsg->nUserDataIdx += 1;
	}

	//The qualifier tells us what the range bytes look like
	eQualPart = pMsg->sDataObj.eQualifier & DNPQual_CodeMask;
	switch (eQualPart) {
		case DNPQual_CodeCountStopAndStart1Bytes:
		case DNPQual_CodeAddrStopAndStart1Bytes:
			nStartBytes = 1;
			nStopBytes = 1;
			break;
		case DNPQual_CodeCountStopAndStart2Bytes:
		case DNPQual_CodeAddrStopAndStart2Bytes:
			nStartBytes = 2;
			nStopBytes = 2;
			break;
		case DNPQual_CodeCountStopAndStart4Bytes:
		case DNPQual_CodeAddrStopAndStart4Bytes:
			nStartBytes = 4;
			nStopBytes = 4;
			break;
		case DNPQual_CodeFreeFormat:
		case DNPQual_CodeSingleVal1Bytes:
			nStartBytes = 0;
			nStopBytes = 1;
			break;
		case DNPQual_CodeSingleVal2Bytes:
			nStartBytes = 0;
			nStopBytes = 2;
			break;
		case DNPQual_CodeSingleVal4Bytes:
			nStartBytes = 0;
			nStopBytes = 4;
			break;
		case DNPQual_CodeNoRange:
			nStartBytes = 0;
			nStopBytes = 0;
			break;
		default:
			//Unrecognized Qualifier code
			return Fail_Invalid;
	}

	//Extract the start and stop addresses
	pMsg->sDataObj.nAddressStart = 0;
	for (nCtr = 0; nCtr < nStartBytes; nCtr++) {
		pMsg->sDataObj.nAddressStart = pMsg->aUserData[pMsg->nUserDataIdx];
		pMsg->nUserDataIdx += 1;
	}

	if ((eQualPart == DNPQual_CodeSingleVal1Bytes) || (eQualPart == DNPQual_CodeSingleVal2Bytes) || (eQualPart == DNPQual_CodeSingleVal4Bytes)) {
		//Single count values must start at 1 to avoid overrun
		pMsg->sDataObj.nAddressStart = 1;
	}

	pMsg->sDataObj.nAddressEnd = 0;
	for (nCtr = 0; nCtr < nStopBytes; nCtr++) {
		pMsg->sDataObj.nAddressEnd = pMsg->aUserData[pMsg->nUserDataIdx];
		pMsg->nUserDataIdx += 1;
	}

	//Get the number of bits in each value
	nCtr = pMsg->sDataObj.nAddressEnd - pMsg->sDataObj.nAddressStart; //Number of values
	pMsg->sDataObj.nDataBytes = DNPGetDataObjectBitSize(pMsg->sDataObj.eGroup, pMsg->sDataObj.nVariation);

	if (pMsg->sDataObj.nDataBytes == 1) { //Packed bits should not have a prefix
		pMsg->sDataObj.nTotalBytes = pMsg->sDataObj.nDataBytes / 8;

		if (pMsg->sDataObj.nDataBytes % 8 != 0) {
			pMsg->sDataObj.nTotalBytes += 1;
		}

		pMsg->sDataObj.nDataBytes = pMsg->sDataObj.nTotalBytes;
	} else { //Regular data is in even bytes
		pMsg->sDataObj.nDataBytes /= 8; //Convert from bits to byte

		pMsg->sDataObj.nTotalBytes = pMsg->sDataObj.nDataBytes * nCtr;
	}

	//Add in any prefix data
	eQualPart = pMsg->sDataObj.eQualifier & DNPQual_IndexMask;
	switch(eQualPart) {
		case DNPQual_IndexPrefixNone:
			pMsg->sDataObj.nPrefixBytes = 0;
			break;
		case DNPQual_IndexPrefix1Bytes:
			pMsg->sDataObj.nPrefixBytes = 1;
			break;
		case DNPQual_IndexPrefix2Bytes:
			pMsg->sDataObj.nPrefixBytes = 2;
			break;
		case DNPQual_IndexPrefix4Bytes:
			pMsg->sDataObj.nPrefixBytes = 4;
			break;
		default :
			return Fail_Invalid;
	}

	pMsg->sDataObj.nTotalBytes = pMsg->sDataObj.nPrefixBytes * nCtr;

	pMsg->sDataObj.nCurrPoint = 0; //Reset to get first point
	return Success;
}

eReturn_t DNPParserNextDataValue(sDNPMsgBuffer_t * pMsg, sDNPDataValue_t *pValue) {
	uint32_t nCtr;

	if (pMsg->sDataObj.eGroup == DNPGrp_Unknown) {
		//No data object was prepared
		return Fail_Invalid;
	}

	if (pMsg->sDataObj.nCurrPoint >= pMsg->sDataObj.nAddressEnd - pMsg->sDataObj.nAddressStart) {
		//Out of points in this data object
		return Warn_EndOfData;
	}

	if (pMsg->nUserDataIdx + pMsg->sDataObj.nDataBytes >= pMsg->nUserDataLen) {
		//Not enough user data left to read this point
		return Fail_BufferSize;
	}

	if ((pMsg->eControlCode == DNPCtrl_Read) || (pMsg->eControlCode == DNPCtrl_FreezeAndClear)) {
		//Read and freeze and clear requests do not have any data in the object
		return Warn_EndOfData;
	}

	//Everything seems to be in order, pull out the point: prefix first
	for (nCtr = 0; nCtr < pMsg->sDataObj.nPrefixBytes; nCtr++) {
		pValue->nPrefix[nCtr] = pMsg->aUserData[pMsg->nUserDataIdx]; //Prefix is least significant byte first
		pMsg->nUserDataIdx += 1;
	}

	//Next is the data
	for (nCtr = 0; nCtr < pMsg->sDataObj.nPrefixBytes; nCtr++) {
		pValue->Data.aBytes[nCtr] = pMsg->aUserData[pMsg->nUserDataIdx]; //Data is least significant byte first
		pMsg->nUserDataIdx += 1;
	}

	//Copy in all the object details
	pValue->eControl = pMsg->eControlCode;
	pValue->eGroup = pMsg->sDataObj.eGroup;
	pValue->nVariation = pMsg->sDataObj.nVariation;
	pValue->nAddress = pMsg->sDataObj.nAddressStart + pMsg->sDataObj.nCurrPoint;

	pMsg->sDataObj.nCurrPoint += 1; //update to get the next point

	return Success;
}
