/**	@defgroup	sanpduino_oop_binin
	@brief		
	@details	
		
*/

#ifndef __SNAPCIRCUITS_OOP
	#define __SNAPCIRCUITS_OOP

/***** Includes		*****/
	#include "CommonUtils.h"

/***** Constants	*****/
	/** Minimum time an input must be low before accepting the change 
		@ingroup sanpduino_oop_binin
	*/
	#define INPUT_DEBOUNCETIME	50

/***** Definitions	*****/
	typedef enum eBinInputFlags_t {
		BI_CurrentState		= 0x01,
		BI_InvertValue		= 0x02,
		BI_Debouncing		= 0x04,
		BI_HasPullUp		= 0x08,

		BI_EventMask		= 0xF0
		BI_EventChanged		= 0x10,
		BI_EventHigh		= 0x20,
		BI_EventLow			= 0x40,
	} eBinInputFlags_t;

	typedef cBinInput_t {
	public:
		cBinInput_t(uint8_t nBinInPin, bool bInvertValue) : cBinInput_t(nBinInPin, bool bInvertValue, true) { }

		cBinInput_t(uint8_t nBinInPin, bool bInvertValue, bool bUsePullUp);

		~cBinInput_t();

		bool Initialize();

		bool IsOn();

		bool IsOff();

		bool DidGoOn();

		bool DidGoOff();

		bool DidChange();

		bool Update();

	protected:
		uint32_t ctDebounceStart;
		uint32_t ctStateStart;
		eBinInputFlags_t ceStatus; 
		uint8_t	cnPinNum;

	private:

	};

/***** Globals		*****/


/***** Prototypes 	*****/


/***** Functions	*****/
cBinInput_t::cBinInput_t(uint8_t nBinInPin, bool bInvertValue, bool bUsePullUp) {
	//Constructor sets class variables onle, init function does the rest
	ctDebounceStart = 0;
	ctStateStart = 0;
	ceStatus = 0;
	cnPinNum = nBinInPin;

	if (bInvertValue == true) {
		ceStatus = BI_InvertValue;
	}

	if (bUsePullUp == true) {
		ceStatus = BI_HasPullUp;
	}

	return;
}

cBinInput_t::~cBinInput_t() {
	//No resources allocated, no clean up needed
}

bool cBinInput_t::Initialize() {
	//Ensure that the pin is in the correct mode
	if (CheckAllBitsInMask(ceFlags, BI_HasPullUp) == true) {
		pinMode(BIN_INPUTPINLIST[nPinCtr], INPUT_PULLUP);
	} else {
		pinMode(BIN_INPUTPINLIST[nPinCtr], INPUT);
	}

	return;
}

bool cBinInput_t::IsOn() {
	Update();

	ceFlags &= ~BI_EventMask; //The user checked the state, clear events

	if (CheckAllBitsInMask(ceFlags, BI_CurrentState) == true) {
		return true;
	} else {
		return false;
	}
}

bool cBinInput_t::IsOff() {
	Update();

	ceFlags &= ~BI_EventMask; //The user checked the state, clear events

	if (CheckAllBitsInMask(ceFlags, BI_CurrentState) == true) {
		return false;
	} else {
		return true;
	}
}

bool cBinInput_t::DidGoOn() {
	bool bResult;

	Update();

	bResult = CheckAllBitsInMask(ceFlags, BI_EventHigh);
	ceFlags &= ~BI_EventHigh; //The user checked the event, clear it

	return bResult;
}

bool cBinInput_t::DidGoOff() {
	bool bResult;

	Update();

	bResult = CheckAllBitsInMask(ceFlags, BI_EventLow);
	ceFlags &= ~BI_EventLow; //The user checked the event, clear it

	return bResult;
}

bool cBinInput_t::DidChange() {
	bool bResult;

	Update();

	bResult = CheckAllBitsInMask(ceFlags, BI_EventChanged);
	ceFlags &= ~BI_EventChanged; //The user checked the event, clear it

	return bResult;
}

bool cBinInput_t::Update() {
	uint8_t nCurrValue, nPriorValue;
	uint32_t tCurrTime;

	nCurrValue = digitalRead(cnPinNum);

	//Figure out the prior state
	if (CheckAllBitsInMask(ceFlags, BI_CurrentState) == true) {
		nPriorValue = true;
	} else {
		nPriorValue = false;
	}

	//Work out the current state
	if (nCurrValue == HIGH) { //Pin is HIGH/Set/True
		if (CheckAllBitsInMask(ceFlags, BI_InvertValue) == true) {
			nCurrValue = false;
		} else {
			nCurrValue = true;
		}
	} else { //Pin is LOW/Clear/False
		if (CheckAllBitsInMask(ceFlags, BI_InvertValue) == true) {
			nCurrValue = true;
		} else {
			nCurrValue = false;
		}
	}

	if (nPriorValue == nCurrValue) { //There was no change in state
		ceFlags &= ~BI_Debouncing; //Stop any debouncing that was happening
		return true;
	}

	//Check debouncing for this pin
	tCurrTime = millis();

	if ((CheckAllBitsInMask(ceFlags, BI_Debouncing) == true) && (tCurrTime - ctDebounceStart >= INPUT_DEBOUNCETIME) {
		//Debounce time hasn't ellapsed yet, wait to ensure input has settled
		return true;
	} else { //Need to start debouncing
		ctDebounceStart = tCurrTime;
		ceFlags |= BI_Debouncing;

		return true;
	}

	//Debounce time has ellapsed, the input has changed state
	if (nCurrValue == true) {
		ceFlags |= BI_CurrentState | BI_EventChanged | BI_EventHigh;
	} else {
		ceFlags &= ~BI_CurrentState;
		ceFlags |= BI_EventChanged | BI_EventLow;
	}

	return true;
}

#endif

