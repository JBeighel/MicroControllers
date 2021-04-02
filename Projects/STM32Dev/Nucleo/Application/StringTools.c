/**	#File Information
		File:	StringTools.c
		Author:	J. Beighel
		Date:	2021-04-01
*/

/*****	Includes	*****/
	#include "StringTools.h"

/*****	Defines		*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	/**	@brief		Attempt to match a regular expression token against a string
	 *	@param		strSrc		The source string to match against
	 *	@param		nSrcIdx		Index in the source to attempt the match
	 *	@param		nSrcLen		Length of the source string
	 *	@param		pPiece		Pointer to token regular expression token structure
	 *	@param		eFlags		Flags to apply in the match attempt
	 *	@return		UINT32_MAXVALUE if the token does not match.  Zero or
	 *		positive number to indicate how many characters were successfully
	 *		matched in the source
	 */
	uint32_t StrRegExTokenMatch(const char *strSrc, uint32_t nSrcIdx, uint32_t nSrcLen, sRegExPiece_t *pPiece, eRegExFlags_t eFlags);

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

eRexExReturn_t StrRegEx(const char *strSrc, const char *strRegEx, eRegExFlags_t eFlags, sRegExResult_t *pResult) {
	uint32_t nRXCtr, nSrcCtr, nMatchCtr, nRXLen, nSrcLen, nMatchCurr, nCtr, nGrpCtr;
	sRegExPiece_t RXPiece;
	bool bMatchStarted;

	pResult->nStart = 0;
	pResult->nLength = 0;
	memset(pResult->aGroups, 0, sizeof(uint32_t) * REGEX_MAXGROUPS * 2);

	nRXLen = strlen(strRegEx);
	nSrcLen = strlen(strSrc);

	bMatchStarted = false;
	nGrpCtr = 0;
	nSrcCtr = 0;
	for (nRXCtr = 0; nRXCtr < nRXLen; ) {
		RXPiece.eType = RegEx_Unknown;
		RXPiece.aToken[0] = '\0';
		RXPiece.nMinMatch = 1;
		RXPiece.nMaxMatch = 1;

		//Find the next RX Token
		switch (strRegEx[nRXCtr]) {
		case '.':	/* * * * * * * * * * * * *	Dot Expression				* * * * * * * * * * * * */
			RXPiece.eType = RegEx_Dot;
			RXPiece.aToken[0] = '.';
			RXPiece.aToken[1] = '\0';
			nRXCtr++;

			break;
		case '^':	/* * * * * * * * * * * * *	Begin Anchor Expression		* * * * * * * * * * * * */
			RXPiece.eType = RegEx_AnchorBgn;
			RXPiece.aToken[0] = '^';
			RXPiece.aToken[1] = '\0';
			nRXCtr++;

			break;
		case '$':	/* * * * * * * * * * * * *	End Anchor Expression		* * * * * * * * * * * * */
			RXPiece.eType = RegEx_AnchorEnd;
			RXPiece.aToken[0] = '$';
			RXPiece.aToken[1] = '\0';
			nRXCtr++;

			break;
		case '[':	/* * * * * * * * * * * * *	Bracket Expression			* * * * * * * * * * * * */
			nRXCtr += 1; //Advance past open bracket

			if (strRegEx[nRXCtr] == '^') { //Inverted bracket expression
				RXPiece.eType = RegEx_AnyNotCharSet;
			} else { //Normal bracket expression
				RXPiece.eType = RegEx_AnyCharSet;
			}

			//Loop through Reg Ex looking for end bracket
			nCtr = 0;
			while ((nCtr < REGEX_TOKENBUFFSIZE) && (nRXCtr < nRXLen) && (strRegEx[nRXCtr] != ']')) {
				if (strRegEx[nRXCtr] == '\\') { //Escaped character
					nRXCtr += 1; //Skip escape
					switch (strRegEx[nRXCtr]) {
					case 'n':
						RXPiece.aToken[nCtr] = '\n';
						break;
					case 'r':
						RXPiece.aToken[nCtr] = '\r';
						break;
					case 't':
						RXPiece.aToken[nCtr] = '\t';
						break;
					default:
						RXPiece.aToken[nCtr] = strRegEx[nRXCtr];
						break;
					}
				} else { //Normal character
					RXPiece.aToken[nCtr] = strRegEx[nRXCtr];
				}

				nCtr += 1;
				nRXCtr += 1;
			}

			if (nCtr >= REGEX_TOKENBUFFSIZE) { //No room for terminator character
				pResult->eResult = RX_FailTknSize;
				return pResult->eResult;
			}

			if (strRegEx[nRXCtr] != ']') {
				pResult->eResult = RX_FailBadRegEx;
				return pResult->eResult;
			}

			nRXCtr += 1; //Advance past close bracket
			RXPiece.aToken[nCtr] = '\0';

			break;
		case '(':	/* * * * * * * * * * * * *	Group Begin Expression		* * * * * * * * * * * * */
			RXPiece.eType = RegEx_GroupBgn;
			RXPiece.aToken[0] = '\0';
			RXPiece.nMinMatch = 0;
			RXPiece.nMaxMatch = 0;
			nRXCtr++;

			break;
		case ')':	/* * * * * * * * * * * * *	Group End Expression		* * * * * * * * * * * * */
			RXPiece.eType = RegEx_GroupEnd;
			RXPiece.aToken[0] = '\0';
			RXPiece.nMinMatch = 0;
			RXPiece.nMaxMatch = 0;
			nRXCtr++;

			break;
		case '\\':	/* * * * * * * * * * * * *	Escaped char Expression		* * * * * * * * * * * * */
			nRXCtr += 1; //Step over slash character

			switch (strRegEx[nRXCtr]) { //See if the next character is special
			case 'n': //Match new line/line feed
				RXPiece.eType = RegEx_AnyCharSet;
				RXPiece.aToken[0] = '\n';
				RXPiece.aToken[1] = '\0';
				nRXCtr++;

				break;
			case 'r': //Match carriage return
				RXPiece.eType = RegEx_AnyCharSet;
				RXPiece.aToken[0] = '\r';
				RXPiece.aToken[1] = '\0';
				nRXCtr++;

				break;
			case 's': //Match whitespace
				RXPiece.eType = RegEx_AnyCharSet;
				RXPiece.aToken[0] = ' ';
				RXPiece.aToken[1] = '\t';
				RXPiece.aToken[2] = '\r';
				RXPiece.aToken[3] = '\n';
				RXPiece.aToken[4] = '\0';
				nRXCtr++;

				break;
			case 'S': //Match not whitespace
				RXPiece.eType = RegEx_AnyNotCharSet;
				RXPiece.aToken[0] = ' ';
				RXPiece.aToken[1] = '\t';
				RXPiece.aToken[2] = '\r';
				RXPiece.aToken[3] = '\n';
				RXPiece.aToken[4] = '\0';
				nRXCtr++;

				break;
			case 't': //Match tab
				RXPiece.eType = RegEx_AnyCharSet;
				RXPiece.aToken[0] = '\t';
				RXPiece.aToken[1] = '\0';
				nRXCtr++;

				break;
			case 'w': //Match word, alphanumeric or underscore
				RXPiece.eType = RegEx_WordChar;
				RXPiece.aToken[0] = '\0';
				nRXCtr++;

				break;
			case 'W': //Match not word, anything that isn't alphanumeric or underscore
				RXPiece.eType = RegEx_NotWordChar;
				RXPiece.aToken[0] = '\0';
				nRXCtr++;

				break;
			default: //Escape sequence, match following character
				RXPiece.eType = RegEx_AnyCharSet;

				RXPiece.aToken[0] = strRegEx[nRXCtr];
				RXPiece.aToken[1] = '\0';

				nRXCtr++;

				break;
			}

			break;
		default:	/* * * * * * * * * * * * *	Default is self Expression		* * * * * * * * * * * * */
			RXPiece.eType = RegEx_AnyCharSet;

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
		if (bMatchStarted == false) {
			if (RXPiece.nMinMatch == 0) { //This is an optional piece, we might not find it
				//Should check for the first non-optional one but we only know 1 piece so hope for the best
				pResult->nStart = nSrcCtr;
				bMatchStarted= true;
			} else { //Non-optional, we have to find it
				//Search through the source until it begins to match
				for ( ; nSrcCtr < nSrcLen; nSrcCtr++) {
					nMatchCurr = StrRegExTokenMatch(strSrc, nSrcCtr, nSrcLen, &RXPiece, eFlags);

					if (nMatchCurr != UINT32_MAXVALUE) { //Match has begun, stop looking
						pResult->nStart = nSrcCtr;
						bMatchStarted = true;
						break;
					}
				}
			}

			if (bMatchStarted == false) {
				pResult->eResult = RX_WarnNoMatch;
				return pResult->eResult;
			}
		}

		if (RXPiece.eType == RegEx_GroupBgn) { //Mark off source index the group began
			if ((RXPiece.nMinMatch != 0) || (RXPiece.nMaxMatch != 0)) {
				//Not allowing repeated groups
				return RX_FailBadRegEx;
			}

			pResult->aGroups[nGrpCtr][0] = nSrcCtr;
			nGrpCtr += 1;
		} else if (RXPiece.eType == RegEx_GroupEnd) { //Mark off source index group ended
			if ((RXPiece.nMinMatch != 0) || (RXPiece.nMaxMatch != 0)) {
				//Not allowing repeated groups
				return RX_FailBadRegEx;
			}

			//Groups end inner (last) to outer (first)
			for (nCtr = nGrpCtr - 1; (nCtr < nGrpCtr) && (nGrpCtr >= 0); nCtr--) {
				if (pResult->aGroups[nCtr][1] == 0) {
					pResult->aGroups[nCtr][1] = nSrcCtr - pResult->aGroups[nCtr][0];
					break; //Don't close any more groups
				}
			}

			if (nCtr > nGrpCtr) { //Didn't find an open group, too many closes?
				pResult->eResult = RX_FailGroupEnd;
				return pResult->eResult;
			}
		} else {
			for (nMatchCtr = 0; nMatchCtr < RXPiece.nMinMatch; nMatchCtr++) {
				nMatchCurr = StrRegExTokenMatch(strSrc, nSrcCtr, nSrcLen, &RXPiece, eFlags);

				if (nMatchCurr == UINT32_MAXVALUE) { //Match failed
					pResult->eResult = RX_WarnNoMatch;
					return pResult->eResult;
				}

				nSrcCtr += nMatchCurr; //Advance past the matched characters
			}

			for (nMatchCtr = RXPiece.nMinMatch; nMatchCtr < RXPiece.nMaxMatch; nMatchCtr++) {
				nMatchCurr = StrRegExTokenMatch(strSrc, nSrcCtr, nSrcLen, &RXPiece, eFlags);
				//Need to see if the next piece matches to do minimal matching

				if (nMatchCurr == UINT32_MAXVALUE) { //Match failed
					nMatchCtr = RXPiece.nMaxMatch - 1; //End this matching loop
				} else {
					nSrcCtr += nMatchCurr; //Advance past the matched characters
				}
			}
		}
	}

	for (nCtr = 0; nCtr < nGrpCtr; nCtr++) {
		if (pResult->aGroups[nCtr][1] == 0) {
			//found a group that wasn't closed
			pResult->eResult = RX_FailGroupBgn;
			return pResult->eResult;
		}
	}

	pResult->nLength = nSrcCtr - pResult->nStart;\
	pResult->eResult = RX_Success;
	return pResult->eResult;
}

uint32_t StrRegExTokenMatch(const char *strSrc, uint32_t nSrcIdx, uint32_t nSrcLen, sRegExPiece_t *pPiece, eRegExFlags_t eFlags) {
	//0 or positive means it successfully matched
	//UINT32_MAXVALUE means it did not match

	char *pFind;

	switch (pPiece->eType) {
	case RegEx_Dot: //Match any character
		if (nSrcIdx < nSrcLen) { //So long as a character is there this matches it
			return 1;
		} else {
			return UINT32_MAXVALUE;
		}
	case RegEx_WordChar: //Match any letter, number, or underscore
		if (nSrcIdx >= nSrcLen) { //No characters to match
			return UINT32_MAXVALUE;
		}

		if ((isalnum((int)strSrc[nSrcIdx]) != 0)|| (strSrc[nSrcIdx] == '_')) {
			return 1;
		} else {
			return UINT32_MAXVALUE;
		}
	case RegEx_NotWordChar: //Match any letter, number, or underscore
		if (nSrcIdx >= nSrcLen) { //No characters to match
			return UINT32_MAXVALUE;
		}

		if ((isalnum((int)strSrc[nSrcIdx]) == 0) && (strSrc[nSrcIdx] != '_')) {
			return 1;
		} else {
			return UINT32_MAXVALUE;
		}
	case RegEx_AnyCharSet: //Match any from a character set
		if (nSrcIdx >= nSrcLen) { //No characters to match
			return UINT32_MAXVALUE;
		}

		if (CheckAllBitsInMask(eFlags, RX_UseCase) == true) {//Case sensitive checking
			pFind = strchr(pPiece->aToken, strSrc[nSrcIdx]);
		} else { //Ignore case in check
			pFind = strchr(pPiece->aToken, toupper(strSrc[nSrcIdx]));

			if (pFind == NULL) {
				pFind = strchr(pPiece->aToken, tolower(strSrc[nSrcIdx]));
			}
		}

		if (pFind != NULL) { //Matched the current character
			return 1;
		} else {
			return UINT32_MAXVALUE;
		}
	case RegEx_AnyNotCharSet: //Match anything not in character set
		if (nSrcIdx >= nSrcLen) { //No characters to match
			return UINT32_MAXVALUE;
		}

		if (CheckAllBitsInMask(eFlags, RX_UseCase) == true) {//Case sensitive checking
			pFind = strchr(pPiece->aToken, strSrc[nSrcIdx]);
		} else { //Ignore case in check
			pFind = strchr(pPiece->aToken, toupper(strSrc[nSrcIdx]));

			if (pFind == NULL) {
				pFind = strchr(pPiece->aToken, tolower(strSrc[nSrcIdx]));
			}
		}

		if (pFind == NULL) { //No match for current character
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

