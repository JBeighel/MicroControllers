/**	@defgroup	binout_obj
	@brief		Class to encapsulate Binary Outputs including Pulse Width functionality
	@details	v 0.1
		
*/

#ifndef __BINOUT_OBJ
	#define __BINOUT_OBJ

/***** Includes		*****/
	#include "CommonUtils.h"

/***** Definitions	*****/
	/**	@brief		Flags describing the current state and configuration of the binary output
		@ingroup	binout_obj
	*/
	enum eBinOutFlags_t {
		BinOut_None				= 0x00,	/**< No state information, used to clear the flags register */
		BinOut_CurrState		= 0x01,	/**< Set if the output is logic true */
		BinOut_CanPWM			= 0x02,	/**< Set if the output supports pulse width modulating */
		BinOut_InvertValue		= 0x04,	/**< Set if the object is using reverse logic */
		BinOut_PWMScheduled		= 0x08,	/**< Set if a PWM change is scheduled */
		BinOut_OnScheduled		= 0x10,	/**< Set if a Turn On (set logic true) is scheduled */
		BinOut_OffScheduled		= 0x20,	/**< Set if a Turn Of (set logic false) is scheduled */
		BinOut_ToggleScheduled	= 0x40,	/**< Set if a Toggle (flip logic level) is scheduled */
		BinOut_ScheduleMask		= 0x71,	/**< Mask of all bits that indicate a scheduled change */
	}

	/**	@brief		Structure to describe the ranges and values allowed 
		@details	Intended to be applied in constant values to act as drivers for devices 
			being controlled through this binary output object.  This was initially designed with
			servo motors in mind, but should be adequate for other devices as well.
		@ingroup	binout_obj
	*/
	struct sPWMControl_t {
		int16_t nMaxAngle;		/**< Maximum angle that the motor can turn to */
		int16_t nMinAngle;		/**< Minimum angle that the motor can turn to */
		uint8_t nMinValue;		/**< Minimum PWM value that the motor can handle */
		uint8_t nMaxValue;		/**< Maximum PWM value that the motor can handle */
		bool bAllowValidOnly;	/**< True if the list of valid PWM values are the only ones the motor can handle */
		uint8_t nNumValid;		/**< Number of valid PWM values in the list */
		uint8_t anValidValues[10];	/**< List of PWM common used values that the motor can use */
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
		
		/**	@brief		Check if the output is currently logic true
			@details	Under normal circumstances logic true is high voltage on the pin.  If
				if the logic is inverted, this would be true for a low voltage on the pin.
			@return		True if the output is logic true, false otherwise
		*/
		bool IsOn();
		
		/**	@brief		Check if the output is currently logic false
			@details	Under normal circumstances logic false is low voltage on the pin.  If
				if the logic is inverted, this would be false for a high voltage on the pin.
			@return		True if the output is logic false, false otherwise
		*/
		bool IsOff();
		bool Update();
		
		/**	@brief		Assign a text name to this binary output pin
			@param		pName		The name to be assigned
			@return		True if the name is copied into Name, false on failure
		*/
		bool SetName(const char *pName);
		
		/**	@brief		Schedules the output to be set to logic true
			@details	The output will be set to true, regardless of its current state or state
				at that time, after the specified number of milliseconds has passed.
				
				If the time is set to zero it will happen immediately.  Any value will set it to
				happen in the future.  In cases where it is scheduled, the Update() function must 
				be called to carry out the operation.
				
				Only one operation can be scheduled at a time.  Wen this is called any existing 
				scheduled operations will be removed.
			@param		tInHowManymSec		Number of milliseconds from now that the state will be set
			@return		True if the change is scheduled, false on any failure
		*/
		bool TurnOn(uint32_t tInHowManymSec = 0);
		
		/**	@brief		Schedules the output to be set to logic false
			@details	The output will be set to false, regardless of its current state or state
				at that time, after the specified number of milliseconds has passed.
				
				If the time is set to zero it will happen immediately.  Any value will set it to
				happen in the future.  In cases where it is scheduled, the Update() function must 
				be called to carry out the operation.
				
				Only one operation can be scheduled at a time.  Wen this is called any existing 
				scheduled operations will be removed.
			@param		tInHowManymSec		Number of milliseconds from now that the state will be set
			@return		True if the change is scheduled, false on any failure
		*/
		bool TurnOff(uint32_t tInHowManymSec = 0);
		
		/**	@brief		Schedules the output t have its logic level switched
			@details	The output will be set to the opposite of its state when the operation
				happens.  When the specified amount of time has ellapsed it will change from logic 
				false to logic true, or from logic true to logic false.
				
				If the time is set to zero it will happen immediately.  Any value will set it to
				happen in the future.  In cases where it is scheduled, the Update() function must 
				be called to carry out the operation.
				
				Only one operation can be scheduled at a time.  Wen this is called any existing 
				scheduled operations will be removed.
			@param		tInHowManymSec		Number of milliseconds from now that the state will be set
			@return		True if the change is scheduled, false on any failure
		*/
		bool Toggle(uint32_t tInHowManymSec = 0);
		
		bool SetPWMControlType(const sPWMControl_t *pPWMControls);
		uint8_t GetPWMValue();
		bool SetPWMValue(uint8_t nPWMValue, uint32_t tOverManymSec = 0);
		float GetPWMPercent();
		bool SetPWMPercent(float nPWMPercent, uint32_t tOverManymSec = 0);
		
	protected:
	
	private:
		eBinOutFlags_t ceFlags,		/** @brief	Holds current state and pending commands for this Bin Out */
		uint8_t cnPinNum;			/** @brief	Pin Number on the Arduino that is being controlled */
		bool cbCanDoPWM;			/** @brief	True if the pin has PWM (pulse width modulation ) support */
		uint8_t cnCurrPWM;			/** @brief	The current PWM setting for the pin */
		uint8_t cnPWMToSet;			/**	@brief	The PWM level that it is to be changed to over some time period */
		uint32_t ctScheduleStart;	/** @brief	Starting mSec of the timed/scheduled command chagne */
		uint32_t ctScheduleEnd;		/** @brief	Ending mSec of the timed/scheduled command change */
		sPWMControl_t cPWMCfg;		/** @brief	Configuration in use for the PWM controls */
	};
	
/***** Constants	*****/
	const sPWMControl_t gsBO_Default {
		.nMinAngle = -180;
		.nMaxAngle = 180;
		.nMinValue = 0;
		.nMaxValue = 255;
		.bAllowValidOnly = true;
		.nNumValid = 0;
		.anValidValues = {
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0 //Other values are not needed
		};
	}

	const sPWMControl_t gsBO_FS90MR {
		.nMinAngle = 0;
		.nMaxAngle = 0;
		.nMinValue = 0;
		.nMaxValue = 255;
		.bAllowValidOnly = true;
		.nNumValid = 3;
		.anValidValues = {
			0,		//Servo is stopped
			90,		//Servo is moving clockwise
			225,	//Servo is moving counter clockwise
			0, 0, 0, 0, 0, 0, 0 //Other values are not needed
		};
	};
	
	const sPWMControl_t gsBO_MG90S {
		.nMinAngle = -60;
		.nMaxAngle = 60;
		.nMinValue = 0;
		.nMaxValue = 255;
		.bAllowValidOnly = false;
		.nNumValid = 9;
		.anValidValues = {
			0,		//Servo is at 60 degrees clockwise
			31,		//Servo is at 45 degrees clockwise
			63,		//Servo is at 30 degrees clockwise
			95,		//Servo is at 15 degrees clockwise
			127,	//Servo is at zero degrees
			160,	//Servo is at 15 degrees counter clockwise
			191,	//Servo is at 30 degrees counter clockwise
			223,	//Servo is at 45 degrees counter clockwise
			255,	//Servo is at 60 degrees counter clockwise
			0 		//Other values are not needed
		};
	};

/***** Globals		*****/


/***** Prototypes 	*****/


/***** Functions	*****/
cBinOutput_t::cBinOutput_t(uint8_t nBinOutPin, char *pName, bool bInvertValue) {
	ceFlags = BinOut_None;
	
	if (bInvertValue == true) {
		SetAllBitsInMask(ceFlags, BinOut_InvertValue);
	}
	
	#ifdef ARDUINO_TYPE
		if (NumberInArray_u8(cnPinNum, BINOUT_PWNPINLIST, BINOUT_PWMCOUNT) == 1) {
			SetAllBitsInMask(ceFlags, BinOut_CanPWM);
		}
	#else
		ZeroAllBitsInMask(ceFlags, BinOut_CanPWM); //No type defined, assume PWM won't work
	#endif
	
	Name = NULL;
	if (pName != NULL) {
		SetName(pName);
	}
	
	return;
}

cBinOutput_t::~cBinOutput_t() {
	if (Name != NULL) {
		delete Name;
	}
	Name = NULL;
	
	return;
}

bool cBinOutput_t::Initialize(bool bForceAllowPWM) {
	pinMode(cnPinNum, OUTPUT);
	
	digitalWrite(cnPinNum, LOW);
	if (CheckAllBitsInMask(ceFlags, BinOut_InvertValue) == false) {
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
		SetAllBitsInMask(ceFlags, BinOut_CanPWM);
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

bool cBinOutput_t::IsOn() {
	Update();
	
	return CheckAllBitsInMask(ceFlags, BinOut_CurrState);
}

bool cBinOutput_t::IsOff() {
	Update();
	
	return CheckAllBitsInMask(ceFlags, BinOut_CurrState);
}

bool cBinOutput_t::TurnOn(uint32_t tInHowManymSec) { //Mark the change as scheduled
	ClearAllBitsInMask(ceFlags, BinOut_ScheduleMask);
	
	SetAllBitsInMask(ceFlags, BinOut_OnScheduled);
	ctPWMChangeStart = millis();
	ctScheduleEnd = ctPWMChangeStart + tInHowManymSec;
	
	return Update(); //Update will apply changes based on the scheduling
}

bool cBinOutput_t::TurnOff(uint32_t tInHowManymSec) { //Mark the change as scheduled
	ClearAllBitsInMask(ceFlags, BinOut_ScheduleMask);

	SetAllBitsInMask(ceFlags, BinOut_OffScheduled);
	ctPWMChangeStart = millis();
	ctScheduleEnd = ctPWMChangeStart + tInHowManymSec;
	
	return Update(); //Update will apply changes based on the scheduling
}

bool cBinOutput_t::TurnOff(uint32_t tInHowManymSec) { //Mark the change as scheduled
	ClearAllBitsInMask(ceFlags, BinOut_ScheduleMask);

	SetAllBitsInMask(ceFlags, BinOut_ToggleScheduled);
	ctPWMChangeStart = millis();
	ctScheduleEnd = ctPWMChangeStart + tInHowManymSec;
	
	return Update(); //Update will apply changes based on the scheduling
}

#endif

