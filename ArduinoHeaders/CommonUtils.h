/**	@defgroup	GROUPNAME
	@brief		
	@details	
		
*/

#ifndef __NAME_H
	#define __NAME_H

/***** Includes		*****/


/***** Constants	*****/


/***** Definitions	*****/


/***** Globals		*****/


/***** Prototypes 	*****/
	/**	@brief		Check if all bits set in Mask, are set in Register
		@param		Register	The value to test
		@param		Mask		The bits to check in the Register
		@return		Will return true if all bits that are 1 in Mask are also 1 in Register.
			If any 1 bits in Mask are 0 in Register will return false.
		@ingroup	snapduino
	*/
	#define CheckAllBitsInMask(Register, Mask)	(((Register & Mask) == Mask) ? true : false)

	/**	@brief		Check if any bits set in Mask, are set in Register
		@param		Register	The value to test
		@param		Mask		The bits to check in the Register
		@return		Will return true if any bits that are 1 in Mask 1 in Register.  If no 
			1 bits in Mask are 1 in Register will return false.
		@ingroup	snapduino
	*/
	#define CheckAnyBitsInMask(Register, Mask)	(((Register & Mask) != 0) ? true : false)

/***** Functions	*****/


#endif

