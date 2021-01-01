/**	@defgroup	raspinetwork
	@brief		
	@details	v0.1
	#Description
	
	#File Information
		File:	Network_RaspberryPi.h
		Author:	J. Beighel
		Date:	12-30-2020
*/

#ifndef __RASPINETWORK_H
	#define __RASPINETWORK_H

/*****	Includes	*****/
	#include <stdio.h>
	
	#include <unistd.h>
    #include <sys/socket.h>
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
	
	/**	@brief		Initialization definition of the Raspberry Pi implementation of the TCP Server
		@ingroup	raspinetwork
	*/
	#define TCPSERV_INIT	RasPiTCPServInitialize
	
	/**	@brief		Initialization definition of the Raspberry Pi implementation of the TCP client
		@ingroup	raspinetwork
	*/
	#define TCPCLIENT_INIT	RasPiTCPClientInitialize

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

