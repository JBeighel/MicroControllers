/**	@defgroup	dht11
	@brief		Driver for the DHT11 temperature and humidity sensor
	@details	v0.1
		The transfer is done on a single wire.  This will remain at logic low until a transfer 
		is started.
		
		To start a transfer the MCU sends a 18+ uSec high pulse.  Then the DHT11 will wait
		for 20-40 uSec + 80 uSec low signal before sending an 80 uSec acknowledgement pulse.
		After this the DHT11 will begin data transmission.
		
		During data transmission each bit will have a 50uSec start delay, followed eitehr by 
		26-28 uSec high pulse for a 0 bit, or 70 uSec high pulse for a 1 bit.
		
		The bits are sent highest order bit first.  The data is 40 bits total.  The data 
		consists of the following:
			- 8 bits of humidity integer value
			- 8 bits of humidity decimal value
			- 8 bits of temperature integer value
			- 8 bits of temperature decimal value
			- 4 bits of checksum, lowest 8 bits of the sum of previous 4 values
			
		Humidity is given as a relative humidity percentage, temperature is given in degrees
		celsius.
*/

#ifndef __DHT11DRIVER
	#define __DHT11DRIVER

/***** Includes		*****/
	#include "CommonUtils.h"

/***** Definitions	*****/
	#define DHT11_DEBUG
	#ifdef DHT11_DEBUG
		#define DebugPrint(...)	Serial.print(__VA_ARGS__)
	#else
		#define DebugPrint(...)
	#endif
	
	#define DHT11_TIMEOUT		1000

class cDHT11_t {
	public:
		float cnTemperature;
		float cnHumidity;
	
		cDHT11_t(uint8_t nDataPin);
		~cDHT11_t();
		bool ReadValues();
		
	protected:
	
	private:
		uint8_t cnPin;
};

/***** Constants	*****/


/***** Globals		*****/


/***** Prototypes 	*****/


/***** Functions	*****/

cDHT11_t::cDHT11_t(uint8_t nDataPin) {
	cnPin = nDataPin;
	
	//pinMode(cnPin, INPUT); //keep it as input to avoid contention on the wire
	
	return;
}

cDHT11_t::~cDHT11_t() {
	//No resources allocated, nothing to clean upper_bound
	return;
}

bool cDHT11_t::ReadValues() {
	uint32_t tCurrTime, tLastEvent, tDuration;
	
	//Step one send the start signal
	pinMode(cnPin, OUTPUT);
	digitalWrite(cnPin, LOW);
	delayMicroseconds(25); //Only need 18 do a bit more just to be sure its enough
	digitalWrite(cnPin, HIGH); //Make a nice clean negative edge
	delayMicroseconds(5); //Only need 18 do a bit more just to be sure its enough
	pinMode(cnPin, INPUT); // Done transmitting, let the sensor talk
	delayMicroseconds(40);
	tLastEvent = micros();
	
	//Wait for the acknowledgement pulse
	while((digitalRead(cnPin) == HIGH) && (tDuration < DHT11_TIMEOUT)) {
		delayMicroseconds(5);
		tCurrTime = micros();
		
		tDuration = tCurrTime - tLastEvent;
	}
	
	if (tDuration >= DHT11_TIMEOUT) {
		DebugPrint("Timeout waiting on ack pulse start\n");
		return false;
	}
	
	//at the start of the pulse, make sure its the right length
	tLastEvent = tCurrTime;
	while((digitalRead(cnPin) == LOW) && (tDuration < DHT11_TIMEOUT)) {
		delayMicroseconds(5);
		tCurrTime = micros();
		
		tDuration = tCurrTime - tLastEvent;
	}
	
	if (tDuration >= DHT11_TIMEOUT) {
		DebugPrint("Timeout waiting on ack pulse end\n");
		return false;
	}
	
	tDuration = tCurrTime - tLastEvent;
	if (IsNumberInInclusiveRange(tDuration, 70, 90) == false) {
		//Acknowledgement pulse was not in expected range
		DebugPrint("Ack pulse from DHT11 was incoorect length");
		DebugPrint(tDuration);
		DebugPrint("\n");
		return false;
	}
	
	DebugPrint("Ack pulse success! length");
	DebugPrint(tDuration);
	DebugPrint("\n");
}

#endif

