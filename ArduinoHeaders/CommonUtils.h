/**	@defgroup	commonutils
	@brief		Common utilities and objects
	@details	v 0.1
		This is a collection of commonly used utilities.
		This includes variable types, macros, and constants.
*/

#ifndef __COMMONUTILS
	#define __COMMONUTILS

/***** Includes		*****/


/***** Constants	*****/
	/**	The maximum integer value that will fit in a uint8_t data type
		@ingroup	commonutils
	*/
	#define UINT8_MAXVALUE	255
	
	/** @brief		One of the arduino types, the Arduino/Genuino Uno
		@details
			- 14 Dedicated binary In/Out pins, 6 that can do PWM
			- 6 Dedicated analog in pins
			- 2048 bytes of RAM
			- 32K bytes of flash (512 bytes used by bootloader)
			- Logic voltage 5V
			- 1KB EEPROM
			- 1 SPI, shares binary I/O pins
		@ingroup	commonutils
	*/
	#define ARDUINO_UNO			1

	/**	@brief		One of the arduino types, the Arduino Uno
		@details
			- 14 Dedicated binary In/Out pins, 7 that can do PWM, 6 can be Analog inputs
			- 6 Dedicated analog in pins (can be used as binary I/O)
			- 2560 bytes of RAM
			- 32K bytes of flash (512 bytes used by bootloader)
			- Logic voltage 5V
			- 1KB EEPROM
			- 1 SPI, dedicated pins
		@ingroup	commonutils
	*/
	#define ARDUINO_MICRO		2

	#ifndef ARDUINO_TYPE
		/** Specifies the type of arduino being used
			Arduino/Genuino Uno is the default
			@ingroup	commonutils
		*/
		#define ARDUINO_TYPE	ARDUINO_UNO
	#endif
	
	//Based on the connected arduino, describe the I/O Pints
	#if ARDUINO_TYPE == ARDUINO_UNO
		/** Number of pulse width modulated binary output pins
			@ingroup arduinobinary
		*/
		#define BINOUT_PWMCOUNT	6

		/** Array holding all pulse width modulated binary output pins 
			@ingroup arduinobinary
		*/
		const uint8_t BINOUT_PWNPINLIST[BINOUT_PWMCOUNT] = {3, 5, 6, 9, 10, 11};

		/** Number of binary inputs on the Arduino
			@ingroup arduinobinary
		*/
		#define ARDUINO_BINPINS	14

		/** Number of analog inputs on the arduino 
			@ingroup arduinoanalog
		*/
		#define ARDUINO_ANAPINS	6

		/** The Arduno has 3 LEDs on the board tied to binary pins, this is labeled RX
			@ingroup	snapduino
		*/
		#define RX_LEDPIN		0
		
		/** The Arduno has 3 LEDs on the board tied to binary pins, this is labeled TX 
			@ingroup	snapduino
		*/
		#define TX_LEDPIN		1
		
		/** The Arduno has 3 LEDs on the board tied to binary pins, this is labeled LX 
			@ingroup	snapduino
		*/
		#define L_LEDPIN		13

		#ifndef SNAPCIR_NOSCREEN
			#ifndef SCREENPLOT_POINTS
				/** @brief		The number of points to display when plotting an analog input
					@details	The number of points that can be shown is dependenta on the free
						RAM the device has with everything else loaded.
				*/
				#define SCREENPLOT_POINTS	24
			#endif
		#endif
	#elif ARDUINO_TYPE == ARDUINO_MICRO
		/** Number of pulse width modulated binary output pins 
			@ingroup arduinobinary
		*/
		#define BINOUT_PWMCOUNT	7

		/** Array holding all pulse width modulated binary output pins 
			@ingroup arduinobinary
		*/
		const uint8_t BINOUT_PWNPINLIST[BINOUT_PWMCOUNT] = {3, 5, 6, 9, 10, 11, 12};

		/** Number of binary inputs on the Arduino 
			@ingroup arduinobinary
		*/
		#define ARDUINO_BINPINS	20

		/** Number of analog inputs on the arduino 
			@ingroup arduinoanalog
		*/
		#define ARDUINO_ANAPINS	12

		/** The Arduno has 3 LEDs on the board tied to binary pins, this is labeled RX 
			@ingroup	snapduino
		*/
		#define RX_LEDPIN		0
		
		/** The Arduno has 3 LEDs on the board tied to binary pins, this is labeled TX 
			@ingroup	snapduino
		*/
		#define TX_LEDPIN		1

		#ifndef SNAPCIR_NOSCREEN
			#ifndef SCREENPLOT_POINTS
				/** @brief		The number of points to display when plotting an analog input
					@details	The number of points that can be shown is dependenta on the free
						RAM the device has with everything else loaded.
					@ingroup	snapscreen
				*/
				#define SCREENPLOT_POINTS	128
			#endif
		#endif
	#endif 

/***** Definitions	*****/


/***** Globals		*****/


/***** Prototypes 	*****/
	/**	@brief		Check if all bits set in Mask, are set in Register
		@param		Register	The value to test
		@param		Mask		The bits to check in the Register
		@return		Will return true if all bits that are 1 in Mask are also 1 in Register.
			If any 1 bits in Mask are 0 in Register will return false.
		@ingroup	commonutils
	*/
	#define CheckAllBitsInMask(Register, Mask)	(((Register & Mask) == Mask) ? true : false)

	/**	@brief		Check if any bits set in Mask, are set in Register
		@param		Register	The value to test
		@param		Mask		The bits to check in the Register
		@return		Will return true if any bits that are 1 in Mask 1 in Register.  If no 
			1 bits in Mask are 1 in Register will return false.
		@ingroup	commonutils
	*/
	#define CheckAnyBitsInMask(Register, Mask)	(((Register & Mask) != 0) ? true : false)

	/**	@brief		Gives the value from Register with the bits set in mask cleared/zeroed
		@details	The Register value will be updated by calling this macro, its return
			is given for convenience.  The values supplied can not 
			exceed 32 bits.

			The word Zero was used in the name instead of Clear for visual clarity
			from the various Check macros.
		@param		Register	The value to be updated, having specified bits cleared
		@param		Mask		The bits to clear in Register
		@return		The value of Register after it is updated
		@ingroup	commonutils
	*/
	#define ZeroAllBitsInMask(Register, Mask)	Register = ((typeof(Register))(((uint32_t)Register) & (~(uint32_t)Mask)))

	/**	@brief		Gives the value from Register with the bits set in mask set to 1
		@details	The Register value will not be updated by calling this macro, its 
			return must be stored to retain this value.  The values supplied can not 
			exceed 32 bits.
		@param		Register	The value to be updated, having specified bits set
		@param		Mask		The bits to set in Register
		@return		The value of Register after it is updated
		@ingroup	commonutils
	*/
	#define SetAllBitsInMask(Register, Mask)	Register = ((typeof(Register))(((uint32_t)Register) | ((uint32_t)Mask)))

	/**	@brief		Returns the larger of two numeric values
		@param		nNum1	The first number to compare
		@param		nNum2	The second number to compare
		@return		The larger of the two values being compared
		@ingroup	commonutils
	*/
	#define GetLargerNum(nNum1, nNum2)		((nNum1 > nNum2) ? nNum1 : nNum2)

	/**	@brief		Returns the smaller of two numeric values
		@param		nNum1	The first number to compare
		@param		nNum2	The second number to compare
		@return		The smaller of the two values being compared
		@ingroup	commonutils
	*/
	#define GetSmallerNum(nNum1, nNum2)		((nNum1 < nNum2) ? nNum1 : nNum2)
	
	/**	@brief		Determines if a given number exists in an array
		@param		nNumber		The number to check for in the array
		@param		aList		The array to searrch through
		@param		nListCnt	The number of elements in the array
		@return		True if the number is found in the array, false if it is not
		@ingroup	commonutils
	*/
	bool NumberInArray_u8(uint8_t nNumber, const uint8_t *aList, uint32_t nListCnt);

/***** Functions	*****/
	bool NumberInArray_u8(uint8_t nNumber, const uint8_t *aList, uint32_t nListCnt) {
		uint32_t nCtr;

		for (nCtr = 0; nCtr < nListCnt; nCtr++) {
			if (nNumber == aList[nCtr]) {
				return true;
			}
		}

		return false;
	}


#endif

