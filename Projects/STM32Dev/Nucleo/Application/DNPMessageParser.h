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
	#include "CommonUtils.h"

	#include "CRC16.h"
	#include "DNPBase.h"

/*****	Defines		*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	eReturn_t DNPParserReceivedData(sDNPMsgBuffer_t *pMsg, uint8_t *pData, uint32_t nDataStart, uint32_t nDataLen, uint32_t *pnDataUsed);

	eReturn_t DNPParserNextDataObject(sDNPMsgBuffer_t *pMsg);

/*****	Functions	*****/


#endif

