/**	@defgroup	keypaddrv
	@brief		
	@details	v0.1
	#Description
	
	#File Information
		File:	Keypad.h
		Author:	J. Beighel
		Date:	12-02-2020
*/

#ifndef __KEYPAD_H
	#define __KEYPAD_H

/*****	Includes	*****/
	#include "CommonUtils.h"
	#include "GPIOGeneralInterface.h"

/*****	Defines		*****/
	#define KEYPAD_GPIOCAPSREQ	(GPIOCap_DigitalWrite | GPIOCap_DigitalRead)
	
	#define KEYPAD_OUTPINS		4
	#define KEYPAD_INPINS		3

/*****	Definitions	*****/
	typedef struct sKeypadMap_t {
		uint8_t nOutIdx;
		uint8_t nInIdx;
		char Key;
	} sKeypadMap_t;

	typedef struct sKeypadInfo_t {
		sGPIOIface_t *pGpio;
		GPIOID_t anPinOut[KEYPAD_OUTPINS];
		GPIOID_t anPinIn[KEYPAD_INPINS];
	} sKeypadInfo_t;

/*****	Constants	*****/
	

/*****	Globals		*****/
//Out pins 17, 27, 22, 18
//In pins 23, 24, 25

/*****	Prototypes 	*****/
	eReturn_t KeypadInit(sKeypadInfo_t *pKeypadInfo, sGPIOIface_t *pGPIOIface, GPIOID_t aOutPins[KEYPAD_OUTPINS], GPIOID_t aInPins[KEYPAD_INPINS]);
	
	eReturn_t KeypadCheckPress(sKeypadInfo_t *pKeypadInfo, bool *bKeyDown, char *Key);

/*****	Functions	*****/
	

#endif

