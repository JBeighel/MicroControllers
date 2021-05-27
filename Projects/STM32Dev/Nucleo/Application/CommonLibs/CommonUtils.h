/**	@defgroup	commonutils
	@brief		Common utilities and objects
	@details	v 0.11
	# Description #
		This is a collection of commonly used utilities.
		This includes variable types, macros, and constants.
		
	# Usage #
		
	# File Info #
		File:	CommonUtils.h
		Author:	J. Beighel
		Date:	2021-03-15
*/

#ifndef __COMMONUTILS
	#define __COMMONUTILS

/***** Includes		*****/
	#include <stdint.h>
	#include <stdbool.h>
	#include <stddef.h>
	
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
	typedef enum eReturn_t {
		Warn_EndOfData	= 3,	/**< Incomplete read request returned, reached end of data */
		Warn_Incomplete	= 2,	/**< Data provided was incomplete or partial processing occurred */
		Warn_Unknown	= 1,	/**< An unknown but recoverable error happened during the operation */
		Success			= 0,	/**< The operation completed successfully */
		Fail_Unknown	= -1,	/**< An unknown and unrecoverable error happened during the operation */
		Fail_NotImplem	= -2,	/**< Function not implemented */
		Fail_CommError	= -3,	/**< Communications layer failure */
		Fail_Invalid	= -4,	/**< Some value provided was invalid for this operation */
		Fail_BufferSize = -5,	/**< A data buffer is too small to fit the requested information */
		Fail_Blocked	= -6,	/**< Requested operation was prevented by other logic */
	} eReturn_t;

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

	/** @brief		Performs an assertion at compile time
		@details	Performs a conditional test at compile time and will throw
			a compiler warning if it fails.  Unlike \#ifdef checks this will
			allow the use of some runtime values, such as sizeof() giving it a
			bit more flexibility.  However, the compiler error will always be
			division by zero so you'll need to check the macro call to see the
			true problem.
			Example:
				//Verify that sSomeStruct_t is less than 51 bytes
				STATIC_ASSERT(StructSizeTest, sizeof(sSomeStruct_t) < 51);
		@param		Desc	Description of the assertion, follows the rules for \#define naming
		@param		Test	The conditional test to perform
		@return Throws a compiler error if the conditional test is false
		@ingroup	commonutils
	*/
	#define STATIC_ASSERT(Desc, Test) enum { Desc = 1 / (Test) }

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
	
	/**	@brief		Returns the parameter value with the bits reversed
		@details	If the value 0xC5 (0b11000101) is passed in then the 
			returned value will be 0xA3 (0b10100011)
		@param		nVal		Value to reverse the bits in
		@return		Bit reversed value
		@ingroup	commonutils
	*/
	uint8_t ReverseBitsInUInt8(uint8_t nVal);

	/**	@brief		Reverses the order of the bytes in a 16 bit value
		@details	The high byte becomes the low byte, the order of the bits in each of these bytes is unchanged.
		@param		nValue		The 2 byte value to operate on
		@return		The value with the bytes in reversed order
		@ingroup	commonutils
	*/
	static inline uint16_t FlipBytesUInt16(uint16_t nValue) {
		return ((nValue & 0x00FF) << 8) | ((nValue & 0xFF00) >> 8);
	}

/***** Functions	*****/


#endif

