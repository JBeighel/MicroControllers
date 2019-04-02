/**	@defgroup	snapduino
	@brief		Library to simplify interacting with snap circuits
	@details	Ver 1.0
	This header file is a library intended to simplify access to Arduino hardware
	when connected to snap circuits.  It smooths over a lot of the common issues 
	in order to allow quick easy coding for demos.

	Available preprocessor options:
	- SNAPCIR_NOSCREEN: If the Arduino is not connected to a SSD1306 0.96 OLED
		screen this will prevent the libraries and objects related to that decive
		from being loaded.
	- SCREENPLOT_POINTS: A function exists to display a line plot to the attached
		SSD1306 OLED screen.  It can display up to 128 points, however if the 
		available RAM can not fit a buffer of that size the number of points can
		be reduces by setting this definition.
	- SSD1306_SELECTPIN: The SSD1306 requires three binary output pins for operation.
		This specifies the Chip Select pin to use when sending it data and commands.
	- SSD1306_RESETPIN: The SSD1306 requires three binary output pins for operation.
		This spiecifies the pin to use when the screen needs to be reset.
	- SSD1306_DATACMDPIN: The SSD1306 requires three binary output pins for operation.
		This specifies the pin to use to distinguish between data and commands being 
		sent over the SPI bus.
*/

/**	@defgroup	arduinobinary
	@ingroup	snapduino
	@brief		A portion of the Snap Circuit Arduino library that covers binary inputs
	@details
	The binary inputs are read regularly and checked for state changes so various timers
	can track when changes have or are to happen.
*/

/**	@defgroup	arduinoanalog
	@ingroup	snapduino
	@brief		A portion of the Snap Circuit Arduino library that covers analog inputs
	@details
	The analog inputs are read in regularly and a simple Low Pass FIR filter is applied 
	to the readings to smooth out the values.
*/

/**	@defgroup	snapscreen
	@ingroup	snapduino
	@brief		A portion of the Snap Circuits Arduino library that covers the OLED screen
	@details
	The SSD1306 OLED screen is included as a quick way to provide output to the user
	when working with the Snap Circuits Arduino librarly.

	Be aware that none of this functionality will be available if SNAPCIR_NOSCREEN is
	defined.  Support for the screen will take just over 1K of RAM to work.
*/

#ifndef __SNAPCIRCUITS_H
	#define __SNAPCIRCUITS_H

/*****  Header Files  *****/
	#ifndef SNAPCIR_NOSCREEN
		#include "SSD1306Driver.h"
	#endif

/*****  Constants  *****/
	/**	The maximum integer value that will fit in a uint8_t data type
		@ingroup	snapduino
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
		@ingroup	snapduino
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
		@ingroup	snapduino
	*/
	#define ARDUINO_MICRO		2

	#ifndef ARDUINO_TYPE
		/** Specifies the type of arduino being used
			Arduino/Genuino Uno is the default
			@ingroup	snapduino
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
		const uint8_t BINOUT_PWNPINLIST[BINOUT_PWMCOUNT] = {3, 5, 6, 9, 10, 11, 13};

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

	/** Minimum time an input must be low before accepting the change 
		@ingroup arduinobinary
	*/
	#define INPUT_DEBOUNCETIME	50

	/** Snapcircuits Arduino has some snap ready binary pins, this is labeled 9 
		@ingroup	snapduino
	*/
	#define SNAPPIN_09			9
	
	/** Snapcircuits Arduino has some snap ready binary pins, this is labeled 10 
		@ingroup	snapduino
	*/
	#define SNAPPIN_10			10
	
	/** Snapcircuits Arduino has some snap ready binary pins, this is labeled 11 
		@ingroup	snapduino
	*/
	#define SNAPPIN_11			11

	/** Analog input values will go through a fir low pass filer to settle the value, 
		this is the number of samples to average
		@ingroup arduinoanalog
	*/
	#define ANAIN_FILTERCNT		10

	/**	The analog inputs have a 10 bit ADC, making this the maximum value they can report 
		@ingroup arduinoanalog
	*/
	#define ANAIN_MAXVALUE		1024

	/**	Bits in the nPin value of the analog input structure reserved for flags 
		@ingroup arduinoanalog
	*/
	#define ANAIN_FLAG_MASK		0xE0

	/**	Flag in the analog input Pin value specifying that this input is being plotted 
		@ingroup arduinoanalog
	*/
	#define ANAIN_FLAG_PLOT		0x80

	/** Minimum value allowed for an output used for pulse width output 
		@ingroup arduinobinary
	*/
	#define BINOUT_PWMMIN		0

	/** Maximum value allowed for an output used for pulse width output 
		@ingroup arduinobinary
	*/
	#define BINOUT_PWMMAX		255

	/** Number of binary I/O pins that will be configured as inputs 
		All other pins will be configured as outputs
		@ingroup arduinobinary
	*/
	#define BIN_NUMINPUT		3

	/** List of binary I/O pins that will be configured as inputs.  Any
		pins not ncluded in these lists will be ignored and unavailable 
		through the IO functions.
		@ingroup arduinobinary
	*/
	const uint8_t BIN_INPUTPINLIST[BIN_NUMINPUT] = {5, 9, 10};

	/** Number of binary I/O pins that will be configured as outputs.  Any
		pins not ncluded in these lists will be ignored and unavailable 
		through the IO functions.
		@ingroup arduinobinary
	*/
	#define BIN_NUMOUTPUT		4

	/** List of binary I/O pins that will be configured as outputs.  Any
		pins not included in these lists will be ignored and unavailable 
		through the IO functions.
		@ingroup arduinobinary
	*/
	const uint8_t BIN_OUTPUTPINLIST[BIN_NUMOUTPUT] = {1, 2, 3, 4};

	/** Standard screen width for the SSD1306 used with the snap circuits arduino
		@ingroup	snapscreen
	*/
	#define SSD1306_WIDTH		128

	/** Standard screen height for the SSD1306 used with the snap circuits arduino
		@ingroup	snapscreen
	*/
	#define SSD1306_HEIGHT		64

	#ifndef SSD1306_SELECTPIN
		/** The standard pin used as chip select for the SSD1306 screen 
			@ingroup	snapscreen
		*/
		#define SSD1306_SELECTPIN	8
	#endif

	#ifndef SSD1306_RESETPIN
		/** The standard pin used as reset for the SSD1306 screen 
			@ingroup	snapscreen
		*/
		#define SSD1306_RESETPIN	6
	#endif

	#ifndef SSD1306_DATACMDPIN
		/** The standard pin used to set command or data mode in the SSD1306 screen 
			@ingroup	snapscreen
		*/
		#define SSD1306_DATACMDPIN	7
	#endif

/*****  Definitiions  *****/

	/** Structure to hold all information about digital I/O 
		@ingroup arduinobinary
	*/
	typedef struct sBinIOInfo_t {
		uint8_t nPin;				/**< Pin number on the arduino */
		uint8_t CurrState;			/**< Current state read this IO pin is in */
		uint8_t PriorState;			/**< Last state read from this IO */
		uint8_t Direction;			/**< Use the pin is set for, Input / Output */
		bool bPressEventWaiting;	/**< True if there is a press event waiting for this pin */
		uint32_t tStateBegin;		/**< Time stamp of when we entered the current state */
		uint32_t tStateChange;		/**< Time stamp of input change, used to deboucne the input */
		uint32_t tFlipTime;			/**< Amount of time to remain inthe current state before changing */
		uint32_t tLastStateLen;		/**< Amount of time the last state lasted */
	} sBinIOInfo_t;

	/** Structure to hold all information about analog inputs 
		@ingroup arduinoanalog
	*/
	typedef struct sAnaIOInfo_t{
		uint8_t nPin;						/**< Pin number on the arduino */
		uint16_t aSamples[ANAIN_FILTERCNT];	/**< Collection of samples used by the FIR low pass filter */
		uint8_t nSamplesIdx;				/**< Index in the samples array where the next sample will be placed */
		uint32_t nAccum;					/**< The FIR low pass filter will accumulate values here */
		uint16_t nValue;					/**< Current value of this pin after the FIR low pass filter was applied */
	} sAnaIOInfo_t;

/*****  Globals  *****/
	/** Array holding information on all of the binary I/O 
		@ingroup arduinobinary
	*/
	sBinIOInfo_t mBinIOPins[BIN_NUMINPUT + BIN_NUMOUTPUT];

	/** Array holding information on all of the analog inputs 
		@ingroup arduinoanalog
	*/
	sAnaIOInfo_t mAnaIOPins[ARDUINO_ANAPINS];

	#ifndef SNAPCIR_NOSCREEN
		/** Class that initializes and manages the SSD 1306 OLED screen 
			@ingroup	snapscreen
		*/
		SSD1306 mScreen = SSD1306(SSD1306_WIDTH, SSD1306_HEIGHT, SSD1306_DATACMDPIN, SSD1306_SELECTPIN, SSD1306_RESETPIN);

		/** Array to hold the data points when plotting to the screen 
			@ingroup	snapscreen
		*/
		uint8_t mScreenPlot[SCREENPLOT_POINTS];
	#endif

//*****  Prototypes  *****/
	/**	@brief		Check if all bits set in Mask, are set in Register
		@param		Register	The value to test
		@param		Mask		The bits to check in the Register
		@return		Will return true if all bits that are 1 in Mask are also 1 in Register.
			If any 1 bits in Mask are 0 in Register will return false.
		@ingroup	snapduino
	*/
	#define CheckAllBitsInMask(Register, Mask)	(((Register & Mask) == Mask) ? true : false)

	/**	@brief		Check if any bits set in Mask, are set in Register
		@param		Register	The value to test
		@param		Mask		The bits to check in the Register
		@return		Will return true if any bits that are 1 in Mask 1 in Register.  If no 
			1 bits in Mask are 1 in Register will return false.
		@ingroup	snapduino
	*/
	#define CheckAnyBitsInMask(Register, Mask)	(((Register & Mask) != 0) ? true : false)

	/**	@brief		Initializes all module variables and I/O pins
		@details	Initializes the binary I/O and analog input arrays.
			Sets binary I/O pins listed in BIN_INPUTPINLIST as inputs 
	 		all other pins will be outputs

	 		Also initializes the SPI bus as well as the SSD 1306 OLED
	 		deisplay.
		@return		Returns true on success, false on any failure
		@ingroup	snapduino
	*/
	bool SetupIOPins();

	/**	@brief		Checks and updates the state of the I/O pins
		@details	Goes through all I/O pins and verifies their current state.
			Debunces all binary inputs.
			Applies timer controls to binary outputs.
			Averages analog inputs

			Will also send the current drawing to the screen.  Then clear out the 
			drawing to take new information.
		@return		True on success, false on any failure
		@ingroup	snapduino
	*/
	bool CheckIOPins();

	/**	@brief		Sets a binary output pin to LOW
		@param		nPin	The pin number to operate
		@return		True if the pin was updated, false on some error
		@ingroup arduinobinary
	*/
	bool TurnOff(uint8_t nPin);

	/**	@brief		Sets a binary output pin to HIGH
		@param		nPin	The pin number to operate
		@return		True if the pin was updated, false on some error
		@ingroup arduinobinary
	*/
	bool TurnOn(uint8_t nPin);

	/**	@brief		Sets a binary output pin to the opposite output
		@details	If the pin was LOW it will be made HIGH, if it was HIGH
			it will be set to LOW.
		@param		nPin	The pin number to operate
		@return		True if the pin was updated, false on some error
		@ingroup arduinobinary
	*/
	bool Flip(uint8_t nPin);

	/**	@brief		Sets a binary output to change its value on regular intervals
		@details	This will specify that the output pin will change its state once
			it has been in the current state for the specified amount of time.  This 
			time begins when it entered that state, which might be before this 
			function was called.

			This function will only cause the pin to change value once, but if it is
			called every time the loop runs it will give the effect of flipping the
			output value on the specified interval.
		@param		nPin	The pin number to operate
		@param		nTime	The amount of time, in seconds, to remain in the state 
			before changing.
		@return		True if the pin was updated, false on any error
		@ingroup arduinobinary
	*/
	bool FlipEverySeconds(uint8_t nPin, float nTime);

	/**	@brief		Sets a binary output to change its value after an interval
		@details	This will specify that the output pin will change its state once
			a specified amount of time has passed.  This time begins when it this 
			function is called, and will reset if the function is called again.  If 
			this funtion is called repeatedly then the timer will never ellapse due 
			to the resets.

			Does not change the current value of the output pin.  It will remain in 
			its current state until the time has ellapsed then it will flip to the 
			opposite state.
		@param		nPin	The pin number to operate
		@param		nTime	The amount of time, in seconds, to wait before changing.
		@return		True if the pin was updated, false on any error
		@ingroup arduinobinary
	*/
	bool InSecondsFlip(uint8_t nPin, uint32_t nTime);

	/**	@brief		Check if a button press event was detected
		@details	When an input transitions from "Off" to "On" it will flag that a 
			button press was made.  This is intended for use with momentary buttons, 
			rather than switches, however the event will be generated for any type of
			input signal.
		@param		nPin	The input pin number to check for the event
		@return		True if the event was flagged, false on error or if the event was 
			not flagged.
		@ingroup	arduinobinary
	*/
	bool ButtonWasPressed(uint8_t nPin);
	
	/**	@brief		Tests if a binary I/O pin is in the 'On' state
		@details	For binary pins set for input the 'On' state is when it 
			reads LOW.  These pins all are configured with pull up resistors,
			there default state is HIGH so inputs must bring them LOW.
			For pins set to output the 'On' state is when it is set to HIGH.
		@param		nPin	The binary I/O pin to check
		@return		True if the pin is 'On', otherwise returns False
		@ingroup arduinobinary
	*/
	bool IsOn(uint8_t nPin);
	
	/**	@brief		Tests if a binary I/O pin is in the 'Off' state
		@details	For binary pins set for input the 'Off' state is when it 
			reads HIGH.  These pins all are configured with pull up resistors,
			there default state is HIGH so inputs must bring them LOW.
			For pins set to output the 'Off' state is when it is set to LOW.
		@param		nPin	The binary I/O pin to check
		@return		True if the pin is 'Off', otherwise returns False
		@ingroup arduinobinary
	*/
	bool IsOff(uint8_t nPin);
	
	/**	@brief		Sets a binary output On for a specified interval
		@details	This will specify that the output pin will be set On for a 
			specified amount of time.  Once this time has passed it will have its 
			value changed to Off, then left there.  This time begins when it this 
			function is called, and will reset if the function is called again.  If 
			this funtion is called repeatedly then the timer will never ellapse due 
			to the resets.

			Will set the value of the output pin immediately then begin the timer.  
			It will remain in this state until the time has ellapsed then it will 
			flip to the opposite state.
		@param		nPin	The pin number to operate
		@param		nTime	The amount of time, in seconds, to wait before changing.
		@return		True if the pin was updated, false on any error
		@ingroup arduinobinary
	*/
	bool OnForSeconds(uint8_t nPin, uint32_t nTime);
	
	/**	@brief		Sets a binary output Off for a specified interval
		@details	This will specify that the output pin will be set Off for a 
			specified amount of time.  Once this time has passed it will have its 
			value changed to On, then left there.  This time begins when it this 
			function is called, and will reset if the function is called again.  If 
			this funtion is called repeatedly then the timer will never ellapse due 
			to the resets.

			Will set the value of the output pin immediately then begin the timer.  
			It will remain in this state until the time has ellapsed then it will 
			flip to the opposite state.
		@param		nPin	The pin number to operate
		@param		nTime	The amount of time, in seconds, to wait before changing.
		@return		True if the pin was updated, false on any error
		@ingroup arduinobinary
	*/
	bool OffForSeconds(uint8_t nPin, uint32_t nTime);
	
	/**	@brief		Reports the reading from an analog input pin
		@details	Analog inputs readings are collected and put through a low pass
			FIR filter to smooth out the readings.  The definition ANAIN_FILTERCNT 
			specifies the number of points included in this filtering.
		@param		nPin	The pin number to read from
		@return		The filtered value recorded by that pin
		@ingroup arduinoanalog
	*/
	uint16_t ReadAnalogInput(uint8_t nPin);

	bool NumberInArray_u8(uint8_t nNumber, const uint8_t *aList, uint32_t nListCnt);

	/**	@brief		Locates a binary I/O pin in the pin info structure array
		@details	The mBinIOPins array has information on all binary inputs that are
			used by the snapduino library.  In order to read or operate one of the pins
			it must first be found in that array to verify its mode withing the 
			microcontroller.  Not all pins are included to save on RAM usage and to keep
			special pins (such as those used by SPI or I2C buses) from being intruded
			upon.
		@param		nPinNum		The number of the arduino binary I/O pin
		@param		pnIndex		Used to return the index in the array where the pin was
			found
		@return		True if the pin was found, false if it wasn't or on error
		@ingroup	arduinobinary
	*/
	bool FindBinIOPin(uint8_t nPinNum, uint8_t *pnIndex);

	/**	@brief		Sets the output of binary pin to a pulse width modulated value
		@details	If the specified binary pin is set as an output, and has support for 
			pulse width modulated output it will set the duty cycle of that pulsed output.

			This value is set by a value between 0 and BINOUT_PWMMAX.  Where BINOUT_PWMMAX 
			is always on.
		@param		nPin		The Arduino binary I/O pin to operate
		@param		nPWMAmount	The duty cycle of the output pulses, from 0 to BINOUT_PWMMAX
		@return		True on success, false on any failure
		@ingroup	arduinobinary
	*/
	bool BinOutSetPWM(uint8_t nPin, uint8_t nPWMAmount);

	/**	@brief		Sets the duty cycle of a binary pin to a pulse width
		@details	Specifies the duty cycle of a pulsed width output as a percentage 
			of the time On.
		@param		nPin		The Arduino binary I/O pin to operate
		@param		nPWMPercent	The duty cycle of the output pulses, from 0 to 100 as 
			a percentage
		@return		True on success, false on any failure
		@ingroup arduinobinary
	*/
	bool BinOutSetPWMPercent(uint8_t nPin, float nPWMPercent);

	#ifndef SNAPCIR_NOSCREEN
		/**	@brief		Draws alpha numeric text to the screen buffer
			@details	All output intended for the screen is first drawn to the RAM
				buffer where it is cached until it is finally sent to the screen to be
				displayed.  This transmission occurs withing the CheckIOPins()
				function.  Once the screen has been drawn, the RAM buffer will be 
				cleared to draw on again.

				This will render any text available in the loaded font.  As the font 
				resides in RAM it is kept to a minimum, only letters and numbers are
				guaranteed to be available due to RAM limitations in the typical
				Arduino.

				When drawing the line and column are based on height and width of the 
				characters on screen.  Characters are 16 pixels tall and 12 pixesl wide.
				This sets the column and row sizes as multiples of these dimensions.
			@param		nLine	The character line number, or multiplied by 16 to get the
				 pixel row
			@param		nColumn	The character column number, or multiplied by 12 to get 
				the pixel column
			@param		Text	The characters to be displayed starting from the specifie 
				location on the screen
			@return		True on success, or false on any error
			@ingroup	snapscreen
		*/
		bool ScreenWrite(uint8_t nLine, uint8_t nColumn, char *Text);

		/**	@brief		Draws a numeric value as text to the screen buffer
			@details	All output intended for the screen is first drawn to the RAM
				buffer where it is cached until it is finally sent to the screen to be
				displayed.  This transmission occurs withing the CheckIOPins()
				function.  Once the screen has been drawn, the RAM buffer will be 
				cleared to draw on again.

				When drawing the line and column are based on height and width of the 
				characters on screen.  Characters are 16 pixels tall and 12 pixesl wide.
				This sets the column and row sizes as multiples of these dimensions.
			@param		nLine	The character line number, or multiplied by 16 to get the
				 pixel row
			@param		nColumn	The character column number, or multiplied by 12 to get 
				the pixel column
			@param		nValue	The integer value to be displayed starting from the
				specifie location on the screen
			@return		True on success, or false on any error
			@ingroup	snapscreen
		*/
		bool ScreenWriteNumber(uint8_t nLine, uint8_t nColumn, int32_t nValue);

		/**	@brief		Adds a new point to the data set drawn in the line plot
			@details	The line plot data set has a fixed number of points that will
				be drawn as a line plot.  Each new point will cause the oldest data
				point to be removed from the set.
			@param		nValue	The new value to add to the data set
			@return		True on success, false on any error
			@ingroup	snapscreen
		*/
		bool ScreenPlotAddPoint(uint8_t nValue);

		/**	@brief		Renders the line plot to the screen
			@details	The data points for the line plot will be added to the draw
				buffer.  Not other pixels in the draw buffer will be changed.
				In addition these points will fit from X coordinats 0 through 128 
				and Y coordinates 16 through 64.  This will leave the top 16 rows of
				pixels unused by the graph.
				Those 16 rows of pixels will be used to display the analog input pin
				number to indicate what is being displayed.
			@return		True on success, false on any error
			@ingroup	snapscreen
		*/
		bool ScreenPlotDraw();

		/**	@breif		Specify an analog input pin to show as the line series plot
			@details	Will flag this analog input to be tracked and displayed in the 
				line plot on the screen.  The values will be recoded by the call to 
				CheckIOPins() in addition this will be when the plot is rendered as well
			@param		nAnaInPin	The analog input pin to record on the line plot
			@return		True on success, false on any failure
			@ingroup	snapscreen
		*/
		bool PlotReading(uint8_t nAnaInPin);

		/**	@brief		Remove all analog inputs from being plotted
			@details	This will stop recording data from analog inputs to be plotted
				returning the screen to general use as nothing will be rendered to it.
			@return		True on success, false on any error
			@ingroup	snapscreen
		*/
		bool StopPlotting();
	#endif

//*****  Function Code  *****/
	bool SetupIOPins() {
		int nPinCtr, nCnt, nIndex;

		for (nPinCtr = 0; nPinCtr < BIN_NUMINPUT; nPinCtr++) {
			mBinIOPins[nPinCtr].nPin = BIN_INPUTPINLIST[nPinCtr];
			mBinIOPins[nPinCtr].tStateBegin = 0;
			mBinIOPins[nPinCtr].tFlipTime = 0;
			mBinIOPins[nPinCtr].bPressEventWaiting = false;

			mBinIOPins[nPinCtr].Direction = INPUT;
			pinMode(BIN_INPUTPINLIST[nPinCtr], INPUT_PULLUP);

			mBinIOPins[nPinCtr].CurrState = digitalRead(BIN_INPUTPINLIST[nPinCtr]);
			if (mBinIOPins[nPinCtr].CurrState == HIGH) {
				mBinIOPins[nPinCtr].PriorState = LOW;
			} else {
				mBinIOPins[nPinCtr].PriorState = HIGH;
			}
		}

		for (nPinCtr = 0; nPinCtr < BIN_NUMOUTPUT; nPinCtr++) {
			nIndex = nPinCtr + BIN_NUMINPUT; //Find the index for this pin

			mBinIOPins[nIndex].nPin = BIN_OUTPUTPINLIST[nPinCtr];
			mBinIOPins[nIndex].tStateBegin = 0;
			mBinIOPins[nIndex].tFlipTime = 0;
			mBinIOPins[nIndex].bPressEventWaiting = false;

			mBinIOPins[nIndex].Direction = OUTPUT;
			pinMode(BIN_OUTPUTPINLIST[nPinCtr], OUTPUT);

			digitalWrite(BIN_OUTPUTPINLIST[nPinCtr], LOW);
			mBinIOPins[nIndex].PriorState = HIGH;
		}

		for (nPinCtr = 0; nPinCtr < ARDUINO_ANAPINS; nPinCtr++) {
			mAnaIOPins[nPinCtr].nPin = nPinCtr;
			mAnaIOPins[nPinCtr].nAccum = 0;
			mAnaIOPins[nPinCtr].nValue = 0;
			mAnaIOPins[nPinCtr].nSamplesIdx = 0;

			for (nCnt = 0; nCnt < ANAIN_FILTERCNT; nCnt++) {
				mAnaIOPins[nPinCtr].aSamples[nCnt] = 0;
			}
		}

		#ifndef SNAPCIR_NOSCREEN
			SPI.begin();
			mScreen.Begin(&SPI);
		#endif
	}

	bool CheckIOPins() {
		uint8_t nPinCtr, nIdx, CurrState;
		unsigned long nCurrTime = millis();

		for (nPinCtr = 0; nPinCtr < BIN_NUMINPUT + BIN_NUMOUTPUT; nPinCtr++) {
			CurrState = digitalRead(mBinIOPins[nPinCtr].nPin);
			if (CurrState != mBinIOPins[nPinCtr].CurrState) { //The pin has changed
				if (mBinIOPins[nPinCtr].tStateChange == 0) {
					//First detection of the change, begin debounce timer
					mBinIOPins[nPinCtr].tStateChange = nCurrTime;
				} else if (nCurrTime - mBinIOPins[nPinCtr].tStateChange > INPUT_DEBOUNCETIME) {
					//The state has persisted, accept it as a valid state change
					mBinIOPins[nPinCtr].CurrState = CurrState;
				}

				if (mBinIOPins[nPinCtr].CurrState == mBinIOPins[nPinCtr].PriorState) { //Pin state has changed
					if (mBinIOPins[nPinCtr].CurrState == HIGH) { //Inputs are pull up, high means switch is off
						mBinIOPins[nPinCtr].PriorState = LOW;
						mBinIOPins[nPinCtr].bPressEventWaiting = false;
					} else {
						mBinIOPins[nPinCtr].PriorState = HIGH;
						mBinIOPins[nPinCtr].bPressEventWaiting = true;
					}

					mBinIOPins[nPinCtr].tLastStateLen = nCurrTime - mBinIOPins[nPinCtr].tStateChange;
					mBinIOPins[nPinCtr].tStateBegin = mBinIOPins[nPinCtr].tStateChange;
					mBinIOPins[nPinCtr].tStateChange = 0;
					mBinIOPins[nPinCtr].tFlipTime = 0;
				} 
			} else { //Pin state has not changed
				if ((mBinIOPins[nPinCtr].tFlipTime != 0) && (nCurrTime - mBinIOPins[nPinCtr].tStateBegin >= mBinIOPins[nPinCtr].tFlipTime)) {
					Flip(mBinIOPins[nPinCtr].nPin);
				}

				mBinIOPins[nPinCtr].tStateChange = 0;
			} 
		}

		for (nPinCtr = 0; nPinCtr < ARDUINO_ANAPINS; nPinCtr++) {
			CurrState = analogRead(nPinCtr);
			CurrState /= ANAIN_FILTERCNT; //This is the common divisor for the filter

			nIdx = mAnaIOPins[nPinCtr].nSamplesIdx;

			mAnaIOPins[nPinCtr].nAccum -= mAnaIOPins[nPinCtr].aSamples[nIdx]; //Remove oldest value from accumulator
			mAnaIOPins[nPinCtr].nAccum += CurrState; //Put hte newest sample into the accumulator

			mAnaIOPins[nPinCtr].aSamples[nIdx] = CurrState;

			nIdx += 1;
			if (nIdx >= ANAIN_FILTERCNT) {
				nIdx = 0;
			}

			mAnaIOPins[nPinCtr].nSamplesIdx = nIdx;

			mAnaIOPins[nPinCtr].nValue = mAnaIOPins[nPinCtr].nAccum;

			if (CheckAllBitsInMask(mAnaIOPins[nPinCtr].nPin, ANAIN_FLAG_PLOT) == true) {
				ScreenPlotAddPoint(mAnaIOPins[nPinCtr].nValue * UINT8_MAXVALUE / ANAIN_MAXVALUE); //scale the reading to an 8 bit value

				mScreen.ClearDrawing(); //New point is added, redraw the plot
				ScreenPlotDraw();
			}
		}

		#ifndef SNAPCIR_NOSCREEN
			mScreen.SendToScreen();
			mScreen.ClearDrawing();
		#endif
	}

	bool TurnOff(uint8_t nPin) {
		if (FindBinIOPin(nPin, &nPin) == false) {
			return false;
		}

		if (mBinIOPins[nPin].Direction != OUTPUT) {
			return false;
		}

		digitalWrite(mBinIOPins[nPin].nPin, LOW);
		mBinIOPins[nPin].tFlipTime = 0; //Cancel any pending flips
		return true;
	}

	bool TurnOn(uint8_t nPin) {
		if (FindBinIOPin(nPin, &nPin) == false) {
			return false;
		}

		if (mBinIOPins[nPin].Direction != OUTPUT) {
			return false;
		}

		digitalWrite(mBinIOPins[nPin].nPin, HIGH);
		mBinIOPins[nPin].tFlipTime = 0; //Cancel any pending flips
		return true;
	}

	bool Flip(uint8_t nPin) {
		int CurrState;

		if (FindBinIOPin(nPin, &nPin) == false) {
			return false;
		}

		if (mBinIOPins[nPin].Direction != OUTPUT) {
			return false;
		}

		CurrState = digitalRead(mBinIOPins[nPin].nPin);

		if (CurrState == HIGH) {
			digitalWrite(mBinIOPins[nPin].nPin, LOW);
		} else {
			digitalWrite(mBinIOPins[nPin].nPin, HIGH);
		}

		mBinIOPins[nPin].tFlipTime = 0; //Cancel any pending flips

		return true;
	}

	bool FlipEverySeconds(uint8_t nPin, float nTime) {
		if (FindBinIOPin(nPin, &nPin) == false) {
			return false;
		}

		if (mBinIOPins[nPin].Direction != OUTPUT) {
			return false;
		}

		if (nTime < 0) { //Can't do negative times
			nTime = 0.001;
		}

		//tFlip time specifies how long we should be in the state before flipping
		mBinIOPins[nPin].tFlipTime = (unsigned int)(nTime * 1000.0);

		return true;
	}

	bool InSecondsFlip(uint8_t nPin, uint32_t nTime) {
		if (FindBinIOPin(nPin, &nPin) == false) {
			return false;
		}

		if (mBinIOPins[nPin].Direction != OUTPUT) {
			return false;
		}

		//tFlip time specifies how long we should be in the state before flipping
		mBinIOPins[nPin].tFlipTime = millis() - mBinIOPins[nPin].tStateBegin; //Amount of time we've been in the state
		mBinIOPins[nPin].tFlipTime += (nTime * 1000); //Ad the requested duration
		return true;
	}

	bool InSecondsFlip(uint8_t nPin, float nTime) {
		if (FindBinIOPin(nPin, &nPin) == false) {
			return false;
		}

		if (mBinIOPins[nPin].Direction != OUTPUT) {
			return false;
		}

		if (nTime < 0) { //Can't do negative times
			nTime = 0.001;
		}

		//tFlip time specifies how long we should be in the state before flipping
		mBinIOPins[nPin].tFlipTime = millis() - mBinIOPins[nPin].tStateBegin; //Amount of time we've been in the state
		mBinIOPins[nPin].tFlipTime += (nTime * 1000.0); //Ad the requested duration
		return true;
	}

	bool ButtonWasPressed(uint8_t nPin) {
		if (FindBinIOPin(nPin, &nPin) == false) {
			return false;
		}

		if (mBinIOPins[nPin].Direction != INPUT) {
			return false;
		}

		if (mBinIOPins[nPin].bPressEventWaiting == false) {
			return false;
		}

		mBinIOPins[nPin].bPressEventWaiting = false; //Clear the waiting event
		return true;
	}
	
	bool IsOn(uint8_t nPin) {
		if (FindBinIOPin(nPin, &nPin) == false) {
			return false;
		}

		if (mBinIOPins[nPin].Direction == INPUT) {
			if (mBinIOPins[nPin].CurrState == LOW) { //Inputs are pull ups, so on switch brings the state low/to ground
				return true;
			} else {
				return false;
			}
		}

		if (mBinIOPins[nPin].Direction == OUTPUT) {
			if (mBinIOPins[nPin].CurrState == LOW) {
				return false;
			} else {
				return true;
			}
		}

		//Somehow not an input or output?
		return false;
	}
	
	bool IsOff(uint8_t nPin) {
		if (FindBinIOPin(nPin, &nPin) == false) {
			return false;
		}

		if (mBinIOPins[nPin].Direction == INPUT) {
			if (mBinIOPins[nPin].CurrState == LOW) { //Inputs are pull ups, so on switch brings the state low/to ground
				return false;
			} else {
				return true;
			}
		}

		if (mBinIOPins[nPin].Direction == OUTPUT) {
			if (mBinIOPins[nPin].CurrState == LOW) {
				return true;
			} else {
				return false;
			}
		}

		//Somehow not an input or output?
		return false;
	}
	
	bool OnForSeconds(uint8_t nPin, uint32_t nTime) {
		if (FindBinIOPin(nPin, &nPin) == false) {
			return false;
		}

		if (mBinIOPins[nPin].Direction != OUTPUT) {
			return false;
		}

		if (TurnOn(mBinIOPins[nPin].nPin) == true) { //Output state set, apply the timer
			//tFlip time specifies how long we should be in the state before flipping
			mBinIOPins[nPin].tFlipTime = millis() - mBinIOPins[nPin].tStateBegin; //Amount of time we've been in the state
			mBinIOPins[nPin].tFlipTime += (nTime * 1000); //Ad the requested duration
			return true;
		} else { //Unable to set the output state, abort
			return false;
		}
	}
	
	bool OffForSeconds(uint8_t nPin, uint32_t nTime) {
		if (FindBinIOPin(nPin, &nPin) == false) {
			return false;
		}

		if (mBinIOPins[nPin].Direction != OUTPUT) {
			return false;
		}

		if (TurnOff(mBinIOPins[nPin].nPin) == true) { //Output state set, apply the timer
			//tFlip time specifies how long we should be in the state before flipping
			mBinIOPins[nPin].tFlipTime = millis() - mBinIOPins[nPin].tStateBegin; //Amount of time we've been in the state
			mBinIOPins[nPin].tFlipTime += (nTime * 1000); //Ad the requested duration
			
			return true;
		} else { //Unable to set the output state, abort
			return false;
		}
	}

	uint16_t ReadAnalogInput(uint8_t nPin) {
		if (nPin >= ARDUINO_ANAPINS) {
			return 0;
		}

		return mAnaIOPins[nPin].nValue;
	}

	bool NumberInArray_u8(uint8_t nNumber, const uint8_t *aList, uint32_t nListCnt) {
		uint32_t nCtr;

		for (nCtr = 0; nCtr < nListCnt; nCtr++) {
			if (nNumber == aList[nCtr]) {
				return true;
			}
		}

		return false;
	}

	bool FindBinIOPin(uint8_t nPinNum, uint8_t *pnIndex) {
		uint8_t nCtr;

		for (nCtr = 0; nCtr < BIN_NUMINPUT + BIN_NUMOUTPUT; nCtr++) {
			if (mBinIOPins[nCtr].nPin == nPinNum) {
				(*pnIndex) = nCtr;
				return true;
			}
		}

		(*pnIndex) = 0;
		return false;
	}

	bool BinOutSetPWM(uint8_t nPin, uint8_t nPWMAmount) {
		if (FindBinIOPin(nPin, &nPin) == false) {
			return false;
		}

		if (mBinIOPins[nPin].Direction != OUTPUT) {
			return false;
		}

		if (NumberInArray_u8(nPin, BINOUT_PWNPINLIST, BINOUT_PWMCOUNT) == false) {
			//Pin is either an input, or doesn't support PWM
			return false;
		}

		analogWrite(nPin, nPWMAmount);

		return true;
	}

	bool BinOutSetPWMPercent(uint8_t nPin, float nPWMPercent) {
		uint8_t nPWMAmount = BINOUT_PWMMAX * nPWMPercent;

		if (FindBinIOPin(nPin, &nPin) == false) {
			return false;
		}

		if (mBinIOPins[nPin].Direction != OUTPUT) {
			return false;
		}

		if (NumberInArray_u8(nPin, BINOUT_PWNPINLIST, BINOUT_PWMCOUNT) == false) {
			//Pin is either an input, or doesn't support PWM
			return false;
		}

		//Make sure the amount is within bounds
		if (nPWMAmount > BINOUT_PWMMAX) {
			nPWMAmount = BINOUT_PWMMAX;
		} else if (nPWMAmount < BINOUT_PWMMIN) {
			nPWMAmount = BINOUT_PWMMIN;
		}

		analogWrite(nPin, nPWMAmount);

		return true;
	}

	#ifndef SNAPCIR_NOSCREEN
		bool ScreenWrite(uint8_t nLine, uint8_t nColumn, char *Text) {
			nLine *= 16; //Each line is 14 character pixels + 2 spacers
			nColumn *= 12; //Each column is 10 character pixels + 2 spacers

			mScreen.DrawTextDblSize(nColumn, nLine, Text);

			return true;
		}

		bool ScreenWriteNumber(uint8_t nLine, uint8_t nColumn, int32_t nValue) {
			char aStrTemp[12]; //max 10 digits + sign character + null terminator

			sprintf(aStrTemp, "%d", nValue);

			nLine *= 16; //Each line is 14 character pixels + 2 spacers
			nColumn *= 12; //Each column is 10 character pixels + 2 spacers

			mScreen.DrawTextDblSize(nColumn, nLine, aStrTemp);

			return true;
		}

		bool ScreenPlotAddPoint(uint8_t nValue) {
			uint8_t nPtCtr;

			nValue = (nValue * (OLED1306_ROWS - 16)) / UINT8_MAXVALUE; //Rescale the value for the space used on the screen
			nValue = (OLED1306_ROWS - 1) - nValue; //Put small readings along the bottom

			//Move all readings down one, dropping the oldest value
			for (nPtCtr = 1; nPtCtr < SCREENPLOT_POINTS; nPtCtr++) {
				mScreenPlot[nPtCtr - 1] = mScreenPlot[nPtCtr];
			}

			mScreenPlot[SCREENPLOT_POINTS - 1] = nValue; //Put the new value in the last slot 

			return true;
		}

		bool ScreenPlotDraw() {
			uint8_t nPinCtr, nDotCtr, nXCoord;

			nDotCtr = ARDUINO_ANAPINS; //Set this to an invalid value
			for (nPinCtr = 0; nPinCtr < ARDUINO_ANAPINS; nPinCtr++) {
				if (CheckAllBitsInMask(mAnaIOPins[nPinCtr].nPin, ANAIN_FLAG_PLOT) == true) {
					nDotCtr = nPinCtr;
				}
			}

			if (nDotCtr == ARDUINO_ANAPINS) {//Couldn't find a pin to plot
				return false;
			}

			ScreenWrite(0, 0, "IO ");
			ScreenWriteNumber(0, 3, nDotCtr);
			ScreenWrite(0, 5, ":");
			ScreenWriteNumber(0, 6, mAnaIOPins[nDotCtr].nValue);

			nXCoord = 0;
			for (nPinCtr = 0; nPinCtr < SCREENPLOT_POINTS; nPinCtr++) { //Loop through all points in the dataset
				for (nDotCtr = 0; nDotCtr < (128 / SCREENPLOT_POINTS); nDotCtr++) {
					//This point is stretched across multiple pixels, draw all pixels needed
					mScreen.DrawPixel(nXCoord, mScreenPlot[nPinCtr - 1], true);
					nXCoord += 1;
				}
			}

			return true;
		}

		bool PlotReading(uint8_t nAnaInPin) {
			uint8_t nCtr;

			if (nAnaInPin >= ANAIN_FLAG_PLOT) { //Point requested is out of range
				return false;
			}

			//Clear out all points in the plot
			for (nCtr = 0; nCtr < SCREENPLOT_POINTS; nCtr++) {
				mScreenPlot[nCtr] = OLED1306_ROWS - 1; //make the values are appear as zero
			}

			//Clear out all plot flags
			for (nCtr = 0; nCtr < ARDUINO_ANAPINS; nCtr++) {
				mAnaIOPins[nCtr].nPin &= ~ANAIN_FLAG_PLOT;
			}

			//Set the new plotting assignment
			mAnaIOPins[nAnaInPin].nPin |= ANAIN_FLAG_PLOT;

			return true;
		}

		bool StopPlotting() {
			uint8_t nCtr;

			//Clear out all points in the plot
			for (nCtr = 0; nCtr < SCREENPLOT_POINTS; nCtr++) {
				mScreenPlot[nCtr] = OLED1306_ROWS - 1; //make the values are appear as zero
			}

			//Clear out all plot flags
			for (nCtr = 0; nCtr < ARDUINO_ANAPINS; nCtr++) {
				mAnaIOPins[nCtr].nPin &= ~ANAIN_FLAG_PLOT;
			}
		}
	#endif
#endif
