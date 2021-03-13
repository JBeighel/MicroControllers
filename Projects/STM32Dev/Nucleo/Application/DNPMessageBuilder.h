/**	@defgroup	dnpmsgbuild
	@ingroup	dnp
	@brief		Library for creating DNP protocol messages
	@details	v0.1
	#Description
	
	#File Information
		File:	DNPMessageBuilder.h
		Author:	J. Beighel
		Date:	2021-03-12
*/

#ifndef __DNPMESSAGEBUILDER_H
	#define __DNPMESSAGEBUILDER_H

/*****	Includes	*****/
	#include "DNPBase.h"
	#include "CRC16.h"

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

/*****	Definitions	*****/
	typedef struct sDNPMsgBuild_t {
		uint8_t aDNPMessage[DNP_MESSAGESIZEMAX];	/**< Buffer to hold the full DNP message ready to send */
		uint32_t nDNPMsgLen;						/**< Index of last byte of the DNP Message */
		uint8_t aUserData[DNP_USERDATAMAX];			/**< Buffer to hold all user data to include in the message */
		uint32_t nUserDataLen;						/**< Index of last byte of user data */
		uint16_t nDestAddr;							/**< Destination address for this message */
		uint16_t nSourceAddr;						/**< Source address for this message */
		uint8_t nTransportSequence;					/**< Sequence number for the Transport layer */
		uint8_t nApplicationSequence;				/**< Sequence number for the application layer */
		eDNPControlCodes_t eControlCode;			/**< Control code used in this message */
		eDNPInternalIndicators_t eIntIndicators;	/**< Internal indicators to set in this message */
		eDNPDataControl_t eDataControl;				/**< Data Control code for this message */
	} sDNPMsgBuild_t;

/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	eReturn_t DNPBuilderNewMessage(sDNPMsgBuild_t *pMsg);

	eReturn_t DNPBuilderGenerateDNP(sDNPMsgBuild_t *pMsg);

	eReturn_t DNPBuilderAddDataObjectRequest(sDNPMsgBuild_t *pMsg, eDNPGroup_t Group, uint8_t Variation, uint16_t CountStart, uint16_t CountStop);

	eReturn_t DNPBuilderAddBinaryOutputCommandDataObject(sDNPMsgBuild_t *pMsg, uint8_t Variation, uint16_t PrefixIndex, uint8_t ControlCode, uint8_t Count, uint32_t OnTime, uint32_t OffTime, uint8_t Status);

/*****	Functions	*****/


#endif

