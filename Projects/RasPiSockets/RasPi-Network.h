/**	@defgroup	raspinetwork
	@brief		
	@details	v0.1
	#Description
	
	#File Information
		File:	RasPi-Network.h
		Author:	J. Beighel
		Date:	12-29-2020
*/

#ifndef __RASPINETWORK_H
	#define __RASPINETWORK_H

/*****	Includes	*****/
	#include <unistd.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
	#include <errno.h>
	
	#include <stdio.h>

/*****	Defines		*****/
	#define SOCKET_INVALID	-1

/*****	Definitions	*****/
	typedef struct sTCPServ_t sTCPServ_t;
	typedef struct sConnInfo_t sConnInfo_t;

	typedef enum eNetReturn_t {
		NetWarn_Unknown		= 1,	/**< An unknown but recoverable error happened during the operation */
		Net_Success			= 0,	/**< The operation completed successfully */
		NetFail_Unknown		= -1,	/**< An unknown and unrecoverable error happened during the operation */
		NetFail_NotImplem	= -2,	/**< Function not implemented */
		NetFail_InvSocket	= -3,	/**< The socket provided was invalid */
		NetFail_BindErr		= -4,	/**< Unable to bind to the requested port */
		NetFail_SocketState	= -5,	/**< Socket was in the wrong state for the request */
	} eNetReturn_t;
	
	typedef int32_t Socket_t;
	
	typedef uint16_t Port_t;
	
	typedef eNetReturn_t (*pfNetTCPServInitialize_t)(sTCPServ_t *pTCPServ);
	typedef eNetReturn_t (*pfNetTCPServBind_t)(sTCPServ_t *pTCPServ, sConnInfo_t *pConn);
	typedef eNetReturn_t (*pfNetTCPCloseHost_t)(sTCPServ_t *pTCPServ);
	typedef eNetReturn_t (*pfNetTCPCloseSocket_t)(sTCPServ_t *pTCPServ, Socket_t nSck);
	typedef eNetReturn_t (*pfNetTCPAcceptClient_t)(sTCPServ_t *pTCPServ, Socket_t *pClientSck);
	
	typedef union uNetIP_t {
		struct {
			uint8_t b0;
			uint8_t b1;
			uint8_t b2;
			uint8_t b3;
		} Octets;
		
		uint8_t Bytes[4];
		
		uint32_t NetLong;
	} uNetIP_t;
	
	typedef struct sConnInfo_t {
		uNetIP_t Addr;
		Port_t Port;
	} sConnInfo_t;
	
	typedef struct sTCPServ_t {
		Socket_t HostSck;
		
		pfNetTCPServInitialize_t pfInitialize;
		pfNetTCPServBind_t pfBind;
		pfNetTCPCloseHost_t pfCloseHost;
		pfNetTCPCloseSocket_t pfCloseSocket;
		pfNetTCPAcceptClient_t pfAcceptClient;
	} sTCPServ_t;

/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	eNetReturn_t IfaceTCPServObjInitialize(sTCPServ_t *pTCPServ);
	
	eNetReturn_t RasPiTCPServInitialize(sTCPServ_t *pTCPServ);

/*****	Functions	*****/


#endif

