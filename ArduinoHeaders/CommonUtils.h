/**	@defgroup	commonutils
	@brief		Common utilities and objects
	@details	v 0.1
		
*/

#ifndef __COMMONUTILS
	#define __COMMONUTILS

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
		@ingroup	commonutils
	*/
	#define CheckAllBitsInMask(Register, Mask)	(((Register & Mask) == Mask) ? true : false)

	/**	@brief		Check if any bits set in Mask, are set in Register
		@param		Register	The value to test
		@param		Mask		The bits to check in the Register
		@return		Will return true if any bits that are 1 in Mask 1 in Register.  If no 
			1 bits in Mask are 1 in Register will return false.
		@ingroup	commonutils
	*/
	#define CheckAnyBitsInMask(Register, Mask)	(((Register & Mask) != 0) ? true : false)

	/**	@brief		Gives the value from Register with the bits set in mask cleared/zeroed
		@details	The Register value will be updated by calling this macro, its return
			is given for convenience.  The values supplied can not 
			exceed 32 bits.

			The word Zero was used in the name instead of Clear for visual clarity
			from the various Check macros.
		@param		Register	The value to be updated, having specified bits cleared
		@param		Mask		The bits to clear in Register
		@return		The value of Register after it is updated
		@ingroup	commonutils
	*/
	#define ZeroAllBitsInMask(Register, Mask)	Register = ((typeof(Register))(((uint32_t)Register) & (~(uint32_t)Mask)))

	/**	@brief		Gives the value from Register with the bits set in mask set to 1
		@details	The Register value will not be updated by calling this macro, its 
			return must be stored to retain this value.  The values supplied can not 
			exceed 32 bits.
		@param		Register	The value to be updated, having specified bits set
		@param		Mask		The bits to set in Register
		@return		The value of Register after it is updated
		@ingroup	commonutils
	*/
	#define SetAllBitsInMask(Register, Mask)	Register = ((typeof(Register))(((uint32_t)Register) | ((uint32_t)Mask)))

/***** Functions	*****/


#endif

