/**	File:	Keypad.c
	Author:	J. Beighel
	Date:	12-02-2020
*/

/*****	Includes	*****/
	#include "Keypad.h"

/*****	Defines		*****/


/*****	Definitions	*****/


/*****	Constants	*****/
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
				//Index nOutCtr, nInCtr
				//Set *Key accordingly
				*bKeyDown = true;
			}
		}
		
		//Return it low to avoid confusion on the next out
		pKeypadInfo->pGpio->pfDigitalWriteByPin(pKeypadInfo->pGpio, pKeypadInfo->anPinOut[nOutCtr], false);
	}
}
