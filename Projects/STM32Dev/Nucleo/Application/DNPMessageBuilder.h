/**	@defgroup	dnpmsgbuild		DNP Message Builder
	@ingroup	dnp
	@brief		Library for creating DNP protocol messages
	@details	v0.1
	#Description
	
	#File Information
		File:	DNPMessageBuilder.h
		Author:	J. Beighel
		Date:	2021-04-27
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
	/**	@brief		Convert data in a aDNPMsgBuffer_t to a complete DNP message
	 *	@details	After adding data objects and setting properties in the
	 *		message buffer call this function to have it correctly framed in
	 *		the DNP syntax.
	 *		The DNP message created will be stored inside pMsg.
	 *	@param		pMsg		Message buffer to create the message from
	 *	@return		Success upon DNP message creation.  Or a code indicating
	 *		the failure encountered.
	 *	@ingroup	dnpmsgbuild
	 */
	eReturn_t DNPBuilderGenerateDNP(sDNPMsgBuffer_t *pMsg);

	/**	@brief		Add a data object request to the DNP message buffer
	 *	@details	This is a request for information packet, it does not
	 *		contain any data in it.  The receiving device is expected to
	 *		respond with the data objects requested.
	 *		DNP allows for various types of data request addressing.  This
	 *		function only provides 2 byte count start and stop values.  If
	 *		count start and stop are both zero, then the message will request
	 *		all available points of the specified group.
	 *	@param		pMsg		Message buffer to add the data object to
	 *	@param		Group		The data object group type to request
	 *	@param		Variation	The data object variation to request
	 *	@param		CountStart	The first object to request
	 *	@param		CountStop	THe last object to request
	 *	@return		Success if the object is added.  Or a code indicating
	 *		the failure encountered.
	 *	@ingroup	dnpmsgbuild
	 */
	eReturn_t DNPBuilderAddDataObjectRequest(sDNPMsgBuffer_t *pMsg, eDNPGroup_t Group, uint8_t Variation, uint16_t CountStart, uint16_t CountStop);

	/**	@brief		Add a command request for a binary output object, group 12
	 *	@param		pMsg		Message buffer to add the data object to
	 *	@param		Variation	The data object variation to request
	 *	@param		PrefixIndex	The index of the object to operate
	 *	@param		eCtrlCode	Control code specifying the type of operation
	 *	@param		nOpCount	Number of times to do the operation
	 *	@param		nOnTime		Time to hold the output true
	 *	@param		nOffTime	Time to hold the output false
	 *	@param		nStatus		Status flags to include
	 *	@return		Success if the object is added.  Or a code indicating
	 *		the failure encountered.
	 *	@ingroup	dnpmsgbuild
	 */
	eReturn_t DNPBuilderAddBinaryOutputCommandDataObject(sDNPMsgBuffer_t *pMsg, uint8_t Variation, uint16_t PrefixIndex, eDNPBinOutControlCode_t eCtrlCode, uint8_t nOpCount, uint32_t nOnTime, uint32_t nOffTime, uint8_t nStatus);

	/**	@brief		Add data point values for binary output objects
	 *	@param		pMsg			Message buffer to add the data object to
	 *	@param		nVariation		Variation type to use for the data object
	 *	@param		nNumPoints		Number of data points to include
	 *	@param		peStatuses		Array of flag values for each data point
	 *	@param		bIncludePrefix	True to include prefixes for each value, false to omit
	 *	@param		nStartAddress	Address of the first data point
	 *	@return		Success if the object is added.  Or a code indicating
	 *		the failure encountered.
	 *	@ingroup	dnpmsgbuild
	 */
	eReturn_t DNPBuilderAddBinaryOutputDataObject(sDNPMsgBuffer_t *pMsg, uint8_t nVariation, uint8_t nNumPoints, eDNPObjBinOutFlags_t *peStatuses, bool bIncludePrefix, uint16_t nStartAddress);

	/**	@brief		Add data point values for binary input objects
	 *	@param		pMsg			Message buffer to add the data object to
	 *	@param		nVariation		Variation type to use for the data object
	 *	@param		nNumPoints		Number of data points to include
	 *	@param		peStatuses		Array of flag values for each data point
	 *	@param		bIncludePrefix	True to include prefixes for each value, false to omit
	 *	@param		nStartAddress	Address of the first data point
	 *	@return		Success if the object is added.  Or a code indicating
	 *		the failure encountered.
	 *	@ingroup	dnpmsgbuild
	 */
	eReturn_t DNPBuilderAddBinaryInputDataObject(sDNPMsgBuffer_t *pMsg, uint8_t nVariation, uint8_t nNumPoints, eDNPObjBinInFlags_t *peStatuses, bool bIncludePrefix, uint16_t nStartAddress);

/*****	Functions	*****/


#endif

