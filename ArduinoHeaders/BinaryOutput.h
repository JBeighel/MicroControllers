/**	@defgroup	binout_obj
	@brief		Class to encapsulate Binary Outputs including Pulse Width functionality
	@details	v 0.1
		
*/

#ifndef __BINOUT_OBJ
	#define __BINOUT_OBJ

/***** Includes		*****/
	#include "CommonUtils.h"

/***** Constants	*****/


/***** Definitions	*****/
	struct sPWMControl_t {
		uint8_t nMinValue;
		uint8_t nMaxValue;
	};

	class cBinOutput_t {
	public:
		char *Name;
		
		/**	@brief		Constructor to prepare the instance for use
			@param		nBinOutPin		Pin number of the binary output to control
			@param		pName			Name to assign to this output pin
			@param		bInvertValue	True if the logic True should be for 0 Volts out, and false for High voltage
		*/
		cBinOutput_t(uint8_t nBinOutPin, char *pName = NULL, bool bInvertValue = false);
		
		/**	@brief		Destructor that releases all allocated resources
		*/
		~cBinOutput_t();
		
		bool Initialize(bool bForceAllowPWM = false);
		bool IsOn();
		bool IsOff();
		bool Update();
		
		/**	@brief		Assign a text name to this binary output pin
			@param		pName		The name to be assigned
			@return		True if the name is copied into Name, false on failure
		*/
		bool SetName(const char *pName);
		
		bool TurnOn();
		bool TurnOff();
		bool Toggle();
		
		bool SetPWMControlType(const sPWMControl_t *pPWMControls);
		uint8_t GetPWMValue();
		bool SetPWMValue(uint8_t nPWMValue, uint32_t nMilliSec = 0);
		float GetPWMPercent();
		bool SetPWMPercent(float nPWMPercent, uint32_t nMilliSec = 0);
		
	protected:
	
	private:
		uint8_t cnPinNum;			/** @brief	Pin Number on the Arduino that is being controlled */
		bool cbCanDoPWM;			/** @brief	True if the pin has PWM (pulse width modulation ) support */
		bool cbInvertValue;			/** @brief	True if the logic is inverted from the voltage level */
		bool cbCurrState;			/** @brief	The current logical state of the pin, as of last update */
		uint8_t cnCurrPWM;			/** @brief	The current PWM setting for the pin */
		uint8_t cnPWMToSet;			/**	@brief	The PWM level that it is to be changed to over some time period */
		uint32_t ctPWMChangeStart;	/** @brief	Starting mSec of the timed PWM chagne */
		uint32_t ctPWMChangeEnd;	/** @brief	Ending mSec of the timed PWM change */
		sPWMControl_t cPWMCfg;		/** @brief	Configuration in use for the PWM controls */
	};

/***** Globals		*****/


/***** Prototypes 	*****/


/***** Functions	*****/
cBinOutput_t(uint8_t nBinOutPin, char *pName, bool bInvertValue) {
	cnPinNum = nBinOutPin;
	cbInvertValue = bInvertValue;
	
	#ifdef ARDUINO_TYPE
		cbCanDoPWM = NumberInArray_u8(cnPinNum, BINOUT_PWNPINLIST, BINOUT_PWMCOUNT);
	#else
		cbCanDoPWM = false; //No type defined, assume PWM won't work
	#endif
	
	Name = NULL;
	if (pName != NULL) {
		SetName(pName);
	}
	
	return;
}

~cBinOutput_t() {
	if (Name != NULL) {
		delete Name;
	}
	Name = NULL;
	
	return;
}

bool cBinOutput_t::Initialize(bool bForceAllowPWM) {
	pinMode(cnPinNum, OUTPUT);
	
	digitalWrite(cnPinNum, LOW);
	if (cbInvertValue == false) {
		cbCurrState = false;
	} else {
		cbCurrState = true;
	}
	
	cnCurrPWM = 0;
	cnPWMToSet = 0;
	ctPWMChangeStart = 0;
	ctPWMChangeEnd = 0;
	
	//Set default PWM limits
	cPWMCfg.nMinValue = 0;
	cPWMCfg.nMaxValue = 255;
	
	if (bForceAllowPWM == true) {
		cbCanDoPWM = true;
	}
	
	return true;
}

bool cBinOutput_t::SetName(const char *pName) {
	uint16_t nLen;

	if (cpName != NULL) {
		free(Name);
	}

	nLen = strlen(pName) + 1; //Add 1 for the null terminator

	Name = new char[nLen];

	if (cpName == NULL) {
		return false;
	}

	memcpy(cpName, pName, sizeof(char) * nLen);
	cpName[nLen - 1] = '\0'; //Just to be sure the string is terminated

	return true;
}

#endif

