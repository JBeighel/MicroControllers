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
eNetReturn_t IfaceTCPServCloseSocket(sTCPServ_t *pTCPServ, sSocket_t *pSck);
eNetReturn_t IfaceTCPServAcceptClient(sTCPServ_t *pTCPServ, sSocket_t *pClientSck);
eNetReturn_t IfaceTCPServReceive(sTCPServ_t *pTCPServ, sSocket_t *pClientSck, uint32_t nNumBytes, void *pData, uint32_t *pnBytesRecv);
eNetReturn_t IfaceTcpServSend(sTCPServ_t *pTCPServ, sSocket_t *pClientSck, uint32_t nDataBytes, void *pData);
eNetReturn_t IfaceTCPServGetClientInfo(sTCPServ_t *pTCPServ, sConnInfo_t *pConn);

eNetReturn_t IfaceTCPClientInitialize(sTCPClient_t *pTCPClient);
eNetReturn_t IfaceTCPClientConnect(sTCPClient_t *pTCPClient, sConnInfo_t *pConn);
eNetReturn_t IfaceTCPClientClose(sTCPClient_t *pTCPClient);
eNetReturn_t IfaceTCPClientReceive(sTCPClient_t *pTCPClient, uint32_t nNumBytes, void *pData, uint32_t *pnBytesRecv);
eNetReturn_t IfaceTCPClientSend(sTCPClient_t *pTCPClient, uint32_t nDataBytes, void *pData);

eNetReturn_t IfaceUDPServInitialize(sUDPServ_t *pUDPServ);
eNetReturn_t IfaceUDPServBind(sUDPServ_t *pUDPServ, sConnInfo_t *pConn);
eNetReturn_t IfaceUDPServCloseHost(sUDPServ_t *pUDPServ);
eNetReturn_t IfaceUDPServReceive(sUDPServ_t *pUDPServ, sSocket_t *pClientSck, uint32_t nDataBytes, void *pData, uint32_t *pnBytesRecv);
eNetReturn_t IfaceUDPServSend(sUDPServ_t *pUDPServ, sConnInfo_t *pConn, uint32_t nDataBytes, void *pData);

eNetReturn_t IfaceUDPClientInitialize(sUDPClient_t *pUDPClient);
eNetReturn_t IfaceUDPClientSetServer(sUDPClient_t *pUDPClient, sConnInfo_t *pConn);
eNetReturn_t IfaceUDPClientClose(sUDPClient_t *pUDPClient);
eNetReturn_t IfaceUDPClientSend(sUDPClient_t *pUDPClient, uint32_t nDataBytes, void *pData);
eNetReturn_t IfaceUDPClientReceive(sUDPClient_t *pUDPClient, uint32_t nDataBytes, void *pData, uint32_t *pnBytesRecv);

/*****	Functions	*****/
eNetReturn_t IfaceTCPServObjInitialize(sTCPServ_t *pTCPServ) {
	pTCPServ->HostSck.nSocket = SOCKET_INVALID;
	pTCPServ->HostSck.Conn.Addr.nNetLong = 0;
	pTCPServ->HostSck.Conn.Port = 0;
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

eNetReturn_t IfaceTCPServCloseSocket(sTCPServ_t *pTCPServ, sSocket_t *pSck) {
	return NetFail_NotImplem;
}

eNetReturn_t IfaceTCPServAcceptClient(sTCPServ_t *pTCPServ, sSocket_t *pClientSck) {
	return NetFail_NotImplem;
}

eNetReturn_t IfaceTCPServReceive(sTCPServ_t *pTCPServ, sSocket_t *pClientSck, uint32_t nNumBytes, void *pData, uint32_t *pnBytesRecv) {
	return NetFail_NotImplem;
}

eNetReturn_t IfaceTcpServSend(sTCPServ_t *pTCPServ, sSocket_t *pClientSck, uint32_t nDataBytes, void *pData) {
	return NetFail_NotImplem;
}

eNetReturn_t IfaceTCPClientObjInitialize(sTCPClient_t *pTCPClient) {
	pTCPClient->Sck.nSocket = SOCKET_INVALID;
	pTCPClient->eCapabilities = TCPClient_None;
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

eNetReturn_t IfaceUDPServObjInitialize(sUDPServ_t *pUDPServ) {
	pUDPServ->HostSck.nSocket = SOCKET_INVALID;
	pUDPServ->HostSck.Conn.Addr.nNetLong = 0;
	pUDPServ->HostSck.Conn.Port = 0;
	pUDPServ->eCapabilities = UDPServ_None;
	pUDPServ->pHWInfo = NULL;
	
	pUDPServ->pfInitialize = &IfaceUDPServInitialize;
	pUDPServ->pfBind = &IfaceUDPServBind;
	pUDPServ->pfCloseHost = &IfaceUDPServCloseHost;
	pUDPServ->pfReceive = &IfaceUDPServReceive;
	pUDPServ->pfSend = &IfaceUDPServSend;
	
	return Net_Success;
}

eNetReturn_t IfaceUDPServInitialize(sUDPServ_t *pUDPServ) {
	return NetFail_NotImplem;
}

eNetReturn_t IfaceUDPServBind(sUDPServ_t *pUDPServ, sConnInfo_t *pConn) {
	return NetFail_NotImplem;
}

eNetReturn_t IfaceUDPServCloseHost(sUDPServ_t *pUDPServ) {
	return NetFail_NotImplem;
}

eNetReturn_t IfaceUDPServReceive(sUDPServ_t *pUDPServ, sSocket_t *pClientSck, uint32_t nDataBytes, void *pData, uint32_t *pnBytesRecv) {
	return NetFail_NotImplem;
}

eNetReturn_t IfaceUDPServSend(sUDPServ_t *pUDPServ, sConnInfo_t *pConn, uint32_t nDataBytes, void *pData) {
	return NetFail_NotImplem;
}

eNetReturn_t IfaceUDPClientObjInitialize(sUDPClient_t *pUDPClient) {
	pUDPClient->Sck.nSocket = SOCKET_INVALID;
	pUDPClient->Sck.Conn.Addr.nNetLong = 0;
	pUDPClient->Sck.Conn.Port = 0;
	pUDPClient->eCapabilities = UDPClient_None;
	pUDPClient->pHWInfo = NULL;
	
	pUDPClient->pfInitialize = &IfaceUDPClientInitialize;
	pUDPClient->pfSetServer = &IfaceUDPClientSetServer;
	pUDPClient->pfClose = &IfaceUDPClientClose;
	pUDPClient->pfReceive = &IfaceUDPClientReceive;
	pUDPClient->pfSend = &IfaceUDPClientSend;
	
	return Net_Success;
}

eNetReturn_t IfaceUDPClientInitialize(sUDPClient_t *pUDPClient) {
	return NetFail_NotImplem;
}

eNetReturn_t IfaceUDPClientSetServer(sUDPClient_t *pUDPClient, sConnInfo_t *pConn) {
	return NetFail_NotImplem;
}

eNetReturn_t IfaceUDPClientClose(sUDPClient_t *pUDPClient) {
	return NetFail_NotImplem;
}

eNetReturn_t IfaceUDPClientSend(sUDPClient_t *pUDPClient, uint32_t nDataBytes, void *pData) {
	return NetFail_NotImplem;
}

eNetReturn_t IfaceUDPClientReceive(sUDPClient_t *pUDPClient, uint32_t nDataBytes, void *pData, uint32_t *pnBytesRecv) {
	return NetFail_NotImplem;
}
