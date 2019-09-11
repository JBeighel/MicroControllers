/**	@defgroup	string
	@brief		
	@details	
		
*/

#ifndef __STRINGCLASS_H
	#define __STRINGCLASS_H

/***** Includes		*****/
	#include <string.h>
	#include <stdarg.h>

	#include "CommonUtils.h"

/***** Constants	*****/
	/**	@brief		Return value from functions that search for text indicated the specified text was not found
		@ingroup	string
	*/
	#define STRING_NOTFOUND		((uint32_t)-1)

	/**	@brief		Default list of characters considered to be whitespace by the trim functions
		@ingroup	string
	*/
	#define STRING_WHITESPACE	" \t\r\n"

/***** Definitions	*****/
	/**	@brief		Flags controlling the string search logic
		@ingroup	string
	*/
	enum eStringFind_t {
		String_UseCase		= 0x00,		/**< Search should be case sensitive */
		String_NoCase		= 0x01,		/**< Search is case insensitive */
		String_StartBegin	= 0x02,		/**< Search begins at the start of the string */
		String_StartEnd		= 0x04,		/**< Search begins at the end of the string working backwards through it */
	};

	/**	@brief		Values that will be stored in the ErrorNumber value on any failed operation
		@ingroup	string
	*/
	enum eStringResult_t {
		String_Success		= 0,		/**< The operation completed successfully */
		String_NotFound		= 1,		/**< A search through the string found no match */
		String_ErrorBounds	= 2,		/**< The boundaries specifed were invalid */
		String_ErrorMemory	= 3,		/**< An error occurred when allocating memory for a string operation */
		String_RegExError	= 4,
		String_RegExNoMatch	= 5,
		String_NoStr		= 6,
		String_RegExNoRegEx	= 7,
		String_RegExSyntax	= 8,
	};

	/**	@brief		Flags controlling the string whitespace trimming logic
		@ingroup	string
	*/
	enum eStringTrim_t {
		String_TrimBegin	= 0x01,		/**< Trim whitespace from the beginning of the string */
		String_TrimEnd		= 0x02,		/**< Trim whitespace from the end of the string */
	};

	enum eStringRegExToken_t {
		RegEx_Slash			= 1,
		RegEx_Bracket		= 2,
		RegEx_Dot			= 3,
		RegEx_Group			= 4,
		RegEx_Self			= 5,
		RegEx_AnchBegin		= 6,
		RegEx_AnchEnd		= 7,
	};

	enum eStringRegExMatch_t {
		RegEx_MatchRegular	= 0x00,
		RegEx_MatchMinimum	= 0x01,
	};

	struct sStringRegExPiece_t {
		uint32_t nType, nRptMin, nRptMax, nListLen;
		char *pData;
		eStringRegExMatch_t eMatchMode;
		sStringRegExPiece_t *pSubPieceList;
	};

	class cStringClass_t {
	public:
		cStringClass_t(void);
		~cStringClass_t();
		bool Initialize(void);
		
		/**	@brief		Releases all resources allocated by this class
			@details	This is the identical routine to what is called when the class deconstructor is called.  The class should not be expected to run correctly unless Initialize() is called after CleanUp().
			@return		True on success, false on any failure.  In addition the ErrorNumber class variable will be set to the failure code.
		*/
		bool CleanUp(void);
		bool SetString(const char *pText, ...);
		bool AddToString(const char *pText, ...);
		
		/**	@brief		Returns the number of characters in the managed string
			@return		The cahracter count in the managed string
		*/
		uint32_t Length(void);
		uint32_t Find(const char *pText, uint32_t nSkipCharNum, eStringFind_t eFlags = (eStringFind_t)(String_UseCase | String_StartBegin));
		bool SubStr(uint32_t nStart, uint32_t nLength);
		bool SubStr(const char *pStart, uint32_t nLength);
		bool SubStr(uint32_t nStart, const char *pEnd);
		bool SubStr(char *pStart, char *pEnd);
		bool Left(uint32_t nNumChars);
		bool Left(const char *pEnd);
		bool Right(uint32_t nNumChars);
		bool Right(const char *pStart);
		bool InsertText(uint32_t nPos, const char *pText);
		bool RemoveText(uint32_t nPos, uint32_t nNumChars);
		bool Trim(const char *pSpace, eStringTrim_t eFlags = (eStringTrim_t)(String_TrimBegin | String_TrimEnd));
		bool Trim(eStringTrim_t eFlags = (eStringTrim_t)(String_TrimBegin | String_TrimEnd));
		
		/**	@brief		Splits the managed string into pieces
			@details	Creates an array of substrings in the StringArray member variable.  These substrings will be delimited by the specified string.  The substrings will not contain the delimiting text.
			
			Exception strings can also be provided to create sections in the managed string where the substrings will not end.  Different strings to start and end these exception zones can be supplied.
			@param		pDelim			The delimiting text
			@param		pExceptStart	The string to begin an exception area where new substrings won't begin or end
			@param		pExceptEnd		The string to end an exception area
			@return		True on success, false on any failure.  In addition the ErrorNumber class variable will be set to the failure code.
		*/
		bool Split(const char *pDelim, char *pExceptStart, char *pExceptEnd);
		
		/**	@brief		Splits the managed string into pieces
			@details	Creates an array of substrings in the StringArray member variable.  These substrings will be delimited by the specified string.  The substrings will not contain the delimiting text.
			
			@param		pDelim			The delimiting text
			@return		True on success, false on any failure.  In addition the ErrorNumber class variable will be set to the failure code.
		*/
		bool Split(const char *pDelim);
		
		bool RegExTest(const char *pRegEx, uint32_t nStart, uint32_t *pnNumChars);

		eStringResult_t ErrorNumber;	/**< Error code to describe the nature of the failed operation */
		uint32_t ArrayCount;			/**< Count of sub string arrays in the StringArray member */
		char *String;					/**< The string managed and manipulated by this class */
		char **StringArray;				/**< List of sub strings generated by operations performed on the managed string */

	protected:
		/**	@brief		Adds text from a string to the end of the managed string
			@param		pText		Pointer to the text to add
			@param		nStartIdx	Index in the text to add to begin copying from
			@param		nNumChars	Number of characters to copy into the managed string
			@return		True on success, false on any failure.  In addition the ErrorNumber class variable will be set to the failure code.
		*/
		bool AddText(const char *pText, uint32_t nStartIdx, uint32_t nNumChars);
		bool SubStr(char **hStr, unsigned int nSkipCharNum, const char *pTxtStart, unsigned int nEnd, const char *pTxtEnd, eStringFind_t eFlags);

	private:
		uint32_t RegExParse(const char *pRegEx, uint32_t nCharNum);
		bool StringArrayCleanup();
		bool RegExCleanup(sStringRegExPiece_t *pPieceList);
		uint32_t ExPieceTest(uint32_t nPlace, sStringRegExPiece_t *pCurrPiece, sStringRegExPiece_t *pNextPiece);
		bool StringBufferResize(uint32_t pNewSetLen, uint32_t nCharsToAdd);
		bool AddToStringArray(uint32_t nNumPieces);

		uint32_t cnStrBuffLen;					/**< Number of bytes contained in the buffer holding the managed string */
		uint32_t cnStrArrLen;					/**< Copy of ArrayCount, preserves that value in case the user modifies it */
		uint32_t cnRegExPieceLen;				/**< Number of the regular expression pieces in the piece list */
		sStringRegExPiece_t *cpRegExPieceList;	/**< List of regular expression pieces parsed from the specified regex string */
	};

/***** Globals		*****/


/***** Prototypes 	*****/


/***** Functions	*****/
cStringClass_t::cStringClass_t(void) {
	Initialize();
	return;
}

cStringClass_t::~cStringClass_t() {
	CleanUp();
	return;
}

bool cStringClass_t::Initialize(void) {
	cnStrBuffLen = 0;
	cnStrArrLen = 0;

	this->StringArray = NULL;
	cpRegExPieceList = NULL;
	
	this->String = new char[1];
	if (this->String != NULL) {
		this->String[0] = '\0';
	}
	cnStrBuffLen = 1;
	
	this->ErrorNumber = String_Success;
	return true;
}
	
bool cStringClass_t::CleanUp(void) {
	uint32_t nCtr;
	
	//Remove the main string
	if (this->String != NULL) {
		delete this->String;
	}
	this->String == NULL;

	//Remove the substring list
	StringArrayCleanup();
	
	//Remove reg ex piece list
	if (cpRegExPieceList != NULL) {
		for (nCtr = 0; nCtr < cnRegExPieceLen; nCtr++) {
			RegExCleanup(&(cpRegExPieceList[nCtr]));
		}
		
		delete cpRegExPieceList;
		cpRegExPieceList = NULL;
	}
	
	ErrorNumber = String_Success;
	return true;
}

bool cStringClass_t::StringArrayCleanup() {
	uint32_t nCtr;
	
	if (this->StringArray != NULL) {
		for (nCtr = 0; nCtr < cnStrArrLen; nCtr++) {
			if (this->StringArray[nCtr] != NULL) {
				delete this->StringArray[nCtr];
				
				this->StringArray[nCtr] = NULL;
			}
		}
		
		delete this->StringArray;
	}
	this->StringArray = NULL;
	
	cnStrArrLen = 0;
	ArrayCount = 0;
	
	return true;
}

bool cStringClass_t:: RegExCleanup(sStringRegExPiece_t *pPieceList) {
	uint32_t nCtr;
	bool bRetVal, bResult;
	
	if (pPieceList->pData != NULL) {
		delete pPieceList->pData;
		pPieceList->pData = NULL;
	}
	
	bRetVal = true;
	if (pPieceList->pSubPieceList != NULL) {
		for (nCtr = 0; nCtr < pPieceList->nListLen; nCtr++) {
			bResult = RegExCleanup(&(pPieceList->pSubPieceList[nCtr]));
			
			if (bResult == false) {
				bRetVal = false;
			}
		}
		
		delete pPieceList->pSubPieceList;
		pPieceList->pSubPieceList = NULL;
	}
	
	return bRetVal;
}

uint32_t cStringClass_t::Length(void) {
	uint32_t nCtr;
	
	for (nCtr = 0; nCtr < cnStrBuffLen; nCtr++) {
		if (this->String[nCtr] == '\0') {
			return nCtr;
		}
	}
	
	ErrorNumber = String_NoStr;
	return 0;
}

bool cStringClass_t::StringBufferResize(uint32_t nNewStrLen, uint32_t nCharsToAdd) {
	char *pNewBuff;
	uint32_t nPrevStrLen = Length(); //Add 1 for terminator
	
	if (nNewStrLen == 0) { //Set length isn't specified, they must be adding length
		nNewStrLen = nPrevStrLen + nCharsToAdd;
	} else { //They gave a set length, include the characters to add
		nNewStrLen += nCharsToAdd;
	}
	nNewStrLen += 1; //Add 1 to ensure space is allocated for null terminator
	
	pNewBuff = new char[nNewStrLen];
	
	if (pNewBuff == NULL) { //Failed to allocate space
		ErrorNumber = String_ErrorMemory;
		return false;
	}
	
	//Copy in the old data
	nPrevStrLen += 1; //Add 1 to account for null terminator
	nCharsToAdd = GetSmallerNum(nPrevStrLen, nNewStrLen);
	memcpy(pNewBuff, this->String, nCharsToAdd * sizeof(char));
	
	pNewBuff[nCharsToAdd - 1] = '\0'; //Make sure the new buffer has null terminator
	
	//Clean up old string buffer, and replace is
	delete this->String;
	this->String = pNewBuff;
	cnStrBuffLen = nNewStrLen;
	
	return true;
}

bool cStringClass_t::AddText(const char *pText, uint32_t nStartIdx, uint32_t nNumChars) {
	uint32_t nCtr, nOldLen, nNewLen;

	//Get the string lengths
	nOldLen = Length();
	
	if (nNumChars == 0) {//No length given, assume full string
		nNewLen = strlen(pText);
	} else { //Use the character length given
		nNewLen = nNumChars;
	}

	nNewLen += nOldLen;
	if (nNewLen == 0) { //Nothing to add
		return true;
	}
	
	if (nStartIdx >= nNewLen) { //Source string doesn't have that many characters, nothing to add
		return true;
	}

	//Increase the buffer that contains the string
	if (StringBufferResize(nNewLen, 0) == false) { //Buffer increase failed, update the error variables
		return false;
	}
	
	//Buffer increase succeeded, copy in the text
	for (nCtr = nOldLen; nCtr < nNewLen; nCtr++) {
		String[nCtr] = pText[nStartIdx];
		nStartIdx += 1; //Move index to next character in string
	}
	String[nNewLen] = '\0'; //New len is the number of characters, buffer 1 larger for terminator
	
	return true;
}

bool cStringClass_t::SetString(const char *pText, ...) {
	va_list VList;
	char aEscBuff[32], aTempBuff[32];
	uint32_t nTextCtr, nTextStartPos, nTextLen, nEscBuffCtr;
	bool bInEscape, bResult, bRetVal;
	unsigned int nTempU32;
	int nTempS32;
	double nTempD;
	char *pTemp;
	
	this->String[0] = '\0'; //Make the string length zero, so text can just be added
	nTextLen = strlen(pText);
	
	va_start(VList, pText);
	
	nTextStartPos = 0;
	bInEscape = false;
	bResult = true;
	for (nTextCtr = 0; nTextCtr < nTextLen; nTextCtr++) { //Loop through all characters in Text adding to String
		if (bInEscape == false) {//Not in an escape sequence, look for start
			if (pText[nTextCtr] == '%') { //Found start of an escape sequence
				bResult = AddText(pText, nTextStartPos, nTextCtr - nTextStartPos); //Add all the plain text to String
				bInEscape = true;
				aEscBuff[0] = '%';
				nEscBuffCtr = 1;
			}
		} else { //Look for end of escape sequence
			if ((isalpha(pText[nTextCtr]) != 0) || (pText[nTextCtr] == '%')) { //Found the end of the escape squence
				nTextStartPos = nTextCtr + 1;// Normal text resume on next character
				aEscBuff[nEscBuffCtr] = pText[nTextCtr];
				aEscBuff[nEscBuffCtr + 1] = '\0';
				bInEscape = false;  //The full escape sequence has been parsed out
			} else { //Haven't found the end of the escape sequence 
				aEscBuff[nEscBuffCtr] = pText[nTextCtr]; //Keep copying in characters
			}
			
			if (bInEscape == false) {//Started in an escap, but have reached the end
				//Remember nEscBuffCtr is pointing to last character of the escape, 1 less than temp buff length
				switch(aEscBuff[nEscBuffCtr]) {
					case ('%'): //Escape to add a % sign
						bResult = AddText("%", 0, 1);
						break;
					
					case ('s'): //String of text
						pTemp = (char *)va_arg(VList, char *);
						bResult = AddText(pTemp, 0, 0);
						break;
					
					case ('c'):
					case ('C'): //Single character, char types are promoted to integers on the stack
					case ('o'): //Unsigned ocatal integer (unsigned int)
					case ('u'): //Unsigned decimal integer (unsigned int)
					case ('x'): //Unsigned hexadecimal integer (unsigned int)
						nTempU32 = va_arg(VList, int);
						sprintf(aTempBuff, aEscBuff, nTempU32);
						
						bResult = AddText(aTempBuff, 0, 0);
						break;					
					
					case ('d'): //Signed decimal integer (int)
					case ('i'): //Signed decimal integer (int)
						nTempS32 = va_arg(VList, unsigned int);
						sprintf(aTempBuff, aEscBuff, nTempS32);
						
						bResult = AddText(aTempBuff, 0, 0);
						break;	
						
					case ('e'): //Signed floating point value
					case ('f'): //Signed floating point value (float)
					case ('g'): //Signed floating point value 
						nTempD = va_arg(VList, double);
						sprintf(aTempBuff, aEscBuff, nTempD);
						
						bResult = AddText(aTempBuff, 0, 0);
						break;
					
					default: //Unknown escape, insert it as text
						bResult = AddText(pText, 0, nEscBuffCtr + 1);
						break;
				}
				
				nEscBuffCtr = 0; //THis needs reset after the escape is handled
			}
		}
		
		if (bResult == false) { //Some addition to the string failed
			return false;
		}
	}

	if (bInEscape == false) { //Finished all the conversions and escapes, add the remainder of the text
		bResult = AddText(pText, nTextStartPos, nTextLen - nTextStartPos);
	} else { //Broken syntax, the escape/conversion wasn't finished
		aEscBuff[nEscBuffCtr + 1] = '\0'; //Make sure its terminated
		bResult = AddText(aEscBuff, 0, nEscBuffCtr);
	}
	
	va_end(VList);
	
	return bResult;
}

bool cStringClass_t::AddToString(const char *pText, ...) {
	va_list VList;
	char aEscBuff[32], aTempBuff[32];
	uint32_t nTextCtr, nTextStartPos, nTextLen, nEscBuffCtr;
	bool bInEscape, bResult, bRetVal;
	unsigned int nTempU32;
	int nTempS32;
	double nTempD;
	char *pTemp;
	
	nTextLen = strlen(pText);
	
	va_start(VList, pText);
	
	nTextStartPos = 0;
	bInEscape = false;
	bResult = true;
	for (nTextCtr = 0; nTextCtr < nTextLen; nTextCtr++) { //Loop through all characters in Text adding to String
		if (bInEscape == false) {//Not in an escape sequence, look for start
			if (pText[nTextCtr] == '%') { //Found start of an escape sequence
				bResult = AddText(pText, nTextStartPos, nTextCtr - nTextStartPos); //Add all the plain text to String
				bInEscape = true;
				aEscBuff[0] = '%';
				nEscBuffCtr = 1;
			}
		} else { //Look for end of escape sequence
			if ((isalpha(pText[nTextCtr]) != 0) || (pText[nTextCtr] == '%')) { //Found the end of the escape squence
				nTextStartPos = nTextCtr + 1;// Normal text resume on next character
				aEscBuff[nEscBuffCtr] = pText[nTextCtr];
				aEscBuff[nEscBuffCtr + 1] = '\0';
				bInEscape = false;  //The full escape sequence has been parsed out
			} else { //Haven't found the end of the escape sequence 
				aEscBuff[nEscBuffCtr] = pText[nTextCtr]; //Keep copying in characters
			}
			
			if (bInEscape == false) {//Started in an escap, but have reached the end
				//Remember nEscBuffCtr is pointing to last character of the escape, 1 less than temp buff length
				switch(aEscBuff[nEscBuffCtr]) {
					case ('%'): //Escape to add a % sign
						bResult = AddText("%", 0, 1);
						break;
					
					case ('s'): //String of text
						pTemp = (char *)va_arg(VList, char *);
						bResult = AddText(pTemp, 0, 0);
						break;
					
					case ('c'):
					case ('C'): //Single character, char types are promoted to integers on the stack
					case ('o'): //Unsigned ocatal integer (unsigned int)
					case ('u'): //Unsigned decimal integer (unsigned int)
					case ('x'): //Unsigned hexadecimal integer (unsigned int)
						nTempU32 = va_arg(VList, int);
						sprintf(aTempBuff, aEscBuff, nTempU32);
						
						bResult = AddText(aTempBuff, 0, 0);
						break;					
					
					case ('d'): //Signed decimal integer (int)
					case ('i'): //Signed decimal integer (int)
						nTempS32 = va_arg(VList, unsigned int);
						sprintf(aTempBuff, aEscBuff, nTempS32);
						
						bResult = AddText(aTempBuff, 0, 0);
						break;	
						
					case ('e'): //Signed floating point value
					case ('f'): //Signed floating point value (float)
					case ('g'): //Signed floating point value 
						nTempD = va_arg(VList, double);
						sprintf(aTempBuff, aEscBuff, nTempD);
						
						bResult = AddText(aTempBuff, 0, 0);
						break;
					
					default: //Unknown escape, insert it as text
						bResult = AddText(pText, 0, nEscBuffCtr + 1);
						break;
				}
				
				nEscBuffCtr = 0; //THis needs reset after the escape is handled
			}
		}
		
		if (bResult == false) { //Some addition to the string failed
			return false;
		}
	}

	if (bInEscape == false) { //Finished all the conversions and escapes, add the remainder of the text
		bResult = AddText(pText, nTextStartPos, nTextLen - nTextStartPos);
	} else { //Broken syntax, the escape/conversion wasn't finished
		aEscBuff[nEscBuffCtr + 1] = '\0'; //Make sure its terminated
		bResult = AddText(aEscBuff, 0, nEscBuffCtr);
	}
	
	va_end(VList);
	
	return bResult;
}

uint32_t cStringClass_t::Find(const char *pSearch, uint32_t nSkipCharNum, eStringFind_t eFlags) {
	uint32_t nFindStartIdx, nFindEndIdx, nFindIncrement, nStrIdx, nStrLen, nSearchLen, nSearchIdx;
	char SearchLtr, StrLtr;
	
	nStrLen = Length();
	nSearchLen = strlen(pSearch);
	
	if (nSkipCharNum >= nStrLen) { //Skipping more characters than exist
		return STRING_NOTFOUND;
	}
	
	if (CheckAllBitsInMask(eFlags, String_StartEnd) == true) { //Searching backwards through the string
		nFindStartIdx = nStrLen - nSkipCharNum; //Start at end
		nFindEndIdx = nSkipCharNum - 1; //Go to beginning (-1 so that the loop hits all characters)
		nFindIncrement = -1; //Increment down
	} else { //Searching forwards through the string
		nFindStartIdx = nSkipCharNum; //Start at beginning
		nFindEndIdx = nStrLen; //Go to end
		nFindIncrement = 1; //Increment up
	}

	for (nStrIdx = nFindStartIdx; nStrIdx != nFindEndIdx; nStrIdx += nFindIncrement) {
		//Scan through looking for first character if search string
		SearchLtr = pSearch[0];
		StrLtr = this->String[nStrIdx];
		
		if (CheckAllBitsInMask(eFlags, String_NoCase) == true) {
			SearchLtr = tolower(SearchLtr);
			StrLtr = tolower(StrLtr);
		}
		
		if (SearchLtr == StrLtr) {
			for (nSearchIdx = 0; nSearchIdx < nSearchLen; nSearchIdx++) {
				if (nStrIdx + nSearchIdx >= nStrLen) { //Reading past the end of String, no match
					nSearchIdx = STRING_NOTFOUND - 1; //Invalidate the search index to exit the loop (decrement so loop increment doesn't botch logic)
				}
				
				SearchLtr = pSearch[nSearchIdx];
				StrLtr = this->String[nStrIdx + nSearchIdx];
				
				if (CheckAllBitsInMask(eFlags, String_NoCase) == true) {
					SearchLtr = tolower(SearchLtr);
					StrLtr = tolower(StrLtr);
				}

				if (SearchLtr != StrLtr) {
					//Found a character in String that doesn't match the search
					nSearchIdx = STRING_NOTFOUND - 1; //Invalidate the search index to exit the loop (decrement so loop increment doesn't botch logic)
				}
			}
			
			if (nSearchIdx != STRING_NOTFOUND) { //Found a complete match!
				return nStrIdx;
			}
		}
	}
	
	ErrorNumber = String_NotFound;
	return STRING_NOTFOUND;
}

bool cStringClass_t::SubStr(char **hStr, unsigned int nStart, const char *pTxtStart, unsigned int nEnd, const char *pTxtEnd, eStringFind_t eFlags) {
	uint32_t nStrLen, nSubLen, nCtr;

	nStrLen	= Length();
	
	if (pTxtStart != NULL) { //Starting on a string
		nStart = Find(pTxtStart, 0, eFlags);
		if (nStart == STRING_NOTFOUND) {
			return false;
		}
		
		nStart += strlen(pTxtStart); //Make sure the start text isn't included in the substring
	}
	
	if (pTxtEnd != NULL) { //Ending on a string match
		nEnd = Find(pTxtEnd, 0, eFlags);
		if (nEnd == STRING_NOTFOUND) {
			return false;
		}
	}

	if (nStart > nEnd) { //Start and stop are out of order
		ErrorNumber = String_ErrorBounds;
		return false;
	} else if (nEnd > nStrLen) {
		ErrorNumber = String_ErrorBounds;
		return false;
	}
	
	nSubLen = nEnd - nStart;
	
	//Create the buffer to hold the sub string
	(*hStr) = new char[nSubLen + 1]; //Add 1 for null terminator
	if (*hStr == NULL) {
		ErrorNumber = String_ErrorMemory;
		return false;
	}

	for (nCtr = 0; nCtr < nSubLen; nCtr++) {
		(*hStr)[nCtr] = this->String[nCtr + nStart];
	}
	(*hStr)[nSubLen] = '\0';
	
	return true;
}

bool cStringClass_t::AddToStringArray(uint32_t nNumPieces) {
	char **pNewArray;
	uint32_t nCtr, nNewArrSize;
	
	nNewArrSize += cnStrArrLen + 1; //Include any existing pieces, plus 1 for a null ending
	
	pNewArray = new char *[nNewArrSize];
	if (pNewArray == NULL) {
		ErrorNumber = String_ErrorMemory;
		return false;
	}
	
	//Copy in existing pieces
	for (nCtr = 0; nCtr < cnStrArrLen; nCtr++) {
		pNewArray[nCtr] = StringArray[nCtr];
	}
	
	//Initialize the new pieces
	for (nCtr = cnStrArrLen; nCtr < nNewArrSize; nCtr++) {
		pNewArray[nCtr] = NULL;
	}
	
	//Update the counts
	cnStrArrLen += nNumPieces; //Only add in the new pieces added (not that extra null pointer)
	ArrayCount = cnStrArrLen;
	
	//Adjust the pointers
	delete StringArray;
	StringArray = pNewArray;
	
	return true;
}

bool cStringClass_t::Left(uint32_t nNumChars) {
	bool bResult;
	
	StringArrayCleanup(); //Results go in the array, remove any left overs
	
	//Create space for the result
	if (AddToStringArray(1) == false) {
		return false;
	}
	
	//Create the sub string in the first element
	bResult = SubStr(&(StringArray[0]), 0, NULL, nNumChars, NULL, (eStringFind_t)(String_UseCase | String_StartBegin));
	
	return bResult;
}

bool cStringClass_t::Left(const char *pEnd) {
	bool bResult;
	
	StringArrayCleanup(); //Results go in the array, remove any left overs
	
	//Create space for the result
	if (AddToStringArray(1) == false) {
		return false;
	}
	
	//Create the sub string in the first element
	bResult = SubStr(&(StringArray[0]), 0, NULL, 0, pEnd, (eStringFind_t)(String_UseCase | String_StartBegin));
	
	return bResult;
}

bool cStringClass_t::Right(uint32_t nNumChars) {
	bool bResult;
	uint32_t nStrLen;
	
	StringArrayCleanup(); //Results go in the array, remove any left overs
	
	//Create space for the result
	if (AddToStringArray(1) == false) {
		return false;
	}
	
	nStrLen = Length();
	
	//Create the sub string in the first element
	bResult = SubStr(&(StringArray[0]), nStrLen - nNumChars, NULL, nStrLen, NULL, (eStringFind_t)(String_UseCase | String_StartBegin));
	
	return bResult;
}

bool cStringClass_t::Right(const char *pStart) {
	bool bResult;
	uint32_t nStrLen;
	
	StringArrayCleanup(); //Results go in the array, remove any left overs
	
	//Create space for the result
	if (AddToStringArray(1) == false) {
		return false;
	}
	
	nStrLen = Length();
	
	//Create the sub string in the first element
	bResult = SubStr(&(StringArray[0]), 0, pStart, nStrLen, NULL, (eStringFind_t)(String_UseCase | String_StartBegin));
	
	return bResult;
}

bool cStringClass_t::SubStr(uint32_t nStart, uint32_t nLength) {
	uint32_t nStrLen;
	
	StringArrayCleanup(); //Results go in the array, remove any left overs
	
	//Create space for the result
	if (AddToStringArray(1) == false) {
		return false;
	}
	
	nStrLen = Length();
	
	nLength += nStart; //We need the ending index, not length
	if (nLength > nStrLen) { //don't exceed string length
		nLength = nStrLen;
	}
	
	return SubStr(&(StringArray[0]), nStart, NULL, nLength, NULL, (eStringFind_t)(String_UseCase | String_StartBegin));
}

bool cStringClass_t::SubStr(const char *pStart, uint32_t nLength) {
	uint32_t nStrLen, nStartIDX;
	
	StringArrayCleanup(); //Results go in the array, remove any left overs
	
	//Create space for the result
	if (AddToStringArray(1) == false) {
		return false;
	}
	
	nStrLen = Length();
	
	nStartIDX = Find(pStart, 0, (eStringFind_t)(String_UseCase | String_StartBegin));
	if (nStartIDX == STRING_NOTFOUND) {
		ErrorNumber = String_NotFound;
		return false;
	}
	
	nLength += nStartIDX; //We need the ending index, not length
	if (nLength > nStrLen) { //don't exceed string length
		nLength = nStrLen;
	}
	
	return SubStr(&(StringArray[0]), nStartIDX, NULL, nLength, NULL, (eStringFind_t)(String_UseCase | String_StartBegin));
}

bool cStringClass_t::SubStr(uint32_t nStart, const char *pEnd) {
	StringArrayCleanup(); //Results go in the array, remove any left overs
	
	//Create space for the result
	if (AddToStringArray(1) == false) {
		return false;
	}
	
	return SubStr(&(StringArray[0]), nStart, NULL, 0, pEnd, (eStringFind_t)(String_UseCase | String_StartBegin));
}

bool cStringClass_t::SubStr(char *pStart, char *pEnd) {
	StringArrayCleanup(); //Results go in the array, remove any left overs
	
	//Create space for the result
	if (AddToStringArray(1) == false) {
		return false;
	}
	
	return SubStr(&(StringArray[0]), 0, pStart, 0, pEnd, (eStringFind_t)(String_UseCase | String_StartBegin));
}

bool cStringClass_t::InsertText(uint32_t nPos, const char *pText) {
	uint32_t nNewLen, nCurrStrLen, nTextLen, nCtr;
	char *pNewStrBuff;
	
	nTextLen = strlen(pText);
	nCurrStrLen = Length();
	nNewLen = nTextLen + nCurrStrLen;
	
	pNewStrBuff = new char[nNewLen + 1]; //+1 for null terminator
	if (pNewStrBuff == NULL) {
		ErrorNumber = String_ErrorMemory;
		return false;
	}
	
	//Copy in text before the insertion
	for (nCtr = 0; nCtr < nPos; nCtr++) {
		pNewStrBuff[nCtr] = String[nCtr];
	}
	
	//Copy in inserted text
	for (nCtr = 0; nCtr < nTextLen; nCtr++) {
		pNewStrBuff[nCtr + nPos] = pText[nCtr];
	}
	
	//Copy in text after the insertion
	for (nCtr = nPos; nCtr < nCurrStrLen; nCtr++) {
		pNewStrBuff[nCtr + nTextLen] = String[nCtr];
	}
	
	pNewStrBuff[nNewLen] = '\0';
	
	//Put hte new buffer in place of the current string
	delete String;
	String = pNewStrBuff;
	cnStrBuffLen = nNewLen + 1; //Matches the bookkeeping of StringBufferResize() which can't be used here since we can't risk losing any of the string's contents
	
	return true;
}

bool cStringClass_t::RemoveText(uint32_t nPos, uint32_t nNumChars) {
	uint32_t nNewLen, nCurrStrLen, nCtr;
	char *pNewStrBuff;
	
	nCurrStrLen = Length();
	
	if (nNumChars > nCurrStrLen) {
		nNewLen = 0;
	} else {
		nNewLen = nCurrStrLen - nNumChars;
	}
	
	
	
	pNewStrBuff = new char[nNewLen + 1]; //+1 for null terminator
	if (pNewStrBuff == NULL) {
		ErrorNumber = String_ErrorMemory;
		return false;
	}
	
	//Copy in text before the removal
	for (nCtr = 0; nCtr < nPos; nCtr++) {
		pNewStrBuff[nCtr] = String[nCtr];
	}
	
	//Copy in text after the removat
	for (nCtr = nPos + nNumChars; nCtr < nCurrStrLen; nCtr++) {
		pNewStrBuff[nCtr - nNumChars] = String[nCtr];
	}
	
	pNewStrBuff[nNewLen] = '\0';
	
	//Put hte new buffer in place of the current string
	delete String;
	String = pNewStrBuff;
	cnStrBuffLen = nNewLen + 1; //Matches the bookkeeping of StringBufferResize() which can't be used here since we can't risk losing any of the string's contents
	
	return true;
}

bool cStringClass_t::Trim(const char *pSpace, eStringTrim_t eFlags) {
	uint32_t nStrIdx, nSpaceIdx, nOldStrLen, nSpaceLen;
	bool bIsSpace;
	
	nSpaceLen = strlen(pSpace);
	
	if (CheckAllBitsInMask(eFlags, String_TrimBegin) == true) {
		nOldStrLen = Length();
		
		for (nStrIdx = 0; nStrIdx < nOldStrLen; nStrIdx++) {
			bIsSpace = false;
			for (nSpaceIdx = 0; nSpaceIdx < nSpaceLen && bIsSpace == false; nSpaceIdx++) {
				if (String[nStrIdx] == pSpace[nSpaceIdx]) {
					bIsSpace = true;
				}
			}
			
			if ((bIsSpace == false) && (nStrIdx > 0)) { //Found a non-whitespace character, trim up to this point
				RemoveText(0, nStrIdx);
				break; //Trim beginning is done, exit the loop
			}
		}
	}
	
	
	
	if (CheckAllBitsInMask(eFlags, String_TrimEnd) == true) {
		nOldStrLen = Length(); //Get length again in case beginning trim changed it
		
		for (nStrIdx = nOldStrLen - 1; nStrIdx > 0; nStrIdx--) {
			bIsSpace = false;
			for (nSpaceIdx = 0; nSpaceIdx < nSpaceLen && bIsSpace == false; nSpaceIdx++) {
				if (String[nStrIdx] == pSpace[nSpaceIdx]) {
					bIsSpace = true;
				}
			}
			
			if ((bIsSpace == false) && (nStrIdx < nOldStrLen)) { //Found a non-whitespace character, trim after this point
				nStrIdx += 1; //Advance to that first white space character
				RemoveText(nStrIdx, nOldStrLen - nStrIdx);
				break; //Trim beginning is done, exit the loop
			}
		}
	}
	
	return true;
}

bool cStringClass_t::Trim(eStringTrim_t eFlags) {
	return Trim(STRING_WHITESPACE, eFlags);
}

bool cStringClass_t::Split(const char *pDelim, char *pExceptStart, char *pExceptEnd) {
	uint32_t nDelimIdx, nExcStartIdx, nExcEndIdx, nFindIdx, nSubIdx, nDelimLen, nExcStartLen, nExcEndLen;
	bool bResult;
	
	StringArrayCleanup(); //Results go in the array, remove any left overs
	
	if (pExceptStart != NULL) {
		nExcStartLen = strlen(pExceptStart);
	}
	
	if (pExceptEnd != NULL) {
		nExcEndLen = strlen(pExceptEnd);
	}
	
	nDelimLen = strlen(pDelim);

	nSubIdx = 0;
	nFindIdx = 0;
	nDelimIdx = Find(pDelim, nFindIdx, (eStringFind_t)(String_UseCase | String_StartBegin));
	while (nDelimIdx != STRING_NOTFOUND) {
		if (pExceptStart != NULL) {
			nExcStartIdx = Find(pExceptStart, nFindIdx, (eStringFind_t)(String_UseCase | String_StartBegin));
		} else {
			nExcStartIdx = STRING_NOTFOUND;
		}

		if (pExceptEnd != NULL) {
			nExcEndIdx = Find(pExceptEnd, nFindIdx, (eStringFind_t)(String_UseCase | String_StartBegin));
		} else {
			nExcEndIdx = STRING_NOTFOUND;
		}

		if (nExcStartIdx < nDelimIdx) { //The exception start is before the delimiter, skip past the exception range
			if (nExcEndIdx == STRING_NOTFOUND) { //No end delimiter, done splitting
				break;
			} else { //Jump past the end exception string
				nFindIdx = nExcEndIdx + nExcEndLen;
			}
		} else { //Delimiter was found first, cut a piece out
			bResult = AddToStringArray(1);
			if (bResult == false) {
				return false;
			}

			bResult = SubStr(&(StringArray[cnStrArrLen - 1]), nSubIdx, NULL, nDelimIdx, NULL, (eStringFind_t)(String_UseCase | String_StartBegin));
			if (bResult == false) {
				return false;
			}
			
			//Move past delimiter for the next search
			nFindIdx = nDelimIdx + nDelimLen;
			nSubIdx = nFindIdx;
		}
		
		//Find the next delimiter
		nDelimIdx = Find(pDelim, nFindIdx, (eStringFind_t)(String_UseCase | String_StartBegin));
	}
	
	nFindIdx = Length();
	if (nSubIdx < nFindIdx) { //Include any remaining text in the las piece
		bResult = AddToStringArray(1);
		if (bResult == false) {
			return false;
		}
		
		bResult = SubStr(&(StringArray[cnStrArrLen - 1]), nSubIdx, NULL, nFindIdx, NULL, (eStringFind_t)(String_UseCase | String_StartBegin));
		if (bResult == false) {
			return false;
		}
	}
	
	return true;
}

bool cStringClass_t::Split(const char *pDelim) {
	return Split(pDelim, NULL, NULL);
}

#endif

