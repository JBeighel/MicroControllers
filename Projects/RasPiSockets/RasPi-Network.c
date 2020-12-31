/**	File:	RasPi-Network.c
	Author:	J. Beighel
	Date:	12-29-2020
*/

/*****	Includes	*****/
	#include "RasPi-Network.h"

/*****	Defines		*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
eNetReturn_t IfaceTCPServInitialize(sTCPServ_t *pTCPServ);
eNetReturn_t IfaceTCPServBind(sTCPServ_t *pTCPServ, sConnInfo_t *pConn);
eNetReturn_t IfaceTCPServCloseHost(sTCPServ_t *pTCPServ);
eNetReturn_t IfaceTCPServCloseSocket(sTCPServ_t *pTCPServ, Socket_t nSck);
eNetReturn_t IfaceTCPServAcceptClient(sTCPServ_t *pTCPServ, Socket_t *pClientSck, sConnInfo_t *pClientInfo);
eNetReturn_t IfaceTCPServReceive(sTCPServ_t *pTCPServ, Socket_t *pClientSck, uint32_t nNumBytes, void *pData, uint32_t *pnBytesRecv);
eNetReturn_t IfaceTcpServSend(sTCPServ_t *pTCPServ, Socket_t *pClientSck, uint32_t nDataBytes, void *pData);
eNetReturn_t IfaceTCPServGetClientInfo(sTCPServ_t *pTCPServ, sConnInfo_t *pConn);

eNetReturn_t RasPiTCPServBind(sTCPServ_t *pTCPServ, sConnInfo_t *pConn);
eNetReturn_t RasPiTCPServCloseHost(sTCPServ_t *pTCPServ);
eNetReturn_t RasPiTCPServCloseSocket(sTCPServ_t *pTCPServ, Socket_t nSck);
eNetReturn_t RasPiTCPServAcceptClient(sTCPServ_t *pTCPServ, Socket_t *pClientSck, sConnInfo_t *pClientInfo);
eNetReturn_t RasPiTCPServReceive(sTCPServ_t *pTCPServ, Socket_t *pClientSck, uint32_t nNumBytes, void *pData, uint32_t *pnBytesRecv);
eNetReturn_t RasPiTCPServSend(sTCPServ_t *pTCPServ, Socket_t *pClientSck, uint32_t nDataBytes, void *pData);
eNetReturn_t RasPiTCPServGetClientInfo(sTCPServ_t *pTCPServ, sConnInfo_t *pConn);

eNetReturn_t IfaceTCPClientInitialize(sTCPClient_t *pTCPClient);
eNetReturn_t IfaceTCPClientConnect(sTCPClient_t *pTCPClient, sConnInfo_t *pConn);
eNetReturn_t IfaceTCPClientClose(sTCPClient_t *pTCPClient);
eNetReturn_t IfaceTCPClientReceive(sTCPClient_t *pTCPClient, uint32_t nNumBytes, void *pData, uint32_t *pnBytesRecv);
eNetReturn_t IfaceTCPClientSend(sTCPClient_t *pTCPClient, uint32_t nDataBytes, void *pData);

eNetReturn_t RasPiTCPClientInitialize(sTCPClient_t *pTCPClient);
eNetReturn_t RasPiTCPClientConnect(sTCPClient_t *pTCPClient, sConnInfo_t *pConn);
eNetReturn_t RasPiTCPClientClose(sTCPClient_t *pTCPClient);
eNetReturn_t RasPiTCPClientReceive(sTCPClient_t *pTCPClient, uint32_t nNumBytes, void *pData, uint32_t *pnBytesRecv);
eNetReturn_t RasPiTCPClientSend(sTCPClient_t *pTCPClient, uint32_t nDataBytes, void *pData);

/*****	Functions	*****/
eNetReturn_t IfaceTCPServObjInitialize(sTCPServ_t *pTCPServ) {
	pTCPServ->HostSck = SOCKET_INVALID;
	pTCPServ->eCapabilities = TCPServ_None;
	pTCPServ->pHWInfo = NULL;
	
	pTCPServ->pfInitialize = &IfaceTCPServInitialize;
	pTCPServ->pfBind = &IfaceTCPServBind;
	pTCPServ->pfCloseHost = &IfaceTCPServCloseHost;
	pTCPServ->pfCloseSocket = &IfaceTCPServCloseSocket;
	pTCPServ->pfAcceptClient = &IfaceTCPServAcceptClient;
	pTCPServ->pfReceive = &IfaceTCPServReceive;
	pTCPServ->pfSend = &IfaceTcpServSend;
	
	return Net_Success;
}

eNetReturn_t IfaceTCPServInitialize(sTCPServ_t *pTCPServ) {
	return NetFail_NotImplem;
}

eNetReturn_t IfaceTCPServBind(sTCPServ_t *pTCPServ, sConnInfo_t *pConn) {
	return NetFail_NotImplem;
}

eNetReturn_t IfaceTCPServCloseHost(sTCPServ_t *pTCPServ) {
	return NetFail_NotImplem;
}

eNetReturn_t IfaceTCPServCloseSocket(sTCPServ_t *pTCPServ, Socket_t nSck) {
	return NetFail_NotImplem;
}

eNetReturn_t IfaceTCPServAcceptClient(sTCPServ_t *pTCPServ, Socket_t *pClientSck, sConnInfo_t *pClientInfo) {
	return NetFail_NotImplem;
}

eNetReturn_t IfaceTCPServReceive(sTCPServ_t *pTCPServ, Socket_t *pClientSck, uint32_t nNumBytes, void *pData, uint32_t *pnBytesRecv) {
	return NetFail_NotImplem;
}

eNetReturn_t IfaceTcpServSend(sTCPServ_t *pTCPServ, Socket_t *pClientSck, uint32_t nDataBytes, void *pData) {
	return NetFail_NotImplem;
}

eNetReturn_t IfaceTCPClientObjInitialize(sTCPClient_t *pTCPClient) {
	pTCPClient->Sck = SOCKET_INVALID;
	pTCPClient->eCapabilities = TPClient_None;
	pTCPClient->pHWInfo = NULL;
	
	pTCPClient->pfInitialize = &IfaceTCPClientInitialize;
	pTCPClient->pfConnect = &IfaceTCPClientConnect;
	pTCPClient->pfClose = &IfaceTCPClientClose;
	pTCPClient->pfReceive = &IfaceTCPClientReceive;
	pTCPClient->pfSend = &IfaceTCPClientSend;
	
	return Net_Success;
}

eNetReturn_t IfaceTCPClientInitialize(sTCPClient_t *pTCPClient) {
	return NetFail_NotImplem;
}

eNetReturn_t IfaceTCPClientConnect(sTCPClient_t *pTCPClient, sConnInfo_t *pConn) {
	return NetFail_NotImplem;
}

eNetReturn_t IfaceTCPClientClose(sTCPClient_t *pTCPClient) {
	return NetFail_NotImplem;
}

eNetReturn_t IfaceTCPClientReceive(sTCPClient_t *pTCPClient, uint32_t nNumBytes, void *pData, uint32_t *pnBytesRecv) {
	return NetFail_NotImplem;
}

eNetReturn_t IfaceTCPClientSend(sTCPClient_t *pTCPClient, uint32_t nDataBytes, void *pData) {
	return NetFail_NotImplem;
}

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
	
	if (pTCPServ->HostSck != SOCKET_INVALID) { //Socket appears open, close it
		RasPiTCPServCloseSocket(pTCPServ, pTCPServ->HostSck);
	}
	
	//Create server socket
	pTCPServ->HostSck = socket(AF_INET, SOCK_STREAM, 0);
	
	if (pTCPServ->HostSck == SOCKET_INVALID) {
		return NetFail_Unknown; //errno may hold more information
	}
	
	//Bind the requested port
	sAddr.sin_port = htons(pConn->Port);
	sAddr.sin_family = AF_INET;
	
	sAddr.sin_addr.s_addr = pConn->Addr.nNetLong;
	
	if (bind(pTCPServ->HostSck, (struct sockaddr *)&sAddr, sizeof(struct sockaddr_in)) != 0) {
		//Returns -1 on error, errno holds code
		return NetFail_BindErr;
	}
	
	if (listen(pTCPServ->HostSck, 1) != 0) {
		//Returns -1 on error, errno holds code
		return NetFail_Unknown;
	}
	
	return Net_Success;
}

eNetReturn_t RasPiTCPServCloseHost(sTCPServ_t *pTCPServ) {
	if (pTCPServ->HostSck == SOCKET_INVALID) {
		return NetFail_InvSocket;
	}
	
	close(pTCPServ->HostSck);
	pTCPServ->HostSck = SOCKET_INVALID;
	
	return Net_Success;
}

eNetReturn_t RasPiTCPServCloseSocket(sTCPServ_t *pTCPServ, Socket_t nSck) {
	if (nSck == SOCKET_INVALID) {
		return NetFail_InvSocket;
	}
	
	close(nSck);
	nSck = SOCKET_INVALID;
	
	return Net_Success;
}

eNetReturn_t RasPiTCPServAcceptClient(sTCPServ_t *pTCPServ, Socket_t *pClientSck, sConnInfo_t *pClientInfo) {
	struct sockaddr_in sAddr;
	socklen_t nAddrSize = sizeof(struct sockaddr_in);
	
	*pClientSck = SOCKET_INVALID;
	while (*pClientSck == SOCKET_INVALID) {
		*pClientSck = accept(pTCPServ->HostSck, (struct sockaddr *)&sAddr, &nAddrSize);
		//printf("Accept fail %d / %d\r\n", *pClientSck, errno);
		if (errno == 9) { //Error 9: "Bad file number" means socket is not listening
			return NetFail_SocketState;
		}
	}
	
	//sAddr holds the client information
	pClientInfo->Addr.nNetLong = sAddr.sin_addr.s_addr;
	pClientInfo->Port = ntohs(sAddr.sin_port);
	
	return Net_Success;
}

eNetReturn_t RasPiTCPServReceive(sTCPServ_t *pTCPServ, Socket_t *pClientSck, uint32_t nDataBytes, void *pData, uint32_t *pnBytesRecv) {
	int nResult;
	
	nResult = recv(*pClientSck, pData, nDataBytes, 0);
	if (nResult == SOCKET_INVALID) {
		*pnBytesRecv = 0;
		
		//errno has the failure code
		return NetFail_Unknown;
	}
	
	*pnBytesRecv = nResult;
	return Net_Success;
}

eNetReturn_t RasPiTCPServSend(sTCPServ_t *pTCPServ, Socket_t *pClientSck, uint32_t nDataBytes, void *pData) {
	int nResult;
	
	nResult = send(*pClientSck, pData, nDataBytes, 0);
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
	pTCPClient->Sck = SOCKET_INVALID;
	pTCPClient->eCapabilities = TCPCLIENT_CAPS;
	pTCPClient->pHWInfo = NULL;
	
	return Net_Success;
}

eNetReturn_t RasPiTCPClientConnect(sTCPClient_t *pTCPClient, sConnInfo_t *pConn) {
	struct sockaddr_in sAddr;
	
	if (pTCPClient->Sck != SOCKET_INVALID) { //Socket appears open, close it
		RasPiTCPClientClose(pTCPClient);
	}
	
	//Create client socket
	pTCPClient->Sck = socket(AF_INET, SOCK_STREAM, 0);
	
	if (pTCPClient->Sck == SOCKET_INVALID) {
		return NetFail_Unknown; //errno may hold more information
	}
	
	//Attempt to make the connection
	sAddr.sin_port = htons(pConn->Port);
	sAddr.sin_family = AF_INET;
	
	sAddr.sin_addr.s_addr = pConn->Addr.nNetLong;
	
	if (connect(pTCPClient->Sck, (struct sockaddr *)&sAddr, sizeof(struct sockaddr_in)) == SOCKET_INVALID) {
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
	if (pTCPClient->Sck == SOCKET_INVALID) { //Socket appears open, close it
		return Net_Success;
	}
	
	close(pTCPClient->Sck);
	pTCPClient->Sck = SOCKET_INVALID;
	
	return Net_Success;
}

eNetReturn_t RasPiTCPClientReceive(sTCPClient_t *pTCPClient, uint32_t nDataBytes, void *pData, uint32_t *pnBytesRecv) {
	int nResult;
	
	nResult = recv(pTCPClient->Sck, pData, nDataBytes, 0);
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
	
	nResult = send(pTCPClient->Sck, pData, nDataBytes, 0);
	if (nResult == SOCKET_INVALID) {
		//errno has the failure code
		return NetFail_Unknown;
	}
	
	return Net_Success;
}

