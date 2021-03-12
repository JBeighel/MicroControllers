/**	@defgroup	dnpmsgbuild
	@ingroup	dnp
	@brief		Library for creating DNP protocol messages
	@details	v0.1
	#Description
	
	#File Information
		File:	DNPMessageBuilder.h
		Author:	J. Beighel
		Date:	2021-03-11
*/

#ifndef __DNPMESSAGEBUILDER_H
	#define __DNPMESSAGEBUILDER_H

/*****	Includes	*****/
	#include "DNPBase.h"

/*****	Defines		*****/
/**	@brief		Number of bytes in each DNP message fragment
 *	@details	Each fragment will have 10 bytes for the transport function.
 *		Then 255 data bytes with 2 CRC bytes for every 16 data bytes.  This
 *		means there will be 16 data chunks each with 2 CRC bytes.
 *		In total there is 255 data bytes + 32 CRC bytes + 10 transport bytes
 *	@ingroup	dnpmsgbuild
 */
#define DNP_MSGFRAGMENTSIZE		297

/**	@brief		Maximum number of message fragments that will be allowed
 *	@details	The builder will be unable to build messages that exceed this
 *		number of message fragments.
 *	@ingroup	dnpmsgbuild
 */
#define	DNP_MAXFRAGMENTMAX		4

#define DNP_MESSAGESIZEMAX		(DNP_MSGFRAGMENTSIZE * DNP_MAXFRAGMENTMAX)

/**	@brief		Maximum number of user data bytes that a message can hold
 *	@details	The first fragment will have 5 bytes of application and transport
 *		headers.  All remaining fragments will only have 1 transport header byte
 *	@ingroup	dnpmsgbuild
 */
#define DNP_USERDATAMAX			(250 + ((DNP_MAXFRAGMENTMAX - 1) * 254))

/*****	Definitions	*****/
	typedef struct sDNPMsgBuild_t {
		uint8_t aDNPMessage[DNP_MESSAGESIZEMAX];	/**< Buffer to hold the full DNP message ready to send */
		uint32_t nDNPMsgLen;						/**< Index of last byte of the DNP Message */
		uint8_t aUserData[DNP_USERDATAMAX];			/**< Buffer to hold all user data to include in the message */
		uint32_t nUserDataLen;						/**< Index of last byte of user data */
		uint16_t nDestAddr;							/**< Destination address for this message */
		uint16_t nSourceAddr;						/**< Source address for this message */
		eDNPControlCodes_t eControlCode;			/**< Control code used in this message */
		eDNPInternalIndicators_t eIntIndicators;	/**< Internal indicators to set in this message */
	} sDNPMsgBuild_t;

/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	eReturn_t DNPBuilderNewMessage(sDNPMsgBuild_t *pMsg);
	eReturn_t DNPBuilderGenerateDNP(sDNPMsgBuild_t *pMsg);

/*****	Functions	*****/


#endif

