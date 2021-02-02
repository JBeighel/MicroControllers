/**	@defgroup	keypaddrv
	@brief		Driver for a style of keypad that connects row pins to column
		pins when a key is pressed
	@details	v0.1
	#Description
		The keypad works by having a set of pins that correspond to the key 
		rows and another set of pins for the key columns.  When a key is 
		pressed the pin for the row and pin for the column are connected.  
		Sending a signal on a column pin then reading it from a row pin 
		indicates which button is pressed.
	
	#File Information
		File:	Keypad.h
		Author:	J. Beighel
		Date:	12-10-2020
*/

#ifndef __KEYPAD_H
	#define __KEYPAD_H

/*****	Includes	*****/
	#include "CommonUtils.h"
	#include "GPIOGeneralInterface.h"

/*****	Defines		*****/
	/**	@brief		GPIO interface capabilities required by this driver
		@details	The driver will attempt to set the pins as Input or Output.
			However, the mode can be established prior to initializing this 
			object.
		@ingroup	keypaddrv
	*/
	#define KEYPAD_GPIOCAPSREQ	(GPIOCap_DigitalWrite | GPIOCap_DigitalRead)
	
	#define KEYPAD_OUTPINS		4
	#define KEYPAD_INPINS		3

/*****	Definitions	*****/
	/**	@brief		Key mapping element
		@details	The keypad works by having a set of pins that correspond to
			the key rows and another set of pins for the key columns.  When a 
			key is pressed the pin for the row and pin for the column are 
			connected.  Sending a signal on a column pin then reading it from
			a row pin indicates which button is pressed.
			
			This object can be included in an array to map all of the keys.  
			The index of the output and input pins can be checked to read the
			ASCII character from the button pressed.
		@ingroup	keypaddrv
	*/
	typedef struct sKeypadMap_t {
		uint8_t nOutIdx;	/**< Output pin the signal is sent out */
		uint8_t nInIdx;		/**< Input pin index the signal is read back */
		char Key;			/**< ASCII chracter code for the key pressed */
	} sKeypadMap_t;

	/**	@brief		Structure containing all information needed to use keypad
		@ingroup	keypaddrv
	*/
	typedef struct sKeypadInfo_t {
		sGPIOIface_t *pGpio;				/**< Pointer to GPIO interface */
		GPIOID_t anPinOut[KEYPAD_OUTPINS];	/**< List of output pins to keypad */
		GPIOID_t anPinIn[KEYPAD_INPINS];	/**< List of input pins from keypad */
	} sKeypadInfo_t;

/*****	Constants	*****/
	

/*****	Globals		*****/


/*****	Prototypes 	*****/
	/**	@brief		Initialize the device driver for the keypad
		@param		pKeypadInfor	Device object for the keypad peripheral
		@param		pGPIOIface		GPIO interface this driver should use
		@param		aOutPins		Array of pin indexes to use as output signals
		@param		aInPins			Array of pin indexes to use as inputs
		@return		Success if initialized correctly, or code indicating failure severity
		@ingroup	keypaddrv
	*/
	eReturn_t KeypadInit(sKeypadInfo_t *pKeypadInfo, sGPIOIface_t *pGPIOIface, GPIOID_t aOutPins[KEYPAD_OUTPINS], GPIOID_t aInPins[KEYPAD_INPINS]);
	
	/**	@brief		Checks if a key is pressed on the keypad
		@param		pKeypadInfo		Device object for the keypad peripheral
		@param		bKeyDown		Returns true if a key is pressed, false otherwise
		@param		Key				Returns the ASCII character representing the key pressed
		@return		Success if initialized correctly, or code indicating failure severity
		@ingroup	keypaddrv
	*/
	eReturn_t KeypadCheckPress(sKeypadInfo_t *pKeypadInfo, bool *bKeyDown, char *Key);

/*****	Functions	*****/
	

#endif

