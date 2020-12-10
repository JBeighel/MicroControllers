/**	File:	Keypad.c
	Author:	J. Beighel
	Date:	12-10-2020
*/

/*****	Includes	*****/
	#include "Keypad.h"

/*****	Defines		*****/


/*****	Definitions	*****/


/*****	Constants	*****/
	/**	@brief		Keypad map for the 12 button phone pad
		@details	This keypad has 4 row pins and 3 column pins.  The rows are
			the outputs and the columns are the inputs.  Index 0 is the top or 
			left key with the index increase down or to the right.
		@ingroup	keypaddrv
	*/
	const sKeypadMap_t gaKeyMap[] = {
		{ .nOutIdx = 0, .nInIdx = 0, .Key = '1', },
		{ .nOutIdx = 0, .nInIdx = 1, .Key = '2', },
		{ .nOutIdx = 0, .nInIdx = 2, .Key = '3', },
		
		{ .nOutIdx = 1, .nInIdx = 0, .Key = '4', },
		{ .nOutIdx = 1, .nInIdx = 1, .Key = '5', },
		{ .nOutIdx = 1, .nInIdx = 2, .Key = '6', },
		
		{ .nOutIdx = 2, .nInIdx = 0, .Key = '7', },
		{ .nOutIdx = 2, .nInIdx = 1, .Key = '8', },
		{ .nOutIdx = 2, .nInIdx = 2, .Key = '9', },
		
		{ .nOutIdx = 3, .nInIdx = 0, .Key = '*', },
		{ .nOutIdx = 3, .nInIdx = 1, .Key = '0', },
		{ .nOutIdx = 3, .nInIdx = 2, .Key = '#', },
	};

/*****	Globals		*****/


/*****	Prototypes 	*****/
	/**	@brief		Checks the pins used to send and read the singal against a key map
		@param		pMap		Pointer to the array of keymap structures matching
			output and input pins to the ASCII character beign pressed
		@param		nMapSize	Number of elements in the map array
		@param		nOutIdx		Index of the output pin sending the signal
		@param		nInIdx		Index of the input pin receiving the signal
		@param		pKey		ASCII code of the key beign pressed
		@return		True if the Out and In combination is a keypress, false if the 
			combination can not be found in the map
		@ingroup	keypaddrv
	*/
	bool KeypadFindButtonInMap(const sKeypadMap_t *pMap, uint32_t nMapSize, uint8_t nOutIdx, uint8_t nInIdx, char *pKey);

/*****	Functions	*****/
eReturn_t KeypadInit(sKeypadInfo_t *pKeypadInfo, sGPIOIface_t *pGPIOIface, GPIOID_t aOutPins[KEYPAD_OUTPINS], GPIOID_t aInPins[KEYPAD_INPINS]) {
	uint8_t nCtr;
	
	//Initialize the keypad infor structure
	pKeypadInfo->pGpio = pGPIOIface;
	
	for (nCtr = 0; nCtr < KEYPAD_OUTPINS; nCtr++) {
		pKeypadInfo->anPinOut[nCtr] = aOutPins[nCtr];
		
		//Request the mode and set default state
		pKeypadInfo->pGpio->pfSetModeByPin(pKeypadInfo->pGpio, aOutPins[nCtr], GPIO_DigitalOutput);
		pKeypadInfo->pGpio->pfDigitalWriteByPin(pKeypadInfo->pGpio, aOutPins[nCtr], false);
	}
	
	for (nCtr = 0; nCtr < KEYPAD_INPINS; nCtr++) {
		pKeypadInfo->anPinIn[nCtr] = aInPins[nCtr];
		
		//Request the mode (pull up/down resistors are needed)
		pKeypadInfo->pGpio->pfSetModeByPin(pKeypadInfo->pGpio, aInPins[nCtr], GPIO_DigitalInput);
	}
	
	return Success;
}
	
eReturn_t KeypadCheckPress(sKeypadInfo_t *pKeypadInfo, bool *bKeyDown, char *Key) {
	uint8_t nOutCtr, nInCtr;
	bool bState;

	//Set no button press defaults
	*bKeyDown = false;
	*Key = 0;
	
	//Put a signal through each of th eoutpts looking for it to appear on an input
	for (nOutCtr = 0; nOutCtr < KEYPAD_OUTPINS; nOutCtr++) {
		//Set this output High to detect its inputs
		pKeypadInfo->pGpio->pfDigitalWriteByPin(pKeypadInfo->pGpio, pKeypadInfo->anPinOut[nOutCtr], true);
		
		//Loop throuh all inputs and see if any see the inputs
		for (nInCtr = 0; nInCtr < KEYPAD_INPINS; nInCtr++) {
			pKeypadInfo->pGpio->pfDigitalReadByPin(pKeypadInfo->pGpio, pKeypadInfo->anPinIn[nInCtr], &bState);
			
			if (bState == true) { //Found a pressed button
				*bKeyDown = true;
				
				KeypadFindButtonInMap(gaKeyMap, KEYPAD_OUTPINS * KEYPAD_INPINS, nOutCtr, nInCtr, Key);
		
				pKeypadInfo->pGpio->pfDigitalWriteByPin(pKeypadInfo->pGpio, pKeypadInfo->anPinOut[nOutCtr], false);
				return Success;
			}
		}
		
		//Return it low to avoid confusion on the next out
		pKeypadInfo->pGpio->pfDigitalWriteByPin(pKeypadInfo->pGpio, pKeypadInfo->anPinOut[nOutCtr], false);
	}
}

bool KeypadFindButtonInMap(const sKeypadMap_t *pMap, uint32_t nMapSize, uint8_t nOutIdx, uint8_t nInIdx, char *pKey) {
	uint32_t nCtr;
	
	*pKey = 0;
	
	for (nCtr = 0; nCtr < nMapSize; nCtr++) {
		if (pMap[nCtr].nOutIdx != nOutIdx) {
			continue;
		}
		
		if (pMap[nCtr].nInIdx != nInIdx) {
			continue;
		}
		
		*pKey = pMap[nCtr].Key;
		return true;
	}
	
	return false;
}