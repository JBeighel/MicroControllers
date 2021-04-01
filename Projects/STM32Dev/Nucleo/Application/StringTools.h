/**	@defgroup	stringtools
	@brief		
	@details	v0.1
	#Description
	
	#File Information
		File:	StringTools.h
		Author:	J. Beighel
		Date:	2021-03-31
*/

#ifndef __STRINGTOOLS_H
	#define __STRINGTOOLS_H

/*****	Includes	*****/
	#include<string.h>

	#include "CommonUtils.h"

/*****	Defines		*****/


/*****	Definitions	*****/
	typedef enum eRegExToken_t {
		RegEx_Unknown,
		RegEx_Dot,
		RegEx_Self,
		RegEx_AnchorBgn,
		RegEx_AnchorEnd,
	} eRegExToken_t;

	typedef struct sRegExPiece_t {
		eRegExToken_t eType;
		uint32_t nMinMatch;
		uint32_t nMaxMatch;
		char aToken[10];
	} sRegExPiece_t;

/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	/**	@breif		Find the first occurrence of a string inside of a larger string
	 *	@param		strSrc		String to search inside of
	 *	@param		strFind		String to search for in strSrc
	 *	@return		The zero based index of the first occurrence of strFind in
	 *		strSrc.  Or -1 (UINT32_MAXVALUE) if it can not be found
	 *	@ingroup	stringtools
	 */
	uint32_t StrFind(const char *strSrc, const char *strFind);

	/**	@breif		Find the first occurrence of any of a set of characters
	 *		inside of a larger string
	 *	@param		strSrc		String to search inside of
	 *	@param		strChars	String containing characters to search for in strSrc
	 *	@return		The zero based index of the first occurrence of any one
	 *		character in strChars in strSrc.  Or -1 (UINT32_MAXVALUE) if none
	 *		can be found
	 *	@ingroup	stringtools
	 */
	uint32_t StrFindFirstChar(const char *strSrc, const char *strChars);

	/**	@brief		Trim a set of characters from the beginning of a string
	 *	@param		strSrc		String to modify
	 *	@param		strTrim		String containing characters to be removed
	 *	@return		Success if the string is correctly modified, or a code
	 *		indicating the error encountered
	 *	@ingroup	stringtools
	 */
	eReturn_t StrTrimStart(char *strSrc, const char *strTrim);

	/**	@brief		Trim a set of characters from the end of a string
	 *	@param		strSrc		String to modify
	 *	@param		strTrim		String containing characters to be removed
	 *	@return		Success if the string is correctly modified, or a code
	 *		indicating the error encountered
	 *	@ingroup	stringtools
	 */
	eReturn_t StrTrimEnd(char *strSrc, const char *strTrim);

	eReturn_t StrPiece(const char *strSrc, uint32_t nStartIdx, uint32_t nLen, char *strDest);

	eReturn_t StrRegEx(const char *strSrc, const char *strRegEx);
/*****	Functions	*****/


#endif

