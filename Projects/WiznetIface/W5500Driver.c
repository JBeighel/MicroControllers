/*	File:	W5500Driver.c
	Author:	J. Beighel
	Date:	2021-03-06
*/

/***** Includes		*****/
	#include "W5500Driver.h"
	

/***** Definitions	*****/
	
	
/***** Constants	*****/


/***** Globals		*****/


/***** Prototypes 	*****/
	/**	@brief		Read data from the Wiznet peripheral
		@details	Performs read operations with the peripheral over the SPI bus
			If the data is W5500_READVERIFYSIZE bytes or less then it will be read a second
			time as a verification.  The function will only return if the data is read back
			with an identical value, otherwise it will continue reading.
			
			If the data is larger than W5500_READVERIFYSIZE no verification is attempted.  
			The results of the first read attempt are returned.
		@param		pDev		Pointer to the Wiznet5500 device object
		@param		nAddress	The address in the requested back to read from
		@param		eControl	Indicate the memory bank to read from
		@param		pBuff		Buffer to hold the read data
		@param		nBytes		Number of bytes to read
		@ingroup	w5500driver
	*/
	eW5500Return_t W5500ReadData(sW5500Obj_t *pDev, uint16_t nAddress, eW5500Control_t eControl, uint8_t *pBuff, uint8_t nBytes);
	
	/**	@brief		Read data from the Wiznet peripheral
		@details	Performs read operations with the peripheral over the SPI bus
			No verification read is attempted.  The data is returned as reported in the first
			read attempt.
		@param		pDev		Pointer to the Wiznet5500 device object
		@param		nAddress	The address in the requested back to read from
		@param		eControl	Indicate the memory bank to read from
		@param		pBuff		Buffer to hold the read data
		@param		nBytes		Number of bytes to read
		@ingroup	w5500driver
	*/
	eW5500Return_t W5500ReadDataSkipVerify(sW5500Obj_t *pDev, uint16_t nAddress, eW5500Control_t eControl, uint8_t *pBuff, uint8_t nBytes);
	
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
	
	eW5500Return_t W5500WriteDataSkipVerify(sW5500Obj_t *pDev, uint16_t nAddress, eW5500Control_t eControl, const uint8_t *pBuff, uint8_t nBytes);
	
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
	
	/**	@brief		Block until a connection request arrives then accept it
		@details	The function will monitor the state of the listening socket until it
			leaves the listening state.  The Wiznet device will automatically accept any
			and all connection requests and change the state to established.  When this
			happens the function will retrieve the connection details set a new socket to
			listen on the socket then return.
		@param		pTCPServ		Pointer to the Wiznet5500 TCP server interface object
		@param		pClientSck		Returns information about the client that connected
		@return		Net_Success on success, or a code indicating the failure
		@ingroup	w5500driver
	*/
	eNetReturn_t W5500NetTCPServAcceptClient(sTCPServ_t *pTCPServ, sSocket_t *pClientSck);
	
	/**	@brief		Closes down a client socket
		@param		pTCPServ		Pointer to the Wiznet5500 TCP server interface object
		@param		pSck			Pointer to the socket to close
		@return		Net_Success on success, or a code indicating the failure
		@ingroup	w5500driver
	*/
	eNetReturn_t W5500NetTCPServCloseSocket(sTCPServ_t *pTCPServ, sSocket_t *pSck);
	
	eNetReturn_t W5500NetTCPServReceive(sTCPServ_t *pTCPServ, sSocket_t *pClientSck, uint32_t nDataBytes, void *pData, uint32_t *pnBytesRecv);
	
	eNetReturn_t W5500NetTCPServSend(sTCPServ_t *pTCPServ, sSocket_t *pClientSck, uint32_t nDataBytes, void *pData);
	
	eNetReturn_t W5500NetTCPClientConnect(sTCPClient_t *pTCPClient, sConnInfo_t *pConn);
	
	/**	@brief		Closes the TCP connection
		@param		pTCPClient		Pointer to hte TCP Client object to use
		@return		Net_Success on succeess, or a code indicating the type of error encountered
		@ingroup	networkgeniface
	*/
	eNetReturn_t W5500NetTCPClientClose(sTCPClient_t *pTCPClient);
	
	/**	@brief		Waits for and receives data from a TCP server
		@param		pTCPClient		Pointer to hte TCP Client object to use
		@param		nDataBytes		Number of bytes the data buffer can hold
		@param		pData			Pointer to the bufer to receive the data 
		@param		pnBytesRecv		Returns the number of data bytes received
		@return		Net_Success on succeess, or a code indicating the type of error encountered
		@ingroup	networkgeniface
	*/
	eNetReturn_t W5500NetTCPClientReceive(sTCPClient_t *pTCPClient, uint32_t nDataBytes, void *pData, uint32_t *pnBytesRecv);
	
	/**	@brief		Send data to a TCP server
		@param		pTCPClient		Pointer to hte TCP Client object to use
		@param		nDataBytes		Number of bytes to send
		@param		pData			Buffer holding the data to send
		@return		Net_Success on succeess, or a code indicating the type of error encountered
		@ingroup	networkgeniface
	*/
	eNetReturn_t W5500NetTCPClientSend(sTCPClient_t *pTCPClient, uint32_t nDataBytes, void *pData);

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
	W5500WriteDataSkipVerify(pDev, W5500SckReg_Mode, W5500BSB_CommonRegister, &nModeReg, 1);
	DELAYMILLISEC(5);
	W5500WriteDataSkipVerify(pDev, W5500SckReg_Mode, W5500BSB_CommonRegister, &nModeReg, 1);
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
		W5500WriteDataSkipVerify(pDev, W5500SckReg_Command, (eW5500Control_t)nControl, &nCmd, 1); //Can't read back socket commands
		
		//Set all buffers to 2K, evenly spreads out available space
		nCmd = W5500SckBuff_2KB;
		W5500WriteData(pDev, W5500SckReg_RXBuffSize, (eW5500Control_t)nControl, &nCmd, 1);
		
		nCmd = W5500SckBuff_2KB;
		W5500WriteData(pDev, W5500SckReg_TXBuffSize, (eW5500Control_t)nControl, &nCmd, 1);
	}
	
	//Check the SPI Mode
	if ((pSpiBus->eMode != SPI_Mode0) && (pSpiBus->eMode != SPI_Mode3)) {
		return W5500Fail_Unknown;
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
	W5500WriteDataSkipVerify(pDev, W5500SckReg_Interrupt, (eW5500Control_t)nControl, &anRegVal[0], 1);
	
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
	W5500WriteDataSkipVerify(pDev, W5500SckReg_Command, (eW5500Control_t)nControl, &anRegVal[0], 1);
	
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
		W5500WriteDataSkipVerify(pDev, W5500SckReg_Command, (eW5500Control_t)nControl, &anRegVal[0], 1);
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

eW5500Return_t W5500ReadDataSkipVerify(sW5500Obj_t *pDev, uint16_t nAddress, eW5500Control_t eControl, uint8_t *pBuff, uint8_t nBytes) {
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

eW5500Return_t W5500ReadData(sW5500Obj_t *pDev, uint16_t nAddress, eW5500Control_t eControl, uint8_t *pBuff, uint8_t nBytes)  {
	sGPIOIface_t *pGPIO = pDev->pGPIO;
	sSPIIface_t *pSPI = pDev->pSPI;
	bool bVerified, bDidFirstRead;
	uint8_t aVerify[W5500_READVERIFYSIZE], aSend[3];
	uint16_t nCtr;
	
	aSend[0] = nAddress >> 8;
	aSend[1] = nAddress & 0x00FF;
	aSend[2] = eControl & (~W5500BSB_OpModeMask); //Strip off data length bits
	aSend[2] &= ~W5500BSB_ReadWriteMask; //Strip off any read/write bits
	aSend[2] |= W5500BSB_Read; //Add correct read/write bit
	
	//Add the correct data length bits
	switch (nBytes) {
		case 1:
			aSend[2] |= W5500BSB_OM1ByteLen;
			break;
		case 2:
			aSend[2] |= W5500BSB_OM2ByteLen;
			break;
		case 4:
			aSend[2] |= W5500BSB_OM4ByteLen;
			break;
		default:
			aSend[2] |= W5500BSB_OMVariableLen;
			break;
	}
	
	bVerified = false;
	bDidFirstRead = false;
	do {
		//Begin SPI transaction
		pGPIO->pfDigitalWriteByPin(pGPIO, pDev->nChipSelectPin, false);
		pSPI->pfBeginTransfer(pSPI);
		
		//Send the address and control
		for (nCtr = 0; nCtr < 3; nCtr++) {
			pSPI->pfTransferByte(pSPI, aSend[nCtr], &(pBuff[0]));
		}
		
		//Read out the data
		for (nCtr = 0; nCtr < nBytes; nCtr++) {
			pSPI->pfTransferByte(pSPI, 0xFF, &(pBuff[nCtr]));
		}
		
		//End SPI transaction
		pSPI->pfEndTransfer(pSPI);
		pGPIO->pfDigitalWriteByPin(pGPIO, pDev->nChipSelectPin, true);
		
		if (bDidFirstRead == true) { //COmpare read data to verify buffer
			bVerified = true; //Assume its a good read
			for (nCtr = 0; (nCtr < nBytes) && (nBytes <= W5500_READVERIFYSIZE); nCtr++) {
				if (aVerify[nCtr] != pBuff[nCtr]) { //Turns out it wasn't good
					bVerified = false;
					nCtr = nBytes + 1; //Break the for loop
				}
			}
		} else { //Need a verification read still, can't compare
			bDidFirstRead = true;
		}
		
		if (nBytes <= 4) { //Copy what was read into the verification buffer
			for (nCtr = 0; nCtr < nBytes; nCtr++) {
				aVerify[nCtr] = pBuff[nCtr];
			}
		}
	} while ((bVerified == false) && (nBytes <= W5500_READVERIFYSIZE));
	
	return W5500_Success;
}

eW5500Return_t W5500WriteDataSkipVerify(sW5500Obj_t *pDev, uint16_t nAddress, eW5500Control_t eControl, const uint8_t *pBuff, uint8_t nBytes) {
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

eW5500Return_t W5500WriteData(sW5500Obj_t *pDev, uint16_t nAddress, eW5500Control_t eControl, const uint8_t *pBuff, uint8_t nBytes) {
	sGPIOIface_t *pGPIO = pDev->pGPIO;
	sSPIIface_t *pSPI = pDev->pSPI;
	bool bVerified;
	uint8_t aSend[3], aVerify[W5500_READVERIFYSIZE];
	uint16_t nCtr;
	
	aSend[0] = nAddress >> 8;
	aSend[1] = nAddress & 0x00FF;
	aSend[2] = eControl & (~W5500BSB_OpModeMask); //Strip off data length bits
	aSend[2] &= ~W5500BSB_ReadWriteMask; //Strip off any read/write bits
	aSend[2] |= W5500BSB_Write; //Add correct read/write bit
	
	//Add the correct data length bits
	switch (nBytes) {
		case 1:
			aSend[2] |= W5500BSB_OM1ByteLen;
			break;
		case 2:
			aSend[2] |= W5500BSB_OM2ByteLen;
			break;
		case 4:
			aSend[2] |= W5500BSB_OM4ByteLen;
			break;
		default:
			aSend[2] |= W5500BSB_OMVariableLen;
			break;
	}
	
	bVerified = false;
	do {
		//Begin SPI transaction
		pGPIO->pfDigitalWriteByPin(pGPIO, pDev->nChipSelectPin, false);
		pSPI->pfBeginTransfer(pSPI);

		//Send the address and control
		for (nCtr = 0; nCtr < 3; nCtr++) {
			pSPI->pfTransferByte(pSPI, aSend[nCtr], aSend);
		}
		
		//Write out the data
		for (nCtr = 0; nCtr < nBytes; nCtr++) {
			pSPI->pfTransferByte(pSPI, pBuff[nCtr], aSend);
		}

		//End SPI transaction
		pSPI->pfEndTransfer(pSPI);
		pGPIO->pfDigitalWriteByPin(pGPIO, pDev->nChipSelectPin, true);
		DELAYMICROSEC(W5500_MICROSECCSHIGH);
		
		if (nBytes <= W5500_READVERIFYSIZE) { //Read back the data to ensure a good write
			W5500ReadData(pDev, nAddress, eControl, aVerify, nBytes);
			
			bVerified = true; //Assume it passed to start
			for (nCtr = 0; nCtr < nBytes; nCtr++) {
				if (pBuff[nCtr] != aVerify[nCtr]) { //It didn't pass, try again
					bVerified = false;
					nCtr = nBytes + 1; //Stop the for loop, no need to keep checking
				}
			}
		} else { //Too much data to verify, hope for the best
			bVerified = true;
		}
	} while (bVerified == false);
	
	return W5500_Success;
}

eW5500Return_t W5500CreateTCPServer(sW5500Obj_t *pDev, sTCPServ_t *pTCPServ) {
	//Start with a clean structure
	IfaceTCPServObjInitialize(pTCPServ);
	
	//Fill in structure variables
	pTCPServ->eCapabilities = W5500_TCPSERVCAPS;
	pTCPServ->HostSck.nSocket = SOCKET_INVALID;
	pTCPServ->HostSck.Conn.Port = 0;
	pTCPServ->HostSck.Conn.Addr.nNetLong = 0;
	pTCPServ->pHWInfo = (void *)pDev;
	
	//Fill in function pointers
	pTCPServ->pfBind = &W5500NetTCPServBind;
	pTCPServ->pfCloseHost = &W5500NetTCPServCloseHost;
	pTCPServ->pfAcceptClient = &W5500NetTCPServAcceptClient;
	pTCPServ->pfCloseSocket = &W5500NetTCPServCloseSocket;
	pTCPServ->pfReceive = &W5500NetTCPServReceive;
	pTCPServ->pfSend = &W5500NetTCPServSend;
	
	return W5500_Success;
}

eNetReturn_t W5500NetTCPServBind(sTCPServ_t *pTCPServ, sConnInfo_t *pConn) {
	eW5500Return_t eResult;
	sW5500Obj_t * pDev = (sW5500Obj_t *)pTCPServ->pHWInfo;
	IN_ADDR IPAddr;
	uint8_t nSck;
	
	W5500ReadIPAddress(pDev, &IPAddr);
	
	eResult = W5500SocketListen(pDev, &nSck, pConn->Port, IPPROTO_TCP);
	pTCPServ->HostSck.nSocket = nSck;
	
	if (eResult < W5500_Success) {
		pTCPServ->HostSck.nSocket = SOCKET_INVALID;
		pTCPServ->HostSck.Conn.Port = 0;
		pTCPServ->HostSck.Conn.Addr.nNetLong = 0;
		
		return NetFail_BindErr;
	} else {
		pTCPServ->HostSck.Conn.Port = pConn->Port;
		pTCPServ->HostSck.Conn.Addr.nNetLong = IPAddr.S_un.S_addr;
		
		return Net_Success;
	}
}

eNetReturn_t W5500NetTCPServCloseHost(sTCPServ_t *pTCPServ) {
	eW5500Return_t eResult;
	sW5500Obj_t * pDev = (sW5500Obj_t *)pTCPServ->pHWInfo;
	
	if (pTCPServ->HostSck.nSocket == SOCKET_INVALID) {
		return NetFail_InvSocket;
	}
	
	eResult = W5500CloseSocket(pDev, pTCPServ->HostSck.nSocket);
	
	pTCPServ->HostSck.nSocket = SOCKET_INVALID;
	pTCPServ->HostSck.Conn.Port = 0;
	pTCPServ->HostSck.Conn.Addr.nNetLong = 0;
	
	if (eResult < W5500_Success) {
		return NetFail_Unknown;
	} else {
		return Net_Success;
	}
}

eNetReturn_t W5500NetTCPServAcceptClient(sTCPServ_t *pTCPServ, sSocket_t *pClientSck) {
	eW5500Return_t eResult;
	sW5500Obj_t *pDev = (sW5500Obj_t *)pTCPServ->pHWInfo;
	uint16_t nCount, nServPort;
	uint32_t nIPAddr;
	uint8_t nSck;
	eW5500SckProt_t eProt;
	eW5500SckStat_t eState, eLastState;
	SOCKADDR_IN ConnAddr;
	
	//Clear socket info in case of failure
	pClientSck->nSocket = SOCKET_INVALID;
	pClientSck->Conn.Port = 0;
	pClientSck->Conn.Addr.nNetLong = 0;
	
	//Get port host is listening on
	nServPort = pTCPServ->HostSck.Conn.Port;
	nIPAddr = pTCPServ->HostSck.Conn.Addr.nNetLong;
	
	//Wait for a connection request
	eState = W5500SckStat_Listen;
	while (eState == W5500SckStat_Listen) {
		eResult = W5500SocketStatus(pDev, pTCPServ->HostSck.nSocket, &eProt, &eState, &nCount, &ConnAddr, sizeof(SOCKADDR_IN));
		
		if (eResult != W5500_Success) { //failed to get socket information
			return NetFail_Unknown;
		}
		
		if (eState != eLastState) {
			eLastState = eState;
			
			if (eLastState != W5500SckStat_Establish) {
				//Need to confirm a state change to avoid spurious read error
				eState = W5500SckStat_Listen;
			}
		}
	}
	
	if (eState != W5500SckStat_Establish) {
		return NetFail_SocketState;
	}
	
	//Pull back the connection information
	pClientSck->nSocket = pTCPServ->HostSck.nSocket;
	pClientSck->Conn.Port = ConnAddr.sin_port;
	pClientSck->Conn.Addr.nNetLong = ConnAddr.sin_addr.S_un.S_addr;
	
	//Start a new host socket listening
	eResult = W5500SocketListen(pDev, &nSck, nServPort, IPPROTO_TCP);
	pTCPServ->HostSck.nSocket = nSck;
	pTCPServ->HostSck.Conn.Port = nServPort;
	pTCPServ->HostSck.Conn.Addr.nNetLong = nIPAddr;
	
	if (eResult < W5500_Success) {
		return NetFail_BindErr;
	} else {
		return Net_Success;
	}
}

eNetReturn_t W5500NetTCPServCloseSocket(sTCPServ_t *pTCPServ, sSocket_t *pSck) {
	eW5500Return_t eResult;
	sW5500Obj_t * pDev = (sW5500Obj_t *)pTCPServ->pHWInfo;
	
	if (pSck->nSocket == SOCKET_INVALID) {
		return NetFail_InvSocket;
	}
	
	eResult = W5500CloseSocket(pDev, pSck->nSocket);
	pSck->nSocket = SOCKET_INVALID;
	pSck->Conn.Port = 0;
	pSck->Conn.Addr.nNetLong = 0;
	
	if (eResult < W5500_Success) {
		return NetFail_Unknown;
	} else {
		return Net_Success;
	}
}

eNetReturn_t W5500NetTCPServReceive(sTCPServ_t *pTCPServ, sSocket_t *pClientSck, uint32_t nDataBytes, void *pData, uint32_t *pnBytesRecv) {
	eW5500Return_t eResult;
	sW5500Obj_t *pDev = (sW5500Obj_t *)pTCPServ->pHWInfo;
	uint16_t nCount;
	eW5500SckProt_t eProt;
	eW5500SckStat_t eState;
	SOCKADDR_IN ConnAddr;
	
	//Set error return just in case
	*pnBytesRecv = 0;
	
	//Wait for data to arrive or the socket to close
	nCount = 0;
	eState = W5500SckStat_Establish;
	while ((eState == W5500SckStat_Establish) && (nCount == 0)) {
		eResult = W5500SocketStatus(pDev, pClientSck->nSocket, &eProt, &eState, &nCount, &ConnAddr, sizeof(SOCKADDR_IN));
		
		if (eResult != W5500_Success) { //failed to get socket information
			return NetFail_Unknown;
		}
	}
	
	if (eState != W5500SckStat_Establish) {
		return NetFail_SocketState;
	}
	
	//Receive the data waiting	
	eResult = W5500SocketReceive(pDev, pClientSck->nSocket, (uint8_t *)pData, nDataBytes, &nCount);
	*pnBytesRecv = nCount;
	
	if (eResult < W5500_Success) {
		return NetFail_Unknown;
	} else {
		return Net_Success;
	}
}

eNetReturn_t W5500NetTCPServSend(sTCPServ_t *pTCPServ, sSocket_t *pClientSck, uint32_t nDataBytes, void *pData) {
	eW5500Return_t eResult;
	sW5500Obj_t *pDev = (sW5500Obj_t *)pTCPServ->pHWInfo;
	uint16_t nAvail;
	eW5500SckProt_t eProt;
	eW5500SckStat_t eState;
	SOCKADDR_IN ConnAddr;
	
	//Make sure the socket is in a valid state
	eResult = W5500SocketStatus(pDev, pClientSck->nSocket, &eProt, &eState, &nAvail, &ConnAddr, sizeof(SOCKADDR_IN));
		
	if (eResult != W5500_Success) { //failed to get socket information
		return NetFail_Unknown;
	} else if (eState != W5500SckStat_Establish) {
		return NetFail_SocketState;
	}
	
	//Ship out the data
	eResult = W5500SocketTCPSend(pDev, pClientSck->nSocket, (uint8_t *)pData, nDataBytes);
	
	if (eResult < W5500_Success) {
		return NetFail_Unknown;
	} else {
		return Net_Success;
	}
}

eW5500Return_t W5500CreateTCPClient(sW5500Obj_t *pDev, sTCPClient_t *pTCPClient) {
	//Start with sane structure
	IfaceTCPClientObjInitialize(pTCPClient);

	//Fill out the structure variables
	pTCPClient->Sck.nSocket = SOCKET_INVALID;
	pTCPClient->Sck.Conn.Port = 0;
	pTCPClient->Sck.Conn.Addr.nNetLong = 0;
	pTCPClient->pHWInfo = (void *)pDev;
	pTCPClient->eCapabilities = W5500_TCPCLIENTCAPS;

	//Fill out function pointers
	pTCPClient->pfConnect = &W5500NetTCPClientConnect;
	pTCPClient->pfClose = &W5500NetTCPClientClose;
	pTCPClient->pfReceive = &W5500NetTCPClientReceive;
	pTCPClient->pfSend = &W5500NetTCPClientSend;
	
	return W5500_Success;
}

eNetReturn_t W5500NetTCPClientConnect(sTCPClient_t *pTCPClient, sConnInfo_t *pConn) {
	sW5500Obj_t *pDev = (sW5500Obj_t *)pTCPClient->pHWInfo;
	eW5500Return_t eResult;
	IN_ADDR IPServ;
	uint8_t nSck;
	
	IPServ.S_un.S_addr = pConn->Addr.nNetLong;
	
	eResult = W5500SocketConnect(pDev, &nSck, &IPServ, pConn->Port);
	
	if (eResult == W5500_Success) {
		pTCPClient->Sck.Conn.Port = pConn->Port;
		pTCPClient->Sck.Addr.nNetLong = pConn->Addr.nNetLong;
		pTCPClient->Sck.nSocket = nSck;
		return Net_Success;
	} else {
		return NetFail_Unknown;
	}
}
	
eNetReturn_t W5500NetTCPClientClose(sTCPClient_t *pTCPClient) {
	eW5500Return_t eResult;
	sW5500Obj_t * pDev = (sW5500Obj_t *)pTCPClient->pHWInfo;
	
	if (pTCPClient->Sck.nSocket == SOCKET_INVALID) {
		return NetFail_InvSocket;
	}
	
	eResult = W5500CloseSocket(pDev, pTCPClient->Sck.nSocket);
	pTCPClient->Sck.nSocket = SOCKET_INVALID;
	pTCPClient->Sck.Conn.Port = 0;
	pTCPClient->Sck.Conn.Addr.nNetLong = 0;
	
	if (eResult != W5500_Success) {
		return NetFail_Unknown;
	} else {
		return Net_Success;
	}
}
	
eNetReturn_t W5500NetTCPClientReceive(sTCPClient_t *pTCPClient, uint32_t nDataBytes, void *pData, uint32_t *pnBytesRecv) {
	sW5500Obj_t *pDev = (sW5500Obj_t *)pTCPClient->pHWInfo;
	eW5500Return_t eResult;
	uint16_t nAvail;
	eW5500SckProt_t eProt;
	eW5500SckStat_t eState;
	SOCKADDR_IN ConnAddr;
	
	(*pnBytesRecv) = 0;
	
	//Make sure the socket is in a valid state
	eResult = W5500SocketStatus(pDev, pTCPClient->Sck.nSocket, &eProt, &eState, &nAvail, &ConnAddr, sizeof(SOCKADDR_IN));
		
	if (eResult != W5500_Success) { //failed to get socket information
		return NetFail_Unknown;
	} else if (eState != W5500SckStat_Establish) {
		return NetFail_SocketState;
	}
	
	//Socket looks good, accept out the data
	eResult = W5500SocketReceive(pDev, pTCPClient->Sck.nSocket, pData, nDataBytes, pnBytesRead);
	if (eResult < W5500_Success) {
		return NetFail_Unknown;
	} else {
		return Net_Success;
	}
}
	
eNetReturn_t W5500NetTCPClientSend(sTCPClient_t *pTCPClient, uint32_t nDataBytes, void *pData) {
	sW5500Obj_t *pDev = (sW5500Obj_t *)pTCPClient->pHWInfo;
	eW5500Return_t eResult;
	uint16_t nAvail;
	eW5500SckProt_t eProt;
	eW5500SckStat_t eState;
	SOCKADDR_IN ConnAddr;
	
	//Make sure the socket is in a valid state
	eResult = W5500SocketStatus(pDev, pTCPClient->Sck.nSocket, &eProt, &eState, &nAvail, &ConnAddr, sizeof(SOCKADDR_IN));
		
	if (eResult != W5500_Success) { //failed to get socket information
		return NetFail_Unknown;
	} else if (eState != W5500SckStat_Establish) {
		return NetFail_SocketState;
	}
	
	//Socket looks good, ship out the data
	eResult = W5500SocketTCPSend(pDev, pTCPClient->Sck.nSocket, pData, nDataBytes);
	if (eResult < W5500_Success) {
		return NetFail_Unknown;
	} else {
		return Net_Success;
	}
}

