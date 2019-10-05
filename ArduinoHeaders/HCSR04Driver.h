/**	@defgroup	hcsr04driver
	@brief		Driver for teh HCSR04 Ultrasonic Distance sensor
	@details	v0.1
		When the trigger pin goes high (min 10 uSec pulse) the devise will send out a series of ultrasonic 
		pulses.  It will then listen for the echo from these pulses.  It will then respond by emitting a 
		pulse on the echo pin, the time in uSec from rising to falling edge of this represents the timing
		of the sound echo.  Divide this time by known constants (148 for inches and 58 for centimeters)
		will result in a distance to the object the echo came from.
		
		Interrupt timing is the most precise, however they are unaware of the pin that called them.  This
		means that each instance will require its own interrupt function.  There's no way to programatically
		handle that.  So this class will block during the sample (up to 2 mSec) while reading distance from
		the sensor.
*/

#ifndef __HCSR04DRIVER
	#define __HCSR04DRIVER

/***** Includes		*****/
	#include "CommonUtils.h"

/***** Definitions	*****/
	/**	@brief		Divider to use to convert microseconds to inches
		@ingroup	hcsr04driver
	*/
	#define HCSR04_DIVIDEINCH	148
	
	/**	@brief		Divider to use to convert microseconds to centimeter
		@ingroup	hcsr04driver
	*/
	#define HCSR04_DIVIDECM		58
	
	/**	@brief		Value specifying that readings are not automatic
		@ingroup	hcsr04driver
	*/
	#define HCSR04_NOINTERVAL	UINT32_MAXVALUE

	class cHCSR04Driver_t {
		public:
			float cnDistanceInches;
			float cnDistanceCM;
			uint32_t ctLastReading;
		
			cHCSR04Driver_t(uint8_t nTriggerPin, uint8_t nEchoPhin);
			~cHCSR04Driver_t();
			
			bool Update();
			bool SetReadInterval(uint32_t tMSecBetweenReads);
			bool StopReadings();
			bool SingleReading();
		
		protected:
			bool TakeReading();
		
		private:
			uint8_t cnTriggerPin;
			uint8_t cnEchoPin;
			uint32_t ctReadingInterval;
			uint32_t ctNextReading;
	};

/***** Constants	*****/


/***** Globals		*****/


/***** Prototypes 	*****/


/***** Functions	*****/
cHCSR04Driver_t::cHCSR04Driver_t(uint8_t nTriggerPin, uint8_t nEchoPhin) {
	//Initialize class variables
	cnDistanceInches = 0;
	cnDistanceCM = 0;
	ctLastReading = 0;
	
	ctReadingInterval = HCSR04_NOINTERVAL;
	ctNextReading = 0;
	
	cnTriggerPin = nTriggerPin;
	cnEchoPin = nEchoPhin;
	
	pinMode(cnTriggerPin, OUTPUT);
	pinMode(cnEchoPin, INPUT_PULLUP);
	
	digitalWrite(cnTriggerPin, LOW);
	
	return;
}

cHCSR04Driver_t::~cHCSR04Driver_t() {
	//No resources allocated, no cleanup needed
	
	return;
}

bool cHCSR04Driver_t::SetReadInterval(uint32_t tMSecBetweenReads) {
	ctReadingInterval = tMSecBetweenReads;

	if (ctReadingInterval != HCSR04_NOINTERVAL) {
		ctNextReading = millis();
		ctNextReading += ctReadingInterval;
	} else {
		ctNextReading = 0;
	}
	
	return true;
}

bool cHCSR04Driver_t::StopReadings() {
	ctReadingInterval = HCSR04_NOINTERVAL;
	ctNextReading = 0;
	
	return true;
}

bool cHCSR04Driver_t::SingleReading() {
	return TakeReading();
}

bool cHCSR04Driver_t::Update() {
	uint32_t tCurrTime = millis();
	
	if ((tCurrTime - ctLastReading > ctReadingInterval) && (ctReadingInterval != HCSR04_NOINTERVAL)) {
		//Readings are scheduled, and its time to take one
		return TakeReading();
	} else {
		return true;
	}
}

bool cHCSR04Driver_t::TakeReading() {
	uint32_t tRiseTime, tFallTime;
	
	//Send the trigger signal
	digitalWrite(cnTriggerPin, HIGH);
	delayMicroseconds(10);  //This pulse must be 10 uSec or more
	digitalWrite(cnTriggerPin, LOW);
	
	//Wait for start of echo pulse
	do {
		delayMicroseconds(1);
	} while (digitalRead(cnEchoPin) == LOW);
	
	tRiseTime = micros();
	
	//Wait for end of echo pulse
	do {
		delayMicroseconds(1);
	} while (digitalRead(cnEchoPin) == HIGH);
	
	tFallTime = micros();
	
	//Calculate the distances
	tFallTime -= tRiseTime; //Fall time holds pulse duration
	
	cnDistanceInches = (float)tFallTime / (float)HCSR04_DIVIDEINCH;
	cnDistanceCM = (float)tFallTime / (float)HCSR04_DIVIDECM;
	ctLastReading = millis();
	
	return true;
}

#endif

