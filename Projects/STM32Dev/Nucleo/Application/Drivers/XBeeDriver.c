/**	File:	XBeeDriver.c
	Author:	J. Beighel
	Date:	2021-08-28
*/

/***** Includes		*****/
	#include "XBeeDriver.h"

/***** Definitions	*****/


/***** Constants	*****/


/***** Globals		*****/


/***** Prototypes 	*****/


/***** Functions	*****/
eXBeeReturn_t XBeeInitialize(sXBeeObject_t *pXbeeObj, sUARTIface_t *pUART, sGPIOIface_t *pGPIO, sTimeIface_t *pTime, uint8_t nDTRPin, uint8_t nRstPin) {
	pXbeeObj->nDataReadyPin = nDTRPin;
	pXbeeObj->nResetPin = nRstPin;
	pXbeeObj->nFrameID = 1; //Must start at 1, zero means no reply needed
	pXbeeObj->pUART = pUART;
	pXbeeObj->pTime = pTime;
	pXbeeObj->pGPIO = pGPIO;
	
	return XB_Success;
}

uint8_t XBeeAPIChecksum(uint8_t *paMsg) {
  uint8_t nCheckSum = 0;
  uint16_t nCtr, nLength;

  nLength = paMsg[XBIdx_LengthMSB] << 8; //MSB
  nLength |= paMsg[XBIdx_LengthLSB]; //LSB
  nLength += XBIdx_DataStart; //Now its the index of the last byte

  for (nCtr = XBIdx_DataStart; nCtr < nLength; nCtr++) {
    nCheckSum += paMsg[nCtr]; //This will overflow, but we only want the lower 8 bits anyway
  }

  nCheckSum = 0xFF - nCheckSum;

  return nCheckSum;
}

eXBeeReturn_t XBeeReadMessage(sXBeeObject_t *pXbeeObj) {
	uint16_t nBytes, nRead, nCtr, nBuffIdx, nMsgLen;
	
	pXbeeObj->pUART->pfUARTDataAvailable(pXbeeObj->pUART, &nBytes);
	for (nCtr = 0; nCtr < XBEE_TIMEOUTLOOPS && nBytes == 0; nCtr++) {
		pXbeeObj->pUART->pfUARTDataAvailable(pXbeeObj->pUART, &nBytes);
		pXbeeObj->pTime->pfDelayMilliSeconds(1);
	}
	
	if (nBytes == 0) {
		return XBWarn_NoData;
	}
	
	pXbeeObj->anDataBuffer[0] = 0;
	//Look for start of message characters
	pXbeeObj->pUART->pfUARTReadData(pXbeeObj->pUART, 1, pXbeeObj->anDataBuffer, &nRead);
	for (nCtr = 0; (nCtr < nBytes) && (pXbeeObj->anDataBuffer[0] != XBEE_STARTBYTE); nCtr++) {
		pXbeeObj->pUART->pfUARTReadData(pXbeeObj->pUART, 1, pXbeeObj->anDataBuffer, &nRead);
		pXbeeObj->pTime->pfDelayMilliSeconds(1);
	}
	
	if (pXbeeObj->anDataBuffer[0] != XBEE_STARTBYTE) {
		//No message was waiting
		return XBWarn_NoMessage;
	} else {
		nBytes -= nCtr; //Adjust the number of bytes waiting to be read
		nBuffIdx = 1; //Write the next point for data to go
	}
	
	//Now read out the number of data bytes
	pXbeeObj->pUART->pfUARTDataAvailable(pXbeeObj->pUART, &nBytes);
	for (nCtr = 0; nCtr < XBEE_TIMEOUTLOOPS && nBytes < 2; nCtr++) { //Loop till the length bytes exist
		pXbeeObj->pUART->pfUARTDataAvailable(pXbeeObj->pUART, &nBytes);
		pXbeeObj->pTime->pfDelayMilliSeconds(1);
	}
	
	if (nCtr >= XBEE_TIMEOUTLOOPS) {
		//Never got our length, no message
		return XBWarn_NoMessage;
	}

	pXbeeObj->pUART->pfUARTReadData(pXbeeObj->pUART, 2, &(pXbeeObj->anDataBuffer[nBuffIdx]), &nRead);
	nMsgLen = pXbeeObj->anDataBuffer[nBuffIdx] << 8;
	nMsgLen |= pXbeeObj->anDataBuffer[nBuffIdx + 1];
	nMsgLen += XBEE_MSGBASELENGTH; //Add in non-data byres
	nBuffIdx += 2;
	
	if (nMsgLen > XBEE_RECVBUFFERSIZE) { //Message is to obig for the buffer
		pXbeeObj->pUART->pfUARTDataAvailable(pXbeeObj->pUART, &nBytes);
		nRead = 0;
		
		//Read out all of the message and dump it
		for (nCtr = 0; (nCtr < nMsgLen) && (nBytes > 0); nCtr += nRead) {
			if (nBytes > XBEE_RECVBUFFERSIZE) {
				nBytes = XBEE_RECVBUFFERSIZE;
			}
			
			pXbeeObj->pUART->pfUARTReadData(pXbeeObj->pUART, nBytes, pXbeeObj->anDataBuffer, &nRead);
			pXbeeObj->pUART->pfUARTDataAvailable(pXbeeObj->pUART, &nBytes);
		}
		
		return XBFail_MsgSize;
	}
	
	//Wait for and read in the rest of the message
	pXbeeObj->pUART->pfUARTDataAvailable(pXbeeObj->pUART, &nBytes);
	for (nCtr = 0; (nCtr < XBEE_TIMEOUTLOOPS) && (nBytes < nMsgLen - 3); nCtr++) { //Loop till the msg received (already read in 3 bytes)
		pXbeeObj->pUART->pfUARTDataAvailable(pXbeeObj->pUART, &nBytes);
		pXbeeObj->pTime->pfDelayMilliSeconds(1);
	}
	
	if (nCtr >= XBEE_TIMEOUTLOOPS) {
		//Never got our length, no message
		return XBWarn_NoMessage;
	}
	
	pXbeeObj->pUART->pfUARTReadData(pXbeeObj->pUART, nMsgLen - 3, &(pXbeeObj->anDataBuffer[nBuffIdx]), &nRead);
	
	//Verify data integrity
	nCtr = XBeeAPIChecksum(pXbeeObj->anDataBuffer);
	
	if (nCtr != pXbeeObj->anDataBuffer[nMsgLen - 1]) {
		//Checksum didn't match, bad message
		return XBFail_Checksum;
	}
	
	return XB_Success;
}

eXBeeReturn_t XBeeParseMessage(sXBeeObject_t *pXbeeObj, sXBeeFrameRecv_t *pFrameRecv) {
	pFrameRecv->nDataLen = pXbeeObj->anDataBuffer[XBIdx_LengthMSB] << 8;
	pFrameRecv->nDataLen |= pXbeeObj->anDataBuffer[XBIdx_LengthLSB];
	
	pFrameRecv->eType = (eXBeeFrameTypes_t)pXbeeObj->anDataBuffer[XBIdx_FrameType];
	pFrameRecv->nFrameID = pXbeeObj->anDataBuffer[XBIdx_FrameID];  //May not exist in all frames, remove it in the switch cases
	
	switch (pFrameRecv->eType) {
		case XBFrame_ATCommand:
			pFrameRecv->uData.ATCmd.aCmd[0] = pXbeeObj->anDataBuffer[XBIdx_ATCmd];
			pFrameRecv->uData.ATCmd.aCmd[1] = pXbeeObj->anDataBuffer[XBIdx_ATCmd + 1];
			
			pFrameRecv->uData.ATCmd.nParamLen = pFrameRecv->nDataLen - 4; //Frame Type, ID, and 2 AT cmd bytes
			
			if (pFrameRecv->uData.ATCmd.nParamLen > 0) {
				memcpy((void *)pFrameRecv->uData.ATCmd.aParam, (void *)&pXbeeObj->anDataBuffer[XBIdx_ATSetParam], pFrameRecv->uData.ATCmd.nParamLen);
			}
			
			break;
		case XBFrame_ATCmdResponse:
			pFrameRecv->uData.ATCmdResp.aCmd[0] = pXbeeObj->anDataBuffer[XBIdx_ATCmd];
			pFrameRecv->uData.ATCmdResp.aCmd[1] = pXbeeObj->anDataBuffer[XBIdx_ATCmd + 1];
			pFrameRecv->uData.ATCmdResp.eStatus = (xBeeATStatus_t)pXbeeObj->anDataBuffer[XBIdx_ATCmdStat];
			
			pFrameRecv->uData.ATCmdResp.nParamLen = pFrameRecv->nDataLen - 5; //Frame Type, ID, 2 AT cmd bytes, 1 status byte
			
			if (pFrameRecv->uData.ATCmdResp.nParamLen > 0) {
				memcpy((void *)pFrameRecv->uData.ATCmdResp.aParam, (void *)&pXbeeObj->anDataBuffer[XBIdx_ATReadParam], pFrameRecv->uData.ATCmd.nParamLen);
			}
			
			break;
		case XBFrame_TXRequest:
			pFrameRecv->uData.TXreq.nDestAddress = ((uint64_t)pXbeeObj->anDataBuffer[XBIdx_TXDestAddrMSB]) << 56;
			pFrameRecv->uData.TXreq.nDestAddress |= ((uint64_t)pXbeeObj->anDataBuffer[XBIdx_TXDestAddrMSB + 1]) << 48;
			pFrameRecv->uData.TXreq.nDestAddress |= ((uint64_t)pXbeeObj->anDataBuffer[XBIdx_TXDestAddrMSB + 2]) << 40;
			pFrameRecv->uData.TXreq.nDestAddress |= ((uint64_t)pXbeeObj->anDataBuffer[XBIdx_TXDestAddrMSB + 3]) << 32;
			pFrameRecv->uData.TXreq.nDestAddress |= ((uint64_t)pXbeeObj->anDataBuffer[XBIdx_TXDestAddrMSB + 4]) << 24;
			pFrameRecv->uData.TXreq.nDestAddress |= ((uint64_t)pXbeeObj->anDataBuffer[XBIdx_TXDestAddrMSB + 5]) << 16;
			pFrameRecv->uData.TXreq.nDestAddress |= ((uint64_t)pXbeeObj->anDataBuffer[XBIdx_TXDestAddrMSB + 6]) << 8;
			pFrameRecv->uData.TXreq.nDestAddress |= pXbeeObj->anDataBuffer[XBIdx_TXDestAddrMSB + 7];
			
			pFrameRecv->uData.TXreq.nNetAddress = pXbeeObj->anDataBuffer[XBIdx_TXNetAddrMSB] << 8;
			pFrameRecv->uData.TXreq.nNetAddress |= pXbeeObj->anDataBuffer[XBIdx_TXNetAddrMSB + 1];
			
			pFrameRecv->uData.TXreq.nBradcastRadius = pXbeeObj->anDataBuffer[XBIdx_TXBCastRad];
			
			pFrameRecv->uData.TXreq.eOptions = (eXBeeTransOptions_t)pXbeeObj->anDataBuffer[XBIdx_TXOpt];
		
			pFrameRecv->uData.TXreq.nDataLen = pFrameRecv->nDataLen - 14; //1 Type, 1 Id, 8 Dest addr, 2 net addr, 1 BCast, 1 Options
			
			if (pFrameRecv->uData.TXreq.nDataLen > 0) {
				memcpy((void *)pFrameRecv->uData.TXreq.aData, (void *)&(pXbeeObj->anDataBuffer[XBIdx_TXUserData]), pFrameRecv->uData.TXreq.nDataLen);
			}
	
			break;
		case XBFrame_TransmitStatus:
			pFrameRecv->uData.TXStatus.nNetAddress = pXbeeObj->anDataBuffer[XBIdx_StatusNetAddrMSB] << 8;
			pFrameRecv->uData.TXStatus.nNetAddress |= pXbeeObj->anDataBuffer[XBIdx_StatusNetAddrMSB + 1];
			
			pFrameRecv->uData.TXStatus.nTXRetryCnt = pXbeeObj->anDataBuffer[XBIdx_TXRetryCnt];
			
			pFrameRecv->uData.TXStatus.eStatus = (eXBeeTXStatus_t)pXbeeObj->anDataBuffer[XBIdx_DelStatus];
			pFrameRecv->uData.TXStatus.eDiscovery = (eXBeeDiscovStatus_t)pXbeeObj->anDataBuffer[XBIdx_Discovery];
			
			break;
		case XBFrame_RecvPacket:
			pFrameRecv->uData.RXPacket.nSrcAddress = (uint64_t)pXbeeObj->anDataBuffer[XBIdx_RXSrcAddrMSB] << 56;
			pFrameRecv->uData.RXPacket.nSrcAddress |= (uint64_t)pXbeeObj->anDataBuffer[XBIdx_RXSrcAddrMSB + 1] << 48;
			pFrameRecv->uData.RXPacket.nSrcAddress |= (uint64_t)pXbeeObj->anDataBuffer[XBIdx_RXSrcAddrMSB + 2] << 40;
			pFrameRecv->uData.RXPacket.nSrcAddress |= (uint64_t)pXbeeObj->anDataBuffer[XBIdx_RXSrcAddrMSB + 3] << 32;
			pFrameRecv->uData.RXPacket.nSrcAddress |= pXbeeObj->anDataBuffer[XBIdx_RXSrcAddrMSB + 4] << 24;
			pFrameRecv->uData.RXPacket.nSrcAddress |= pXbeeObj->anDataBuffer[XBIdx_RXSrcAddrMSB + 5] << 16;
			pFrameRecv->uData.RXPacket.nSrcAddress |= pXbeeObj->anDataBuffer[XBIdx_RXSrcAddrMSB + 6] << 8;
			pFrameRecv->uData.RXPacket.nSrcAddress |= pXbeeObj->anDataBuffer[XBIdx_RXSrcAddrMSB + 7];
			
			pFrameRecv->uData.RXPacket.nNetAddress = pXbeeObj->anDataBuffer[XBIdx_RXNetAddrMSB] << 8;
			pFrameRecv->uData.RXPacket.nNetAddress |= pXbeeObj->anDataBuffer[XBIdx_RXNetAddrMSB + 1];
			
			pFrameRecv->uData.RXPacket.eOptions = (eXBeeRecvOptions_t)pXbeeObj->anDataBuffer[XBIdx_RXOpt];
			
			pFrameRecv->uData.RXPacket.nDataLen = pFrameRecv->nDataLen - 12; //1 Type, 8 Src addr, 2 net addr, 1 Options
			
			if (pFrameRecv->uData.RXPacket.nDataLen > 0) {
				memcpy((void *)pFrameRecv->uData.RXPacket.aData, (void *)&(pXbeeObj->anDataBuffer[XBIdx_RXUserData]), pFrameRecv->uData.RXPacket.nDataLen);
			}
			
			break;
		default :
			pFrameRecv->eType = XBFrame_Unknown;
			
			pFrameRecv->nFrameID = 0;

			memcpy((void *)pFrameRecv->uData.aUnknown, (void *)&(pXbeeObj->anDataBuffer[XBIdx_DataStart]), pFrameRecv->nDataLen);
			
			break;
	}
	
	return XB_Success;
}

eXBeeReturn_t XBeeParseNetworkDiscoveryData(const uint8_t *pATCmdData, sXBeeNetDiscovResult_t *pNetworkDevice) {
	uint16_t nCtr;
	eXBeeReturn_t eRetVal = XB_Success;
	
	//2 bytes are network address, MSB -> LSB
	pNetworkDevice->nNetAddr = pATCmdData[0] << 8; //MSB
	pNetworkDevice->nNetAddr |= pATCmdData[1]; //LSB
	
	//8 bytes of Serail Number, MSB -> LSB
	pNetworkDevice->nSerial = (uint64_t)pATCmdData[2] << 56; //MSB
	pNetworkDevice->nSerial |= (uint64_t)pATCmdData[3] << 48;
	pNetworkDevice->nSerial |= (uint64_t)pATCmdData[4] << 40;
	pNetworkDevice->nSerial |= (uint64_t)pATCmdData[5] << 32;
	pNetworkDevice->nSerial |= pATCmdData[6] << 24;
	pNetworkDevice->nSerial |= pATCmdData[7] << 16;
	pNetworkDevice->nSerial |= pATCmdData[8] << 8;
	pNetworkDevice->nSerial |= pATCmdData[9]; //LSB
	
	//Variable bytes of Node Identifier, ends with 0x00 (11 starting index + Node ID buffer)
	for (nCtr = 10; pATCmdData[nCtr] != 0x00 && nCtr < 10 + XBEE_NODEIDLEN; nCtr++) {
		pNetworkDevice->aNodeID[nCtr - XBEE_NODEIDLEN] = pATCmdData[nCtr];
	}
	
	if (nCtr >= XBEE_NODEIDLEN) {
		pNetworkDevice->aNodeID[XBEE_NODEIDLEN - 1] = 0x00;
		
		for (nCtr = 11; pATCmdData[nCtr] != 0x00; nCtr++) {
			//Do nothing, just counting until the end of the node identifier
		}
		nCtr -= XBEE_NODEIDLEN;
		eRetVal = XBWarn_NodeIDLen;
	} else {
		nCtr -= XBEE_NODEIDLEN;
		pNetworkDevice->aNodeID[nCtr] = 0x00;
	} //Aftet this If nCtr is the number of bytes in the Node ID
	pNetworkDevice->nNodeIDLen = nCtr;
	nCtr += 1; //Add 1 for the null terminator byte
	
	//2 bytes parent network, MSB -> LSB
	pNetworkDevice->nParentNetwork = pATCmdData[10 + nCtr] << 8; //MSB
	pNetworkDevice->nParentNetwork |= pATCmdData[11 + nCtr]; //LSB
	
	//1 byte device type
	pNetworkDevice->eDeviceType = (eXBeeDeviceType_t)pATCmdData[12 + nCtr];
	
	//1 byte status
	pNetworkDevice->eStatus = (xBeeATStatus_t)pATCmdData[13 + nCtr];
	
	//2 bytes profile ID, MSB -> LSB
	pNetworkDevice->nProfileID = pATCmdData[14 + nCtr] << 8; //MSB
	pNetworkDevice->nProfileID |= pATCmdData[15 + nCtr]; //LSB
	
	//2 bytes manufacturer ID, MSB -> LSB
	pNetworkDevice->nManufacturer = pATCmdData[16 + nCtr] << 8; //MSB
	pNetworkDevice->nManufacturer |= pATCmdData[17 + nCtr]; //LSB
	
	return eRetVal;
}

eXBeeReturn_t XBeeATQueryCommand(sXBeeObject_t *pXbeeObj, const char ATCmd[2], uint8_t *pnFrameID) {
	uint16_t nDataLen = 4; //Frame Type byte + ID byte + 2 AT command characters
	
	//Set values common to all frames
	pXbeeObj->anDataBuffer[XBIdx_Start] = XBEE_STARTBYTE;
	pXbeeObj->anDataBuffer[XBIdx_LengthMSB] = (nDataLen & 0xFF00) >> 8;
	pXbeeObj->anDataBuffer[XBIdx_LengthLSB] = (nDataLen & 0x00FF);
	pXbeeObj->anDataBuffer[XBIdx_FrameType] = XBFrame_ATCommand;
	
	//Set values specific to this frame type
	pXbeeObj->anDataBuffer[XBIdx_FrameID] = pXbeeObj->nFrameID;
	pXbeeObj->anDataBuffer[XBIdx_DataStart + 2] = ATCmd[0];
	pXbeeObj->anDataBuffer[XBIdx_DataStart + 3] = ATCmd[1];
	
	pXbeeObj->anDataBuffer[XBIdx_DataStart + nDataLen] = XBeeAPIChecksum(pXbeeObj->anDataBuffer);
	
	//Send the message
	pXbeeObj->pUART->pfUARTWriteData(pXbeeObj->pUART, XBEE_MSGBASELENGTH + nDataLen, pXbeeObj->anDataBuffer);
	pXbeeObj->pUART->pfUARTWaitDataSend(pXbeeObj->pUART);
	
	//Increment the frame ID for the next message
	*pnFrameID = pXbeeObj->nFrameID;
	pXbeeObj->nFrameID += 1;
	
	return XB_Success;
}

eXBeeReturn_t XBeeATSetCommand(sXBeeObject_t *pXbeeObj, const char ATCmd[2], uint32_t nValue, uint8_t *pnFrameID) {
	
	
	return XB_Success;
}

eXBeeReturn_t XBeeTXReqest(sXBeeObject_t *pXbeeObj, uint64_t nDestAddr, uint16_t nNetAddr, uint16_t nDataBytes, const void *pData, uint8_t *pnFrameID) {
	uint16_t nDataLen = 14 + nDataBytes; //Frame Type byte + ID byte + 8 Dest addr + 2 net addr + 1 BCast + 1 Options + Data
	
	if (nDataLen > XBEE_DATABYTES) {
		return XBFail_MsgSize;
	}
	
	//Set values common to all frames
	pXbeeObj->anDataBuffer[XBIdx_Start] = XBEE_STARTBYTE;
	pXbeeObj->anDataBuffer[XBIdx_LengthMSB] = (nDataLen & 0xFF00) >> 8;
	pXbeeObj->anDataBuffer[XBIdx_LengthLSB] = (nDataLen & 0x00FF);
	pXbeeObj->anDataBuffer[XBIdx_FrameType] = XBFrame_TXRequest;
	
	//Set values specific to this frame type
	pXbeeObj->anDataBuffer[XBIdx_FrameID] = pXbeeObj->nFrameID;
	pXbeeObj->anDataBuffer[XBIdx_TXDestAddrMSB] = (nDestAddr >> 56) & 0xFF;
	pXbeeObj->anDataBuffer[XBIdx_TXDestAddrMSB + 1] = (nDestAddr >> 48) & 0xFF;
	pXbeeObj->anDataBuffer[XBIdx_TXDestAddrMSB + 2] = (nDestAddr >> 40) & 0xFF;
	pXbeeObj->anDataBuffer[XBIdx_TXDestAddrMSB + 3] = (nDestAddr >> 32) & 0xFF;
	pXbeeObj->anDataBuffer[XBIdx_TXDestAddrMSB + 4] = (nDestAddr >> 24) & 0xFF;
	pXbeeObj->anDataBuffer[XBIdx_TXDestAddrMSB + 5] = (nDestAddr >> 16) & 0xFF;
	pXbeeObj->anDataBuffer[XBIdx_TXDestAddrMSB + 6] = (nDestAddr >> 8) & 0xFF;
	pXbeeObj->anDataBuffer[XBIdx_TXDestAddrMSB + 7] = nDestAddr & 0xFF;
	
	pXbeeObj->anDataBuffer[XBIdx_TXNetAddrMSB] = (nNetAddr & 0xFF00) >> 8;
	pXbeeObj->anDataBuffer[XBIdx_TXNetAddrMSB + 1] = (nNetAddr & 0x00FF);
	
	pXbeeObj->anDataBuffer[XBIdx_TXBCastRad] = 0; //Use maximum number of hops
	
	pXbeeObj->anDataBuffer[XBIdx_TXOpt] = XBTXOpt_None; //Use default options
	
	memcpy((void *)&(pXbeeObj->anDataBuffer[XBIdx_TXUserData]), pData, nDataLen);
	
	pXbeeObj->anDataBuffer[XBIdx_DataStart + nDataLen] = XBeeAPIChecksum(pXbeeObj->anDataBuffer);
	
	//Send the message
	pXbeeObj->pUART->pfUARTWriteData(pXbeeObj->pUART, XBEE_MSGBASELENGTH + nDataLen, pXbeeObj->anDataBuffer);
	pXbeeObj->pUART->pfUARTWaitDataSend(pXbeeObj->pUART);
	
	//Increment the frame ID for the next message
	*pnFrameID = pXbeeObj->nFrameID;
	pXbeeObj->nFrameID += 1;
	
	return XB_Success;
}

