/**	File:	Network_RaspberryPi.c
	Author:	J. Beighel
	Date:	12-30-2020
*/

/*****	Includes	*****/
	#include "Network_RaspberryPi.h"

/*****	Defines		*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
eNetReturn_t RasPiTCPServBind(sTCPServ_t *pTCPServ, sConnInfo_t *pConn);
eNetReturn_t RasPiTCPServCloseHost(sTCPServ_t *pTCPServ);
eNetReturn_t RasPiTCPServCloseSocket(sTCPServ_t *pTCPServ, sSocket_t *pSck);
eNetReturn_t RasPiTCPServAcceptClient(sTCPServ_t *pTCPServ, sSocket_t *sSocket_t);
eNetReturn_t RasPiTCPServReceive(sTCPServ_t *pTCPServ, sSocket_t *sSocket_t, uint32_t nNumBytes, void *pData, uint32_t *pnBytesRecv);
eNetReturn_t RasPiTCPServSend(sTCPServ_t *pTCPServ, sSocket_t *sSocket_t, uint32_t nDataBytes, void *pData);

eNetReturn_t RasPiTCPClientConnect(sTCPClient_t *pTCPClient, sConnInfo_t *pConn);
eNetReturn_t RasPiTCPClientClose(sTCPClient_t *pTCPClient);
eNetReturn_t RasPiTCPClientReceive(sTCPClient_t *pTCPClient, uint32_t nNumBytes, void *pData, uint32_t *pnBytesRecv);
eNetReturn_t RasPiTCPClientSend(sTCPClient_t *pTCPClient, uint32_t nDataBytes, void *pData);

eNetReturn_t RasPiUDPServBind(sUDPServ_t *pUDPServ, sConnInfo_t *pConn);
eNetReturn_t RasPiUDPServCloseHost(sUDPServ_t *pUDPServ);
eNetReturn_t RasPiUDPServReceive(sUDPServ_t *pUDPServ, sSocket_t *pClientSck, uint32_t nDataBytes, void *pData, uint32_t *pnBytesRecv);
eNetReturn_t RasPiUDPServSend(sUDPServ_t *pUDPServ, sSocket_t *pClientSck, uint32_t nDataBytes, void *pData);

eNetReturn_t RasPiUDPClientSetServer(sUDPClient_t *pUDPClient, sConnInfo_t *pConn);
eNetReturn_t RasPiUDPClientClose(sUDPClient_t *pUDPClient);
eNetReturn_t RasPiUDPClientSend(sUDPClient_t *pUDPClient, uint32_t nDataBytes, void *pData);
eNetReturn_t RasPiUDPClientReceive(sUDPClient_t *pUDPClient, uint32_t nDataBytes, void *pData, uint32_t *pnBytesRecv);

/*****	Functions	*****/
eNetReturn_t RasPiTCPServInitialize(sTCPServ_t *pTCPServ) {
	//Always begin with default settigns
	IfaceTCPServObjInitialize(pTCPServ);
	
	//Update the function pointers to this implementation
	pTCPServ->pfInitialize = &RasPiTCPServInitialize;
	pTCPServ->pfBind = &RasPiTCPServBind;
	pTCPServ->pfCloseHost = &RasPiTCPServCloseHost;
	pTCPServ->pfCloseSocket = &RasPiTCPServCloseSocket;
	pTCPServ->pfAcceptClient = &RasPiTCPServAcceptClient;
	pTCPServ->pfReceive = &RasPiTCPServReceive;
	pTCPServ->pfSend = &RasPiTCPServSend;
	
	//Set other object values
	pTCPServ->eCapabilities = TCPSERV_CAPS;
	pTCPServ->pHWInfo = NULL;
	
	return Net_Success;
}

eNetReturn_t RasPiTCPServBind(sTCPServ_t *pTCPServ, sConnInfo_t *pConn) {
	struct sockaddr_in sAddr;
	
	if (pTCPServ->HostSck.nSocket != SOCKET_INVALID) { //Socket appears open, close it
		RasPiTCPServCloseSocket(pTCPServ, &pTCPServ->HostSck);
	}
	
	//Create server socket
	pTCPServ->HostSck.nSocket = socket(AF_INET, SOCK_STREAM, 0);
	pTCPServ->HostSck.Conn.Addr.nNetLong = pConn->Addr.nNetLong;
	pTCPServ->HostSck.Conn.Port = pConn->Port;
	
	if (pTCPServ->HostSck.nSocket == SOCKET_INVALID) {
		return NetFail_Unknown; //errno may hold more information
	}
	
	//Bind the requested port
	sAddr.sin_port = htons(pConn->Port);
	sAddr.sin_family = AF_INET;
	
	sAddr.sin_addr.s_addr = pConn->Addr.nNetLong;
	
	if (bind(pTCPServ->HostSck.nSocket, (struct sockaddr *)&sAddr, sizeof(struct sockaddr_in)) != 0) {
		//Returns -1 on error, errno holds code
		return NetFail_BindErr;
	}
	
	if (listen(pTCPServ->HostSck.nSocket, 1) != 0) {
		//Returns -1 on error, errno holds code
		return NetFail_Unknown;
	}
	
	return Net_Success;
}

eNetReturn_t RasPiTCPServCloseHost(sTCPServ_t *pTCPServ) {
	if (pTCPServ->HostSck.nSocket == SOCKET_INVALID) {
		return NetFail_InvSocket;
	}
	
	close(pTCPServ->HostSck.nSocket);
	pTCPServ->HostSck.nSocket = SOCKET_INVALID;
	pTCPServ->HostSck.Conn.Addr.nNetLong = 0;
	pTCPServ->HostSck.Conn.Port = 0;
	
	return Net_Success;
}

eNetReturn_t RasPiTCPServCloseSocket(sTCPServ_t *pTCPServ, sSocket_t *pSck) {
	if (pSck->nSocket == SOCKET_INVALID) {
		return NetFail_InvSocket;
	}
	
	close(pSck->nSocket);
	pSck->nSocket = SOCKET_INVALID;
	
	return Net_Success;
}

eNetReturn_t RasPiTCPServAcceptClient(sTCPServ_t *pTCPServ, sSocket_t *pSck) {
	struct sockaddr_in sAddr;
	socklen_t nAddrSize = sizeof(struct sockaddr_in);
	
	pSck->nSocket = SOCKET_INVALID;
	while (pSck->nSocket== SOCKET_INVALID) {
		pSck->nSocket= accept(pTCPServ->HostSck.nSocket, (struct sockaddr *)&sAddr, &nAddrSize);
		//printf("Accept fail %d / %d\r\n", *sSocket_t, errno);
		if (errno == 9) { //Error 9: "Bad file number" means socket is not listening
			return NetFail_SocketState;
		}
	}
	
	//sAddr holds the client information
	pSck->Conn.Addr.nNetLong = sAddr.sin_addr.s_addr;
	pSck->Conn.Port = ntohs(sAddr.sin_port);
	
	return Net_Success;
}

eNetReturn_t RasPiTCPServReceive(sTCPServ_t *pTCPServ, sSocket_t *pSck, uint32_t nDataBytes, void *pData, uint32_t *pnBytesRecv) {
	int nResult;
	
	nResult = recv(pSck->nSocket, pData, nDataBytes, 0);
	if (nResult == SOCKET_INVALID) {
		*pnBytesRecv = 0;
		
		//errno has the failure code
		return NetFail_Unknown;
	}
	
	*pnBytesRecv = nResult;
	return Net_Success;
}

eNetReturn_t RasPiTCPServSend(sTCPServ_t *pTCPServ, sSocket_t *pSck, uint32_t nDataBytes, void *pData) {
	int nResult;
	
	nResult = send(pSck->nSocket, pData, nDataBytes, 0);
	if (nResult == SOCKET_INVALID) {
		//errno has the failure code
		return NetFail_Unknown;
	}
	
	return Net_Success;
}

eNetReturn_t RasPiTCPClientInitialize(sTCPClient_t *pTCPClient) {
	//Always begin with default settigns
	IfaceTCPClientObjInitialize(pTCPClient);
	
	//Update the function pointers to this implementation
	pTCPClient->pfInitialize = &RasPiTCPClientInitialize;
	pTCPClient->pfConnect = &RasPiTCPClientConnect;
	pTCPClient->pfClose = &RasPiTCPClientClose;
	pTCPClient->pfReceive = &RasPiTCPClientReceive;
	pTCPClient->pfSend = &RasPiTCPClientSend;
	
	//Set other object values
	pTCPClient->Sck.nSocket = SOCKET_INVALID;
	pTCPClient->Sck.Conn.Addr.nNetLong = 0;
	pTCPClient->Sck.Conn.Port = 0;
	pTCPClient->eCapabilities = TCPCLIENT_CAPS;
	pTCPClient->pHWInfo = NULL;
	
	return Net_Success;
}

eNetReturn_t RasPiTCPClientConnect(sTCPClient_t *pTCPClient, sConnInfo_t *pConn) {
	struct sockaddr_in sAddr;
	
	if (pTCPClient->Sck.nSocket != SOCKET_INVALID) { //Socket appears open, close it
		RasPiTCPClientClose(pTCPClient);
	}
	
	//Create client socket
	pTCPClient->Sck.nSocket = socket(AF_INET, SOCK_STREAM, 0);
	pTCPClient->Sck.Conn.Addr.nNetLong = pConn->Addr.nNetLong;
	pTCPClient->Sck.Conn.Port = pConn->Port;
	
	if (pTCPClient->Sck.nSocket == SOCKET_INVALID) {
		return NetFail_Unknown; //errno may hold more information
	}
	
	//Attempt to make the connection
	sAddr.sin_port = htons(pConn->Port);
	sAddr.sin_family = AF_INET;
	
	sAddr.sin_addr.s_addr = pConn->Addr.nNetLong;
	
	if (connect(pTCPClient->Sck.nSocket, (struct sockaddr *)&sAddr, sizeof(struct sockaddr_in)) == SOCKET_INVALID) {
		//printf("Connect fail %d\r\n", errno);
		
		if (errno == 111) {
			return NetFail_ConnRefuse;
		} else {
			return NetFail_Unknown;
		}
	}
	
	return Net_Success;
}

eNetReturn_t RasPiTCPClientClose(sTCPClient_t *pTCPClient) {
	if (pTCPClient->Sck.nSocket == SOCKET_INVALID) { //Socket appears open, close it
		return Net_Success;
	}
	
	close(pTCPClient->Sck.nSocket);
	pTCPClient->Sck.nSocket = SOCKET_INVALID;
	pTCPClient->Sck.Conn.Addr.nNetLong = 0;
	pTCPClient->Sck.Conn.Port = 0;
	
	return Net_Success;
}

eNetReturn_t RasPiTCPClientReceive(sTCPClient_t *pTCPClient, uint32_t nDataBytes, void *pData, uint32_t *pnBytesRecv) {
	int nResult;
	
	nResult = recv(pTCPClient->Sck.nSocket, pData, nDataBytes, 0);
	if (nResult == SOCKET_INVALID) {
		*pnBytesRecv = 0;
		
		//errno has the failure code
		return NetFail_Unknown;
	}
	
	*pnBytesRecv = nResult;
	return Net_Success;
}

eNetReturn_t RasPiTCPClientSend(sTCPClient_t *pTCPClient, uint32_t nDataBytes, void *pData) {
	int nResult;
	
	nResult = send(pTCPClient->Sck.nSocket, pData, nDataBytes, 0);
	if (nResult == SOCKET_INVALID) {
		//errno has the failure code
		return NetFail_Unknown;
	}
	
	return Net_Success;
}

