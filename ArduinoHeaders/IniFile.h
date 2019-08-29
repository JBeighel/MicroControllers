/**	@defgroup	inifile
	@brief		
	@details	
		
*/

#ifndef __INIFILE_H
	#define __INIFILE_H

/***** Includes		*****/
	#include <SD.h>

/***** Constants	*****/


/***** Definitions	*****/
	typedef struct sIniSetting_t {
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

		if (pLine[nLineLen] == '=') { //Equals reached, parse out name
			pLine[nLineLen] = '\0'; //Switch to null terminator at the end
Serial.println("Name complete");
delay(50);
Serial.println(pLine);
delay(50);

			bFound = false;
			for (nStartIdx = 0; nStartIdx < nLineLen && bFound == false; nStartIdx++) {
				//0x21 is '!' and 0x7E is '~', which are the ends of the printable ascii characters
				if ((pLine[nStartIdx] >= 0x21) && (pLine[nStartIdx] <= 0x7E)) {
					bFound = true; //Set this to exit for loop
				}
			}

			bFound = false;
			for (nStopIdx = nLineLen; nStopIdx < nLineLen && bFound == false; nStopIdx--) {
				//0x21 is '!' and 0x7E is '~', which are the ends of the printable ascii characters
				if ((pLine[nStopIdx] >= 0x21) && (pLine[nStopIdx] <= 0x7E)) {
					bFound = true; //Set this to exit for loop
				}
			}

			NewSetting.pName = new char[(nStopIdx - nStartIdx) + 1]; //Create space for the name string and null terminator
			for (nCtr = nStartIdx; nCtr < nStopIdx; nCtr++) { //Copy in the name value
				NewSetting.pName[nCtr - nStartIdx] = pLine[nCtr];
			}
			NewSetting.pName[nCtr - nStartIdx] = '\0';
			nLineLen = 0;
		} else if (pLine[nLineLen] == '\n') { //End of line reached, parse out the value 
			pLine[nLineLen] = '\0'; //Switch to null terminator at the end
Serial.println("Value complete");
delay(50);
Serial.println(pLine);
delay(50);

			bFound = false;
			for (nStartIdx = 0; nStartIdx < nLineLen && bFound == false; nStartIdx++) {
				//0x21 is '!' and 0x7E is '~', which are the ends of the printable ascii characters
				if ((pLine[nStartIdx] >= 0x21) && (pLine[nStartIdx] <= 0x7E)) {
					bFound = true; //Set this to exit for loop
				}
			}

			bFound = false;
			for (nStopIdx = nLineLen; nStopIdx < nLineLen && bFound == false; nStopIdx--) {
				//0x21 is '!' and 0x7E is '~', which are the ends of the printable ascii characters
				if ((pLine[nStopIdx] >= 0x21) && (pLine[nStopIdx] <= 0x7E)) {
					bFound = true; //Set this to exit for loop
				}
			}

			NewSetting.pName = new char[(nStopIdx - nStartIdx) + 1]; //Create space for the name string and null terminator
			for (nCtr = nStartIdx; nCtr < nStopIdx; nCtr++) { //Copy in the name value
				NewSetting.pValue[nCtr - nStartIdx] = pLine[nCtr];
			}
			NewSetting.pValue[nCtr - nStartIdx] = '\0';

			if ((NewSetting.pName != NULL) && (NewSetting.pValue != NULL)) {//This setting is now fully parsed, add it to the list
				bFound = AddNewSetting(&NewSetting);

				if (bFound == false) {
					return false;
				}
			} else { //Some portion of the setting is missing, dump and continue
Serial.println("Invalid line");
delay(50);
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
Serial.println("End of file");

	hFile.close();

	return true;
}

bool cIniFile_t::WriteIniFile(const char *pFileName) {
	return false;
}

char *cIniFile_t::GetSettingValue(const char *pName, bool bCaseSensitive) {
	return NULL;
}

bool cIniFile_t::SetSettingValue(const char *pName, const char *pValue) {
	return false;
}

sIniSetting_t *cIniFile_t::FindSetting(const char * pName, bool bCaseSensitive) {
	return NULL;
}

bool cIniFile_t::AddNewSetting(sIniSetting_t *pNewSetting) {
	delete pNewSetting->pName;
	delete pNewSetting->pValue;

	return true;
}

#endif

