/**	@defgroup	dnpmsgbuild		DNP Message Builder
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


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	eReturn_t DNPBuilderGenerateDNP(sDNPMsgBuffer_t *pMsg);

	eReturn_t DNPBuilderAddDataObjectRequest(sDNPMsgBuffer_t *pMsg, eDNPGroup_t Group, uint8_t Variation, uint16_t CountStart, uint16_t CountStop);

	eReturn_t DNPBuilderAddBinaryOutputCommandDataObject(sDNPMsgBuffer_t *pMsg, uint8_t Variation, uint16_t PrefixIndex, uint8_t ControlCode, uint8_t Count, uint32_t OnTime, uint32_t OffTime, uint8_t Status);

/*****	Functions	*****/


#endif

