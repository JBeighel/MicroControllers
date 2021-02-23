/**	@defgroup	w5500driver
	@brief		Driver for the Wizner W5500 Ethernet device
	@details	v0.2
	# Description #
		This is an ethernet device that includes buffers for transmit/receive as well as includes
		the entire Ethernet stack.  It will allow 8 sockets to be used for Ethernet communication
	
	# Usage #
		You must establish a Generic Interface to GPIO and a SPI bus.  Then create an instance
		of this driver connected to those interfaces.
		
		To prepare the device for use you must set the IP address, default gateway, sugnet mask
		and mac address.  At this point you should be able to ping the device.
		
		Socket Listen and Socket Connect functions will create sockets as client or server for 
		a specified protocol (TCP or UDP).  The Socket number can be used in Socket Status to
		see the condition of the socker.
		
		The part requires SPI Mode 0 or 3
	
	# File Info #
		File:	W5500Driver.c
		Author:	J. Beighel
		Date:	08-27-2020
*/

#ifndef __W5500DRIVER
	#define __W5500DRIVER

/***** Includes		*****/
	#include <stdint.h>
	#include <stdbool.h>

	#include "CommonUtils.h"
	#include "GPIOGeneralInterface.h"
	#include "SPIGeneralInterface.h"
	#include "NetworkGeneralInterface.h"

/***** Definitions	*****/
	#define W5500_TCPSERVCAPS		((eTCPServerCapabilities_t)(TCPServ_Bind | TCPServ_CloseHost | TCPServ_CloseClient | TCPServ_AcceptConn))

	/**	@brief		W5500 Version register always indicates version 0x04
		@ingroup	w5500driver
	*/
	#define W5500_VERSION			0x04
	
	/**	@breif		Number of sockets the ethernet device supports
		@ingroup	w5500driver
	*/
	#define W5500_NUMSOCKETS		8
	
	/**	@brief		The number of microseconds the chip select pin must remain high
		@details	Data sheet says the minimum time is 30 nanoSeconds
		@ingroup	w5500driver
	*/
	#define W5500_MICROSECCSHIGH	1
	
	/**	@brief		The port number to begin with when making outbound connections
		@ingroup	w5500driver
	*/
	#define W5500_HIGHPORTSTART		50000
	
	/**	@brief		All return codes for the W5500 driver 
		@ingroup	w5500driver
	*/
	typedef enum eW5500Return_t {
		W5500Warn_Unknown		= 1,	/**< An unknown but recoverable error happened during the operation */
		W5500_Success			= 0,	/**< The operation completed successfully */
		W5500Fail_Unknown		= -1,	/**< An unknown and unrecoverable error happened during the operation */
		W5500Fail_WrongPart		= -2,	/**< The peripheral did not self report as a W5500 */
		W5500Fail_VerifyRead	= -3,	/**< A readback verification of a write did not match */
		W5500Fail_Unsupported	= -4,	/**< A request for an unsupported protocol or socket type */
		W5500Fail_NoSockets		= -5,	/**< Unable to create a socket, all are in use */
		W5500Fail_InvalidSocket	= -6,	/**< A request was made for an invalid socket number */
		W5500Fail_TXFreeSpace	= -7	/**< Transmit data size was too large */
	} eW5500Return_t;
	
	/**	@brief		Control codes for area of memory in W5500 to access
		@ingroup	w5500driver
	*/
	typedef enum eW5500Control_t {
		W5500BSB_BSBFullMask	= 0xF8,
		
		W5500BSB_CommonRegister	= 0x00,	/**< Flag to indicate use of the common registers */
		
		W5500BSB_SocketMask		= 0xE0,
		W5500BSB_SocketLShift	= 5,	/**< Number of bits to left shift the socket number */
		W5500BSB_Socket0		= 0x00,	/**< Indicate socket 0 to be used */
		W5500BSB_Socket1		= 0x20,	/**< Indicate socket 1 to be used */
		W5500BSB_Socket2		= 0x40,	/**< Indicate socket 2 to be used */
		W5500BSB_Socket3		= 0x60,	/**< Indicate socket 3 to be used */
		W5500BSB_Socket4		= 0x80,	/**< Indicate socket 4 to be used */
		W5500BSB_Socket5		= 0xA0,	/**< Indicate socket 5 to be used */
		W5500BSB_Socket6		= 0xC0,	/**< Indicate socket 6 to be used */
		W5500BSB_Socket7		= 0xE0,	/**< Indicate socket 7 to be used */
		
		W5500BSB_RegBuffMask	= 0x18,
		W5500BSB_Register		= 0x08,	/**< Flag to indicate use of the socket registers */
		W5500BSB_TXBuffer		= 0x10,
		W5500BSB_RXBuffer		= 0x18,
		
		W5500BSB_ReadWriteMask	= 0x04,
		W5500BSB_Read			= 0x00,
		W5500BSB_Write			= 0x04,
		
		W5500BSB_OpModeMask		= 0x03,
		W5500BSB_OMVariableLen	= 0x00,
		W5500BSB_OM1ByteLen		= 0x01,
		W5500BSB_OM2ByteLen		= 0x02,
		W5500BSB_OM4ByteLen		= 0x03,
	} eW5500Control_t;
	
	/**	@brief		W5500 Common register addresses
		@ingroup	w5500driver
	*/
	typedef enum eW5500CmnReg_t {
		W5500CmnReg_Mode		= 0x0000,
		
		W5500CmnReg_GatewayAddr0= 0x0001,
		W5500CmnReg_GatewayAddr1= 0x0002,
		W5500CmnReg_GatewayAddr2= 0x0003,
		W5500CmnReg_GatewayAddr3= 0x0004,
		
		W5500CmnReg_SubnetMask0	= 0x0005,
		W5500CmnReg_SubnetMask1	= 0x0006,
		W5500CmnReg_SubnetMask2	= 0x0007,
		W5500CmnReg_SubnetMask3	= 0x0008,
		
		W5500CmnReg_SrcHWAddr0	= 0x0009,
		W5500CmnReg_SrcHWAddr1	= 0x000A,
		W5500CmnReg_SrcHWAddr2	= 0x000B,
		W5500CmnReg_SrcHWAddr3	= 0x000C,
		W5500CmnReg_SrcHWAddr4	= 0x000D,
		W5500CmnReg_SrcHWAddr5	= 0x000E,
		
		W5500CmnReg_SrcIPAddr0	= 0x000F,
		W5500CmnReg_SrcIPAddr1	= 0x0010,
		W5500CmnReg_SrcIPAddr2	= 0x0011,
		W5500CmnReg_SrcIPAddr3	= 0x0012,
		
		W5500CmnReg_IntLevel0	= 0x0013,
		W5500CmnReg_IntLevel1	= 0x0014,
		
		W5500CmnReg_Interrupt	= 0x0015,
		W5500CmnReg_IntMask		= 0x0016,
		
		W5500CmnReg_SockInt		= 0x0017,
		W5500CmnReg_SockIntMask	= 0x0018,
		
		W5500CmnReg_RetryTime0	= 0x0019,
		W5500CmnReg_RetryTime1	= 0x001A,
		
		W5500CmnReg_RetryCount	= 0x001B,
		
		W5500CmnReg_PPPLCPReqTmr= 0x001C,
		W5500CmnReg_PPPLCPMgcNum= 0x001D,
		
		W5500CmnReg_PPPLCPDestMac0= 0x001E,
		W5500CmnReg_PPPLCPDestMac1= 0x001F,
		W5500CmnReg_PPPLCPDestMac2= 0x0020,
		W5500CmnReg_PPPLCPDestMac3= 0x0021,
		W5500CmnReg_PPPLCPDestMac4= 0x0022,
		W5500CmnReg_PPPLCPDestMac5= 0x0023,
		
		W5500CmnReg_PPPSessID0	= 0x0024,
		W5500CmnReg_PPPSessID1	= 0x0025,
		
		W5500CmnReg_PPPMaxSegSz0= 0x0026,
		W5500CmnReg_PPPMaxSegSz1= 0x0027,
		
		W5500CmnReg_UnreachIPAddr0= 0x0028,
		W5500CmnReg_UnreachIPAddr1= 0x0029,
		W5500CmnReg_UnreachIPAddr2= 0x002A,
		W5500CmnReg_UnreachIPAddr3= 0x002B,
		
		W5500CmnReg_UnreachPort0= 0x002C,
		W5500CmnReg_UnreachPort1= 0x002D,
		
		W5500CmnReg_PhyConfig	= 0x002E,
		
		W5500CmnReg_Version		= 0x0039,
	} eW5500CmnReg_t;

	/**	@brief		W5500 Common register Mode bit values
		@ingroup	w5500driver
	*/
	typedef enum eW5500CmdMode_t {
		W5500CmdMode_Reset		= 0x80,	/**< Set to reset the device, will clear automatically */
		W5500CmdMode_WakeOnLAN	= 0x20,	
		W5500CmdMode_PingBlock	= 0x10,	/**< Set to disable ping responses, clear to allow */
		W5500CmdMode_PPPoE		= 0x08,
		W5500CmdMode_ForceARP	= 0x02,
	} eW5500CmdMode_t;
	
	/**	@brief		Socket register addresses
		@ingroup	w5500driver
	*/
	typedef enum eW5500SockReg_t {
		W5500SckReg_Mode		= 0x0000,
		W5500SckReg_Command		= 0x0001,
		W5500SckReg_Interrupt	= 0x0002,
		W5500SckReg_Status		= 0x0003,
		W5500SckReg_SourcePort0	= 0x0004,
		W5500SckReg_SourcePort1	= 0x0005,
		W5500SckReg_DestMACAddr0= 0x0006,
		W5500SckReg_DestMACAddr1= 0x0007,
		W5500SckReg_DestMACAddr2= 0x0008,
		W5500SckReg_DestMACAddr3= 0x0009,
		W5500SckReg_DestMACAddr4= 0x000a,
		W5500SckReg_DestMACAddr5= 0x000B,
		W5500SckReg_DestIPAddr0	= 0x000C,
		W5500SckReg_DestIPAddr1	= 0x000D,
		W5500SckReg_DestIPAddr2	= 0x000E,
		W5500SckReg_DestIPAddr3	= 0x000F,
		W5500SckReg_DestPort0	= 0x0010,
		W5500SckReg_DestPort1	= 0x0011,
		W5500SckReg_MaxSegSize0	= 0x0012,
		W5500SckReg_MaxSegSize1	= 0x0013,
		W5500SckReg_IPTOS		= 0x0015,
		W5500SckReg_IPTTL		= 0x0016,
		W5500SckReg_RXBuffSize	= 0x001E,
		W5500SckReg_TXBuffSize	= 0x001F,
		W5500SckReg_TXFreeSize0	= 0x0020,
		W5500SckReg_TXFreeSize1	= 0x0021,
		W5500SckReg_TXReadPtr0	= 0x0022,
		W5500SckReg_TXReadPtr1	= 0x0023,
		W5500SckReg_TXWritePtr0	= 0x0024,
		W5500SckReg_TXWritePtr1	= 0x0025,
		W5500SckReg_RXRecvSize0	= 0x0026,
		W5500SckReg_RXRecvSize1	= 0x0027,
		W5500SckReg_RXReadPtr0	= 0x0028,
		W5500SckReg_RXReadPtr1	= 0x0029,
		W5500SckReg_RXWritePtr0	= 0x002A,
		W5500SckReg_RXWritePtr1	= 0x002B,
		W5500SckReg_IntMask		= 0x002C,
		W5500SckReg_FragOffset0 = 0x002D,
		W5500SckReg_FragOffset1 = 0x002E,
		W5500SckReg_KeepAliveTmr= 0x002F,
	} eW5500SockReg_t;
	
	/**	@brief		Individual socket mode configuration register
		@ingroup	w5500driver
	*/
	typedef enum eW5500SckMode_t{
		W5500SckMode_MultiMFEn	= 0x80,	/**< UDP Mode Set Enable/Clear Disable Multicastng, MAC Raw Set Enable/Clear Disabel MAC filtering */
		W5500SckMode_BCastBl	= 0x40,	/**< Set Enable/Clear Disable broadcast blcoking in MAC Raw mode */
		W5500SckMode_NDMCMMB	= 0x20,	/**< TCP Set Enable/Clear Disable No Delayed ACK, UDP Set use IGMP ver2/Clear use IGMP ver1, MAC Raw Set Enable/Clear Disable Multicast blocking */
		W5500SckMode_UCastBIP6B	= 0x10,	/**< UDP Set Enable/Clear Disable Unicast Blocking, MAC Raw set Enable/Clear Disable IPv6 Blocking */
		
		W5500SckMode_ProtMask	= 0x0F,	/**< Mask for all bits used to select protocol */
		W5500SckMode_ProtClosed = 0x00,	/**< Socket is closed */
		W5500SckMode_ProtTCP	= 0x01,	/**< Socket is intended for TCP */
		W5500SckMode_ProtUDP	= 0x02,	/**< Socket is intended for UDP */
		W5500SckMode_ProtMACRAW	= 0x04,	/**< Socket is intended for MAC Raw (Socket 0 only) */
	} eW5500SckMode_t;
	
	/**	@brief		Individual socket command register values
		@ingroup	w5500driver
	*/
	typedef enum eW5500SckCmd_t{
		W5500SckCmd_Open		= 0x01,
		W5500SckCmd_Listen		= 0x02,
		W5500SckCmd_Connect		= 0x04,
		W5500SckCmd_Disconnect	= 0x08,
		W5500SckCmd_Close		= 0x10,
		W5500SckCmd_Send		= 0x20,
		W5500SckCmd_SendMAC		= 0x21,
		W5500SckCmd_SendKeep	= 0x22,
		W5500SckCmd_Recv		= 0x40,
	} eW5500SckCmd_t;
	
	/**	@brief		Individual socket interrupt register values
		@ingroup	w5500driver
	*/
	typedef enum eW5500SckInt_t {
		W5500SckInt_SendOK		= 0x10,
		W5500SckInt_Timeout		= 0x08,
		W5500SckInt_Recv		= 0x04,
		W5500SckInt_Disconnect	= 0x02,
		W5500SckInt_Connect		= 0x01,
		
		W5500SckInt_AllMask		= 0x1F,	/**< Mask of all interrupt bits */
	} eW5500SckInt_t;
	
	/**	@brief		Individual socket status register values
		@ingroup	w5500driver
	*/
	typedef enum eW5500SckStat_t {
		W5500SckStat_Closed		= 0x00,	/**< Socket is closed */
		W5500SckStat_Init		= 0x13,	/**< Socket has been initialized, but is not is use */
		W5500SckStat_Listen		= 0x14,	/**< Socket is listening for incoming connections (TCP only) */
		W5500SckStat_Establish	= 0x17,	/**< Socket is connected to a remote device */
		W5500SckStat_CloseWait	= 0x1C,	/**< Socket is closing, will switch to Closed when done */
		W5500SckStat_UDP		= 0x22,	/**< Socket is listening for incoming data (UDP only) */
		W5500SckStat_MACRaw		= 0x42,
		W5500SckStat_SynSent	= 0x15,
		W5500SckStat_SynRecv	= 0x16,
		W5500SckStat_FinWait	= 0x18,
		W5500SckStat_Closing	= 0x1A,
		W5500SckStat_TimeWait	= 0x1B,
		W5500SckStat_LastAck	= 0x1D,
	} eW5500SckStat_t;
	
	/**	@brief		Phy configuration register values
		@ingroup	w5500driver
	*/
	typedef enum eW5500PhyCfg_t {
		W5500Phy_LinkStatus		= 0x01,	/**< Set if Link is up, clear if Down */
		W5500Phy_SpeedStatus	= 0x02,	/**< Set for 100 Mbps speed, clear for 10 Mbps */
		W5500Phy_DuplexStatus	= 0x04,	/**< Set for Full Duplex, clear for Half Duplex */
		W5500Phy_OpMode			= 0x40,	/**< Set to use Config Bits, Clear for hardware pins */
		W5500Phy_Reset			= 0x80,	/**< Set during operation, write clear to reset the phy */
		
		W5500Phy_CfgBitsMask	= 0x38,	/**< Mask of all bts used o configure Phy operation */
		W5500Phy_Cfg10HalfNoAuto= 0x00,	/**< Set Phy to 10Mbps, Half duplex, auto-negotiation disabled */
		W5500Phy_Cfg10FullNoAuto= 0x08,	/**< Set Phy to 10Mbps, Full duplex, auto-negotiation disabled */
		W5500Phy_Cfg100HalfNoAuto= 0x10,/**< Set Phy to 100Mbps, Half duplex, auto-negotiation disabled */
		W5500Phy_Cfg100FullNoAuto= 0x18,/**< Set Phy to 100Mbps, Full duplex, auto-negotiation disabled */
		W5500Phy_Cfg100HalfAuto	= 0x20,	/**< Set Phy to 100Mbps, Half duplex, auto-negotiation enabled */
		W5500Phy_CfgPowerDown	= 0x30,
		W5500Phy_CfgAllCapAuto	= 0x38,	/**< Set Phy to allow all capabilities and auto-negotiation enabled */
	} eW5500PhyCfg_t;
	
	/**	@brief		Individual socket transmit an recieve buffer size values
		@ingroup	w5500driver
	*/
	typedef enum eW5500SckBuffSz_t {
		W5500SckBuff_0KB		= 0,	/**< Set socket buffer to 0 bytes in size */
		W5500SckBuff_1KB		= 1,	/**< Set socket buffer to 1K bytes in size */
		W5500SckBuff_2KB		= 2,	/**< Set socket buffer to 2K bytes in size */
		W5500SckBuff_4KB		= 4,	/**< Set socket buffer to 4K bytes in size */
		W5500SckBuff_8KB		= 8,	/**< Set socket buffer to 8K bytes in size */
		W5500SckBuff_16KB		= 16,	/**< Set socket buffer to 16K bytes in size */
	} eW5500SckBuffSz_t;
	
	typedef enum eW5500SckFamily_t {
		AF_INET,
	} eW5500SckFamily_t;
	
	typedef enum eW5500SckProt_t {
		IPPROTO_TCP,		/**< TCP protocol, stream type */
		IPPROTO_UDP,		/**< UDP protocol, datagram type */
		IPPROTO_INVALID,	/**< Unknown or invalid protocol */
	} eW5500SckProt_t;
	
	/**	@brief		Object representing the W5500 device
		@ingroup	w5500driver
	*/
	typedef struct sW5500Obj_t {
		sGPIOIface_t *pGPIO;		/**< Pointer to GPIO module for Chip Select and Interrupt */
		uint8_t nChipSelectPin;		/**< Chip select pin in the GPIO module */
		sSPIIface_t *pSPI;			/**< Pointer to SPI device for bus communication */
		uint16_t nNextPort;			/**< Next port to use for outbound communications */
	} sW5500Obj_t;
	
	/**	@breif		Standard BSD sockets stucture for holding addresses
		@details	Netowork order is Big Endian, byte 1 is most significat byte
		@ingroup	w5500driver
	*/
	typedef struct in_addr {
		/**	Union to convert address bytes to words */
		union {
			/** Individual address bytes, suggested use as it avoid endinness issues */
			struct {
				uint8_t s_b1; /**< Most significant byte */
				uint8_t s_b2,s_b3;
				uint8_t s_b4; /**< Least significant byte */
			} S_un_b;
			
			struct {
				uint16_t s_w1,s_w2;
			} S_un_w;
    
			/** Address as a single integer value */
			uint32_t S_addr;
		} S_un;
	} IN_ADDR;
	
	/**	@brief		Standard BSD sockets structure for address and port descriptions
		@ingroup	w5500driver
	*/
	typedef struct sockaddr_in {
		uint8_t sin_familiy;	/**< Always AF_INET */
		uint8_t sin_port;		/**< Service port */
		IN_ADDR sin_addr;		/**< IP address */
		uint8_t sin_zero[8];	/**< Unused reserved space */
	} SOCKADDR_IN;
	
	
/***** Constants	*****/


/***** Globals		*****/


/***** Prototypes 	*****/
	/**	@brief		Initializes the Wiznet W5500 object
		@details	Prepares the object for use, will alsoe reach out to the peripheral to
			restart it and apply some basic configuration.
		@param		pDev		Pointer to the object structure to prepare
		@param		pSpiBus		Pointer to the SPI bus interface for this object to use
		@param		pIOObj		Pointer to the GPIO interface for this object to use
		@param		nCSPin		Identifier for the GPIO pin to use as chip select
		@return		W5500_Success on successful completion, or a code indicating the failure
		@ingroup	w5500driver
	*/
	eW5500Return_t W5500Initialize(sW5500Obj_t *pDev, sSPIIface_t *pSpiBus, sGPIOIface_t *pIOObj, uint8_t nCSPin);
	
	/**	@brief		Discovers the peripheral on the bus
		@details	Does not configure the device, only queries it to determine the correct part
			is connected to the bus.
		@param		pDev		Pointer to the device object
		@return		W5500_Success on successful completion, or a code indicating the failure
		@ingroup	w5500driver
	*/
	eW5500Return_t W5500VerifyPart(sW5500Obj_t *pDev);
	
	/**	@brief		Reports if the hardwre link is up and available or not
		@param		pDev		Pointer to the W5500 driver object that owns this socket
		@param		pbIsUp		Returned value, true if the link is Up, false if the link is Down
		@return		W5500_Success if the port is correctly bind, an error or warning code will
			be provided to indicate the failure
		@ingroup	w5500driver
	*/
	eW5500Return_t W5500LinkStatus(sW5500Obj_t *pDev, bool *pbIsUp);
	
	/**	@brief		Set the MAC address for the W5500 to use
		@param		pDev		Pointer to the W5500 driver object to configure
		@param		aMac		6 byte array containing the max address to use
		@return		W5500_Success if the port is correctly bind, an error or warning code will
			be provided to indicate the failure
		@ingroup	w5500driver
	*/
	eW5500Return_t W5500SetMACAddress(sW5500Obj_t *pDev, const uint8_t aMac[6]);
	
	/**	@brief		Set the IP address for the W5500 to use
		@param		pDev		Pointer to the W5500 driver object to configure
		@param		Address		IP address to set in the device
		@return		W5500_Success if the port is correctly bind, an error or warning code will
			be provided to indicate the failure
		@ingroup	w5500driver
	*/
	eW5500Return_t W5500SetIPAddress(sW5500Obj_t *pDev, const IN_ADDR *Address);
	
	/**	@brief		Set the subnet mask for the W5500 to use
		@param		pDev		Pointer to the W5500 driver object to configure
		@param		Address		Subnet mask to set in the device
		@return		W5500_Success if the port is correctly bind, an error or warning code will
			be provided to indicate the failure
		@ingroup	w5500driver
	*/
	eW5500Return_t W5500SetSubnetMask(sW5500Obj_t *pDev, const IN_ADDR *Address);
	
	/**	@brief		Set the default gateway address for the W5500 to use
		@param		pDev		Pointer to the W5500 driver object to configure
		@param		Address		Default gateway address to set in the device
		@return		W5500_Success if the port is correctly bind, an error or warning code will
			be provided to indicate the failure
		@ingroup	w5500driver
	*/
	eW5500Return_t W5500SetDefaultGateway(sW5500Obj_t *pDev, const IN_ADDR *Address);
	
	/**	@brief		Reads the IP address in use by the W5500
		@param		pDev		Pointer to the W5500 driver object to read from
		@param		Address		Returns the IP address configured into the device
		@return		W5500_Success if the port is correctly bind, an error or warning code will
			be provided to indicate the failure
		@ingroup	w5500driver
	*/
	eW5500Return_t W5500ReadIPAddress(sW5500Obj_t *pDev, IN_ADDR *Address);
		
	/**	@brief		Sets a socket to listen for incoming connections
		@details	For TCP sockets it will listen for connection requests.  The first request 
			will be accepted then the socket will stop listening for more connections and be
			dedicated to that connection.  The status will need to be checked to determine when 
			this connection request came in.
			For UDP sockets there is no connection, so it will begin acceptign data on that port.
		@param		pDev		Pointer to the W5500 driver object that owns this socket
		@param		pnSocket	Returns socket that is listening on the requested port
		@param		nPort		The local port the socket should listen on
		@return		W5500_Success if the port is correctly bind, an error or warning code will
			be provided to indicate the failure
		@ingroup	w5500driver
	*/
	eW5500Return_t W5500SocketListen(sW5500Obj_t *pDev, uint8_t *pnSocket, uint16_t nPort, eW5500SckProt_t eProtocol);

	/**	@brief		Attempt a connection to a TCP Server
		@details	Used for TCP connection only.  
		@param		pDev		Pointer to the W5500 driver object that owns this socket
		@param		pnSocket	Returns socket that is connected to the requested server
		@param		Address		IP Address of the server to connect to
		@param		nPort		Port number on the server to connect to
		@return		W5500_Success if the port is correctly bind, an error or warning code will
			be provided to indicate the failure
		@ingroup	w5500driver
	*/
	eW5500Return_t W5500SocketConnect(sW5500Obj_t *pDev, uint8_t *pnSocket, IN_ADDR *Address, uint16_t nPort);

	/**	@brief		Retrieve information regarding a created socket
		@details	The state of socket is the main indicator or connection status.  Outside of
			interrupts from the device this is the only way to know if a connection attempt 
			succeeded or if a connection has been closed down.
		@param		pDev			Pointer to the W5500 driver object that owns this socket
		@param		nSocket			Identifier of the socket to retrieve the status from
		@param		eProtocol		Returns the protocol this socket is using
		@param		eCurrState		Returns the current state of this socket
		@param		nBytesWaiting	Number of bytes of data waiting to be reqad from this socket
		@param		pConnAddr		Will receive the IP address this socket is connected to
		@param		nConnAddrLen	Size of the pConnAddr object
		@return		W5500_Success if the port is correctly bind, an error or warning code will
			be provided to indicate the failure
		@ingroup	w5500driver
	*/
	eW5500Return_t W5500SocketStatus(sW5500Obj_t *pDev, uint8_t nSocket, eW5500SckProt_t *eProtocol, eW5500SckStat_t *eCurrState, uint16_t *nBytesWaiting, SOCKADDR_IN *pConnAddr, uint8_t nConnAddrLen);
	
	/**	@brief		Reads in any data waiting for a specific socket
		@details	The W5500 does not provide information on the source of the data.  Will read
			until either the buffer is filled or all available data has been read.
		@param		pDev		Pointer to the W5500 driver object that owns this socket
		@param		nSocket		The iedintifier of the socket to read from
		@param		pBuff		Pointer to the buffer that will receive the data
		@param		nBuffSize	The maximum number of bytes the buffer can holding
		@param		pnBytesRead	Used to return the number of bytes that were actually read
		@return		W5500_Success if the port is correctly bind, an error or warning code will
			be provided to indicate the failure
		@ingroup	w5500driver
	*/
	eW5500Return_t W5500SocketReceive(sW5500Obj_t *pDev, uint8_t nSocket, uint8_t *pBuff, uint16_t nBuffSize, uint16_t *pnBytesRead);
	
	eW5500Return_t W5500SocketTCPSend(sW5500Obj_t *pDev, uint8_t nSocket, uint8_t *pBuff, uint16_t nBuffSize);

	eW5500Return_t W5500SocketUDPSend(sW5500Obj_t *pDev, uint8_t nSocket, IN_ADDR *pAddr, uint16_t nPort, uint8_t *pBuff, uint16_t nBuffSize);
	
	/**	@brief		Closes down a client socket
		@param		pTCPServ		Pointer to the Wiznet5500 TCP server interface object
		@param		nSocket			Socket to close
		@return		Net_Success on success, or a code indicating the failure
		@ingroup	w5500driver
	*/
	eW5500Return_t W5500CloseSocket(sW5500Obj_t *pDev, uint8_t nSocket);
	
	/**	@brief		Create a TCP Server interface object through the Wiznet 5500
		@details	Establishes an implementation of the TCP Server Network Interface
			using the Ethernet connection provided by the Wiznet 5500 peripheral.
		@param		pDev		Pointer to the Wiznet 5500 device object
		@param		pTCPServ	TCP Server interface structure to put the implementation in
		@return		W5500_Success if the port is interface is created, an error or warning code
			will be provided to indicate the failure
		@ingroup	w5500driver
	*/
	eW5500Return_t W5500CreateTCPServer(sW5500Obj_t *pDev, sTCPServ_t *pTCPServ);
	
/***** Functions	*****/

#endif

