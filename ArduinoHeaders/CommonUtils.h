/**	@defgroup	commonutils
	@brief		Common utilities and objects
	@details	v 0.2
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
	
	/**	The maximum integer value that will fit in a uint16_t data type
		@ingroup	commonutils
	*/
	#define UINT16_MAXVALUE	0xFFFF
	
	/**	The maximum integer value that will fit in a uint32_t data type
		@ingroup	commonutils
	*/
	#define UINT32_MAXVALUE	0xFFFFFFFF
	
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
	
	/**	@brief		One of the arduino types, the Arduino Nano
		@details	There are multiple varieties of the Nano, all have identical pinouts
			- 22 Dedicated binary In/Out pins, 6 that can do PWM, 8 can be Analog inputs
			- 2048+ bytes of RAM
			- 32K+ bytes of flash (512 bytes used by bootloader)
			- Logic voltage varies per model
			- 1KB EEPROM
			- 1 SPI, dedicated pins
		@ingroup	commonutils
	*/
	#define ARDUINO_NANO		3

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
		
		/**	Number of GPIO pins that provide interrupt support
			@ingroup arduinobinary
		*/
		#define BININ_INTERRUPTCOUNT	2
		
		/**	Array holding all interrupt capable binary input pins
			@ingroup arduinobinary
		*/
		const uint8_t BININ_INTERRUPTLIST[BININ_INTERRUPTCOUNT] = {2, 3};

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
		const uint8_t BINOUT_PWNPINLIST[BINOUT_PWMCOUNT] = {3, 5, 6, 9, 10, 11, 13};

		/**	Number of GPIO pins that provide interrupt support
			@ingroup arduinobinary
		*/
		#define BININ_INTERRUPTCOUNT	5
		
		/**	Array holding all interrupt capable binary input pins
			@ingroup arduinobinary
		*/
		const uint8_t BININ_INTERRUPTLIST[BININ_INTERRUPTCOUNT] = {0, 1, 2, 3, 7};

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
	#elif ARDUINO_TYPE == ARDUINO_NANO
		/** Number of pulse width modulated binary output pins 
			@ingroup arduinobinary
		*/
		#define BINOUT_PWMCOUNT	6

		/** Array holding all pulse width modulated binary output pins 
			@ingroup arduinobinary
		*/
		const uint8_t BINOUT_PWNPINLIST[BINOUT_PWMCOUNT] = {6, 8, 9, 12, 13, 14};

		/**	Number of GPIO pins that provide interrupt support
			@ingroup arduinobinary
		*/
		#define BININ_INTERRUPTCOUNT	2
		
		/**	Array holding all interrupt capable binary input pins
			@ingroup arduinobinary
		*/
		const uint8_t BININ_INTERRUPTLIST[BININ_INTERRUPTCOUNT] = {2, 3};

		/** Number of binary inputs on the Arduino 
			@ingroup arduinobinary
		*/
		#define ARDUINO_BINPINS	22

		/** Number of analog inputs on the arduino 
			@ingroup arduinoanalog
		*/
		#define ARDUINO_ANAPINS	8

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
	#else //No arduino type specified
		/** Number of pulse width modulated binary output pins 
			@ingroup arduinobinary
		*/
		#define BINOUT_PWMCOUNT	0
		
		/**	Number of GPIO pins that provide interrupt support
			@ingroup arduinobinary
		*/
		#define BININ_INTERRUPTCOUNT	0
		
		/** Array holding all pulse width modulated binary output pins 
			@ingroup arduinobinary
		*/
		const uint8_t *BINOUT_PWNPINLIST = NULL;
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
	#define ZeroAllBitsInMask(Register, Mask)	Register = ((typeof(Register))(((uint32_t)Register) & (~(uint32_t)(Mask))))

	/**	@brief		Gives the value from Register with the bits set in mask set to 1
		@details	The Register value will not be updated by calling this macro, its 
			return must be stored to retain this value.  The values supplied can not 
			exceed 32 bits.
		@param		Register	The value to be updated, having specified bits set
		@param		Mask		The bits to set in Register
		@return		The value of Register after it is updated
		@ingroup	commonutils
	*/
	#define SetAllBitsInMask(Register, Mask)	Register = ((typeof(Register))(((uint32_t)Register) | ((uint32_t)(Mask))))

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
	
	/**	@brief		Checks if a number is within a specified range
		@details	Tests if the number is within the bounds of the number range specified.  If the number
			matches the bounds it is considered within the range.
		@param		nNumber		The number to test
		@param		nRangeMin	The minimum bound of the range
		@param		nRangeMax	The maximum bound of the range
		@return		True if the number is within the range, false otherwise
		@ingroup	commonutils
	*/
	#define	IsNumberInInclusiveRange(nNumber, nRangeMin, nRangeMax)		(((nNumber >= nRangeMin) && (nNumber <= nRangeMax)) ? true : false)
	
	/**	@brief		Determines if a given number exists in an array
		@param		nNumber		The number to check for in the array
		@param		aList		The array to searrch through
		@param		nListCnt	The number of elements in the array
		@return		True if the number is found in the array, false if it is not
		@ingroup	commonutils
	*/
	bool NumberInArray_u8(uint8_t nNumber, const uint8_t *aList, uint32_t nListCnt);
	
	uint8_t CalculateCRC8(uint8_t nPolynomial, uint8_t nInit, uint8_t nXOROut, const uint8_t *pData, uint32_t nDataLen);

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

	uint8_t ContinueCalcCRC8(uint8_t nPolynomial, uint8_t nByte, uint8_t nPrevCRC) {
		uint16_t nBitCtr;
		uint8_t nBit;
		uint8_t nTemp;
		
		for (nBitCtr = 0; nBitCtr < 8; nBitCtr++) {
			nBit = nByte & 0x80; //Get the highest bit
			
			nTemp = nPrevCRC & 0x80;
			if (nBit != 0) {
				nTemp ^= 0x80;
			}
			
			nPrevCRC <<= 1;
			
			if (nTemp != 0) {
				nPrevCRC ^= nPolynomial;
			}
			
			nByte <<= 1;
		}
		
		return nPrevCRC;
	}

	uint8_t CalculateCRC8(uint8_t nPolynomial, uint8_t nInit, uint8_t nXOROut, const uint8_t *pData, uint32_t nDataLen) {
		uint8_t nCrc;
		uint32_t nCtr;
		
		nCrc = nInit;
		
		for (nCtr = 0; nCtr < nDataLen; nCtr++) {
			nCrc = ContinueCalcCRC8(nPolynomial, pData[nCtr], nCrc);
		}
		
		nCrc ^= nXOROut;
		
		return nCrc;
	}

#endif

