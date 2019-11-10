/**	@defgroup	stepper
	@brief		Controller for a stepper motor driver
	@details	v0.1
		Stepper motors require specific sequences sent to the coils to give it precise
		movement.  There are libraries available in the Arduino to handle this; however, these
		have a number of shortcomings.  They don't provide half-step control and do not 
		provide simple methods for controlling multiple motors.  This library is an attempt
		to understand how steppers work and provide functionality encompassing those 
		limitations.
		
		
		Nema-17 MOtors, Rotor Up Count left to right
		1 - 2B -> Out4 (Black)
		2 - NC
		3 - 2A -> Out2 (Green)
		4 - 1B -> Out3 (Red)
		5 - NC
		6 - 1A -> Out1 (Blue)
*/

#ifndef __STEPPER_H
	#define __STEPPER_H

/***** Includes		*****/


/***** Definitions	*****/
	//#define STEPPER_DEBUG
	#ifdef STEPPER_DEBUG
		#define DebugPrint(...)	Serial.print(__VA_ARGS__)
	#else
		#define DebugPrint(...)
	#endif

	#define STEPPER_NOPIN		0xFF

/**	@brief		Enumeration of all motor driving options available
	@details	This will configure the driver operations on how to control the stepper motor.
		There will be options to control motors using 2 to 4 wire/coil motors, as well as a
		driver that uses direction and step pins.
		
		These values will have two values encoded in them.  The lower 4 bits represent the 
		number of values in the sequence used to go from step to step.  The high 4 bytes will
		store the number of wires used by the control scheme.
	@ingroup	stepper
*/
typedef enum eStepperType_t {
	Stepper_Driver			= 0x20,
	Stepper_2WireFull		= 0x24,
	Stepper_3WireFull		= 0x33,
	Stepper_3WireHalf		= 0x36,
	Stepper_4WireFull		= 0x44,
	Stepper_4WireHalf		= 0x48,
	
	Stepper_NumStepMask		= 0x0F,
	Stepper_NumStepShift	= 0x00,
	
	Stepper_WireCntMask		= 0xF0,
	Stepper_WireCntShift	= 0x04,
} eStepperType_t;

/***** Constants	*****/
const uint8_t gaStep2WireFull[4] = { 0x02, 0x03, 0x01, 0x00 };
const uint8_t gaStep3WireFull[3] = { 0x04, 0x01, 0x02 };
const uint8_t gaStep3WireHalf[6] = { 0x04, 0x05, 0x01, 0x03, 0x02, 0x06 };

//These versions swap control lines 2 & 3, this matches the arduino step library
//const uint8_t gaStep4WireFull[4] = { 0x05, 0x06, 0x0A, 0x09 };
//const uint8_t gaStep4WireHalf[8] = { 0x01, 0x05, 0x04, 0x06, 0x02, 0x0A, 0x08, 0x09 };

//These put 2 and 3 in order, which matched the 28BYJ-48 stepper motor with ULN2003 driver 
const uint8_t gaStep4WireFull[4] = { 0x03, 0x06, 0x0C, 0x09 };
const uint8_t gaStep4WireHalf[8] = { 0x01, 0x03, 0x02, 0x06, 0x04, 0x0C, 0x08, 0x09 };

class cStepperMotor_t {
	public:
		cStepperMotor_t(eStepperType_t eMotorType, uint8_t nCtrlPin1, uint8_t nCtrlPin2, uint8_t nCtrlPin3, uint8_t nCtrlPin4);
		~cStepperMotor_t();
		
		bool Step(bool bForward);
		
	protected:
		eStepperType_t ceMotorType;
		uint8_t caCtrlPins[4];
		uint8_t cnCurrStep;
		uint8_t *cpStepSequence;
	
	private:
	
};

/***** Globals		*****/


/***** Prototypes 	*****/


/***** Functions	*****/

cStepperMotor_t::cStepperMotor_t(eStepperType_t eMotorType, uint8_t nCtrlPin1, uint8_t nCtrlPin2, uint8_t nCtrlPin3 = STEPPER_NOPIN, uint8_t nCtrlPin4 = STEPPER_NOPIN) {
	uint8_t nCtr;
	
	cnCurrStep = 0;
	
	caCtrlPins[0] = nCtrlPin1;
	caCtrlPins[1] = nCtrlPin2;
	caCtrlPins[2] = nCtrlPin3;
	caCtrlPins[3] = nCtrlPin4;
	
	for (nCtr = 0; nCtr < 4; nCtr++) {
		if (caCtrlPins[nCtr] != STEPPER_NOPIN) {
			pinMode(caCtrlPins[nCtr], OUTPUT);
		}
	}
	
	ceMotorType = eMotorType;
	
	switch(ceMotorType) {
		case (Stepper_Driver) :
			cpStepSequence = NULL;
			break;
		case (Stepper_2WireFull) :
			cpStepSequence = (uint8_t *)gaStep2WireFull;
			break;
		case (Stepper_3WireFull) :
			cpStepSequence = (uint8_t *)gaStep3WireFull;
			break;
		case (Stepper_3WireHalf) :
			cpStepSequence = (uint8_t *)gaStep3WireHalf;
			break;
		case (Stepper_4WireFull) :
			cpStepSequence = (uint8_t *)gaStep4WireFull;
			break;
		case (Stepper_4WireHalf) :
			cpStepSequence = (uint8_t *)gaStep4WireHalf;
			break;
		default: //Unknown type, choose a safe-ish one sicne we can't return an error
			ceMotorType = Stepper_2WireFull;
			cpStepSequence = (uint8_t *)gaStep2WireFull;
			break;
	}
	
	return;
}

cStepperMotor_t::~cStepperMotor_t() {
	//No resources allocatd, cleanup isn't necessary
	return;
}

bool cStepperMotor_t::Step(bool bForward) {
	uint8_t nCtr, nOutput;
	uint8_t nNumSteps = (ceMotorType & Stepper_NumStepMask) >> Stepper_NumStepShift;
	uint8_t nWireCnt = (ceMotorType & Stepper_WireCntMask) >> Stepper_WireCntShift;
	
	if (ceMotorType == Stepper_Driver) { //Using driver chip, let it handle step sequencing
		//Specify direction pin
		if (bForward == true) {
			digitalWrite(caCtrlPins[0], HIGH);
		} else {
			digitalWrite(caCtrlPins[0], LOW);
		}

		//Pulse the step pin to move the motor
		digitalWrite(caCtrlPins[1], HIGH);
		delayMicroseconds(200); //Delay to allow the driver to recognize the pulse
		digitalWrite(caCtrlPins[1], LOW);
		
		return true;
	}
	
	//Determine the outputs needed for the next step
	if (bForward == true) { //Rotating the motor forwards
		cnCurrStep += 1;
		
		if (cnCurrStep >= nNumSteps) { //At end of sequence, start over
			cnCurrStep = 0;
		}
	} else { //Rotating the motor backwards
		cnCurrStep -= 1;
		
		if (cnCurrStep == 0) { //At end of sequence, start over
			cnCurrStep = nNumSteps - 1;
		}
	}
	
	DebugPrint("Seq# ");
	DebugPrint(cnCurrStep);
	DebugPrint(" Val ");
	DebugPrint(cpStepSequence[cnCurrStep], HEX);
	DebugPrint("\n");
	
	//Send those outputs to the control pins
	nOutput = cpStepSequence[cnCurrStep];
	for (nCtr = 0; nCtr < nWireCnt; nCtr++) {
		if ((nOutput & (1 << nCtr)) != 0) {
			digitalWrite(caCtrlPins[nCtr], HIGH);
		} else {
			digitalWrite(caCtrlPins[nCtr], LOW);
		}
	}
	
	return true;
}

#endif

