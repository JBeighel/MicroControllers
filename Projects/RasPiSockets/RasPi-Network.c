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
eNetReturn_t IfaceNetTCPCloseHost(sTCPServ_t *pTCPServ);
eNetReturn_t IfaceNetTCPCloseSocket(sTCPServ_t *pTCPServ, Socket_t nSck);

eNetReturn_t RasPiTCPServBind(sTCPServ_t *pTCPServ, sConnInfo_t *pConn);
eNetReturn_t RasPiNetTCPCloseHost(sTCPServ_t *pTCPServ);
eNetReturn_t RasPiNetTCPCloseSocket(sTCPServ_t *pTCPServ, Socket_t nSck);

/*****	Functions	*****/
eNetReturn_t IfaceTCPServObjInitialize(sTCPServ_t *pTCPServ) {
	pTCPServ->HostSck = SOCKET_INVALID;
	
	pTCPServ->pfInitialize = &IfaceTCPServInitialize;
	pTCPServ->pfBind = &IfaceTCPServBind;
	pTCPServ->pfCloseHost = &IfaceNetTCPCloseHost;
	pTCPServ->pfCloseSocket = &IfaceNetTCPCloseSocket;
	
	return Net_Success;
}

eNetReturn_t IfaceTCPServInitialize(sTCPServ_t *pTCPServ) {
	return NetFail_NotImplem;
}

eNetReturn_t IfaceTCPServBind(sTCPServ_t *pTCPServ, sConnInfo_t *pConn) {
	return NetFail_NotImplem;
}

eNetReturn_t IfaceNetTCPCloseHost(sTCPServ_t *pTCPServ) {
	return NetFail_NotImplem;
}

eNetReturn_t IfaceNetTCPCloseSocket(sTCPServ_t *pTCPServ, Socket_t nSck) {
	return NetFail_NotImplem;
}

eNetReturn_t RasPiTCPServInitialize(sTCPServ_t *pTCPServ) {
	//Always begin with default settigns
	IfaceTCPServObjInitialize(pTCPServ);
	
	//Update the function pointers to this implementation
	pTCPServ->pfInitialize = &RasPiTCPServInitialize;
	pTCPServ->pfBind = &RasPiTCPServBind;
	pTCPServ->pfCloseHost = &RasPiNetTCPCloseHost;
	pTCPServ->pfCloseSocket = &RasPiNetTCPCloseSocket;
	
	return Net_Success;
}

eNetReturn_t RasPiTCPServBind(sTCPServ_t *pTCPServ, sConnInfo_t *pConn) {
	struct sockaddr_in sAddr;
	uint32_t nIP;
	
	if (pTCPServ->HostSck != SOCKET_INVALID) { //Socket appears open, close it
		RasPiNetTCPCloseSocket(pTCPServ, pTCPServ->HostSck); //errno may hold more information
	}
	
	//Create server socket
	pTCPServ->HostSck = socket(AF_INET, SOCK_STREAM, 0);
	
	if (pTCPServ->HostSck == SOCKET_INVALID) {
		return NetFail_Unknown;
	}
	
	//Bind the requested port
	sAddr.sin_port = htons(pConn->Port);
	sAddr.sin_family = AF_INET;
	
	sAddr.sin_addr.s_addr = pConn->Addr.NetLong;
	
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

eNetReturn_t RasPiNetTCPCloseHost(sTCPServ_t *pTCPServ) {
	if (pTCPServ->HostSck == SOCKET_INVALID) {
		return NetFail_InvSocket;
	}
	
	close(pTCPServ->HostSck);
	
	return Net_Success;
}

eNetReturn_t RasPiNetTCPCloseSocket(sTCPServ_t *pTCPServ, Socket_t nSck) {
	if (nSck == SOCKET_INVALID) {
		return NetFail_InvSocket;
	}
	
	close(nSck);
	
	return Net_Success;
}
