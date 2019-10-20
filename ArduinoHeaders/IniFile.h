/**	@defgroup	inifile
	@brief		
	@details	
		
*/

#ifndef __INIFILE_H
	#define __INIFILE_H

/***** Includes		*****/
	#include <string.h>
	#include <SD.h>

/***** Constants	*****/


/***** Definitions	*****/
	typedef struct sIniSetting_t {
		uint16_t nIndex;
		char *pName;
		char *pValue;
	} sIniSetting_t;

	class cIniFile_t {
	public:
		cIniFile_t();
		~cIniFile_t();
		bool LoadIniFile(const char *pFileName);
		bool WriteIniFile(const char *pFileName);
		char *GetSettingValue(const char *pName, bool bCaseSensitive = true);
		bool SetSettingValue(const char *pName, const char *pValue);

	protected:
		sIniSetting_t	*cpSettingList;
		uint16_t		cnSettingNum;

		sIniSetting_t *FindSetting(const char * pName, bool bCaseSensitive);
		bool AddNewSetting(sIniSetting_t *pNewSetting);

	private:
	};

/***** Globals		*****/


/***** Prototypes 	*****/


/***** Functions	*****/
cIniFile_t::cIniFile_t() {
	cpSettingList = NULL;
	cnSettingNum = 0;

	return;
}

cIniFile_t::~cIniFile_t() {
	uint16_t nCtr;

	if (cpSettingList != NULL) {
		for (nCtr = 0; nCtr < cnSettingNum; nCtr++) {
			if (cpSettingList[nCtr].pName != NULL) {
				delete cpSettingList[nCtr].pName;
				cpSettingList[nCtr].pName = NULL;
			}

			if (cpSettingList[nCtr].pValue != NULL) {
				delete cpSettingList[nCtr].pValue;
				cpSettingList[nCtr].pValue = NULL;
			}
		}

		delete cpSettingList;
		cpSettingList = NULL;
	}

	return;
}

bool cIniFile_t::LoadIniFile(const char *pFileName) {
	File hFile;
	bool bFound;
	char *pLine, *pTemp;
	uint32_t nLineLen, nLineBuff, nStartIdx, nStopIdx, nCtr;
	sIniSetting_t NewSetting;

	if (SD.exists(pFileName) == false) {
		return false;
	}

	hFile = SD.open(pFileName, FILE_READ);

	pLine = new char[64];
	if (pLine == NULL) {
		return false;
	}

	nLineBuff = 64;
	nLineLen = 0;

	NewSetting.pName = NULL;
	NewSetting.pValue = NULL;

	while (hFile.available() != 0) { //Available returns the number of bytes available
		if (nLineLen >= nLineBuff) { //Reached the end of the buffer, extend it
			pTemp = pLine;

			//Create a new larger buffer to use
			nLineBuff += 64;
			pLine = new char[nLineBuff];
			if (pLine == NULL) {
				delete pTemp;
				return false;
			}

			//Copy the old date in
			memcpy(pLine, pTemp, nLineBuff - 64);
			delete pTemp;
		}

		hFile.read(&(pLine[nLineLen]), sizeof(char));
		if (hFile.available() == 0) { //At end of file, won't find another \n
			nLineLen += 1;
			pLine[nLineLen] = '\n'; //Add a spare \n to ensure the formatting is appeased
		}

		if (pLine[nLineLen] == '=') { //Equals reached, parse out name
			pLine[nLineLen] = '\0'; //Switch to null terminator at the end

			bFound = false;
			for (nStartIdx = 0; nStartIdx < nLineLen && bFound == false; nStartIdx++) {
				//0x21 is '!' and 0x7E is '~', which are the ends of the printable ascii characters
				if ((pLine[nStartIdx] >= 0x21) && (pLine[nStartIdx] <= 0x7E)) {
					nStartIdx -= 1; //The loop counts 1 extra, so remove that
					bFound = true; //Set this to exit for loop
				}
			}

			bFound = false;
			for (nStopIdx = nLineLen; nStopIdx > 0 && bFound == false; nStopIdx--) {
				//0x21 is '!' and 0x7E is '~', which are the ends of the printable ascii characters
				if ((pLine[nStopIdx] >= 0x21) && (pLine[nStopIdx] <= 0x7E)) {
					nStopIdx += 1; //Loop ends on the last valid character, move 1 past that
					bFound = true; //Set this to exit for loop
				}
			}

			if (nStopIdx >= nStartIdx) {
				NewSetting.pName = new char[(nStopIdx - nStartIdx) + 2]; //Create space for the name string and null terminator
				if (NewSetting.pName == NULL) {
					return false;
				}
			
				for (nCtr = nStartIdx; nCtr <= nStopIdx; nCtr++) { //Copy in the name value
					NewSetting.pName[nCtr - nStartIdx] = pLine[nCtr];
				}
				NewSetting.pName[nCtr - nStartIdx] = '\0';
			}

			nLineLen = 0;
		} else if (pLine[nLineLen] == '\n') { //End of line reached, parse out the value 
			pLine[nLineLen] = '\0'; //Switch to null terminator at the end

			bFound = false;
			for (nStartIdx = 0; nStartIdx < nLineLen && bFound == false; nStartIdx++) {
				//0x21 is '!' and 0x7E is '~', which are the ends of the printable ascii characters
				if ((pLine[nStartIdx] >= 0x21) && (pLine[nStartIdx] <= 0x7E)) {
					nStartIdx -= 1; //The loop counts 1 extra, so remove that
					bFound = true; //Set this to exit for loop
				}
			}

			bFound = false;
			for (nStopIdx = nLineLen; nStopIdx > 0 && bFound == false; nStopIdx--) {
				//0x21 is '!' and 0x7E is '~', which are the ends of the printable ascii characters
				if ((pLine[nStopIdx] >= 0x21) && (pLine[nStopIdx] <= 0x7E)) {
					nStopIdx += 1; //Loop ends on the last valid character, move 1 past that
					bFound = true; //Set this to exit for loop
				}
			}

			if (nStopIdx >= nStartIdx) {
				NewSetting.pValue = new char[(nStopIdx - nStartIdx) + 2]; //Create space for the name string and null terminator
				if (NewSetting.pValue == NULL) {
					return false;
				}

				for (nCtr = nStartIdx; nCtr <= nStopIdx; nCtr++) { //Copy in the name value
					NewSetting.pValue[nCtr - nStartIdx] = pLine[nCtr];
				}
				NewSetting.pValue[nCtr - nStartIdx] = '\0';
			}

			if ((NewSetting.pName != NULL) && (NewSetting.pValue != NULL)) {//This setting is now fully parsed, add it to the list
				bFound = AddNewSetting(&NewSetting);

				if (bFound == false) {
					return false;
				}
			} else { //Some portion of the setting is missing, dump and continue
				if (NewSetting.pName != NULL) {
					delete NewSetting.pName;
				}

				if (NewSetting.pValue != NULL) {
					delete NewSetting.pValue;
				}
			}

			//Reset for the next setting
			nLineLen = 0;
			NewSetting.pName = NULL;
			NewSetting.pValue = NULL;
		} else {
			nLineLen += 1;
		}
	}

	hFile.close();

	return true;
}

bool cIniFile_t::WriteIniFile(const char *pFileName) {
	return false;
}

char *cIniFile_t::GetSettingValue(const char *pName, bool bCaseSensitive) {
	sIniSetting_t *pSetting;

	pSetting = FindSetting(pName, bCaseSensitive);

	if (pSetting != NULL) {
		return pSetting->pValue;
	} else {
		return NULL;
	}
}

bool cIniFile_t::SetSettingValue(const char *pName, const char *pValue) {
	return false;
}

sIniSetting_t *cIniFile_t::FindSetting(const char * pName, bool bCaseSensitive) {
	uint16_t nCtr;

	if ((cpSettingList == NULL) || (cnSettingNum == 0)) { //No list to search
		return NULL;
	}

	for (nCtr = 0; nCtr < cnSettingNum; nCtr++) {
		if (strcasecmp(pName, cpSettingList[nCtr].pName) == 0) {
			return &(cpSettingList[nCtr]);
		}
	}

	//Couldn't find the requested setting
	return NULL;
}

bool cIniFile_t::AddNewSetting(sIniSetting_t *pNewSetting) {
	sIniSetting_t *pTempBuff;

	if ((cpSettingList == NULL) || (cnSettingNum == 0)) { //No list exists, make new
		cnSettingNum = 0;

		cpSettingList = new sIniSetting_t[cnSettingNum + 1];
		if (cpSettingList == NULL) {
			return false;
		}
	} else { //Increase current list
		pTempBuff = cpSettingList;

		//Create the new buffer
		cpSettingList = new sIniSetting_t[cnSettingNum + 1];
		if (cpSettingList == NULL) {
			cnSettingNum = 0;
			return false;
		}

		//Copy in all old entries
		memcpy(cpSettingList, pTempBuff, sizeof(sIniSetting_t) * cnSettingNum);

		//Delete the old buffer
		delete pTempBuff;
	}

	//Copy in the new setting data
	cpSettingList[cnSettingNum].pName = pNewSetting->pName;
	cpSettingList[cnSettingNum].pValue = pNewSetting->pValue;
	cpSettingList[cnSettingNum].nIndex = cnSettingNum;
	cnSettingNum += 1;

	return true;
}

#endif

