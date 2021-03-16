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
	uint32_t nCtr, nChunkStartIdx, nCurrIndex = nDataStart;
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
				pMsg->nDNPMsgLen = 2;
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

	if ((pMsg->nDNPMsgLen < DNP_MSGHEADERLEN) && (nCurrIndex < nDataLen)) { //Need to get the rest of the header
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

			//First byte in the data section should be the transport header
			nU16Val = pMsg->aDNPMessage[DNPHdrIdx_TransportHdr];

			if (CheckAllBitsInMask(nU16Val, DNPTransHdr_LastMsg) == true) {
				//This fragment ends the message
				return Success;
			} else { //This fragment is complete, but the message isn't
				return Warn_Incomplete;
			}
		} else if (pMsg->nDNPMsgLen < nExpMsgSize) { //Still missing some
			return Warn_Incomplete; //Current fragment is incomplete
		} else { //We have too much data in this message?
			DNPBufferNewMessage(pMsg); //This message is screwed, start over
			return Fail_Unknown;
		}
	}

	//Only should get here if we run out of data in the buffer
	return Warn_Incomplete; //Message is still incomplete and we're out of data
}
