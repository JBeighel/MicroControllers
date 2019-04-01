/*	@defgroup snapduino
	@brief		Library to simplify interacting with snap circuits
	@details
	This header file is a library intended to simplify access to Arduino hardware
	when connected to snap circuits.  It smooths over a lot of the common issues 
	in order to allow quick easy coding for demos.

	Available preprocessor options:
	- SNAPCIR_NOSCREEN if the Arduino is not connected to a SSD1306 0.96 OLED
		screen this will prevent the libraries and objects related to that decive
		from being loaded.
*/

#ifndef __SNAPCIRCUITS_H
	#define __SNAPCIRCUITS_H

/*****  Header Files  *****/
	#ifndef SNAPCIR_NOSCREEN
		#include "SSD1306Driver.h"
	#endif

/*****  Constants  *****/
	/** One of the arduino types, the Arduino/Genuino Uno */
	#define ARDUINO_UNO			1

	#ifndef ARDUINO_TYPE
		/** Specifies the type of arduino being used
			Arduino/Genuino Uno is the default
		*/
		#define ARDUINO_TYPE	ARDUINO_UNO
	#endif

	//Based on the connected arduino, describe the I/O Pints
	#if ARDUINO_TYPE == ARDUINO_UNO
		/** Number of pulse width modulated binary output pins */
		#define BINOUT_PWMCOUNT	6

		/** Array holding all pulse width modulated binary output pins */
		const uint8_t BINOUT_PWNPINLIST[BINOUT_PWMCOUNT] = {3, 5, 6, 9, 10, 11};

		/** Number of binary inputs on the Arduino */
		#define ARDUINO_BINPINS	14

		/** Number of analog inputs on the arduino */
		#define ARDUINO_ANAPINS	6

		/** The Arduno has 3 LEDs on the board tied to binary pins, this is labeled RX */
		#define RX_LEDPIN		0
		
		/** The Arduno has 3 LEDs on the board tied to binary pins, this is labeled TX */
		#define TX_LEDPIN		1
		
		/** The Arduno has 3 LEDs on the board tied to binary pins, this is labeled LX */
		#define L_LEDPIN		13
	#endif 

	/** Minimum time an input must be low before accepting the change */
	#define INPUT_DEBOUNCETIME	50

	/** Snapcircuits Arduino has some snap ready binary pins, this is labeled 9 */
	#define SNAPPIN_09			9
	
	/** Snapcircuits Arduino has some snap ready binary pins, this is labeled 10 */
	#define SNAPPIN_10			10
	
	/** Snapcircuits Arduino has some snap ready binary pins, this is labeled 11 */
	#define SNAPPIN_11			11

	/** Analog input values will go through a fir low pass filer to settle the value, 
		this is the number of samples to average
	*/
	#define ANAIN_FILTERCNT		10

	/** Minimum value allowed for an output used for pulse width output 
	*/
	#define BINOUT_PWMMIN		0

	/** Maximum value allowed for an output used for pulse width output 
	*/
	#define BINOUT_PWMMAX		255

	/** Number of binary I/O pins that will be configured as inputs 
		All other pins will be configured as outputs
	*/
	#define BIN_NUMINPUT		3

	/** List of binary I/O pins that will be configured as inputs.  Any
		pins not ncluded in these lists will be ignored and unavailable 
		through the IO functions.
	*/
	const uint8_t BIN_INPUTPINLIST[BIN_NUMINPUT] = {5, 9, 10};

	/** Number of binary I/O pins that will be configured as outputs.  Any
		pins not ncluded in these lists will be ignored and unavailable 
		through the IO functions.
	*/
	#define BIN_NUMOUTPUT		4

	const uint8_t BIN_OUTPUTPINLIST[BIN_NUMOUTPUT] = {1, 2, 3, 4};

	/** Standard screen width for the SSD1306 used with the snap circuits arduino
	*/
	#define SSD1306_WIDTH		128

	/** Standard screen height for the SSD1306 used with the snap circuits arduino
	*/
	#define SSD1306_HEIGHT		64

	#ifndef SSD1306_SELECTPIN
		/** The standard pin used as chip select for the SSD1306 screen */
		#define SSD1306_SELECTPIN	8
	#endif

	#ifndef SSD1306_RESETPIN
		/** The standard pin used as reset for the SSD1306 screen */
		#define SSD1306_RESETPIN	6
	#endif

	#ifndef SSD1306_DATACMDPIN
		/** The standard pin used to set command or data mode in the SSD1306 screen */
		#define SSD1306_DATACMDPIN	7
	#endif


/*****  Definitiions  *****/

	/** Structure to hold all information about digital I/O */
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

	/** Structure to hold all information about analog inputs */
	typedef struct sAnaIOInfo_t{
		uint8_t nPin;						/**< Pin number on the arduino */
		uint16_t aSamples[ANAIN_FILTERCNT];	/**< Collection of samples used by the FIR low pass filter */
		uint8_t nSamplesIdx;				/**< Index in the samples array where the next sample will be placed */
		uint32_t nAccum;					/**< The FIR low pass filter will accumulate values here */
		uint16_t nValue;					/**< Current value of this pin after the FIR low pass filter was applied */
	} sAnaIOInfo_t;

/*****  Globals  *****/
	/** Array holding information on all of the binary I/O */
	sBinIOInfo_t mBinIOPins[BIN_NUMINPUT + BIN_NUMOUTPUT];

	/** Array holding information on all of the analog inputs */
	sAnaIOInfo_t mAnaIOPins[ARDUINO_ANAPINS];

	#ifndef SNAPCIR_NOSCREEN
		/** Class that initializes and manages the SSD 1306 OLED screen */
		SSD1306 mScreen = SSD1306(SSD1306_WIDTH, SSD1306_HEIGHT, SSD1306_DATACMDPIN, SSD1306_SELECTPIN, SSD1306_RESETPIN);
	#endif

//*****  Prototypes  *****/
	/**	@brief      Initializes all module variables and I/O pins
		@details    Initializes the binary I/O and analog input arrays.
			Sets binary I/O pins listed in BIN_INPUTPINLIST as inputs 
	 		all other pins will be outputs

	 		Also initializes the SPI bus as well as the SSD 1306 OLED
	 		deisplay.
		@return     Returns true on success, false on any failure
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
	*/
	bool CheckIOPins();

	/**	@brief		Sets a binary output pin to LOW
		@param		nPin	The pin number to operate
		@return		True if the pin was updated, false on some error
	*/
	bool TurnOff(uint8_t nPin);

	/**	@brief		Sets a binary output pin to HIGH
		@param		nPin	The pin number to operate
		@return		True if the pin was updated, false on some error
	*/
	bool TurnOn(uint8_t nPin);

	/**	@brief		Sets a binary output pin to the opposite output
		@details	If the pin was LOW it will be made HIGH, if it was HIGH
			it will be set to LOW.
		@param		nPin	The pin number to operate
		@return		True if the pin was updated, false on some error
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
	*/
	bool InSecondsFlip(uint8_t nPin, uint32_t nTime);

	bool ButtonWasPressed(uint8_t nPin);
	
	/**	@brief		Tests if a binary I/O pin is in the 'On' state
		@details	For binary pins set for input the 'On' state is when it 
			reads LOW.  These pins all are configured with pull up resistors,
			there default state is HIGH so inputs must bring them LOW.
			For pins set to output the 'On' state is when it is set to HIGH.
		@param		nPin	The binary I/O pin to check
		@return		True if the pin is 'On', otherwise returns False
	*/
	bool IsOn(uint8_t nPin);
	
	/**	@brief		Tests if a binary I/O pin is in the 'Off' state
		@details	For binary pins set for input the 'Off' state is when it 
			reads HIGH.  These pins all are configured with pull up resistors,
			there default state is HIGH so inputs must bring them LOW.
			For pins set to output the 'Off' state is when it is set to LOW.
		@param		nPin	The binary I/O pin to check
		@return		True if the pin is 'Off', otherwise returns False
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
	*/
	bool OffForSeconds(uint8_t nPin, uint32_t nTime);
	
	/**	@brief		Reports the reading from an analog input pin
		@details	Analog inputs readings are collected and put through a low pass
			FIR filter to smooth out the readings.  The definition ANAIN_FILTERCNT 
			specifies the number of points included in this filtering.
		@param		nPin	The pin number to read from
		@return		The filtered value recorded by that pin
	*/
	uint16_t ReadAnalogInput(uint8_t nPin);

	bool NumberInArray_u8(uint8_t nNumber, const uint8_t *aList, uint32_t nListCnt);

	bool FindBinIOPin(uint8_t nPinNum, uint8_t *pnIndex);

	bool BinOutSetPWM(uint8_t nPin, uint8_t nPWMAmount);

	bool BinOutSetPWMPercent(uint8_t nPin, float nPWMPercent);

	#ifndef SNAPCIR_NOSCREEN
		bool ScreenWrite(uint8_t nLine, uint8_t nColumn, char *Text);

		bool ScreenWriteNumber(uint8_t nLine, uint8_t nColumn, int32_t nValue);
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
		int nPinCtr, nIdx, CurrState;
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
	#endif
#endif
