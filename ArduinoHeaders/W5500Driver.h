/**	@defgroup	w5500driver
	@brief		Driver for the Wizner W5500 Ethernet device
	@details	v0.1
		The device does not allow multiple socket types.  Once you pick TCP/UDP all of them get
		set that way
*/

#ifndef __W5500DRIVER
	#define __W5500DRIVER

/***** Includes		*****/
	#include "CommonUtils.h"
	#include "GPIOGeneralInterface.h"
	#include "SPIGeneralInterface.h"

/***** Definitions	*****/
	/**	@brief		W5500 Version register always indicates version 0x04
		@ingroup	w5500driver
	*/
	#define W5500_VERSION		0x04
	
	/**	@breif		Number of sockets the ethernet device supports
		@ingroup	w5500driver
	*/
	#define W5500_NUMSOCKETS	8
	
	/**	@brief		The number of microseconds the chip select pin must remain high
		@details	Data sheet says the minimum time is 30 nanoSeconds
		@ingroup	w5500driver
	*/
	#define W5500_MICROSECCSHIGH	1
	
	/** @brief		The port number to begin with when making outbound connections
		@ingroup	w5500driver
	*/
	#define W5500_HIGHPORTSTART		50000
	
	typedef enum eW5500Return_t {
		W5500Warn_Unknown		= 1,	/**< An unknown but recoverable error happened during the operation */
		W5500_Success			= 0,	/**< The operation completed successfully */
		W5500Fail_Unknown		= -1,	/**< An unknown and unrecoverable error happened during the operation */
		W5500Fail_WrongPart		= -2,	/**< The peripheral did not self report as a W5500 */
		W5500Fail_VerifyRead	= -3,	/**< A readback verification of a write did not match */
		W5500Fail_Unsupported	= -4,	/**< A request for an unsupported protocol or socket type */
		W5500Fail_NoSockets		= -5,	/**< Unable to create a socket, all are in use */
		W5500Fail_InvalidSocket	= -6,	/**< A request was made for an invalid socket number */
	} eW5500Return_t;
	
	typedef enum eW5500Control_t {
		W5500BSB_BSBFullMask	= 0xF8,
		
		W5500BSB_CommonRegister	= 0x00,	/**< Flag to indicate use of the common registers */
		
		W5500BSB_SocketMask		= 0xE0,
		W5500BSB_SocketLShift	= 5,	/**< Number of bits to left shift the socket number */
		W5500BSB_Socket0		= 0x00,
		W5500BSB_Socket1		= 0x20,
		W5500BSB_Socket2		= 0x40,
		W5500BSB_Socket3		= 0x60,
		W5500BSB_Socket4		= 0x80,
		W5500BSB_Socket5		= 0xA0,
		W5500BSB_Socket6		= 0xC0,
		W5500BSB_Socket7		= 0xE0,
		
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
	
	typedef enum eW5500CmdMode_t {
		W5500CmdMode_Reset		= 0x80,
		W5500CmdMode_WakeOnLAN	= 0x20,
		W5500CmdMode_PingBlock	= 0x10,
		W5500CmdMode_PPPoE		= 0x08,
		W5500CmdMode_ForceARP	= 0x02,
	} eW5500CmdMode_t;
	
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
	
	typedef enum eW5500SckInt_t {
		W5500SckInt_SendOK		= 0x10,
		W5500SckInt_Timeout		= 0x08,
		W5500SckInt_Recv		= 0x04,
		W5500SckInt_Disconnect	= 0x02,
		W5500SckInt_Connect		= 0x01,
		
		W5500SckInt_AllMask		= 0x1F,	/**< Mask of all interrupt bits */
	} eW5500SckInt_t;
	
	typedef enum eW5500SckStat_t {
		W5500SckStat_Closed		= 0x00,
		W5500SckStat_Init		= 0x13,
		W5500SckStat_Listen		= 0x14,
		W5500SckStat_Establish	= 0x17,
		W5500SckStat_CloseWait	= 0x1C,
		W5500SckStat_UDP		= 0x22,
		W5500SckStat_MACRaw		= 0x42,
		W5500SckStat_SynSent	= 0x15,
		W5500SckStat_SynRecv	= 0x16,
		W5500SckStat_FinWait	= 0x18,
		W5500SckStat_Closing	= 0x1A,
		W5500SckStat_TimeWait	= 0x1B,
		W5500SckStat_LastAck	= 0x1D,
	} eW5500SckStat_t;
	
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
	
	typedef enum eW5500SckBuffSz_t {
		W5500SckBuff_0KB		= 0,	/**< Set socket buffer to 0 bytes in size */
		W5500SckBuff_1KB		= 1,	/**< Set socket buffer to 1K bytes in size */
		W5500SckBuff_2KB		= 2,	/**< Set socket buffer to 2K bytes in size */
		W5500SckBuff_4KB		= 4,	/**< Set socket buffer to 4K bytes in size */
		W5500SckBuff_8KB		= 8,	/**< Set socket buffer to 8K bytes in size */
		W5500SckBuff_16KB		= 16,	/**< Set socket buffer to 16K bytes in size */
	} eW5500SckBuffSz_t;
	
	enum eW5500SckFamily_t {
		AF_INET,
	};
	
	enum eW5500SckProt_t {
		IPPROTO_TCP,		/**< TCP protocol, stream type */
		IPPROTO_UDP,		/**< UDP protocol, datagram type */
		IPPROTO_INVALID,	/**< Unknown or invalid protocol */
	};
	
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
		in_addr sin_addr;		/**< IP address */
		uint8_t sin_zero[8];	/**< Unused reserved space */
	} SOCKADDR_IN;
	
	
/***** Constants	*****/


/***** Globals		*****/


/***** Prototypes 	*****/
	eW5500Return_t W5500Initialize(sW5500Obj_t *pDev, sSPIIface_t *pSpiBus, sGPIOIface_t *pIOObj, uint8_t nCSPin);
	eW5500Return_t W5500VerifyPart(sW5500Obj_t *pDev);
	
	/**	@brief		Reports if the hardwre link is up and available or not
		@param		pDev		Pointer to the W5500 driver object that owns this socket
		@param		pbIsUp		Returned value, true if the link is Up, false if the link is Down
		@return		W5500_Success if the port is correctly bind, an error or warning code will
			be provided to indicate the failure
		@ingroup	w5500driver
	*/
	eW5500Return_t W5500LinkStatus(sW5500Obj_t *pDev, bool *pbIsUp);
	
	eW5500Return_t W5500SetMACAddress(sW5500Obj_t *pDev, const uint8_t aMac[6]);
	eW5500Return_t W5500SetIPAddress(sW5500Obj_t *pDev, const in_addr *Address);
	eW5500Return_t W5500SetSubnetMask(sW5500Obj_t *pDev, const in_addr *Address);
	eW5500Return_t W5500SetDefaultGateway(sW5500Obj_t *pDev, const in_addr *Address);
	
	eW5500Return_t W5500ReadIPAddress(sW5500Obj_t *pDev, in_addr *Address);
		
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
	eW5500Return_t W5500SocketConnect(sW5500Obj_t *pDev, uint8_t *pnSocket, in_addr *Address, uint16_t nPort);

	//Detect accepted comms or see if data is waiting, connaddr is remote connection (tcp only)
	eW5500Return_t W5500SocketStatus(sW5500Obj_t *pDev, uint8_t nSocket, eW5500SckStat_t *eCurrState, uint16_t *nBytesWaiting, sockaddr_in *pConnAddr, uint8_t nConnAddrLen);
	
	//Accept incoming data, addr is source of data
	eW5500Return_t W5500SocketReceiveFrom(sW5500Obj_t *pDev, uint8_t nSocket, sockaddr_in *pAddr, uint16_t nAddrLen, uint8_t pBuff, uint16_t nBuffSize);
	
	//TCP only
	eW5500Return_t W5500SocketSend(sW5500Obj_t *pDev, uint8_t nSocket, uint8_t pBuff, uint16_t nBuffSize);

	//UDP only, addr is destination of data
	eW5500Return_t W5500SocketSendTo(sW5500Obj_t *pDev, uint8_t nSocket, sockaddr_in *pAddr, uint16_t nAddrLen, uint8_t pBuff, uint16_t nBuffSize);
	
	eW5500Return_t W5500CloseSocket(sW5500Obj_t *pDev, uint8_t nSocket);
	
	eW5500Return_t W5500ReadData(sW5500Obj_t *pDev, uint16_t nAddress, eW5500Control_t eControl, uint8_t *pBuff, uint8_t nBytes);
	eW5500Return_t W5500WriteData(sW5500Obj_t *pDev, uint16_t nAddress, eW5500Control_t eControl, const uint8_t *pBuff, uint8_t nBytes);

/***** Functions	*****/
eW5500Return_t W5500Initialize(sW5500Obj_t *pDev, sSPIIface_t *pSpiBus, sGPIOIface_t *pIOObj, uint8_t nCSPin) {
	uint8_t nCtr, nModeReg, nCmd, nControl;
	
	//Set up module object
	pDev->pGPIO = pIOObj;
	pDev->pSPI = pSpiBus;
	pDev->nChipSelectPin = nCSPin;
	pDev->nNextPort = W5500_HIGHPORTSTART;
	
	//Prep the chip select pin
	pDev->pGPIO->pfSetModeByPin(pDev->pGPIO, pDev->nChipSelectPin, GPIO_DigitalOutput);
	pDev->pGPIO->pfDigitalWriteByPin(pDev->pGPIO, pDev->nChipSelectPin, true);
	
	//Reset the device
	nModeReg = W5500CmdMode_Reset;
	W5500WriteData(pDev, W5500SckReg_Mode, W5500BSB_CommonRegister, &nModeReg, 1);
	DELAYMILLISEC(5);
	
	//Clear ping disable bit
	W5500ReadData(pDev, W5500SckReg_Mode, W5500BSB_CommonRegister, &nModeReg, 1);
	nModeReg &= ~W5500CmdMode_PingBlock; //Turn off ping blockign 
	W5500WriteData(pDev, W5500SckReg_Mode, W5500BSB_CommonRegister, &nModeReg, 1);
	
	//Set no protocol for all ports
	nModeReg = W5500SckMode_ProtClosed;
		
	nCmd = W5500SckCmd_Close;
	for (nCtr = 0; nCtr < W5500_NUMSOCKETS; nCtr++) {
		nControl = nCtr << W5500BSB_SocketLShift;
		nControl |= W5500BSB_Register;
		
		W5500WriteData(pDev, W5500SckReg_Mode, (eW5500Control_t)nControl, &nModeReg, 1);
		W5500WriteData(pDev, W5500SckReg_Command, (eW5500Control_t)nControl, &nCmd, 1);
		
		//Set all buffers to 2K, evenly spreads out available space
		nCmd = W5500SckBuff_2KB;
		W5500WriteData(pDev, W5500SckReg_RXBuffSize, (eW5500Control_t)nControl, &nCmd, 1);
		
		nCmd = W5500SckBuff_2KB;
		W5500WriteData(pDev, W5500SckReg_TXBuffSize, (eW5500Control_t)nControl, &nCmd, 1);
	}
	
	return W5500_Success;
}

eW5500Return_t W5500VerifyPart(sW5500Obj_t *pDev) {
	uint8_t nRecv;
	
	W5500ReadData(pDev, W5500CmnReg_Version, W5500BSB_CommonRegister, &nRecv, 1);
	
	if (nRecv == W5500_VERSION) {
		return W5500_Success;
	} else {
		return W5500Fail_WrongPart;
	}
}

eW5500Return_t W5500SetMACAddress(sW5500Obj_t *pDev, const uint8_t aMac[6]) {
	uint8_t aVerify[6];
	uint8_t nCtr;
	
	W5500WriteData(pDev, W5500CmnReg_SrcHWAddr0, W5500BSB_CommonRegister, aMac, 6);
	
	W5500ReadData(pDev, W5500CmnReg_SrcHWAddr0, W5500BSB_CommonRegister, aVerify, 6);
	
	for (nCtr = 0; nCtr < 6; nCtr++) {
		if (aVerify[nCtr] != aMac[nCtr]) {
			return W5500Fail_VerifyRead;
		}
	}
	
	return W5500_Success;
}

eW5500Return_t W5500SetIPAddress(sW5500Obj_t *pDev, const in_addr *Address) {
	in_addr Verify;
	
	W5500WriteData(pDev, W5500CmnReg_SrcIPAddr0, W5500BSB_CommonRegister, (uint8_t *)Address, 4);
	
	Verify.S_un.S_addr = 0;
	W5500ReadData(pDev, W5500CmnReg_SrcIPAddr0, W5500BSB_CommonRegister, (uint8_t *)&Verify, 4);
	
	if (Address->S_un.S_addr == Verify.S_un.S_addr) {
		return W5500_Success;
	} else {
		return W5500Fail_VerifyRead;
	}
}

eW5500Return_t W5500ReadIPAddress(sW5500Obj_t *pDev, in_addr *Address) {
	W5500ReadData(pDev, W5500CmnReg_SrcIPAddr0, W5500BSB_CommonRegister, (uint8_t *)Address, 4);

	return W5500_Success;
}

eW5500Return_t W5500SetSubnetMask(sW5500Obj_t *pDev, const in_addr *Address) {
	in_addr Verify;
	
	W5500WriteData(pDev, W5500CmnReg_SubnetMask0, W5500BSB_CommonRegister, (uint8_t *)Address, 4);
	
	W5500ReadData(pDev, W5500CmnReg_SubnetMask0, W5500BSB_CommonRegister, (uint8_t *)&Verify, 4);
	
	if (Address->S_un.S_addr == Verify.S_un.S_addr) {
		return W5500_Success;
	} else {
		return W5500Fail_VerifyRead;
	}
}

eW5500Return_t W5500SetDefaultGateway(sW5500Obj_t *pDev, const in_addr *Address) {
	in_addr Verify;
	
	W5500WriteData(pDev, W5500CmnReg_GatewayAddr0, W5500BSB_CommonRegister, (uint8_t *)Address, 4);
	
	W5500ReadData(pDev, W5500CmnReg_GatewayAddr0, W5500BSB_CommonRegister, (uint8_t *)&Verify, 4);
	
	if (Address->S_un.S_addr == Verify.S_un.S_addr) {
		return W5500_Success;
	} else {
		return W5500Fail_VerifyRead;
	}
}

eW5500Return_t W5500LinkStatus(sW5500Obj_t *pDev, bool *pbIsUp) {
	uint8_t nRegVal;
	
	W5500ReadData(pDev, W5500CmnReg_PhyConfig, W5500BSB_CommonRegister, &nRegVal, 1);
	
	if (CheckAllBitsInMask(nRegVal, W5500Phy_LinkStatus) == true) {
		*pbIsUp = true;
	} else {
		*pbIsUp = false;
	}
	
	return W5500_Success;
}

eW5500Return_t W5500SocketListen(sW5500Obj_t *pDev, uint8_t *pnSocket, uint16_t nPort, eW5500SckProt_t eProtocol) {
	uint8_t nCtr, nControl;
	uint8_t anRegVal[2];
	
	//Look for an unused socket
	for (nCtr = 0; nCtr < W5500_NUMSOCKETS; nCtr++) {
		nControl = nCtr << W5500BSB_SocketLShift;
		nControl |= W5500BSB_Register;
		W5500ReadData(pDev, W5500SckReg_Status, (eW5500Control_t)nControl, &anRegVal[0], 1);
		
		if (anRegVal[0] == W5500SckStat_Closed) {
			break;
		}
	}
	
	if (nCtr >= W5500_NUMSOCKETS) { //All sockets are in use
		return W5500Fail_NoSockets;
	} else {
		*pnSocket = nCtr;
	}
	
	//Found a socket to use, get it setup
	nControl = (*pnSocket) << W5500BSB_SocketLShift;
	nControl |= W5500BSB_Register;
	
	anRegVal[0] = W5500SckInt_AllMask; //Clear any pending interrupts
	W5500WriteData(pDev, W5500SckReg_Interrupt, (eW5500Control_t)nControl, &anRegVal[0], 1);
	
	anRegVal[0] = nPort >> 8;
	anRegVal[1] = nPort & 0xFF;
	W5500WriteData(pDev, W5500SckReg_SourcePort0, (eW5500Control_t)nControl, anRegVal, 2);
	
	switch (eProtocol) { //Set the socket to the requested protocol
		case IPPROTO_TCP:
			anRegVal[0] = W5500SckMode_ProtTCP;
			break;
			
		case IPPROTO_UDP:
			anRegVal[0] = W5500SckMode_ProtUDP;
			break;
			
		default:
			return W5500Fail_Unsupported;
	}
	
	W5500WriteData(pDev, W5500SckReg_Mode, (eW5500Control_t)nControl, &(anRegVal[0]), 1);
	
	anRegVal[0] = W5500SckCmd_Open; //Command it to open to establish the socket
	W5500WriteData(pDev, W5500SckReg_Command, (eW5500Control_t)nControl, &anRegVal[0], 1);
	
	//Should move the socket to INIT status
	anRegVal[0] = W5500SckStat_Closed;
	nCtr = 0;
	while ((anRegVal[0] == W5500SckStat_Closed) && (nCtr < 10)) {
		W5500ReadData(pDev, W5500SckReg_Status, (eW5500Control_t)nControl, &anRegVal[0], 1);
		DELAYMILLISEC(1);
		nCtr += 1;
	}
	
	if (eProtocol == IPPROTO_TCP) { //Must command TCP sockets to listen next
		anRegVal[0] = W5500SckCmd_Listen;
		W5500WriteData(pDev, W5500SckReg_Command, (eW5500Control_t)nControl, &anRegVal[0], 1);
	}
	
	if (pDev->nNextPort == nPort) { //Just in case don't clobber next outbound
		pDev->nNextPort += 1;
		if (pDev->nNextPort == 0) {
			pDev->nNextPort = W5500_HIGHPORTSTART;
		}
	}
	
	return W5500_Success;
}

eW5500Return_t W5500SocketConnect(sW5500Obj_t *pDev, uint8_t *pnSocket, in_addr *Address, uint16_t nPort) {
	uint8_t nControl = W5500BSB_Register;
	uint8_t anBytes[2];
	uint16_t nCtr;
	
	//Look for an unused socket
	for (nCtr = 0; nCtr < W5500_NUMSOCKETS; nCtr++) {
		nControl = nCtr << W5500BSB_SocketLShift;
		nControl |= W5500BSB_Register;
		W5500ReadData(pDev, W5500SckReg_Status, (eW5500Control_t)nControl, &anBytes[0], 1);
		
		if (anBytes[0] == W5500SckStat_Closed) {
			break;
		}
	}
	
	if (nCtr >= W5500_NUMSOCKETS) { //All sockets are in use
		return W5500Fail_NoSockets;
	} else {
		*pnSocket = nCtr;
	}
	
	//Found a socket to use, get it setup
	nControl = (*pnSocket) << W5500BSB_SocketLShift;
	nControl |= W5500BSB_Register;
	
	W5500WriteData(pDev, W5500SckReg_DestIPAddr0, (eW5500Control_t)nControl, (uint8_t *)Address, 4);
	
	anBytes[0] = nPort >> 8;
	anBytes[1] = nPort & 0xFF;
	W5500WriteData(pDev, W5500SckReg_DestPort0, (eW5500Control_t)nControl, anBytes, 2);
	
	anBytes[0] = pDev->nNextPort >> 8;
	anBytes[1] = pDev->nNextPort& 0xFF;
	W5500WriteData(pDev, W5500SckReg_SourcePort0, (eW5500Control_t)nControl, anBytes, 2);
	
	//Open the port, then command it to connect
	anBytes[0] = W5500SckCmd_Open;
	W5500WriteData(pDev, W5500SckReg_Command, (eW5500Control_t)nControl, &anBytes[0], 1);
	
	anBytes[0] = W5500SckCmd_Connect;
	W5500WriteData(pDev, W5500SckReg_Command, (eW5500Control_t)nControl, &anBytes[0], 1);
	
	///@todo Verify that the connection was made and return that status. W5500SckStat_Establish on connect W5500SckStat_Closed on failure
	
	return W5500_Success;
}

eW5500Return_t W5500SocketStatus(sW5500Obj_t *pDev, uint8_t nSocket, eW5500SckProt_t *eProtocol, eW5500SckStat_t *eCurrState, uint16_t *nBytesWaiting, sockaddr_in *pConnAddr, uint8_t nConnAddrLen) {
	uint8_t nControl;
	uint8_t aBytes[4];
	uint16_t nVerify;
	
	if (nSocket >= W5500_NUMSOCKETS) {
		return W5500Fail_InvalidSocket;
	}
	
	//Read the status of the socket
	nControl = nSocket << W5500BSB_SocketLShift;
	nControl |= W5500BSB_Register;
	W5500ReadData(pDev, W5500SckReg_Status, (eW5500Control_t)nControl, (uint8_t *)eCurrState, 1);

	//Read the mode to get the protocol
	W5500ReadData(pDev, W5500SckReg_Mode, (eW5500Control_t)nControl, &(aBytes[0]), 1);
	aBytes[0] &= W5500SckMode_ProtMask; //Strip off anything that isn't the protocol flags
	Serial.print("Mode ");
	Serial.print(aBytes[0], HEX);
	Serial.print("\n");
	if (aBytes[0] == W5500SckMode_ProtTCP) {
		*eProtocol = IPPROTO_TCP;
	} else if (aBytes[0] == W5500SckMode_ProtUDP) {
		*eProtocol = IPPROTO_UDP;
	} else {
		*eProtocol = IPPROTO_INVALID;
	}
	
	//Look for bytes waiting in the socket
	W5500ReadData(pDev, W5500SckReg_RXRecvSize0, (eW5500Control_t)nControl, aBytes, 2);
	
	//Read connected socket infromation (TCP only)
	pConnAddr->sin_familiy = AF_INET; //Always AF_INET
	
	if (((*eCurrState) != W5500SckStat_Closed) && ((*eCurrState) != W5500SckStat_UDP) && ((*eCurrState) != W5500SckStat_MACRaw)) {
		//Read out the port
		W5500ReadData(pDev, W5500SckReg_DestPort0, (eW5500Control_t)nControl, aBytes, 2);
		
		pConnAddr->sin_port = aBytes[0] << 8;
		pConnAddr->sin_port |= aBytes[1];
			
		//Read the recv byte count. it can change mid-read, read until we get a matched reading
		nVerify = 0;
		W5500ReadData(pDev, W5500SckReg_RXRecvSize0, (eW5500Control_t)nControl, aBytes, 2);
		(*nBytesWaiting) = aBytes[0] << 8;
		(*nBytesWaiting) |= aBytes[1];
		while (nVerify != (*nBytesWaiting)) {
			nVerify = (*nBytesWaiting);
			
			W5500ReadData(pDev, W5500SckReg_RXRecvSize0, (eW5500Control_t)nControl, aBytes, 2);
			(*nBytesWaiting) = aBytes[0] << 8;
			(*nBytesWaiting) |= aBytes[1];
		}
		
		W5500ReadData(pDev, W5500SckReg_DestIPAddr0, (eW5500Control_t)nControl, aBytes, 4);
		pConnAddr->sin_addr.S_un.S_un_b.s_b1 = aBytes[0];
		pConnAddr->sin_addr.S_un.S_un_b.s_b2 = aBytes[1];
		pConnAddr->sin_addr.S_un.S_un_b.s_b3 = aBytes[2];
		pConnAddr->sin_addr.S_un.S_un_b.s_b4 = aBytes[3];
	} else { //Not a TCP socket, no connection to report
		pConnAddr->sin_familiy = AF_INET;
		pConnAddr->sin_port = 0;
		pConnAddr->sin_addr.S_un.S_addr = 0;
	}
	
	return W5500_Success;
}

eW5500Return_t W5500CloseSocket(sW5500Obj_t *pDev, uint8_t nSocket) {
	uint8_t nSetting, nControl;
	
	if (nSocket >= W5500_NUMSOCKETS) {
		return W5500Fail_InvalidSocket;
	}
	
	nControl = nSocket << W5500BSB_SocketLShift;
	
	nSetting = W5500SckCmd_Close; //Command it to close down any connections
	W5500WriteData(pDev, W5500SckReg_Command, (eW5500Control_t)nControl, &nSetting, 1);
	
	return W5500_Success;
}

eW5500Return_t W5500ReadData(sW5500Obj_t *pDev, uint16_t nAddress, eW5500Control_t eControl, uint8_t *pBuff, uint8_t nBytes) {
	sGPIOIface_t *pGPIO = pDev->pGPIO;
	sSPIIface_t *pSPI = pDev->pSPI;
	uint8_t nSend[3], nCtr;
	
	nSend[0] = nAddress >> 8;
	nSend[1] = nAddress & 0x00FF;
	nSend[2] = eControl & (~W5500BSB_OpModeMask); //Strip off data length bits
	nSend[2] &= ~W5500BSB_ReadWriteMask; //Strip off any read/write bits
	nSend[2] |= W5500BSB_Read; //Add correct read/write bit
	
	//Add the correct data length bits
	switch (nBytes) {
		case 1:
			nSend[2] |= W5500BSB_OM1ByteLen;
			break;
		case 2:
			nSend[2] |= W5500BSB_OM2ByteLen;
			break;
		case 4:
			nSend[2] |= W5500BSB_OM4ByteLen;
			break;
		default:
			nSend[2] |= W5500BSB_OMVariableLen;
			break;
	}
	
	//Begin SPI transaction
	pGPIO->pfDigitalWriteByPin(pGPIO, pDev->nChipSelectPin, false);
	pSPI->pfBeginTransfer(pSPI);
	
	//Send the address and control
	for (nCtr = 0; nCtr < 3; nCtr++) {
		pSPI->pfTransferByte(pSPI, nSend[nCtr], &(pBuff[0]));
	}
	
	//Read out the data
	for (nCtr = 0; nCtr < nBytes; nCtr++) {
		pSPI->pfTransferByte(pSPI, 0xFF, &(pBuff[nCtr]));
	}
	
	//End SPI transaction
	pSPI->pfEndTransfer(pSPI);
	pGPIO->pfDigitalWriteByPin(pGPIO, pDev->nChipSelectPin, true);
	
	return W5500_Success;
}

eW5500Return_t W5500WriteData(sW5500Obj_t *pDev, uint16_t nAddress, eW5500Control_t eControl, const uint8_t *pBuff, uint8_t nBytes) {
	sGPIOIface_t *pGPIO = pDev->pGPIO;
	sSPIIface_t *pSPI = pDev->pSPI;
	uint8_t nSend[3], nRecv, nCtr;
	
	nSend[0] = nAddress >> 8;
	nSend[1] = nAddress & 0x00FF;
	nSend[2] = eControl & (~W5500BSB_OpModeMask); //Strip off data length bits
	nSend[2] &= ~W5500BSB_ReadWriteMask; //Strip off any read/write bits
	nSend[2] |= W5500BSB_Write; //Add correct read/write bit
	
	//Add the correct data length bits
	switch (nBytes) {
		case 1:
			nSend[2] |= W5500BSB_OM1ByteLen;
			break;
		case 2:
			nSend[2] |= W5500BSB_OM2ByteLen;
			break;
		case 4:
			nSend[2] |= W5500BSB_OM4ByteLen;
			break;
		default:
			nSend[2] |= W5500BSB_OMVariableLen;
			break;
	}
	
	//Begin SPI transaction
	pGPIO->pfDigitalWriteByPin(pGPIO, pDev->nChipSelectPin, false);
	pSPI->pfBeginTransfer(pSPI);

	//Send the address and control
	for (nCtr = 0; nCtr < 3; nCtr++) {
		pSPI->pfTransferByte(pSPI, nSend[nCtr], &nRecv);
	}
	
	//Write out the data
	for (nCtr = 0; nCtr < nBytes; nCtr++) {
		pSPI->pfTransferByte(pSPI, pBuff[nCtr], &nRecv);
	}

	//End SPI transaction
	pSPI->pfEndTransfer(pSPI);
	pGPIO->pfDigitalWriteByPin(pGPIO, pDev->nChipSelectPin, true);
	DELAYMICROSEC(W5500_MICROSECCSHIGH);
	
	return W5500_Success;
}

#endif

