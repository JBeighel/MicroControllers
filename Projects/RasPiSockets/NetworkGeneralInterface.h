/**	@defgroup	networkgeniface
	@brief		
	@details	v0.1
	#Description
	
	#File Information
		File:	NetworkGeneralInterface.h
		Author:	J. Beighel
		Date:	12-30-2020
*/

#ifndef __NETWORKGENIFACE_H
	#define __NETWORKGENIFACE_H

/*****	Includes	*****/
	#include <stddef.h>
	#include <stdint.h>

/*****	Defines		*****/
	/**	@brief		Indication of unused socket variables
		@ingroup	networkgeniface
	*/
	#define SOCKET_INVALID	-1
	
/*****	Definitions	*****/
	typedef struct sTCPServ_t sTCPServ_t;
	typedef struct sTCPClient_t sTCPClient_t;
	typedef struct sUDPServ_t sUDPServ_t;
	typedef struct sUDPClient_t sUDPClient_t;
	typedef struct sConnInfo_t sConnInfo_t;
	typedef struct sSocket_t sSocket_t;

	/**	@brief		Enumeration of all return codes for ethernet functions
		@ingroup	networkgeniface
	*/
	typedef enum eNetReturn_t {
		NetWarn_WrongPort	= 3,	/**< Data was received from an nexpected port */
		NetWarn_WrongIP		= 2,	/**< Data was received from an unexpected IP Address */
		NetWarn_Unknown		= 1,	/**< An unknown but recoverable error happened during the operation */
		Net_Success			= 0,	/**< The operation completed successfully */
		NetFail_Unknown		= -1,	/**< An unknown and unrecoverable error happened during the operation */
		NetFail_NotImplem	= -2,	/**< Function not implemented */
		NetFail_InvSocket	= -3,	/**< The socket provided was invalid */
		NetFail_BindErr		= -4,	/**< Unable to bind to the requested port */
		NetFail_SocketState	= -5,	/**< Socket was in the wrong state for the request */
		NetFail_ConnRefuse	= -6,	/**< A connection attempt was refused by the server */
	} eNetReturn_t;
	
	/**	@brief		Enumeration of all capabilities the TCP Server General Interface defines
		@ingroup	networkgeniface
	*/
	typedef enum eTCPServerCapabilities_t {
		TCPServ_None		= 0x00,
		TCPServ_Bind		= 0x01,
		TCPServ_CloseHost	= 0x02,
		TCPServ_AcceptConn	= 0x04,
		TCPServ_Receive		= 0x08,
		TCPServ_Send		= 0x10,
		TCPServ_CloseClient	= 0x20,
	} eTCPServerCapabilities_t;
	
	/**	@brief		Enumeration of all capabilities the TCP Client General Interface defines
		@ingroup	networkgeniface
	*/
	typedef enum eTCPClientCapabilities_t {
		TCPClient_None		= 0x00,
		TCPClient_Connect	= 0x01,
		TCPClient_Close		= 0x02,
		TCPClient_Receive	= 0x04,
		TCPClient_Send		= 0x08,
	} eTCPClientCapabilities_t;
	
	/**	@brief		Enumeration of all capabilities the UDP Server General Interface defines
		@ingroup	networkgeniface
	*/
	typedef enum eUDPServerCapabilities_t {
		UDPServ_None		= 0x00,
		UDPServ_Bind		= 0x01,
		UDPServ_CloseHost	= 0x02,
		UDPServ_Receive		= 0x04,
		UDPServ_Send		= 0x08,
	} eUDPServerCapabilities_t;
	
	/**	@brief		Enumeration of all capabilities the UDP Client General Interface defines
		@ingroup	networkgeniface
	*/
	typedef enum eUDPClientCapabilities_t {
		UDPClient_None		= 0x00,
		UDPClient_SetServ	= 0x01,
		UDPClient_Receive	= 0x02,
		UDPClient_Send		= 0x04,
	} eUDPClientCapabilities_t;
	
	/**	@brief		Definition for variables containing ethernet port information
		@ingroup	networkgeniface
	*/
	typedef uint16_t Port_t;
	
	/**	@brief		Function that initializes the TCP Server object and readies it for use
		@param		pTCPServ		Pointer to the TCP Server object to prepare
		@return		Net_Success on succeess, or a code indicating the type of error encountered
		@ingroup	networkgeniface
	*/
	typedef eNetReturn_t (*pfNetTCPServInitialize_t)(sTCPServ_t *pTCPServ);
	
	/**	@brief		Binds a port on the specified IP address and listens for incoming connections
		@param		pTCPServ		Pointer to the TCP Server object to use
		@param		pConn			IP address and port number to bind
		@return		Net_Success on succeess, or a code indicating the type of error encountered
		@ingroup	networkgeniface
	*/
	typedef eNetReturn_t (*pfNetTCPServBind_t)(sTCPServ_t *pTCPServ, sConnInfo_t *pConn);
	
	/**	@brief		Closes the socket the TCP service is listening for client connections on
		@param		pTCPServ		Pointer to the TCP Server object to use
		@return		Net_Success on succeess, or a code indicating the type of error encountered
		@ingroup	networkgeniface
	*/
	typedef eNetReturn_t (*pfNetTCPServCloseHost_t)(sTCPServ_t *pTCPServ);
	
	/**	@brief		Closes the socket used for an individual client connection
		@param		pTCPServ		Pointer to the TCP Server object to use
		@param		nSck			Client conneciton to close
		@return		Net_Success on succeess, or a code indicating the type of error encountered
		@ingroup	networkgeniface
	*/
	typedef eNetReturn_t (*pfNetTCPServClosesSocket_t)(sTCPServ_t *pTCPServ, sSocket_t *pSck);
	
	/**	@brief		Waits for and accepts an incoming client connection request
		@param		pTCPServ		Pointer to the TCP Server object to use
		@param		pClientSck		Returns the socket used for this individual client connection
		@param		pClientInfo		Connection information for the client request that was received
		@return		Net_Success on succeess, or a code indicating the type of error encountered
		@ingroup	networkgeniface
	*/
	typedef eNetReturn_t (*pfNetTCPServAcceptClient_t)(sTCPServ_t *pTCPServ, sSocket_t *pClientSck);
	
	/**	@brief		Wait for and receive data from a client connection
		@details	Blocks until some data is received.  All data received that will fit in the 
			provided buffer is returned.  This may not fill the entire buffer.
		@param		pTCPServ		Pointer to the TCP Server object to use
		@param		pClientSock		Pointer to the client socket to receive the data from
		@param		nDataBytes		Number of data bytes the buffer can hold
		@param		pData			Buffer that will receive any data that is received
		@param		pnBytesRecv		Number of data bytes that were actually received
		@return		Net_Success on succeess, or a code indicating the type of error encountered
		@ingroup	networkgeniface
	*/
	typedef eNetReturn_t (*pfNetTCPServReceive_t)(sTCPServ_t *pTCPServ, sSocket_t *pClientSck, uint32_t nDataBytes, void *pData, uint32_t *pnBytesRecv);
	
	/**	@brief		Send data to a client connection
		@param		pTCPServ		Pointer to the TCP Server object to use
		@param		pClientSock		Pointer to the client socket to send the data through
		@param		nDataBytes		Number of data bytes to be sent
		@param		pData			Buffer holding the data to send
		@return		Net_Success on succeess, or a code indicating the type of error encountered
		@ingroup	networkgeniface
	*/
	typedef eNetReturn_t (*pfNetTCPServSend_t)(sTCPServ_t *pTCPServ, sSocket_t *pClientSck, uint32_t nDataBytes, void *pData);
	
	/**	@brief		Initialize a TCP client object and prepare it for use
		@param		pTCPClient		Pointer to hte TCP Client object to use
		@return		Net_Success on succeess, or a code indicating the type of error encountered
		@ingroup	networkgeniface
	*/
	typedef eNetReturn_t (*pfNetTCPClientInitialize_t)(sTCPClient_t *pTCPClient);
	
	/**	@brief		Attempts to establish a connection to a TCP Server
		@param		pTCPClient		Pointer to hte TCP Client object to use
		@param		pConn			COnnection information of the TCP server
		@return		Net_Success on succeess, or a code indicating the type of error encountered
		@ingroup	networkgeniface
	*/
	typedef eNetReturn_t (*pfNetTCPClientConnect_t)(sTCPClient_t *pTCPClient, sConnInfo_t *pConn);
	
	/**	@brief		Closes the TCP connection
		@param		pTCPClient		Pointer to hte TCP Client object to use
		@return		Net_Success on succeess, or a code indicating the type of error encountered
		@ingroup	networkgeniface
	*/
	typedef eNetReturn_t (*pfNetTCPClientClose_t)(sTCPClient_t *pTCPClient);
	
	/**	@brief		Waits for and receives data from a TCP server
		@param		pTCPClient		Pointer to hte TCP Client object to use
		@param		nDataBytes		Number of bytes the data buffer can hold
		@param		pData			Pointer to the bufer to receive the data 
		@param		pnBytesRecv		Returns the number of data bytes received
		@return		Net_Success on succeess, or a code indicating the type of error encountered
		@ingroup	networkgeniface
	*/
	typedef eNetReturn_t (*pfNetTCPClientReceive_t)(sTCPClient_t *pTCPClient, uint32_t nDataBytes, void *pData, uint32_t *pnBytesRecv);
	
	/**	@brief		Send data to a TCP server
		@param		pTCPClient		Pointer to hte TCP Client object to use
		@param		nDataBytes		Number of bytes to send
		@param		pData			Buffer holding the data to send
		@return		Net_Success on succeess, or a code indicating the type of error encountered
		@ingroup	networkgeniface
	*/
	typedef eNetReturn_t (*pfNetTCPClientSend_t)(sTCPClient_t *pTCPClient, uint32_t nDataBytes, void *pData);
	
	typedef eNetReturn_t (*pfNetUDPServInitialize_t)(sUDPServ_t *pUDPServ);
	typedef eNetReturn_t (*pfNetUDPServBind_t)(sUDPServ_t *pUDPServ, sConnInfo_t *pConn);
	typedef eNetReturn_t (*pfNetUDPServCloseHost_t)(sUDPServ_t *pUDPServ);
	typedef eNetReturn_t (*pfNetUDPServReceive_t)(sUDPServ_t *pUDPServ, sConnInfo_t *pConn, uint32_t nDataBytes, void *pData, uint32_t *pnBytesRecv);
	typedef eNetReturn_t (*pfNetUDPServSend_t)(sUDPServ_t *pUDPServ, sConnInfo_t *pConn, uint32_t nDataBytes, void *pData);
	
	typedef eNetReturn_t (*pfNetUDPClientInitialize_t)(sUDPClient_t *pUDPClient);
	typedef eNetReturn_t (*pgNetUDPClientSetServer_t)(sUDPClient_t *pUDPClient, sConnInfo_t *pConn);
	typedef eNetReturn_t (*pfNetUDPClientClose_t)(sUDPClient_t *pUDPClient);
	typedef eNetReturn_t (*pfNetUDPClientSend_t)(sUDPClient_t *pUDPClient, uint32_t nDataBytes, void *pData);
	typedef eNetReturn_t (*pfNetUDPClientReceive_t)(sUDPClient_t *pUDPClient, uint32_t nDataBytes, void *pData, uint32_t *pnBytesRecv);
	
	/**	@brief		Union holding an IPv4 address
		@ingroup	networkgeniface
	*/
	typedef union uNetIP_t {
		/**	Individual octects of the address, b3.b2.b1.b0 */
		struct {
			uint8_t b0;
			uint8_t b1;
			uint8_t b2;
			uint8_t b3;
		} Octets;
		
		uint8_t aBytes[4];	/**< The IP address broken into 4 byte values */
		
		uint32_t nNetLong;	/**< The IP address as a single 32bit value */
	} uNetIP_t;
	
	/**	@brief		Structure containing all information describing an ethernet service
		@ingroup	networkgeniface
	*/
	typedef struct sConnInfo_t {
		uNetIP_t Addr;	/**< IPv4 Address */
		Port_t Port;	/**< Port number */
	} sConnInfo_t;
	
	/**	@brief		Definition for variables containing ethernet socket information
		@ingroup	networkgeniface
	*/
	typedef struct sSocket_t {
		int32_t nSocket;	/**< Socket the connection is through */
		sConnInfo_t Conn;	/**< Information for the remote system */
	} sSocket_t;
	
	/**	@brief		TCP Server object
		@ingroup	networkgeniface
	*/
	typedef struct sTCPServ_t {
		sSocket_t HostSck;							/**< Socket server is listening on */
		eTCPServerCapabilities_t eCapabilities;		/**< Capabilities this implementation provides */
		
		pfNetTCPServInitialize_t pfInitialize;		/**< Function to initialize the implementation */
		pfNetTCPServBind_t pfBind;					/**< Function to bind a port on an address to listen for connections */
		pfNetTCPServCloseHost_t pfCloseHost;		/**< Function to close the host socket */
		pfNetTCPServClosesSocket_t pfCloseSocket;	/**< Function to close a client socket */
		pfNetTCPServAcceptClient_t pfAcceptClient;	/**< Function to wait for an accept an client connection */
		pfNetTCPServReceive_t pfReceive;			/**< Function to receive data from a client connection */
		pfNetTCPServSend_t pfSend;					/**< Function to send data to a client connection */
		
		void *pHWInfo;								/**< Information for use by the implementation */
	} sTCPServ_t;
	
	/**	@brief		TCP Client object
		@ingroup	networkgeniface
	*/
	typedef struct sTCPClient_t {
		sSocket_t Sck;								/**< Socket client communicates through */
		eTCPClientCapabilities_t eCapabilities;		/**< Capabilities this implementation provides */
		
		pfNetTCPClientInitialize_t pfInitialize;	/**< Function to initialize the implementation */
		pfNetTCPClientConnect_t pfConnect;			/**< Function to establish the connection through */
		pfNetTCPClientClose_t pfClose;				/**< Function to close the connection */
		pfNetTCPClientReceive_t pfReceive;			/**< Function to receive data from the server */
		pfNetTCPClientSend_t pfSend;				/**< Function to send data to the server */
		
		void *pHWInfo;								/**< Information for use by the implementation */
	} sTCPClient_t;
	
	typedef struct sUDPServ_t {
		sSocket_t HostSck;
		eUDPServerCapabilities_t eCapabilities;
		
		pfNetUDPServInitialize_t pfInitialize;
		pfNetUDPServBind_t pfBind;
		pfNetUDPServCloseHost_t pfCloseHost;
		pfNetUDPServReceive_t pfReceive;
		pfNetUDPServSend_t pfSend;
		
		void *pHWInfo;
	} sUDPServ_t;
	
	typedef struct sUDPClient_t {
		sSocket_t Sck;
		eUDPClientCapabilities_t eCapabilities;
		
		pfNetUDPClientInitialize_t pfInitialize;
		pgNetUDPClientSetServer_t pfSetServer;
		pfNetUDPClientReceive_t pfReceive;
		pfNetUDPClientSend_t pfSend;
		
		void *pHWInfo;
	} sUDPClient_t;

/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	eNetReturn_t IfaceTCPServObjInitialize(sTCPServ_t *pTCPServ);
	
	eNetReturn_t IfaceTCPClientObjInitialize(sTCPClient_t *pTCPClient);
	
	eNetReturn_t IfaceUDPServObjInitialize(sUDPServ_t *pUDPServ);
	
	eNetReturn_t IfaceUDPClientObjInitialize(sUDPClient_t *pUDPClient);
	
/*****	Functions	*****/


#endif

