/**	@defgroup	commonutils
	@brief		Common utilities and objects
	@details	v 0.6
	# Description #
		This is a collection of commonly used utilities.
		This includes variable types, macros, and constants.
		
	# Usage #
		
	# File Info #
		File:	CommonUtils.h
		Author:	J. Beighel
		Date:	11-25-2020
*/

#ifndef __COMMONUTILS
	#define __COMMONUTILS

/***** Includes		*****/
	#include <stdint.h>
	#include <stdbool.h>

/***** Constants	*****/
	/**	The maximum integer value that will fit in a uint8_t data type
		@ingroup	commonutils
	*/
	#define UINT8_MAXVALUE	255
	
	/**	The maximum integer value that will fit in a uint16_t data type
		@ingroup	commonutils
	*/
	#define UINT16_MAXVALUE	0xFFFF
	
	/**	The maximum integer value that will fit in a uint32_t data type
		@ingroup	commonutils
	*/
	#define UINT32_MAXVALUE	0xFFFFFFFF

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
	#define CheckAllBitsInMask(Register, Mask)	((((Register) & (Mask)) == (Mask)) ? true : false)

	/**	@brief		Check if any bits set in Mask, are set in Register
		@param		Register	The value to test
		@param		Mask		The bits to check in the Register
		@return		Will return true if any bits that are 1 in Mask 1 in Register.  If no 
			1 bits in Mask are 1 in Register will return false.
		@ingroup	commonutils
	*/
	#define CheckAnyBitsInMask(Register, Mask)	((((Register) & (Mask)) != 0) ? true : false)

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
	#ifdef __cplusplus
		#define ZeroAllBitsInMask(Register, Mask)	Register = ((typeof(Register))(((uint32_t)Register) & (~(uint32_t)(Mask))))
	#else
		#define ZeroAllBitsInMask(Register, Mask)	Register = (((uint32_t)Register) & (~(uint32_t)(Mask)))
	#endif

	/**	@brief		Gives the value from Register with the bits set in mask set to 1
		@details	The Register value will not be updated by calling this macro, its 
			return must be stored to retain this value.  The values supplied can not 
			exceed 32 bits.
		@param		Register	The value to be updated, having specified bits set
		@param		Mask		The bits to set in Register
		@return		The value of Register after it is updated
		@ingroup	commonutils
	*/
	#ifdef __cplusplus
		#define SetAllBitsInMask(Register, Mask)	Register = ((typeof(Register))(((uint32_t)Register) | ((uint32_t)(Mask))))
	#else
		#define SetAllBitsInMask(Register, Mask)	Register = (((uint32_t)Register) | ((uint32_t)(Mask)))
	#endif

	/**	@brief		Returns the larger of two numeric values
		@param		nNum1	The first number to compare
		@param		nNum2	The second number to compare
		@return		The larger of the two values being compared
		@ingroup	commonutils
	*/
	#define GetLargerNum(nNum1, nNum2)		((nNum1 > nNum2) ? nNum1 : nNum2)

	/**	@brief		Returns the smaller of two numeric values
		@param		nNum1	The first number to compare
		@param		nNum2	The second number to compare
		@return		The smaller of the two values being compared
		@ingroup	commonutils
	*/
	#define GetSmallerNum(nNum1, nNum2)		((nNum1 < nNum2) ? nNum1 : nNum2)
	
	/**	@brief		Checks if a number is within a specified range
		@details	Tests if the number is within the bounds of the number range specified.  If the number
			matches the bounds it is considered within the range.
		@param		nNumber		The number to test
		@param		nRangeMin	The minimum bound of the range
		@param		nRangeMax	The maximum bound of the range
		@return		True if the number is within the range, false otherwise
		@ingroup	commonutils
	*/
	#define	IsNumberInInclusiveRange(nNumber, nRangeMin, nRangeMax)		(((nNumber >= nRangeMin) && (nNumber <= nRangeMax)) ? true : false)
	
	/**	@brief		Determines if a given number exists in an array
		@param		nNumber		The number to check for in the array
		@param		aList		The array to searrch through
		@param		nListCnt	The number of elements in the array
		@return		True if the number is found in the array, false if it is not
		@ingroup	commonutils
	*/
	bool NumberInArray_u8(uint8_t nNumber, const uint8_t *aList, uint32_t nListCnt);
	
	/**	@brief		Determines if a given number exists in an array
		@param		nNumber		The number to check for in the array
		@param		aList		The array to searrch through
		@param		nListCnt	The number of elements in the array
		@return		True if the number is found in the array, false if it is not
		@ingroup	commonutils
	*/
	bool NumberInArray_u16(uint16_t nNumber, const uint16_t *aList, uint32_t nListCnt);

	/**	@brief		Determines the index of a given number in an array
		@param		nNumber		The number to check for in the array
		@param		aList		The array to search through
		@param		nListCnt	The number of elements in the array
		@return		The index if the number is found in the array, nListCnt if it is not
		@ingroup	commonutils
	*/
	uint32_t IndexInArray_u8(uint8_t nNumber, const uint8_t *aList, uint32_t nListCnt);
	
	/**	@brief		Determines the index of a given number in an array
		@param		nNumber		The number to check for in the array
		@param		aList		The array to search through
		@param		nListCnt	The number of elements in the array
		@return		The index if the number is found in the array, nListCnt if it is not
		@ingroup	commonutils
	*/
	uint32_t IndexInArray_u16(uint16_t nNumber, const uint16_t *aList, uint32_t nListCnt);

	/**	@brief		Calculates an 8 bit CRC value for an array of data
		@param		nPolynomial		The 8 bit polynomial to use while generating the CRC
		@param		nInit			The starting value to use when generating the CRC
		@param		nXOROut			Exclusive or the output with this value, set to 0x00 for no XOR
		@param		pData			Pointer to the data to CRC
		@param		nDataLen		The number of bytes in the data
		@ingroup	commonutils
	*/
	uint8_t CalculateCRC8(uint8_t nPolynomial, uint8_t nInit, uint8_t nXOROut, const uint8_t *pData, uint32_t nDataLen);
	
	/**	@brief		Counts the 1 bits in an integer value
		@param		nVal		The value to count the set bits in
		@return		Count of the 1 bits in the value
		@ingroup	commonutils
	*/
	uint32_t CountSetBitsInInt32(uint32_t nVal);
	
	/**	@brief		Counts the 1 bits in an integer value
		@param		nVal		The value to count the set bits in
		@return		Count of the 1 bits in the value
		@ingroup	commonutils
	*/
	uint16_t CountSetBitsInInt16(uint32_t nVal);

/***** Functions	*****/


#endif

