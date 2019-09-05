/**	@defgroup	binin_obj
	@brief		Class to encapsulate Binary Inputs and core functionality
	@details	v 0.1
		
*/

#ifndef __BININ_OBJ
	#define __BININ_OBJ

/***** Includes		*****/
	#include <string.h>

	#include "CommonUtils.h"

/***** Constants	*****/
	/** Minimum time in milliseconds an input must be low before accepting the change 
		@ingroup binin_obj
	*/
	#ifndef INPUT_DEBOUNCETIME
		#define INPUT_DEBOUNCETIME	50
	#endif

/***** Definitions	*****/
	/**	@brief		Enumeration of all flags used internally in cBinInput_t class
		@ingroup	binin_obj
	*/
	typedef enum eBinInputFlags_t {
		BI_None				= 0x00,	/**< Clear all flags, no statuses are set */

		BI_CurrentState		= 0x01,	/**< Current state of the I/O pin */
		BI_InvertValue		= 0x02,	/**< Set in the value is inverted from the voltage level */
		BI_Debouncing		= 0x04,	/**< Set if the pin has recently changed and debounce checking is active */
		BI_HasPullUp		= 0x08,	/**< Set if the input is using the internal pull up resistor */

		BI_EventMask		= 0xF0,
		BI_EventChanged		= 0x10,
		BI_EventHigh		= 0x20,
		BI_EventLow			= 0x40,
	} eBinInputFlags_t;

	typedef class cBinInput_t {
	public:
		char *cpName;

		cBinInput_t(uint8_t nBinInPin, bool bInvertValue) : cBinInput_t(nBinInPin, bInvertValue, true, NULL) { }

		cBinInput_t(uint8_t nBinInPin, bool bInvertValue, bool bUsePullUp) : cBinInput_t(nBinInPin, bInvertValue, bUsePullUp, NULL) { }

		cBinInput_t(uint8_t nBinInPin, bool bInvertValue, bool bUsePullUp, const char *pName);

		~cBinInput_t();

		bool Initialize();

		bool IsOn();

		bool IsOff();

		bool DidGoOn();

		bool DidGoOff();

		bool DidChange();

		bool Update();

		bool SetName(const char *pName);

	protected:
		uint32_t ctDebounceStart;
		uint32_t ctStateStart;
		eBinInputFlags_t ceFlags; 
		uint8_t	cnPinNum;

	private:

	};

/***** Globals		*****/


/***** Prototypes 	*****/


/***** Functions	*****/
cBinInput_t::cBinInput_t(uint8_t nBinInPin, bool bInvertValue, bool bUsePullUp, const char *pName) {
	//Constructor sets class variables only, init function does the rest
	ctDebounceStart = 0;
	ctStateStart = 0;
	ceFlags = BI_None;
	cnPinNum = nBinInPin;

	if (bInvertValue == true) {
		SetAllBitsInMask(ceFlags, BI_InvertValue);
	}

	if (bUsePullUp == true) {
		SetAllBitsInMask(ceFlags, BI_HasPullUp);
	}

	if (pName != NULL) {
		//Need space for every character + NULL terminator
		cpName = (char *)malloc(sizeof(char) * (strlen(pName) + 1));

		if (cpName != NULL) {
			strcpy(cpName, pName);
		}
	} else {
		cpName = NULL;
	}

	return;
}

cBinInput_t::~cBinInput_t() {
	if (cpName != NULL) {
		free(cpName);
	}

	return;
}

bool cBinInput_t::Initialize() {
	uint32_t nCurrValue;

	//Ensure that the pin is in the correct mode
	if (CheckAllBitsInMask(ceFlags, BI_HasPullUp) == true) {
		pinMode(cnPinNum, INPUT_PULLUP);
	} else {
		pinMode(cnPinNum, INPUT);
	}

	nCurrValue = digitalRead(cnPinNum);
	//Work out the current state, and set the initial state
	if (nCurrValue == HIGH) { //Pin is HIGH/Set/True
		if (CheckAllBitsInMask(ceFlags, BI_InvertValue) == true) {
			ZeroAllBitsInMask(ceFlags, BI_CurrentState);
		} else {
			SetAllBitsInMask(ceFlags, BI_CurrentState);
		}
	} else { //Pin is LOW/Clear/False
		if (CheckAllBitsInMask(ceFlags, BI_InvertValue) == true) {
			SetAllBitsInMask(ceFlags, BI_CurrentState);
		} else {
			ZeroAllBitsInMask(ceFlags, BI_CurrentState);
		}
	}

	return true;
}

bool cBinInput_t::IsOn() {
	Update();
	int TestVal;

	ZeroAllBitsInMask(ceFlags, BI_EventMask); //The user checked the state, clear events

	if (CheckAllBitsInMask(ceFlags, BI_CurrentState) == true) {
		return true;
	} else {
		return false;
	}
}

bool cBinInput_t::IsOff() {
	Update();

	ZeroAllBitsInMask(ceFlags, BI_EventMask); //The user checked the state, clear events

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
	ZeroAllBitsInMask(ceFlags, BI_EventHigh); //The user checked the event, clear it

	return bResult;
}

bool cBinInput_t::DidGoOff() {
	bool bResult;

	Update();

	bResult = CheckAllBitsInMask(ceFlags, BI_EventLow);
	ZeroAllBitsInMask(ceFlags, BI_EventLow); //The user checked the event, clear it

	return bResult;
}

bool cBinInput_t::DidChange() {
	bool bResult;

	Update();

	bResult = CheckAllBitsInMask(ceFlags, BI_EventChanged);
	ZeroAllBitsInMask(ceFlags, BI_EventChanged); //The user checked the event, clear it

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
		ZeroAllBitsInMask(ceFlags, BI_Debouncing); //Stop any debouncing that was happening

		return true;
	}

	//Check debouncing for this pin
	tCurrTime = millis();

	if ((CheckAllBitsInMask(ceFlags, BI_Debouncing) == true) && (tCurrTime - ctDebounceStart >= INPUT_DEBOUNCETIME)) {
		//Debounce time hasn't ellapsed yet, wait to ensure input has settled
		return true;
	} else if (CheckAllBitsInMask(ceFlags, BI_Debouncing) == false) { //Need to start debouncing
		ctDebounceStart = tCurrTime;
		SetAllBitsInMask(ceFlags, BI_Debouncing);

		return true;
	}

	//Debounce time has ellapsed, the input has changed state
	if (nCurrValue == true) {
		SetAllBitsInMask(ceFlags, BI_CurrentState | BI_EventChanged | BI_EventHigh);
	} else {
		ZeroAllBitsInMask(ceFlags, BI_CurrentState);
		SetAllBitsInMask(ceFlags, BI_EventChanged | BI_EventLow);
	}

	return true;
}

bool cBinInput_t::SetName(const char *pName) {
	uint16_t nLen;

	if (cpName != NULL) {
		free(cpName);
	}

	nLen = strlen(pName) + 1; //Add 1 for the null terminator

	cpName = (char *)malloc(sizeof(char) * nLen);

	if (cpName == NULL) {
		return false;
	}

	memcpy(cpName, pName, sizeof(char) * nLen);
	cpName[nLen - 1] = '\0'; //Just to be sure the string is terminated

	return true;
}

#endif

