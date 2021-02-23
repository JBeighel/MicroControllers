/*	File:	W5500Driver.c
	Author:	J. Beighel
	Date:	08-27-2020
*/

/***** Includes		*****/
	#include "W5500Driver.h"
	

/***** Definitions	*****/
	
	
/***** Constants	*****/


/***** Globals		*****/


/***** Prototypes 	*****/
	/**	@brief		Read data from the Wiznet peripheral
		@details	Performs read operations with the peripheral over the SPI bus
		@param		pDev		Pointer to the Wiznet5500 device object
		@param		nAddress	The address in the requested back to read from
		@param		eControl	Indicate the memory bank to read from
		@param		pBuff		Buffer to hold the read data
		@param		nBytes		Number of bytes to read
		@ingroup	w5500driver
	*/
	eW5500Return_t W5500ReadData(sW5500Obj_t *pDev, uint16_t nAddress, eW5500Control_t eControl, uint8_t *pBuff, uint8_t nBytes);
	
	/**	@brief		Write data from the Wiznet peripheral
		@details	Performs write operations with the peripheral over the SPI bus
		@param		pDev		Pointer to the Wiznet5500 device object
		@param		nAddress	The address in the requested back to write to
		@param		eControl	Indicate the memory bank to write to
		@param		pBuff		Buffer to hold the data to write
		@param		nBytes		Number of bytes to write
		@ingroup	w5500driver
	*/
	eW5500Return_t W5500WriteData(sW5500Obj_t *pDev, uint16_t nAddress, eW5500Control_t eControl, const uint8_t *pBuff, uint8_t nBytes);
	
	/**	@brief		Binds a TCP port on the Wiznet device and listens for connections
		@details	The wiznet device does not allow multiply IP addresses, so the IP
			value in the pConn paramater will not be checked.  This will always assume
			it is being asked to bind on the Wiznet's assigned IP address.
			
			The Wiznet also can not accept multiple connections on a single port.  Once
			a connection request is made, it will be accepted and the socket will no 
			longer be bound.  When a connection is accepted it is necessary to bind
			the socket again.
		@param		pTCPServ		Pointer to the Wiznet5500 TCP server interface object
		@param		pConn			Connection information on the socket to bind
		@return		Net_Success on success, or a code indicating the failure
		@ingroup	w5500driver
	*/
	eNetReturn_t W5500NetTCPServBind(sTCPServ_t *pTCPServ, sConnInfo_t *pConn);
	
	/**	@brief		Closes down the host socket if it is current bind and listening
		@param		pTCPServ		Pointer to the Wiznet5500 TCP server interface object
		@return		Net_Success on success, or a code indicating the failure
		@ingroup	w5500driver
	*/
	eNetReturn_t W5500NetTCPServCloseHost(sTCPServ_t *pTCPServ);
	
	eNetReturn_t W5500NetTCPServAcceptClient(sTCPServ_t *pTCPServ, sSocket_t *pClientSck);
	
	eNetReturn_t W5500NetTCPServCloseSocket(sTCPServ_t *pTCPServ, sSocket_t *pSck);

/***** Functions	*****/
eW5500Return_t W5500Initialize(sW5500Obj_t *pDev, sSPIIface_t *pSpiBus, sGPIOIface_t *pIOObj, uint8_t nCSPin) {
	uint8_t nCtr, nModeReg, nCmd, nControl;
	
	//Set up module object
	pDev->pGPIO = pIOObj;
	pDev->pSPI = pSpiBus;
	pDev->nChipSelectPin = nCSPin;
	pDev->nNextPort = W5500_HIGHPORTSTART;
	
	//Prep the chip select pin
	pDev->pGPIO->pfSetModeByPin(pDev->pGPIO, pDev->nChipSelectPin, GPIO_DigitalOutput);
	pDev->pGPIO->pfDigitalWriteByPin(pDev->pGPIO, pDev->nChipSelectPin, true);
	
	//Reset the device
	nModeReg = W5500CmdMode_Reset;
	W5500WriteData(pDev, W5500SckReg_Mode, W5500BSB_CommonRegister, &nModeReg, 1);
	DELAYMILLISEC(5);
	
	//Clear ping disable bit
	W5500ReadData(pDev, W5500SckReg_Mode, W5500BSB_CommonRegister, &nModeReg, 1);
	nModeReg &= ~W5500CmdMode_PingBlock; //Turn off ping blockign 
	W5500WriteData(pDev, W5500SckReg_Mode, W5500BSB_CommonRegister, &nModeReg, 1);
	
	//Set no protocol for all ports
	nModeReg = W5500SckMode_ProtClosed;
		
	nCmd = W5500SckCmd_Close;
	for (nCtr = 0; nCtr < W5500_NUMSOCKETS; nCtr++) {
		nControl = nCtr << W5500BSB_SocketLShift;
		nControl |= W5500BSB_Register;
		
		W5500WriteData(pDev, W5500SckReg_Mode, (eW5500Control_t)nControl, &nModeReg, 1);
		W5500WriteData(pDev, W5500SckReg_Command, (eW5500Control_t)nControl, &nCmd, 1);
		
		//Set all buffers to 2K, evenly spreads out available space
		nCmd = W5500SckBuff_2KB;
		W5500WriteData(pDev, W5500SckReg_RXBuffSize, (eW5500Control_t)nControl, &nCmd, 1);
		
		nCmd = W5500SckBuff_2KB;
		W5500WriteData(pDev, W5500SckReg_TXBuffSize, (eW5500Control_t)nControl, &nCmd, 1);
	}
	
	return W5500_Success;
}

eW5500Return_t W5500VerifyPart(sW5500Obj_t *pDev) {
	uint8_t nRecv;
	
	W5500ReadData(pDev, W5500CmnReg_Version, W5500BSB_CommonRegister, &nRecv, 1);
	
	if (nRecv == W5500_VERSION) {
		return W5500_Success;
	} else {
		return W5500Fail_WrongPart;
	}
}

eW5500Return_t W5500SetMACAddress(sW5500Obj_t *pDev, const uint8_t aMac[6]) {
	uint8_t aVerify[6];
	uint8_t nCtr;
	
	W5500WriteData(pDev, W5500CmnReg_SrcHWAddr0, W5500BSB_CommonRegister, aMac, 6);
	
	W5500ReadData(pDev, W5500CmnReg_SrcHWAddr0, W5500BSB_CommonRegister, aVerify, 6);
	
	for (nCtr = 0; nCtr < 6; nCtr++) {
		if (aVerify[nCtr] != aMac[nCtr]) {
			return W5500Fail_VerifyRead;
		}
	}
	
	return W5500_Success;
}

eW5500Return_t W5500SetIPAddress(sW5500Obj_t *pDev, const IN_ADDR *Address) {
	IN_ADDR Verify;
	
	W5500WriteData(pDev, W5500CmnReg_SrcIPAddr0, W5500BSB_CommonRegister, (uint8_t *)Address, 4);
	
	Verify.S_un.S_addr = 0;
	W5500ReadData(pDev, W5500CmnReg_SrcIPAddr0, W5500BSB_CommonRegister, (uint8_t *)&Verify, 4);
	
	if (Address->S_un.S_addr == Verify.S_un.S_addr) {
		return W5500_Success;
	} else {
		return W5500Fail_VerifyRead;
	}
}

eW5500Return_t W5500ReadIPAddress(sW5500Obj_t *pDev, IN_ADDR *Address) {
	W5500ReadData(pDev, W5500CmnReg_SrcIPAddr0, W5500BSB_CommonRegister, (uint8_t *)Address, 4);

	return W5500_Success;
}

eW5500Return_t W5500SetSubnetMask(sW5500Obj_t *pDev, const IN_ADDR *Address) {
	IN_ADDR Verify;
	
	W5500WriteData(pDev, W5500CmnReg_SubnetMask0, W5500BSB_CommonRegister, (uint8_t *)Address, 4);
	
	W5500ReadData(pDev, W5500CmnReg_SubnetMask0, W5500BSB_CommonRegister, (uint8_t *)&Verify, 4);
	
	if (Address->S_un.S_addr == Verify.S_un.S_addr) {
		return W5500_Success;
	} else {
		return W5500Fail_VerifyRead;
	}
}

eW5500Return_t W5500SetDefaultGateway(sW5500Obj_t *pDev, const IN_ADDR *Address) {
	IN_ADDR Verify;
	
	W5500WriteData(pDev, W5500CmnReg_GatewayAddr0, W5500BSB_CommonRegister, (uint8_t *)Address, 4);
	
	W5500ReadData(pDev, W5500CmnReg_GatewayAddr0, W5500BSB_CommonRegister, (uint8_t *)&Verify, 4);
	
	if (Address->S_un.S_addr == Verify.S_un.S_addr) {
		return W5500_Success;
	} else {
		return W5500Fail_VerifyRead;
	}
}

eW5500Return_t W5500LinkStatus(sW5500Obj_t *pDev, bool *pbIsUp) {
	uint8_t nRegVal;
	
	W5500ReadData(pDev, W5500CmnReg_PhyConfig, W5500BSB_CommonRegister, &nRegVal, 1);
	
	if (CheckAllBitsInMask(nRegVal, W5500Phy_LinkStatus) == true) {
		*pbIsUp = true;
	} else {
		*pbIsUp = false;
	}
	
	return W5500_Success;
}

eW5500Return_t W5500SocketListen(sW5500Obj_t *pDev, uint8_t *pnSocket, uint16_t nPort, eW5500SckProt_t eProtocol) {
	uint8_t nCtr, nControl;
	uint8_t anRegVal[2];
	
	//Look for an unused socket
	for (nCtr = 0; nCtr < W5500_NUMSOCKETS; nCtr++) {
		nControl = nCtr << W5500BSB_SocketLShift;
		nControl |= W5500BSB_Register;
		W5500ReadData(pDev, W5500SckReg_Status, (eW5500Control_t)nControl, &anRegVal[0], 1);
		
		if (anRegVal[0] == W5500SckStat_Closed) {
			break;
		}
	}
	
	if (nCtr >= W5500_NUMSOCKETS) { //All sockets are in use
		return W5500Fail_NoSockets;
	} else {
		*pnSocket = nCtr;
	}
	
	//Found a socket to use, get it setup
	nControl = (*pnSocket) << W5500BSB_SocketLShift;
	nControl |= W5500BSB_Register;
	
	anRegVal[0] = W5500SckInt_AllMask; //Clear any pending interrupts
	W5500WriteData(pDev, W5500SckReg_Interrupt, (eW5500Control_t)nControl, &anRegVal[0], 1);
	
	anRegVal[0] = nPort >> 8;
	anRegVal[1] = nPort & 0xFF;
	W5500WriteData(pDev, W5500SckReg_SourcePort0, (eW5500Control_t)nControl, anRegVal, 2);
	
	switch (eProtocol) { //Set the socket to the requested protocol
		case IPPROTO_TCP:
			anRegVal[0] = W5500SckMode_ProtTCP | W5500SckMode_NDMCMMB;
			break;
			
		case IPPROTO_UDP:
			anRegVal[0] = W5500SckMode_ProtUDP;
			break;
			
		default:
			return W5500Fail_Unsupported;
	}
	
	W5500WriteData(pDev, W5500SckReg_Mode, (eW5500Control_t)nControl, &(anRegVal[0]), 1);
	
	anRegVal[0] = W5500SckCmd_Open; //Command it to open to establish the socket
	W5500WriteData(pDev, W5500SckReg_Command, (eW5500Control_t)nControl, &anRegVal[0], 1);
	
	//Should move the socket to INIT status
	anRegVal[0] = W5500SckStat_Closed;
	nCtr = 0;
	while ((anRegVal[0] == W5500SckStat_Closed) && (nCtr < 10)) {
		W5500ReadData(pDev, W5500SckReg_Status, (eW5500Control_t)nControl, &anRegVal[0], 1);
		DELAYMILLISEC(1);
		nCtr += 1;
	}
	
	if (eProtocol == IPPROTO_TCP) { //Must command TCP sockets to listen next
		anRegVal[0] = W5500SckCmd_Listen;
		W5500WriteData(pDev, W5500SckReg_Command, (eW5500Control_t)nControl, &anRegVal[0], 1);
	}
	
	if (pDev->nNextPort == nPort) { //Just in case don't clobber next outbound
		pDev->nNextPort += 1;
		if (pDev->nNextPort == 0) {
			pDev->nNextPort = W5500_HIGHPORTSTART;
		}
	}
	
	return W5500_Success;
}

eW5500Return_t W5500SocketConnect(sW5500Obj_t *pDev, uint8_t *pnSocket, IN_ADDR *Address, uint16_t nPort) {
	uint8_t nControl = W5500BSB_Register;
	uint8_t anBytes[2];
	uint16_t nCtr;
	
	//Look for an unused socket
	for (nCtr = 0; nCtr < W5500_NUMSOCKETS; nCtr++) {
		nControl = nCtr << W5500BSB_SocketLShift;
		nControl |= W5500BSB_Register;
		W5500ReadData(pDev, W5500SckReg_Status, (eW5500Control_t)nControl, &anBytes[0], 1);
		
		if (anBytes[0] == W5500SckStat_Closed) {
			break;
		}
	}
	
	if (nCtr >= W5500_NUMSOCKETS) { //All sockets are in use
		return W5500Fail_NoSockets;
	} else {
		*pnSocket = nCtr;
	}
	
	//Found a socket to use, get it setup
	nControl = (*pnSocket) << W5500BSB_SocketLShift;
	nControl |= W5500BSB_Register;
	
	W5500WriteData(pDev, W5500SckReg_DestIPAddr0, (eW5500Control_t)nControl, (uint8_t *)Address, 4);
	
	anBytes[0] = nPort >> 8;
	anBytes[1] = nPort & 0xFF;
	W5500WriteData(pDev, W5500SckReg_DestPort0, (eW5500Control_t)nControl, anBytes, 2);
	
	anBytes[0] = pDev->nNextPort >> 8;
	anBytes[1] = pDev->nNextPort& 0xFF;
	W5500WriteData(pDev, W5500SckReg_SourcePort0, (eW5500Control_t)nControl, anBytes, 2);
	
	//Open the port, then command it to connect
	anBytes[0] = W5500SckCmd_Open;
	W5500WriteData(pDev, W5500SckReg_Command, (eW5500Control_t)nControl, &anBytes[0], 1);
	
	anBytes[0] = W5500SckCmd_Connect;
	W5500WriteData(pDev, W5500SckReg_Command, (eW5500Control_t)nControl, &anBytes[0], 1);
	
	///@todo Verify that the connection was made and return that status. W5500SckStat_Establish on connect W5500SckStat_Closed on failure
	
	return W5500_Success;
}

eW5500Return_t W5500SocketStatus(sW5500Obj_t *pDev, uint8_t nSocket, eW5500SckProt_t *eProtocol, eW5500SckStat_t *eCurrState, uint16_t *nBytesWaiting, SOCKADDR_IN *pConnAddr, uint8_t nConnAddrLen) {
	uint8_t nControl;
	uint8_t aBytes[4];
	uint16_t nVerify;
	
	if (nSocket >= W5500_NUMSOCKETS) {
		return W5500Fail_InvalidSocket;
	}
	
	//Read the status of the socket
	nControl = nSocket << W5500BSB_SocketLShift;
	nControl |= W5500BSB_Register;
	W5500ReadData(pDev, W5500SckReg_Status, (eW5500Control_t)nControl, (uint8_t *)eCurrState, 1);

	//Read the mode to get the protocol
	W5500ReadData(pDev, W5500SckReg_Mode, (eW5500Control_t)nControl, &(aBytes[0]), 1);
	aBytes[0] &= W5500SckMode_ProtMask; //Strip off anything that isn't the protocol flags

	if (aBytes[0] == W5500SckMode_ProtTCP) {
		*eProtocol = IPPROTO_TCP;
	} else if (aBytes[0] == W5500SckMode_ProtUDP) {
		*eProtocol = IPPROTO_UDP;
	} else {
		*eProtocol = IPPROTO_INVALID;
	}
	
	//Look for bytes waiting in the socket
	W5500ReadData(pDev, W5500SckReg_RXRecvSize0, (eW5500Control_t)nControl, aBytes, 2);
	
	//Read connected socket infromation (TCP only)
	pConnAddr->sin_familiy = AF_INET; //Always AF_INET
	
	if (((*eCurrState) != W5500SckStat_Closed) && ((*eCurrState) != W5500SckStat_UDP) && ((*eCurrState) != W5500SckStat_MACRaw)) {
		//Read out the port
		W5500ReadData(pDev, W5500SckReg_DestPort0, (eW5500Control_t)nControl, aBytes, 2);
		
		pConnAddr->sin_port = aBytes[0] << 8;
		pConnAddr->sin_port |= aBytes[1];
			
		//Read the recv byte count. it can change mid-read, read until we get a matched reading
		nVerify = 0;
		W5500ReadData(pDev, W5500SckReg_RXRecvSize0, (eW5500Control_t)nControl, aBytes, 2);
		(*nBytesWaiting) = aBytes[0] << 8;
		(*nBytesWaiting) |= aBytes[1];
		while (nVerify != (*nBytesWaiting)) {
			nVerify = (*nBytesWaiting);
			
			W5500ReadData(pDev, W5500SckReg_RXRecvSize0, (eW5500Control_t)nControl, aBytes, 2);
			(*nBytesWaiting) = aBytes[0] << 8;
			(*nBytesWaiting) |= aBytes[1];
		}
		
		W5500ReadData(pDev, W5500SckReg_DestIPAddr0, (eW5500Control_t)nControl, aBytes, 4);
		pConnAddr->sin_addr.S_un.S_un_b.s_b1 = aBytes[0];
		pConnAddr->sin_addr.S_un.S_un_b.s_b2 = aBytes[1];
		pConnAddr->sin_addr.S_un.S_un_b.s_b3 = aBytes[2];
		pConnAddr->sin_addr.S_un.S_un_b.s_b4 = aBytes[3];
	} else { //Not a TCP socket, no connection to report
		pConnAddr->sin_familiy = AF_INET;
		pConnAddr->sin_port = 0;
		pConnAddr->sin_addr.S_un.S_addr = 0;
	}
	
	return W5500_Success;
}

eW5500Return_t W5500SocketReceive(sW5500Obj_t *pDev, uint8_t nSocket, uint8_t *pBuff, uint16_t nBuffSize, uint16_t *pnBytesRead) {
	uint8_t nControl;
	uint8_t aBytes[4];
	uint16_t nReadAddr, nAvail;
	
	if (nSocket >= W5500_NUMSOCKETS) {
		return W5500Fail_InvalidSocket;
	}
	
	nControl = nSocket << W5500BSB_SocketLShift;
	nControl |= W5500BSB_Register;
	
	//Find out how many bytes are available
	nReadAddr = 0;
	W5500ReadData(pDev, W5500SckReg_RXRecvSize0, (eW5500Control_t)nControl, aBytes, 2);
	nAvail = aBytes[0] << 8;
	nAvail |= aBytes[1];
	while (nReadAddr != nAvail) { //Read till we get a match in case data is arriving
		nReadAddr = nAvail;
		
		W5500ReadData(pDev, W5500SckReg_RXRecvSize0, (eW5500Control_t)nControl, aBytes, 2);
		nAvail = aBytes[0] << 8;
		nAvail |= aBytes[1];
	}
	
	if (nBuffSize > nAvail) { //Can't read more data than what is available
		nBuffSize = nAvail;
	}
	
	//Get the RX read address
	W5500ReadData(pDev, W5500SckReg_RXReadPtr0, (eW5500Control_t)nControl, aBytes, 2);
	nReadAddr = aBytes[0] << 8;
	nReadAddr |= aBytes[1];
	
	//Read from the RX buffer at RX read address (up to RX write or 0xFFFF)
	*pnBytesRead = nBuffSize;
	nControl = nSocket << W5500BSB_SocketLShift;
	nControl |= W5500BSB_RXBuffer;
	W5500ReadData(pDev, nReadAddr, (eW5500Control_t)nControl, pBuff, nBuffSize);
	
	//Update RX Read address with where we've read to
	nControl = nSocket << W5500BSB_SocketLShift;
	nControl |= W5500BSB_Register;
	
	nReadAddr += nBuffSize;
	aBytes[0] = nReadAddr >> 8;
	aBytes[1] = nReadAddr & 0xFF;
	W5500WriteData(pDev, W5500SckReg_RXReadPtr0, (eW5500Control_t)nControl, aBytes, 2);
	
	//Issue RECV command to take receipt of the data
	aBytes[0] = W5500SckCmd_Recv;
	W5500WriteData(pDev, W5500SckReg_Command, (eW5500Control_t)nControl, aBytes, 1);
	
	return W5500_Success;
}

eW5500Return_t W5500SocketTCPSend(sW5500Obj_t *pDev, uint8_t nSocket, uint8_t *pBuff, uint16_t nBuffSize) {
	uint8_t nControl;
	uint8_t aBytes[4];
	uint16_t nTXAddr, nAvail;
	
	if (nSocket >= W5500_NUMSOCKETS) {
		return W5500Fail_InvalidSocket;
	}
	
	nControl = nSocket << W5500BSB_SocketLShift;
	nControl |= W5500BSB_Register;
	
	//Find out how many bytes are available
	W5500ReadData(pDev, W5500SckReg_TXFreeSize0, (eW5500Control_t)nControl, aBytes, 2);
	nAvail = aBytes[0] << 8;
	nAvail |= aBytes[1];
	
	if (nBuffSize > nAvail) { //Can't send more data than what is available
		return W5500Fail_TXFreeSpace;
	}
	
	//Get the TX write address
	W5500ReadData(pDev, W5500SckReg_TXWritePtr0, (eW5500Control_t)nControl, aBytes, 2);
	nTXAddr = aBytes[0] << 8;
	nTXAddr |= aBytes[1];
	
	//Read from the RX buffer at RX read address (up to RX write or 0xFFFF)
	nControl = nSocket << W5500BSB_SocketLShift;
	nControl |= W5500BSB_TXBuffer;
	W5500WriteData(pDev, nTXAddr, (eW5500Control_t)nControl, pBuff, nBuffSize);
	
	//Update RX Read address with where we've read to
	nControl = nSocket << W5500BSB_SocketLShift;
	nControl |= W5500BSB_Register;
	
	nTXAddr += nBuffSize;
	aBytes[0] = nTXAddr >> 8;
	aBytes[1] = nTXAddr & 0xFF;
	W5500WriteData(pDev, W5500SckReg_TXWritePtr0, (eW5500Control_t)nControl, aBytes, 2);
	
	//Issue RECV command to take receipt of the data
	aBytes[0] = W5500SckCmd_Send;
	W5500WriteData(pDev, W5500SckReg_Command, (eW5500Control_t)nControl, aBytes, 1);
	
	return W5500_Success;
}

eW5500Return_t W5500SocketUDPSend(sW5500Obj_t *pDev, uint8_t nSocket, IN_ADDR *pAddr, uint16_t nPort, uint8_t *pBuff, uint16_t nBuffSize) {
	uint8_t nControl;
	uint8_t aBytes[4];
	uint16_t nTXAddr, nAvail;
	
	if (nSocket >= W5500_NUMSOCKETS) {
		return W5500Fail_InvalidSocket;
	}
	
	nControl = nSocket << W5500BSB_SocketLShift;
	nControl |= W5500BSB_Register;
	
	//Setup the IP and port to send to
	aBytes[0] = pAddr->S_un.S_un_b.s_b1;
	aBytes[1] = pAddr->S_un.S_un_b.s_b2;
	aBytes[2] = pAddr->S_un.S_un_b.s_b3;
	aBytes[3] = pAddr->S_un.S_un_b.s_b4;
	W5500WriteData(pDev, W5500SckReg_DestIPAddr0, (eW5500Control_t)nControl, aBytes, 4);
	
	aBytes[0] = nPort >> 8;
	aBytes[1] = nPort & 0xFF;;
	W5500WriteData(pDev, W5500SckReg_DestPort0, (eW5500Control_t)nControl, aBytes, 2);
	
	//Find out how many bytes are available
	W5500ReadData(pDev, W5500SckReg_TXFreeSize0, (eW5500Control_t)nControl, aBytes, 2);
	nAvail = aBytes[0] << 8;
	nAvail |= aBytes[1];
	
	if (nBuffSize > nAvail) { //Can't send more data than what is available
		return W5500Fail_TXFreeSpace;
	}
	
	//Get the TX write address
	W5500ReadData(pDev, W5500SckReg_TXWritePtr0, (eW5500Control_t)nControl, aBytes, 2);
	nTXAddr = aBytes[0] << 8;
	nTXAddr |= aBytes[1];
	
	//Read from the RX buffer at RX read address (up to RX write or 0xFFFF)
	nControl = nSocket << W5500BSB_SocketLShift;
	nControl |= W5500BSB_TXBuffer;
	W5500WriteData(pDev, nTXAddr, (eW5500Control_t)nControl, pBuff, nBuffSize);
	
	//Update RX Read address with where we've read to
	nControl = nSocket << W5500BSB_SocketLShift;
	nControl |= W5500BSB_Register;
	
	nTXAddr += nBuffSize;
	aBytes[0] = nTXAddr >> 8;
	aBytes[1] = nTXAddr & 0xFF;
	W5500WriteData(pDev, W5500SckReg_TXWritePtr0, (eW5500Control_t)nControl, aBytes, 2);
	
	//Issue RECV command to take receipt of the data
	aBytes[0] = W5500SckCmd_Send;
	W5500WriteData(pDev, W5500SckReg_Command, (eW5500Control_t)nControl, aBytes, 1);
	
	return W5500_Success;
}

eW5500Return_t W5500CloseSocket(sW5500Obj_t *pDev, uint8_t nSocket) {
	uint8_t nSetting, nControl;
	
	if (nSocket >= W5500_NUMSOCKETS) {
		return W5500Fail_InvalidSocket;
	}
	
	nControl = nSocket << W5500BSB_SocketLShift;
	
	nSetting = W5500SckCmd_Close; //Command it to close down any connections
	W5500WriteData(pDev, W5500SckReg_Command, (eW5500Control_t)nControl, &nSetting, 1);
	
	return W5500_Success;
}

eW5500Return_t W5500ReadData(sW5500Obj_t *pDev, uint16_t nAddress, eW5500Control_t eControl, uint8_t *pBuff, uint8_t nBytes) {
	sGPIOIface_t *pGPIO = pDev->pGPIO;
	sSPIIface_t *pSPI = pDev->pSPI;
	uint8_t nSend[3], nCtr;
	
	nSend[0] = nAddress >> 8;
	nSend[1] = nAddress & 0x00FF;
	nSend[2] = eControl & (~W5500BSB_OpModeMask); //Strip off data length bits
	nSend[2] &= ~W5500BSB_ReadWriteMask; //Strip off any read/write bits
	nSend[2] |= W5500BSB_Read; //Add correct read/write bit
	
	//Add the correct data length bits
	switch (nBytes) {
		case 1:
			nSend[2] |= W5500BSB_OM1ByteLen;
			break;
		case 2:
			nSend[2] |= W5500BSB_OM2ByteLen;
			break;
		case 4:
			nSend[2] |= W5500BSB_OM4ByteLen;
			break;
		default:
			nSend[2] |= W5500BSB_OMVariableLen;
			break;
	}
	
	//Begin SPI transaction
	pGPIO->pfDigitalWriteByPin(pGPIO, pDev->nChipSelectPin, false);
	pSPI->pfBeginTransfer(pSPI);
	
	//Send the address and control
	for (nCtr = 0; nCtr < 3; nCtr++) {
		pSPI->pfTransferByte(pSPI, nSend[nCtr], &(pBuff[0]));
	}
	
	//Read out the data
	for (nCtr = 0; nCtr < nBytes; nCtr++) {
		pSPI->pfTransferByte(pSPI, 0xFF, &(pBuff[nCtr]));
	}
	
	//End SPI transaction
	pSPI->pfEndTransfer(pSPI);
	pGPIO->pfDigitalWriteByPin(pGPIO, pDev->nChipSelectPin, true);
	
	return W5500_Success;
}

eW5500Return_t W5500WriteData(sW5500Obj_t *pDev, uint16_t nAddress, eW5500Control_t eControl, const uint8_t *pBuff, uint8_t nBytes) {
	sGPIOIface_t *pGPIO = pDev->pGPIO;
	sSPIIface_t *pSPI = pDev->pSPI;
	uint8_t nSend[3], nRecv, nCtr;
	
	nSend[0] = nAddress >> 8;
	nSend[1] = nAddress & 0x00FF;
	nSend[2] = eControl & (~W5500BSB_OpModeMask); //Strip off data length bits
	nSend[2] &= ~W5500BSB_ReadWriteMask; //Strip off any read/write bits
	nSend[2] |= W5500BSB_Write; //Add correct read/write bit
	
	//Add the correct data length bits
	switch (nBytes) {
		case 1:
			nSend[2] |= W5500BSB_OM1ByteLen;
			break;
		case 2:
			nSend[2] |= W5500BSB_OM2ByteLen;
			break;
		case 4:
			nSend[2] |= W5500BSB_OM4ByteLen;
			break;
		default:
			nSend[2] |= W5500BSB_OMVariableLen;
			break;
	}
	
	//Begin SPI transaction
	pGPIO->pfDigitalWriteByPin(pGPIO, pDev->nChipSelectPin, false);
	pSPI->pfBeginTransfer(pSPI);

	//Send the address and control
	for (nCtr = 0; nCtr < 3; nCtr++) {
		pSPI->pfTransferByte(pSPI, nSend[nCtr], &nRecv);
	}
	
	//Write out the data
	for (nCtr = 0; nCtr < nBytes; nCtr++) {
		pSPI->pfTransferByte(pSPI, pBuff[nCtr], &nRecv);
	}

	//End SPI transaction
	pSPI->pfEndTransfer(pSPI);
	pGPIO->pfDigitalWriteByPin(pGPIO, pDev->nChipSelectPin, true);
	DELAYMICROSEC(W5500_MICROSECCSHIGH);
	
	return W5500_Success;
}

eW5500Return_t W5500CreateTCPServer(sW5500Obj_t *pDev, sTCPServ_t *pTCPServ) {
	//Start with a clean structure
	IfaceTCPServObjInitialize(pTCPServ);
	
	//Fill in structure variables
	pTCPServ->eCapabilities = W5500_TCPSERVCAPS;
	pTCPServ->HostSck = SOCKET_INVALID;
	pTCPServ->pHWInfo = (void *)pDev;
	
	//Fill in function pointers
	pTCPServ->pfBind = &W5500NetTCPServBind;
	pTCPServ->pfCloseHost = &W5500NetTCPServCloseHost;
	pTCBServ->pfAcceptClient = &W5500NetTCPServAcceptClient;
	pTCPServ->pfCloseSocket = &W5500NetTCPServClosesSocket;
	
	return W5500_Success;
}

eNetReturn_t W5500NetTCPServBind(sTCPServ_t *pTCPServ, sConnInfo_t *pConn) {
	eW5500Return_t eResult;
	sW5500Obj_t * pDev = (sW5500Obj_t *)pTCPServ->pHWInfo;
	
	eResult = W5500SocketListen(pDev, &(pTCPServ->HostSck), pConn->Port, IPPROTO_TCP);
	
	if (eResult < W5500_Success) {
		return NetFail_BindErr;
	} else {
		return Net_Success;
	}
}

eNetReturn_t W5500NetTCPServCloseHost(sTCPServ_t *pTCPServ) {
	eW5500Return_t eResult;
	sW5500Obj_t * pDev = (sW5500Obj_t *)pTCPServ->pHWInfo;
	
	if (pTCPServ->HostSck == SOCKET_INVALID) {
		return NetFail_InvSocket;
	}
	
	eResult = W5500CloseSocket(pDev, pTCPServ->HostSck);
	
	if (eResult < W5500_Success) {
		return NetFail_Unknown;
	} else {
		return Net_Success;
	}
}

eNetReturn_t W5500NetTCPServAcceptClient(sTCPServ_t *pTCPServ, sSocket_t *pClientSck) {
	eW5500Return_t eResult;
	sW5500Obj_t *pDev = (sW5500Obj_t *)pTCPServ->pHWInfo;
	uint16_t nServPort, nCount;
	eW5500SckProt_t eProt;
	eW5500SckStat_t eState;
	SOCKADDR_IN ConnAddr;
	uint8_t anPortBytes[2];
	
	//Get port host is listening on
	W5500ReadData(pDev, W5500SckReg_SourcePort0, (eW5500Control_t)(pTCPServ->HostSck << W5500BSB_SocketLShift), anRegVal, 2);
	nServPort = anPortBytes[0] << 8;
	nServPort |= anPortBytes[1];
	
	//Wait for a connection request
	eState = W5500SckStat_Listen;
	while (eState == W5500SckStat_Listen) {
		eResult = W5500SocketStatus(pDev, pTCPServ->HostSck, &eProt, &eState, &nBytesAvail, &ConnAddr, sizeof(SOCKADDR_IN));
		
		if (eResult != W5500_Success) { //failed to get socket information
			return NetFail_Unknown;
		}
	}
	
	//Pull back the connection information
	pClientSck->nSocket = pTCPServ->HostSck;
	pClientSck->Conn.Port = ConnAddr.sin_port;
	pClientSck->Conn.Addr.nNetLong = ConnAddr.sin_addr.S_addr;
	
	//Start a new host socket listening
	eResult = W5500SocketListen(pDev, &(pTCPServ->HostSck), nServPort, IPPROTO_TCP);
	
	if (eResult < W5500_Success) {
		return NetFail_BindErr;
	} else {
		return Net_Success;
	}
}

eNetReturn_t W5500NetTCPServCloseSocket(sTCPServ_t *pTCPServ, sSocket_t *pSck) {
	
}
