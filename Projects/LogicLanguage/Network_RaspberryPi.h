/**	@defgroup	raspinetwork
	@brief		
	@details	v0.1
	#Description
	
	#File Information
		File:	Network_RaspberryPi.h
		Author:	J. Beighel
		Date:	2021-04-18
*/

#ifndef __RASPINETWORK_H
	#define __RASPINETWORK_H

/*****	Includes	*****/
	#include <stdio.h>
	
	#include <unistd.h>
    #include <sys/socket.h>
	#include <sys/time.h>
    #include <netinet/in.h>
	#include <errno.h>
	
	#include "NetworkGeneralInterface.h"	

/*****	Defines		*****/
	/**	@brief		Capabilities of the Raspberry Pi implementation of the TCP Server
		@ingroup	raspinetwork
	*/
	#define TCPSERV_CAPS	(TCPServ_Bind | TCPServ_CloseHost | TCPServ_AcceptConn | TCPServ_CloseClient | TCPServ_Receive | TCPServ_Send)
	
	/**	@brief		Capabilities of the Raspberry Pi implementation of the TCP Client
		@ingroup	raspinetwork
	*/
	#define TCPCLIENT_CAPS	(TCPClient_Connect | TCPClient_Close | TCPClient_Receive | TCPClient_Send)
	
	#define UDPSERV_CAPS	(UDPServ_Bind | UDPServ_CloseHost | UDPServ_Receive | UDPServ_Send)
	
	#define UDPCLIENT_CAPS	(UDPClient_SetServ | UDPClient_Send | UDPClient_Receive)
	
	/**	@brief		Initialization definition of the Raspberry Pi implementation of the TCP Server
		@ingroup	raspinetwork
	*/
	#define TCPSERV_INIT	RasPiTCPServInitialize
	
	/**	@brief		Initialization definition of the Raspberry Pi implementation of the TCP client
		@ingroup	raspinetwork
	*/
	#define TCPCLIENT_INIT	RasPiTCPClientInitialize
	
	#define UDPSERV_INIT	RasPiUDPServInitialize
	
	#define UDPCLIENT_INIT	RasPiUDPClientInitialize

/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	eNetReturn_t RasPiTCPServInitialize(sTCPServ_t *pTCPServ);
	
	eNetReturn_t RasPiTCPClientInitialize(sTCPClient_t *pTCPClient);
	
	eNetReturn_t RasPiUDPServInitialize(sUDPServ_t *pUDPServ);
	
	eNetReturn_t RasPiUDPClientInitialize(sUDPClient_t *pUDPClient);

/*****	Functions	*****/


#endif

