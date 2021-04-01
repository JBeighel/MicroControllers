/**	#File Information
		File:	StringTools.c
		Author:	J. Beighel
		Date:	2021-03-31
*/

/*****	Includes	*****/
	#include "StringTools.h"

/*****	Defines		*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
uint32_t StrRegExTokenMatch(const char *strSrc, uint32_t nSrcIdx, uint32_t nSrcLen, sRegExPiece_t *pPiece);

/*****	Functions	*****/
uint32_t StrFind(const char *strSrc, const char *strFind) {
	uint32_t nIdx, nSrcLen, nFindLen;
	const char *pStart;

	nSrcLen = strlen(strSrc);
	nFindLen = strlen(strFind);

	pStart = strSrc;

	while (pStart != NULL) {
		pStart = strchr(strSrc, strFind[0]);

		if (pStart == NULL) {
			break;
		}

		nIdx = pStart - strSrc; //how many bytes is this start?

		if (nSrcLen - nIdx < nFindLen) { //Not enough space left
			break;
		}

		if (strncmp(pStart, strFind, nFindLen) == 0) { //Found it!
			return nIdx;
		} else { //Keep advancing through the string
			pStart += 1;
		}
	}

	//Couldn't find the string in the source
	return UINT32_MAXVALUE;
}

uint32_t StrFindFirstChar(const char *strSrc, const char *strChars) {
	uint32_t nFirstIdx, nCurrIdx, nCtr, nCharsLen;
	const char *pStart;

	nCharsLen = strlen(strChars);

	nFirstIdx = UINT32_MAXVALUE;
	for (nCtr = 0; nCtr < nCharsLen; nCtr++) {
		pStart = strchr(strSrc, strChars[nCtr]);

		if (pStart != NULL) { //Found a matching character
			nCurrIdx = pStart - strSrc;

			if (nCurrIdx < nFirstIdx) { //And its earlier in the string
				nFirstIdx = nCurrIdx;
			}
		}
	}

	return nFirstIdx;
}

eReturn_t StrTrimStart(char *strSrc, const char *strTrim) {
	uint32_t nCtr, nIdx, nSrcLen;
	char *pStart;

	nSrcLen = strlen(strSrc);

	//Find out how many characters are being removed
	for (nCtr = 0; nCtr < nSrcLen; nCtr++) {
		pStart = strchr(strTrim, strSrc[nCtr]);

		if (pStart == NULL) { //Found a character not to trim
			break;
		}
	}

	//Now chop off that many characters
	nIdx = nCtr; //Trim off this many characters
	nSrcLen -= nIdx; //Reduce length by the trimmed amount
	for (nCtr = 0; nCtr < nSrcLen; nCtr++) {
		strSrc[nCtr] = strSrc[nCtr + nIdx];
	}

	strSrc[nSrcLen] = '\0'; //Just to make sure its terminated

	return Success;
}

eReturn_t StrTrimEnd(char *strSrc, const char *strTrim) {
	uint32_t nCtr, nSrcLen;
	char *pStart;

	nSrcLen = strlen(strSrc);

	//Find out how many characters are being removed
	for (nCtr = nSrcLen - 1; nCtr >= 0; nCtr--) {
		pStart = strchr(strTrim, strSrc[nCtr]);

		if (pStart == NULL) { //Found a character not to trim
			break;
		}
	}

	strSrc[nCtr + 1] = '\0'; //Reposition the terminator to the last trimmed spot

	return Success;
}

eReturn_t StrPiece(const char *strSrc, uint32_t nStartIdx, uint32_t nLen, char *strDest) {
	uint32_t nCtr, nEnd, nSrcLen;

	//Figure out the last character to copy
	nSrcLen = strlen(strSrc);
	nEnd = nStartIdx + nLen;

	if (nEnd > nSrcLen) {
		nEnd = nSrcLen;
	}

	for (nCtr = nStartIdx; nCtr <nEnd; nCtr++) {
		strDest[nCtr - nStartIdx] = strSrc[nCtr];
	}

	strDest[nCtr - nStartIdx] = '\0';

	return Success;
}

eReturn_t StrRegEx(const char *strSrc, const char *strRegEx) {
	uint32_t nRXCtr, nSrcCtr, nMatchCtr, nRXLen, nSrcLen, nMatchCurr;
	sRegExPiece_t RXPiece;

	nRXLen = strlen(strRegEx);
	nSrcLen = strlen(strSrc);

	nSrcCtr = 0;
	for (nRXCtr = 0; nRXCtr < nRXLen; ) {
		RXPiece.eType = RegEx_Unknown;
		RXPiece.aToken[0] = '\0';
		RXPiece.nMinMatch = 1;
		RXPiece.nMaxMatch = 1;

		//Find the next RX Token
		switch (strRegEx[nRXCtr]) {
		case '.':
			RXPiece.eType = RegEx_Dot;
			RXPiece.aToken[0] = '.';
			RXPiece.aToken[1] = '\0';
			nRXCtr++;

			break;
		case '^':
			RXPiece.eType = RegEx_AnchorBgn;
			RXPiece.aToken[0] = '^';
			RXPiece.aToken[1] = '\0';
			nRXCtr++;

			break;
		case '$':
			RXPiece.eType = RegEx_AnchorEnd;
			RXPiece.aToken[0] = '$';
			RXPiece.aToken[1] = '\0';
			nRXCtr++;

			break;
		case '\\':
			nRXCtr += 1; //Slash means match the next character
		default:
			RXPiece.eType = RegEx_Self;
			RXPiece.aToken[0] = strRegEx[nRXCtr];
			RXPiece.aToken[1] = '\0';
			nRXCtr++;

			break;
		}

		//See if there's any repeat markers
		switch (strRegEx[nRXCtr]) {
		case '*': //Match 0 or more
			RXPiece.nMinMatch = 0;
			RXPiece.nMaxMatch = UINT32_MAXVALUE;
			nRXCtr++;

			break;
		case '+': //match 1 or more
			RXPiece.nMinMatch = 1;
			RXPiece.nMaxMatch = UINT32_MAXVALUE;
			nRXCtr++;

			break;
		case '?': //Match 0 or 1
			RXPiece.nMinMatch = 0;
			RXPiece.nMaxMatch = 1;
			nRXCtr++;

			break;
		default: //No match marker
			break;
		}

		//We have the piece, check it against the source string
		for (nMatchCtr = 0; nMatchCtr < RXPiece.nMinMatch; nMatchCtr++) {
			nMatchCurr = StrRegExTokenMatch(strSrc, nSrcCtr, nSrcLen, &RXPiece);

			if (nMatchCurr == UINT32_MAXVALUE) { //Match failed
				return Fail_Unknown;
			}

			nSrcCtr += nMatchCurr; //Advance past the matched characters
		}

		for (nMatchCtr = RXPiece.nMinMatch; nMatchCtr < RXPiece.nMaxMatch; nMatchCtr++) {
			nMatchCurr = StrRegExTokenMatch(strSrc, nSrcCtr, nSrcLen, &RXPiece);
			//Need to see if the next piece matches to do min matching

			if (nMatchCurr == UINT32_MAXVALUE) { //Match failed
				nMatchCtr = RXPiece.nMaxMatch - 1; //End this matching loop
			} else {
				nSrcCtr += nMatchCurr; //Advance past the matched characters
			}
		}
	}

	return Success;
}

uint32_t StrRegExTokenMatch(const char *strSrc, uint32_t nSrcIdx, uint32_t nSrcLen, sRegExPiece_t *pPiece) {
	//0 or positive means it successfully matched
	//UINT32_MAXVALUE means it did not match

	switch (pPiece->eType) {
	case RegEx_Dot: //Match any character
		if (nSrcIdx < nSrcLen) { //So long as a character is there this matches it
			return 1;
		} else {
			return UINT32_MAXVALUE;
		}
	case RegEx_Self: //Match itself only
		if (strSrc[nSrcIdx] == pPiece->aToken[0]) { //Matched the current character
			return 1;
		} else {
			return UINT32_MAXVALUE;
		}
	case RegEx_AnchorBgn: //Match beginning of string or line (preceded by new line)
		if ((nSrcIdx == 0) || (strSrc[nSrcIdx - 1] == '\n')) {
			return 0;
		} else {
			return UINT32_MAXVALUE;
		}
	case RegEx_AnchorEnd: //Match Ending of string or line (followed by new line)
		if ((nSrcIdx == nSrcLen) || (strSrc[nSrcIdx + 1] == '\n')) {
			return 0;
		} else {
			return UINT32_MAXVALUE;
		}
	default: //Should not reach here
		return Fail_Invalid;
	}
}
