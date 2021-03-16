/**	@defgroup	dnp		DNP Protocol Library
	@brief		Handling of the DNP protocol
	@details	v0.1
	#Description
	
	#File Information
		File:	DNPBase.h
		Author:	J. Beighel
		Date:	2021-03-15
*/

#ifndef __DNPBASE_H
	#define __DNPBASE_H

/*****	Includes	*****/
	#include "CommonUtils.h"

/*****	Defines		*****/
	/**	@brief		Maximum number of data bytes a message fragment can contain
	 *	@details	This excludes any framing a CRC checks, just a count of payload
	 *		bytes that a message fragment can have
	 *	@ingroup	dnpmsgbuild
	 */
	#define DNP_MAXDATABYTES		255

	/**	@brief		Number of data bytes that fit in a single DNP message fragment
	 *	@details	Each DNP message can have 255 data bytes in them.  Every 16 data
	 *		bytes will get 2 CRC bytes.
	 *	@ingroup	dnpmsgbuild
	 */
	#define DNP_MSGFRAGMENTDATASIZE	287

	/**	@brief		Total number of bytes in each DNP message fragment
	 *	@details	Each fragment will have 10 bytes for the transport function.
	 *		Then 255 data bytes with 2 CRC bytes for every 16 data bytes.  This
	 *		means there will be 16 data chunks each with 2 CRC bytes.
	 *		In total there is 255 data bytes + 32 CRC bytes + 10 transport bytes
	 *	@ingroup	dnpmsgbuild
	 */
	#define DNP_MSGFRAGMENTTOTSIZE	(DNP_MSGFRAGMENTDATASIZE + 10)

	/**	@brief		Maximum number of message fragments that will be allowed
	 *	@details	The builder will be unable to build messages that exceed this
	 *		number of message fragments.
	 *	@ingroup	dnpmsgbuild
	 */
	#define	DNP_MAXFRAGMENTMAX		4

	/**	@breif		Maximum size that a full DNP message can be
	 *	@ingroup	dnpmsgbuild
	 */
	#define DNP_MESSAGESIZEMAX		(DNP_MSGFRAGMENTTOTSIZE * DNP_MAXFRAGMENTMAX)

	/**	@brief		Maximum number of user data bytes that a message can hold
	 *	@details	The first fragment will have 5 bytes of application and transport
	 *		headers.  All remaining fragments will only have 1 transport header byte
	 *	@ingroup	dnpmsgbuild
	 */
	#define DNP_USERDATAMAX			(250 + ((DNP_MAXFRAGMENTMAX - 1) * 254))

	/**	@brief		Every 16 data bytes will get a 2 byte CRC value
	 *	@ingroup	dnpmsgbuild
	 */
	#define DNP_DATACRCCHUNKSIZE	16

	#define DNP_MSGSTARTBYTES		0x0564

	#define DNP_MSGHEADERLEN		10

/*****	Definitions	*****/
	typedef enum eDNPHeaderIndexes_t {
		DNPHdrIdx_StartBytes	= 0,
		DNPHdrIdx_DataLength	= 2,
		DNPHdrIdx_DataControl	= 3,
		DNPHdrIdx_DestAddr		= 4,
		DNPHdrIdx_SourceAddr	= 6,
		DNPHdrIdx_CRC			= 8,
		DNPHdrIdx_TransportHdr	= 11,
		DNPHdrIdx_AppHdr		= 12,
	} eDNPHeaderIndexes_t;

	typedef enum eDNPAddresses_t {
		DNPAddr_AllStations		= 0xFFFF,
		DNPAddr_SelfAddress		= 0xFFFC,
	} eDNPAddresses_t;

	typedef enum eDNPControlCodes_t {
		DNPCtrl_Read			= 1,
		DNPCtrl_Write			= 2,
		DNPCtrl_Select			= 3,
		DNPCtrl_Operate			= 4,
		DNPCtrl_DirectOperate	= 5,
		DNPCtrl_ColdRestart		= 13,
		DNPCtrl_WarmRestart		= 14,
		DNPCtrl_FileOpen		= 25,
		DNPCtrl_FileClose		= 26,
		DNPCtrl_Response		= 129,
		DNPCtrl_Unsolicited		= 130,
	} eDNPControlCodes_t;

	typedef enum eDNPDataControl_t {
		DNPData_Direction		= 0x80,
		DNPData_Primary			= 0x40,
		DNPData_FrameCntBit		= 0x20,
		DNPData_FrameCntValid	= 0x10,
		DNPData_UnconfirmData	= 0x04,
	} eDNPDataControl_t;

	typedef enum eDNPTransportHeader_t {
		DNPTransHdr_FirstMsg	= 0x40,
		DNPTransHdr_LastMsg		= 0x80,
		DNPTransHdr_SequenceMask= 0x3F,
	} eDNPTransportHeader_t;

	typedef enum eDNPAppHeader_t {
		DNPAppHdr_FirstMsg		= 0x80,
		DNPAppHdr_LastMsg		= 0x40,
		DNPAppHdr_ConfirmExpect	= 0x20,
		DNPAppHdr_SequenceMask	= 0x1F,
	} eDNPAppHeader_t;

	typedef enum eDNPQualifier_t {
		DNPQual_IndexMask					= 0xF0,

		DNPQual_IndexPrefixNone				= 0x00,
		DNPQual_IndexPrefix1Bytes			= 0x10,
		DNPQual_IndexPrefix2Bytes			= 0x20,
		DNPQual_IndexPrefix4Bytes			= 0x30,

		DNPQual_IndexSize1Bytes				= 0x40,
		DNPQual_IndexSize2Bytes				= 0x50,
		DNPQual_IndexSize4Bytes				= 0x60,

		DNPQual_CodeMask					= 0x0F,

		DNPQual_CodeCountStopAndStart1Bytes	= 0x00,
		DNPQual_CodeCountStopAndStart2Bytes	= 0x01,
		DNPQual_CodeCountStopAndStart4Bytes	= 0x02,

		DNPQual_CodeAddrStopAndStart1Bytes	= 0x03,
		DNPQual_CodeAddrStopAndStart2Bytes	= 0x04,
		DNPQual_CodeAddrStopAndStart4Bytes	= 0x05,

		DNPQual_CodeNoRange					= 0x06,

		DNPQual_CodeSingleVal1Bytes			= 0x07,
		DNPQual_CodeSingleVal2Bytes			= 0x08,
		DNPQual_CodeSingleVal4Bytes			= 0x09,

		DNPQual_CodeFreeFormat				= 0x0B,
	} eDNPQualifier_t;

	typedef enum eDNPInternalIndicators_t {
		DNPIntInd_None					= 0x0000,
		DNPIntInd_AllStationRecv		= 0x0001,
		DNPIntInd_Class1Data			= 0x0002,
		DNPIntInd_Class2Data			= 0x0004,
		DNPIntInd_Class3Data			= 0x0008,
		DNPIntInd_TimeSyncRequired		= 0x0010,
		DNPIntInd_LocalState			= 0x0020,
		DNPIntInd_DeviceTrouble			= 0x0040,
		DNPIntInd_Restart				= 0x0080,
		DNPIntInd_NotImplemented		= 0x0100,
		DNPIntInd_ObjectUnknown			= 0x0200,
		DNPIntInd_OutOfRange			= 0x0400,
		DNPIntInd_BufferOverflow		= 0x0800,
		DNPIntInd_OperationExecuting	= 0x1000,
		DNPIntInd_CorruptConfig			= 0x2000,
	} eDNPInternalIndicators_t;

	typedef enum eDNPGroup_t {
		DNPGrp_Unknown					= 0x00,
		DNPGrp_DeviceAttrib				= 0x00,
		DNPGrp_BinaryInput				= 0x01,
		DNPGrp_BinaryInputEvent			= 0x02,
		DNPGrp_BinaryOutput				= 0x0A,
		DNPGrp_BinaryOutputCmd			= 0x0C,
		DNPGrp_Counter					= 0x14,
		DNPGrp_AnalogInput				= 0x1E,
		DNPGrp_FrozenAnalogInput		= 0x1F,
		DNPGrp_AnalogInputDeadband		= 0x22,
		DNPGrp_AnalogOutput				= 0x28,
		DNPGrp_AnalogOutputBlock		= 0x29,
		DNPGrp_TimeDelay				= 0x34,
		DNPGrp_ClassObjects				= 0x3C,
		DNPGrp_FileControl				= 0x46,
		DNPGrp_InternalIndications		= 0x50,
		DNPGrp_VirtualTerminalOut		= 0x70,
		DNPGrp_VirtualTerminalEvent		= 0x71
	} eDNPGroup_t;

	typedef enum eDNPBinOutControlCode_t {
		DNPBinOutCtrl_None				= 0x00,
		DNPBinOutCtrl_PulseOn			= 0x01,
		DNPBinOutCtrl_PulseOff			= 0x02,
		DNPBinOutCtrl_LatchOn			= 0x03,
		DNPBinOutCtrl_LatchOff			= 0x04,
		DNPBinOutCtrl_Close				= 0x40,
		DNPBinOutCtrl_Trip				= 0x80,
	} eDNPBinOutControlCode_t;

	/**	@brief		Structure to hold a DNP message during creation and parsing
	 *	@details	All fields should be treated as read only to ensure the module
	 *		operating on the message keeps the contents sane.
	 *	@ingroup	dnp
	 */
	typedef struct sDNPMsgBuffer_t {
		uint8_t aDNPMessage[DNP_MESSAGESIZEMAX];	/**< Buffer to hold the full DNP message */
		uint32_t nDNPMsgLen;						/**< Index of last byte of the DNP Message */
		uint8_t aUserData[DNP_USERDATAMAX];			/**< Buffer to hold all user data in the message */
		uint32_t nUserDataLen;						/**< Index of last byte of user data */
		uint32_t nUserDataIdx;						/**< Index of last processed byte in the user data */
		uint16_t nDestAddr;							/**< Destination address for this message */
		uint16_t nSourceAddr;						/**< Source address for this message */
		uint8_t nTransportSequence;					/**< Sequence number for the Transport layer */
		uint8_t nApplicationSequence;				/**< Sequence number for the application layer */
		eDNPControlCodes_t eControlCode;			/**< Control code used in this message */
		eDNPInternalIndicators_t eIntIndicators;	/**< Internal indicators to set in this message */
		eDNPDataControl_t eDataControl;				/**< Data Control code for this message */
	} sDNPMsgBuffer_t;

/*****	Constants	*****/

/*****	Globals		*****/


/*****	Prototypes 	*****/
	/**	@brief		Convert a 16 bit integer into 2 bytes an an array
	 *	@param		nValue		The integer value to convert
	 *	@param		bLSBFirst	True to put the low byte first in the array,
	 *		false to have the high byte first
	 *	@param		pBuffer		Pointer to the buffer to store the byte values
	 *	@param		nBuffOffset	Index in the array to put the first byte value
	 *	@ingroup	dnp
	 */
	eReturn_t UInt16ToBytes(uint16_t nValue, bool bLSBFirst, uint8_t *pBuffer, uint32_t nBuffOffset);

	/**	@brief		Convert a 32 bit integer into 4 bytes an an array
	 *	@param		nValue		The integer value to convert
	 *	@param		bLSBFirst	True to put the low byte first in the array,
	 *		false to have the high byte first
	 *	@param		pBuffer		Pointer to the buffer to store the byte values
	 *	@param		nBuffOffset	Index in the array to put the first byte value
	 *	@ingroup	dnp
	 */
	eReturn_t UInt32ToBytes(uint32_t nValue, bool bLSBFirst, uint8_t *pBuffer, uint32_t nBuffOffset);

	/**	@brief		Convert a byte array to a 16 bit integer
	 *	@ingroup	dnp
	 */
	uint16_t BytesToUInt16(uint8_t *pBuffer, bool bLSBFirst, uint32_t nBuffOffset);

	/**	@brief		Convert a byte array to a 32 bit integer
	 *	@ingroup	dnp
	 */
	uint32_t BytesToUInt32(uint8_t *pBuffer, bool bLSBFirst, uint32_t nBuffOffset, uint32_t nLength);

	eReturn_t DNPBufferNewMessage(sDNPMsgBuffer_t *pMsg);

/*****	Functions	*****/


#endif

