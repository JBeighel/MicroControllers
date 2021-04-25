/**	@defgroup	stringtools
	@brief		
	@details	v0.1
	#Description
	
	#File Information
		File:	StringTools.h
		Author:	J. Beighel
		Date:	2021-04-01
*/

#ifndef __STRINGTOOLS_H
	#define __STRINGTOOLS_H

/*****	Includes	*****/
	#include <string.h>
	#include <stdio.h>
	#include <ctype.h>

	#include "CommonUtils.h"

/*****	Defines		*****/
	#define REGEX_TOKENBUFFSIZE		10

	#define REGEX_MAXGROUPS			10

/*****	Definitions	*****/
	typedef enum eRexExReturn_t {
		RX_WarnNoMatch	= 2,	/**< The regular expression did not match the string */
		RX_WarnUnknown	= 1,
		RX_Success		= 0,	/**< The regular expression matched the string */
		RX_FailUnknown	= -1,
		RX_FailBadRegEx	= -2,	/**< Expression had invalid formatting */
		RX_FailTknSize	= -3,	/**< A token exceeded the buffer size */
		RX_FailGroupEnd	= -4,	/**< An unmatched close group token was found */
		RX_FailGroupBgn	= -5,	/**< An unmatched open group token was found */
	} eRexExReturn_t;

	typedef enum eRegExFlags_t {
		RX_IgnoreCase	= 0x0000,	/**< Ignore case in testing expressions */
		RX_UseCase		= 0x0001,	/**< Case must match in expression tests */
	} eRegExFlags_t;

	typedef enum eRegExToken_t {
		RegEx_Unknown,
		RegEx_Dot,
		RegEx_WordChar,
		RegEx_NotWordChar,
		RegEx_AnyCharSet,
		RegEx_AnyNotCharSet,
		RegEx_AnchorBgn,
		RegEx_AnchorEnd,
		RegEx_GroupBgn,
		RegEx_GroupEnd,
	} eRegExToken_t;

	typedef struct sRegExPiece_t {
		eRegExToken_t eType;	/**< Type of regular expression token */
		uint32_t nMinMatch;		/**< Minimum times this token must match */
		uint32_t nMaxMatch;		/**< Maximum times this token can match */
		char aToken[REGEX_TOKENBUFFSIZE];	/**< All characters included in this token */
	} sRegExPiece_t;

	typedef struct sRegExResult_t {
		eRexExReturn_t eResult;	/**< Result of this regular expression test */
		uint32_t nStart;		/**< First character in the string that matched the expression */
		uint32_t nLength;		/**< Total number of characters in the string that matched */
		/**	Array for all groups in the regular expression.  Index zero is the
		 *	index in the string the group began matching.  Index one is the
		 *	total number of characters in the string that were matched by the
		 *	group.
		 */
		uint32_t aGroups[REGEX_MAXGROUPS][2];
	} sRegExResult_t;

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

	/**	@brief		Copy out a part of a string into another buffer
	 *	@param		strSrc		Source string to copy from
	 *	@param		nStartIdx	Starting index to begin copying from
	 *	@param		nLen		Number of charaters to copy
	 *	@param		strDest		Destination buffer to copy into
	 *	@return		Success if the string piece is copied, otherwise a code
	 *		indicating the failure encountered
	 *	@ingroup	stringtools
	 */
	eReturn_t StrPiece(const char *strSrc, uint32_t nStartIdx, uint32_t nLen, char *strDest);

	/**	@brief		Test a string to see if it matches a regular expression
	 *	@details	The regular expression can include the following components:
	 *	# [ ] Bracket expressions, match any character in the brackets.  Use \n
	 *		for newline, \r for carriage return, or \t for tab
	 *	# [^ ] Inverse bracket expression, match any character not in the brackets.
	 *		Use \n for newline, \r for carriage return, or \t for tab
	 *	# . Dot, match any character
	 *	# \w Word, match any alphabet, number, or underscore character
	 *	# \W Not word, match any character not alphabet, number, or underscore
	 *	# ^ Beginning anchor, match at start of string or line
	 *	# $ Ending ancher, match at end of string or line
	 *	# ( ) Group expression, for noting where an expression matched
	 *	# \n New line
	 *	# \r Carriage return
	 *	# \t Tab
	 *	# \s White space, match space, tab, new line, or carriage return
	 *	# \S Not white space, match any character not space, tab, new line, or
	 *		carriage return
	 *	# * Match previous expression zero or more times
	 *	# + Match previous expression one or more times
	 *	# ? Match previous expression zero or one times
	 *	# \ Escape, match next special character as itself
	 *
	 *	All other characters will match themselves
	 *
	 *	@param		strSrc		String to compare
	 *	@param		strRegEx	Regular expression
	 *	@param		eFlags		Flags to apply when matching
	 *	@param		pResult		Returns a structure containing match results
	 *	@return		RX_Success if the regular expression matched the string,
	 *		RX_WarnNoMatch if the expression did not.  Otherwise a code
	 *		indicating the failure encountered
	 *	@ingroup	stringtools
	 */
	eRexExReturn_t StrRegEx(const char *strSrc, const char *strRegEx, eRegExFlags_t eFlags, sRegExResult_t *pResult);

/*****	Functions	*****/


#endif

