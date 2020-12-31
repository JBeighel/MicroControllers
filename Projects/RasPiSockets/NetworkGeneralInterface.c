/**	File:	NetworkGeneralInterface.c
	Author:	J. Beighel
	Date:	12-30-2020
*/

/*****	Includes	*****/
	#include "NetworkGeneralInterface.h"

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

eNetReturn_t IfaceTCPClientInitialize(sTCPClient_t *pTCPClient);
eNetReturn_t IfaceTCPClientConnect(sTCPClient_t *pTCPClient, sConnInfo_t *pConn);
eNetReturn_t IfaceTCPClientClose(sTCPClient_t *pTCPClient);
eNetReturn_t IfaceTCPClientReceive(sTCPClient_t *pTCPClient, uint32_t nNumBytes, void *pData, uint32_t *pnBytesRecv);
eNetReturn_t IfaceTCPClientSend(sTCPClient_t *pTCPClient, uint32_t nDataBytes, void *pData);

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

