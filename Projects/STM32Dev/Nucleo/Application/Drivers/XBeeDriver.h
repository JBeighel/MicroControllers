/**	@defgroup	xbeedriver
	@brief		Driver for the XBee 3 radio module in API mode
	@details	v 0.11
	# Description #

	# Usage #

	# File Info #
		File:	XBeeDriver.c
		Author:	J. Beighel
		Date:	2021-08-28
*/

#ifndef __XBEEDRIVER
	#define __XBEEDRIVER

/***** Includes		*****/
	#include "string.h"

	#include "CommonUtils.h"

	#include "GPIOGeneralInterface.h"
	#include "UARTGeneralInterface.h"
	#include "TimeGeneralInterface.h"

/**	@brief		Minimum frame length
	@details	This value is calculated as follows:
	Start Byte + 2 Length bytes + checksum byte
	Any other data bytes must be added to this amount when creatign a message
	@ingorup	xbeedriver
*/
#define XBEE_MSGBASELENGTH		4

/**	@brief		Minimum number of bytes that the message buffer can container
	@details	This is the number of bytes that are required by the largest supported XBee 
		API frame plus the bytes needed in the generic message syntax.
		
		Generic message bytes:
		1 Start byte, 2 length bytes, 1 CRC byte
		Total 4 bytes
		
		Remote AT Command Request frame (largest supported):
		1 frame type byte, 1 frame ID byte, 8 destination address, 2 network address,
		1 remote command options, 2 AT command, 8 parameter bytes
		Total 23 bytes
	@ingorup xbeedriver
*/
#define XBEE_FRAMINGBYTES		(XBEE_MSGBASELENGTH + 23)

#ifndef XBEE_DATABYTES
	#define XBEE_DATABYTES	512
#endif

#define XBEE_RECVBUFFERSIZE			(XBEE_DATABYTES + XBEE_FRAMINGBYTES)

#define XBEE_STARTBYTE			0x7E

#define XBEE_TIMEOUTLOOPS		20

#define XBEE_FRAMEIDNORESPONSE	0x00

#define XBEE_ADDRESSBROADCAST	0x0000000000000000000000000000FFFF

#define XBEE_NETADDRUNKNOWN		0xFFFE

#define XBEE_NODEIDLEN			10

/***** Definitions	*****/
typedef enum eXBeeReturn_t {
	XBWarn_NodeIDLen= 3,
	XBWarn_NoMessage= 2,
	XBWarn_NoData	= 2,
	XBWarn_Unknown	= 1,
	XB_Success		= 0,
	XBFail_Unknown	= -1,
	XBFail_Checksum	= -2,
	XBFail_MsgSize	= -3,
} eXBeeReturn_t;

typedef enum eXBeeFrameIndexes_t {
	XBIdx_Start				= 0,
	XBIdx_LengthMSB			= 1,
	XBIdx_LengthLSB			= 2,
	
	XBIdx_DataStart			= 3,	/**< Beginning of the frame data, this data is specific to the frame type */
	
	XBIdx_FrameType			= 3,
	XBIdx_FrameID			= 4,
	
	XBIdx_ATCmd				= 5,
	XBIdx_ATSetParam		= 7,
	XBIdx_ATCmdStat			= 7,
	XBIdx_ATReadParam		= 8,
	
	XBIdx_TXDestAddrMSB		= 5,
	XBIdx_TXNetAddrMSB		= 13,
	XBIdx_TXBCastRad		= 15,
	XBIdx_TXOpt				= 16,
	XBIdx_TXUserData		= 17,
	
	XBIdx_StatusNetAddrMSB	= 5,
	XBIdx_TXRetryCnt		= 7,
	XBIdx_DelStatus			= 8,
	XBIdx_Discovery			= 9,
	
	XBIdx_RXSrcAddrMSB		= 4,
	XBIdx_RXNetAddrMSB		= 12,
	XBIdx_RXOpt				= 14,
	XBIdx_RXUserData		= 15,
} eXBeeFrameIndexes_t;

typedef enum eXBeeFrameTypes_t {
	XBFrame_Unknown			= 0xFF,
	XBFrame_ATCommand		= 0x08,
	XBFrame_ATQueueParam	= 0x09,
	XBFrame_TXRequest		= 0x10,
	XBFrame_ExplicitAddrCmd	= 0x11,
	XBFrame_RemoteATCmdReq	= 0x17,
	XBFrame_ATCmdResponse	= 0x88,
	XBFrame_ModemStatus		= 0x8A,
	XBFrame_TransmitStatus	= 0x8B,
	XBFrame_RecvPacket		= 0x90,
	XBFrame_NodeID			= 0x95,
} eXBeeFrameTypes_t;

typedef enum xBeeATStatus_t {
	XBAT_OK					= 0x00,
	XBAT_Error				= 0x01,
	XBAT_InvalidCommand		= 0x02,
	XBAT_InvalidParam		= 0x03,
	XBAT_TxFailure			= 0x04,
} xBeeATStatus_t;

typedef enum eXBeeTransOptions_t {
	XBTXOpt_None 			= 0x00,
	XBTXOpt_DisableRetries	= 0x01,
	XBTXOpt_EnableEncrypt	= 0x20,
	XBTXOpt_ExtendTimeout	= 0x40,
} eXBeeTransOptions_t;

typedef enum eXBeeTXStatus_t {
	XBTXStat_Success		= 0x00,
	XBTXStat_MACACKFail		= 0x01,
	XBTXStat_CCAFail		= 0x02,
	XBTXStat_InvalidDest	= 0x15,
	XBTXStat_NetACKFail		= 0x21,
	XBTXStat_NoNetwork		= 0x22,
	XBTXStat_SelfAddr		= 0x23,
	XBTXStat_AddrNotFound	= 0x24,
	XBTXStat_RouteNotFound	= 0x25,
	XBTXStat_BCastRelayFail	= 0x26,
	XBTXStat_InvalidBindIdx	= 0x2B,
	XBTXStat_ResourceFail1	= 0x2C,
	XBTXStat_BCastWithAPS	= 0x2D,
	XBTXStat_UnicaseWithAPS	= 0x2E,
	XBTXStat_ResourceFail2	= 0x32,
	XBTXStat_DataTooLarge	= 0x74,
	XBTXStat_IndirectMessage= 0x75,
} eXBeeTXStatus_t;

typedef enum eXBeeDiscovStatus_t {
	XBDisc_None				= 0x00,
	XBDisc_Address			= 0x01,
	XBDisc_Route			= 0x02,
	XBDisc_AddrRoute		= 0x03,
	XBDisc_ExtTimeout		= 0x40,
} eXBeeDiscovStatus_t;

typedef enum eXBeeRecvOptions_t {
	XBRXOpt_Acked			= 0x01,
	XBRXOpt_Broadcast		= 0x02,
	XBRXOpt_APSEncrypt		= 0x20,
	XBRXOpt_FromEndDevice	= 0x40,
} eXBeeRecvOptions_t;

typedef enum eXBeeDeviceType_t {
	XBDevType_Coordinator	= 0x00,
	XBDevType_Router		= 0x01,
	XBDevType_EndDevice		= 0x02,
} eXBeeDeviceType_t;

typedef struct sXBeeObject_t {
	uint8_t nDataReadyPin;
	uint8_t nResetPin;
	uint8_t nFrameID;
	uint8_t anDataBuffer[XBEE_RECVBUFFERSIZE];
	
	sGPIOIface_t *pGPIO;
	sUARTIface_t *pUART;
	sTimeIface_t *pTime;
} sXBeeObject_t;

typedef struct sXBeeFrameATCmd_t {
	char aCmd[2];
	uint8_t nParamLen;
	uint8_t aParam[XBEE_DATABYTES];
} sXBeeFrameATCmd_t;

typedef struct sXBeeFrameATCmdResp_t {
	char aCmd[2];
	uint8_t nParamLen;
	uint8_t aParam[XBEE_DATABYTES];
	xBeeATStatus_t eStatus;
} sXBeeFrameATCmdResp_t;

typedef struct sXBeeFrameTXRequest_t {
	uint64_t nDestAddress;
	uint16_t nNetAddress;
	uint8_t nBradcastRadius;
	eXBeeTransOptions_t eOptions;
	uint16_t nDataLen;
	uint8_t aData[XBEE_DATABYTES];
} sXBeeFrameTXRequest_t;

typedef struct sXBeeFrameTXStatus_t {
	uint16_t nNetAddress;
	uint8_t nTXRetryCnt;
	eXBeeTXStatus_t eStatus;
	eXBeeDiscovStatus_t eDiscovery;
} sXBeeFrameTXStatus_t;

typedef struct sXBeeFrameRXPacket_t {
	uint64_t nSrcAddress;
	uint16_t nNetAddress;
	eXBeeRecvOptions_t eOptions;
	uint16_t nDataLen;
	uint8_t aData[XBEE_DATABYTES];
} sXBeeFrameRXPacket_t;

typedef union uXBeeFrameData_t {
	sXBeeFrameATCmd_t ATCmd;
	sXBeeFrameATCmdResp_t ATCmdResp;
	sXBeeFrameTXRequest_t TXreq;
	sXBeeFrameTXStatus_t TXStatus;
	sXBeeFrameRXPacket_t RXPacket;
	uint8_t aUnknown[XBEE_RECVBUFFERSIZE];
} uXBeeFrameData_t;

typedef struct sXBeeFrameRecv_t {
	uint16_t nDataLen;
	eXBeeFrameTypes_t eType;
	uint8_t nFrameID;
	uXBeeFrameData_t uData;
} sXBeeFrameRecv_t;

typedef struct sXBeeNetDiscovResult_t {
	uint16_t nNetAddr;
	uint64_t nSerial;
	uint8_t nNodeIDLen;
	uint8_t aNodeID[XBEE_NODEIDLEN];
	uint16_t nParentNetwork;
	eXBeeDeviceType_t eDeviceType;
	xBeeATStatus_t eStatus;
	uint8_t nProfileID;
	uint16_t nManufacturer;
} sXBeeNetDiscovResult_t;

/***** Constants	*****/


/***** Globals		*****/


/***** Prototypes 	*****/
eXBeeReturn_t XBeeInitialize(sXBeeObject_t *pXbeeObj, sUARTIface_t *pUART, sGPIOIface_t *pGPIO, sTimeIface_t *pTime, uint8_t nDTRPin, uint8_t nRstPin);

uint8_t XBeeAPIChecksum(uint8_t *paMsg);

eXBeeReturn_t XBeeReadMessage(sXBeeObject_t *pXbeeObj);

eXBeeReturn_t XBeeParseMessage(sXBeeObject_t *pXbeeObj, sXBeeFrameRecv_t *pFrameRecv);

eXBeeReturn_t XBeeParseNetworkDiscoveryData(const uint8_t *pATCmdData, sXBeeNetDiscovResult_t *pNetworkDevice);

eXBeeReturn_t XBeeATQueryCommand(sXBeeObject_t *pXbeeObj, const char ATCmd[2], uint8_t *pnFrameID);

eXBeeReturn_t XBeeATSetCommand(sXBeeObject_t *pXbeeObj, const char ATCmd[2], uint32_t nValue, uint8_t *pnFrameID);

eXBeeReturn_t XBeeTXReqest(sXBeeObject_t *pXbeeObj, uint64_t nDestAddr, uint16_t nNetAddr, uint16_t nDataLen, const void *pData, uint8_t *pnFrameID);

/***** Functions	*****/

#endif
