/**	@defgroup	dnpmsgparser		DNP Message Parser
	@ingroup	dnp
	@brief		Module to parse DNP messages
	@details	v0.1
	#Description
	
	#File Information
		File:	DNPMessageParser.h
		Author:	J. Beighel
		Date:	2021-03-15
*/

#ifndef __DNPMESSAGEPARSER_H
	#define __DNPMESSAGEPARSER_H

/*****	Includes	*****/
	#include <string.h>

	#include "CommonUtils.h"

	#include "CRC16.h"
	#include "DNPBase.h"

/*****	Defines		*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	/**	@brief		Add newly received data to the DNP message being built
	 *	@param		pMsg		DNP message object to add the data to
	 *	@param		pData		Data buffer holding the newly received data
	 *	@param		nDataStart	Offset in the buffer to where the new data begins
	 *	@param		nDataLen	Last index with valid data in the buffer
	 *	@param		pnDataUsed	Returns the number of bytes accepted into the message
	 *	@return		Success upon completion of an entire DNP message, Warn_Incomplete
	 *		if additional data is required to complete the message, or a failure code
	 *		if the message was invalid or damaged
	 *	@ingroup	dnpmsgparser
	 */
	eReturn_t DNPParserReceivedData(sDNPMsgBuffer_t *pMsg, uint8_t *pData, uint32_t nDataStart, uint32_t nDataLen, uint32_t *pnDataUsed);

	/**	@brief		Read the next data object from the loaded DNP message
	 *	@param		pMsg		DNP message object to read from
	 *	@return		Success upon completion of an entire DNP message, Warn_EndOfData
	 *		if the last object had been returned, or a failure code if the message
	 *		was invalid or damaged
	 *	@ingroup	dnpmsgparser
	 */
	eReturn_t DNPParserNextDataObject(sDNPMsgBuffer_t *pMsg);

	/**	@brief		Read the next Data point from the parsed data object
	 *	@param		pMsg		DNP message object to read from
	 *	@param		pValue		Returns the data point information
	 *	@return		Success upon completion of an entire DNP message, Warn_EndOfData
	 *		if the last object had been returned, or a failure code if the message
	 *		was invalid or damaged
	 *	@ingroup	dnpmsgparser
	 */
	eReturn_t DNPParserNextDataValue(sDNPMsgBuffer_t * pMsg, sDNPDataValue_t *pValue);

/*****	Functions	*****/


#endif

