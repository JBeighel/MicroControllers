/**	@defgroup	string
	@brief		
	@details	
		
*/

#ifndef __STRINGCLASS_H
	#define __STRINGCLASS_H

/***** Includes		*****/
#include<string.h>

#include "CommonUtils.h"

/***** Constants	*****/
#define STRING_NOTFOUND		((uint32_t)-1)

/***** Definitions	*****/
enum eStringFind_t {
	String_UseCase		= 0x00,
	String_NoCase		= 0x01,
	String_StartBegin	= 0x02,
	String_StartEnd		= 0x04,
};

enum eStringResult_t {
	String_Success		= 0,
	String_NotFound		= 1,
	String_ErrorBounds	= 2,
	String_ErrorMemory	= 3,
	String_RegExError	= 4,
	String_RegExNoMatch	= 5,
	String_NoStr		= 6,
	String_RegExNoRegEx	= 7,
	String_RegExSyntax	= 8,
};

enum eStringTrim_t {
	String_TrimBegin	= 0x01,
	String_TrimEnd		= 0x02,
	String_TrimLF		= 0x04,
	String_TrimCR		= 0x08,
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
	bool CleanUp(void);
	bool SetString(const char *pText, ...);
	bool AddToString(const char *pText, ...);
	uint32_t Length(void);
	uint32_t Find(const char *pText, uint32_t nStart, eStringFind_t eFlags = (eStringFind_t)(String_UseCase | String_StartBegin));
	bool SubStr(uint32_t nStart, uint32_t nLength);
	bool SubStr(const char *nStart, uint32_t nLength);
	bool SubStr(uint32_t nStart, const char *pEnd);
	bool SubStr(char *pStart, char *pEnd);
	bool Left(uint32_t nNumChars);
	bool Left(const char *pEnd);
	bool Right(uint32_t nNumChars);
	bool Right(const char *pEnd);
	bool Split(const char *pDelim, char *pExceptStart, char *pExceptEnd);
	bool Split(const char *pDelim);
	bool AddDecimal(double nNum, uint8_t nNumDecimals);
	bool Trim(const char *pSpace, eStringTrim_t eFlags);
	bool Trim(const char *pSpace);
	bool Trim(eStringTrim_t eFlags);
	bool Trim(void);
	bool InsertText(char *pText, uint32_t nPos);
	bool RemoveText(uint32_t nPos, uint32_t nCount);

	bool RegExTest(const char *pRegEx, uint32_t nStart, uint32_t *pnNumChars);

	uint32_t ErrorNumber;
	char *String, **StringArray;

protected:
	bool AddText(const char *pSearch, uint32_t nSkipCharNum, uint32_t nNumChars);
	bool SubStr(char **hStr, unsigned int nStart, char *pTxtStart, unsigned int nEnd, char *pTxtEnd, eStringFind_t eFlags);

private:
	uint32_t RegExParse(const char *pRegEx, uint32_t nCharNum);
	bool StringArrayCleanup();
	bool RegExCleanup(sStringRegExPiece_t *pPieceList);
	uint32_t ExPieceTest(uint32_t nPlace, sStringRegExPiece_t *pCurrPiece, sStringRegExPiece_t *pNextPiece);
	bool StringBufferResize(uint32_t pNewSetLen, uint32_t nCharsToAdd);

	uint32_t cnStrBuffLen, cnStrArrLen, cnRegExPieceLen;
	sStringRegExPiece_t *cpRegExPieceList;
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
			}
		}
		
		delete this->StringArray;
	}
	this->StringArray = NULL;
	
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
	
	if (cnStrBuffLen > nNewStrLen) { //Buffer is already large enough, make no change
		return true;
	}
	
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
		nFindStartIdx = nStrLen; //Start at end
		nFindEndIdx = nSkipCharNum - 1; //Go to beginning (-1 so that the loop hits all characters)
		nFindIncrement = -1; //Increment down
	} else { //Searching forwards through the string
		nFindStartIdx = 0; //Start at beginning
		nFindEndIdx = nStrLen - nSkipCharNum; //Go to end
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

bool SubStr(char **hStr, unsigned int nStart, char *pTxtStart, unsigned int nEnd, char *pTxtEnd, eStringFind_t eFlags) {
	uint32_t nStrLen, nSubLen, nCtr;

	nStrLen	= Length();
	
	StringArrayCleanup(); //Results go in the array, remove any garbage
	
	if (pTxtStart != NULL) { //Starting on a string
		nStart = Find(pTxtStart, 0, eFlags);
		if (nStart == STRING_NOTFOUND) {
			return false;
		}
		
		nStart += strlen(pTxtStart); //Make sure the start text isn't included in the substring
	}
	
	if (pTxtEnd != NULL) { //Ending on a string match
		nStart = Find(pTxtStart, 0, eFlags);
		if (nStart == STRING_NOTFOUND) {
			return false;
		}
	}
	
	if (nStart > nEnd) { //Start and stop are out of order
		ErrorNumber = String_ErrorBounds;
		return false;
	} else if (nEnd >= nStrLen) {
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

#endif

