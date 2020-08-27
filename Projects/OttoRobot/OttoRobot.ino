//----- Include files     -----//
  #include "CommonUtils.h"
  #include "SAMDTimer.h"

//----- Constants         -----//
  #define GPIO_PWM1         10    /**< GPIO pin number for the first PWM output */
  #define GPIO_PWM2         9     /**< GPIO pin number for the second PWM output */
  #define GPIO_PWM3         6     /**< GPIO pin number for the third PWM output */
  #define GPIO_PWM4         5     /**< GPIO pin number for the fourth PWM output */

  #define GPIO_SPEAKER      7     /**< GPIO pin for the piezo speaker */
  #define TIMER_SPEAKER     Timer_2     /**< Timer used to control the output frequency of the piezo speaker */
  
  #define PWM_COUNT         4     /**< Total count of PWM outputs */
  #define PWM_PERIOD        20000 /**< Period of the PWM sequence, 50Hz or 20 mSec */
  #define PWM_MAXPULSE      2000  /**< Maximum high time for the PWM pulses */
  #define PWM_CENTERPULSE   ((PWM_MAXPULSE + PWM_MINPULSE) / 2)  /**< PWM Pulse to center the motor [(2000 + 700) / 2 = 1350] */
  #define PWM_MINPULSE      700   /**< Minimum high time for the PWM pulses */
  
  #define PWMIDX_LEFTFOOT   0     /**< PWM output index for the left foot motor */
  #define PWMIDX_LEFTLEG    1     /**< PWM output index for the left leg motor */
  #define PWMIDX_RIGHTFOOT  2     /**< PWM output index for the right foot motor */
  #define PWMIDX_RIGHTLEG   3     /**< PWM output index for the right leg motor */

//----- Definitions       -----//
  typedef enum eFootPos_t {
    FOOT_TOEUPALL = 0,
    FOOT_TOEUPBALANCE,
    FOOT_TOEUPHALF,
    FOOT_FLAT,
    FOOT_TOEDOWNHALF,
    FOOT_TOEDOWNALL,
    
    FOOT_NUMPOS,
	FOOT_NOINSTR,
  } eFootPos_t;

  typedef enum eLegPos_t {
    LEG_OUTALL = 0,
    LEG_OUTHALF,
    LEG_CENTER,
    LEG_INHALF,
    LEG_INALL,
    
    LEG_NUMPOS,
	LEG_NOINSTR,
  } eLegPos_t;

  typedef enum eSpeakerTone_t {
    SPKR_MIN    = 400,

    SPKR_WHERE    = 700, //250 mSec
    SPKR_ARE      = 400, //500 mSec
    SPKR_YOU      = 500, //750 mSec

    SPKR_I        = 350, //250 mSec, then 250 mSec quiet
    SPKR_SEE      = 450, //500 mSec, then 250 mSec quiet (then you for 500 mSec)

    SPKR_MAX    = 2100, //Lowest tone tested
  } eSpeakerTone_t;

  typedef enum eMove_t {
	MOVE_LEFT_STEPFWRD,
	MOVE_RIGHT_STEPFWD,
	MOVE_LEFT_STEPBACK,
	MOVE_RIGHT_STEPBACK,
	MOVE_TIPTOE,
	MOVE_SIT,
	MOVE_STANDSTILL,
  } eMove_t;

  /** Structure of all information needed to control a PWM output, all are 50Hz/20mSec periods with max of 2mSec On time */
  typedef struct sPWMInfo_t {
    uint32_t tPulseDuration;      /**< Duration in microseconds that the pulse will remain high */
    uint32_t nGPIONum;            /**< GPIO in number for this PWM output */
    bool bIsHigh;                 /**< Set true with the output is High/On, false for output Low/Off */
  } sPWMInfo_t;

  typedef struct sMoveInstr_t {
    eFootPos_t eLFootPos;
    eFootPos_t eRFootPos;
    eLegPos_t eLLegPos;
    eLegPos_t eRLegPos;
    uint32_t tInstrDur;
  } sMoveInstr_t;
  
  typedef struct sMotorRunInfo_t {
    uint32_t tPrevTime;  		/*< Time stamp when the motor run state was last checked */
    uint32_t tNextEvent; 		/*< Time when the next motor state change will happen */

    eMove_t eCurrMove;			/*< Current move that is being carried out */
    uint8_t nNextMovIdx;		/*< Index in the list of moves that will happen next */
	
	uint8_t nMoveCnt;			/*< Number of moves in the list being carried out */
	sMoveInstr_t *pMoveList;	/*< Pointer to list of moves to be carried out */
  } sMotorRunInfo_t;

//----- Globals           -----//
  const uint32_t ganLeftFootPos[FOOT_NUMPOS] = {
    PWM_MINPULSE,
    ((PWM_CENTERPULSE + PWM_MINPULSE) * 7) / 16,
    ((PWM_CENTERPULSE + PWM_MINPULSE) / 2),
    PWM_CENTERPULSE + 100,
    (PWM_MAXPULSE + PWM_CENTERPULSE) / 2,
    PWM_MAXPULSE,
  };

  const uint32_t ganRightFootPos[FOOT_NUMPOS] = {
    PWM_MAXPULSE,
    (((PWM_MAXPULSE + PWM_CENTERPULSE) * 9) / 16) + 100,
    ((PWM_MAXPULSE + PWM_CENTERPULSE) / 2) + 150,
    PWM_CENTERPULSE + 100,
    (PWM_CENTERPULSE + PWM_MINPULSE) / 2,
    PWM_MINPULSE
  };

  const uint32_t ganLeftLegPos[LEG_NUMPOS] = {
    PWM_MINPULSE + 300,
    (PWM_CENTERPULSE + PWM_MINPULSE) / 2,
    PWM_CENTERPULSE + 115,
    (PWM_MAXPULSE + PWM_CENTERPULSE) / 2,
    PWM_MAXPULSE - 180
  };

  const uint32_t ganRightLegPos[LEG_NUMPOS] = {
    PWM_MAXPULSE - 200,
    (PWM_MAXPULSE + PWM_CENTERPULSE) / 2,
    PWM_CENTERPULSE + 50,
    (PWM_CENTERPULSE + PWM_MINPULSE) / 2,
    PWM_MINPULSE + 300
  };

  const sMoveInstr_t gaMoveStandStill[] = {
    { .eLFootPos = FOOT_FLAT,			.eRFootPos = FOOT_FLAT,			.eLLegPos = LEG_CENTER,		.eRLegPos = LEG_CENTER,		.tInstrDur = 200 },
  };
  
  const sMoveInstr_t gaMoveTipToe[] = {
    { .eLFootPos = FOOT_TOEDOWNHALF,	.eRFootPos = FOOT_TOEDOWNHALF,	.eLLegPos = LEG_CENTER,		.eRLegPos = LEG_CENTER,		.tInstrDur = 200 },
  };
  
  const sMoveInstr_t gaMoveSit[] = {
    { .eLFootPos = FOOT_TOEUPALL,		.eRFootPos = FOOT_TOEUPALL,		.eLLegPos = LEG_CENTER,		.eRLegPos = LEG_CENTER,		.tInstrDur = 200 },
  };
  
  const sMoveInstr_t gaMoveLStepFwd[] = {
    { .eLFootPos = FOOT_FLAT,			.eRFootPos = FOOT_TOEUPHALF,	.eLLegPos = LEG_CENTER,		.eRLegPos = LEG_CENTER,		.tInstrDur = 200 }, 
	{ .eLFootPos = FOOT_TOEDOWNALL,		.eRFootPos = FOOT_TOEUPHALF,	.eLLegPos = LEG_CENTER,		.eRLegPos = LEG_CENTER,		.tInstrDur = 200 }, //Begin lean
	{ .eLFootPos = FOOT_TOEDOWNALL,		.eRFootPos = FOOT_TOEUPHALF,	.eLLegPos = LEG_OUTALL,		.eRLegPos = LEG_CENTER,		.tInstrDur = 300 }, //Push onto one foot
 { .eLFootPos = FOOT_TOEDOWNALL,   .eRFootPos = FOOT_TOEUPBALANCE,  .eLLegPos = LEG_OUTALL,   .eRLegPos = LEG_CENTER,   .tInstrDur = 500 }, //Get other foot off ground
	{ .eLFootPos = FOOT_FLAT,		.eRFootPos = FOOT_TOEUPBALANCE,	.eLLegPos = LEG_CENTER,		.eRLegPos = LEG_INALL,		.tInstrDur = 500 }, //Rotate to get foot forward
 { .eLFootPos = FOOT_FLAT,   .eRFootPos = FOOT_FLAT,  .eLLegPos = LEG_CENTER,    .eRLegPos = LEG_INHALF,   .tInstrDur = 500 }, //plant the foot

{ .eLFootPos = FOOT_FLAT,     .eRFootPos = FOOT_FLAT,     .eLLegPos = LEG_CENTER,   .eRLegPos = LEG_CENTER,   .tInstrDur = 300 },
 
	{ .eLFootPos = FOOT_TOEUPHALF,			.eRFootPos = FOOT_FLAT,			.eLLegPos = LEG_CENTER,		.eRLegPos = LEG_CENTER,		.tInstrDur = 200 },
	{ .eLFootPos = FOOT_TOEUPHALF,			.eRFootPos = FOOT_TOEDOWNALL,			.eLLegPos = LEG_CENTER,		.eRLegPos = LEG_CENTER,		.tInstrDur = 200 },
 { .eLFootPos = FOOT_TOEUPHALF,     .eRFootPos = FOOT_TOEDOWNALL,     .eLLegPos = LEG_CENTER,   .eRLegPos = LEG_OUTALL,   .tInstrDur = 300 },
 { .eLFootPos = FOOT_TOEUPBALANCE,     .eRFootPos = FOOT_TOEDOWNALL,     .eLLegPos = LEG_CENTER,   .eRLegPos = LEG_OUTALL,   .tInstrDur = 500 },
 { .eLFootPos = FOOT_TOEUPBALANCE,   .eRFootPos = FOOT_FLAT,  .eLLegPos = LEG_INALL,    .eRLegPos = LEG_CENTER,   .tInstrDur = 500 },
 { .eLFootPos = FOOT_FLAT,   .eRFootPos = FOOT_FLAT,  .eLLegPos = LEG_INHALF,    .eRLegPos = LEG_CENTER,   .tInstrDur = 500 },

 { .eLFootPos = FOOT_FLAT,     .eRFootPos = FOOT_FLAT,     .eLLegPos = LEG_CENTER,   .eRLegPos = LEG_CENTER,   .tInstrDur = 300 },
  };
  
  sMotorRunInfo_t gMotorInfo = {
    .tPrevTime = 0,
    .tNextEvent = 0,
	
	.eCurrMove = MOVE_STANDSTILL,
	.nNextMovIdx = 0,
	
	.nMoveCnt = 0,
	.pMoveList = NULL,
  };

  uint8_t gnNextPWMIdx;             /**< Index of the next PWM output that will need to be set low */
  sPWMInfo_t gaPWMTimes[PWM_COUNT]; /**< Array of all PWM output information structures */

//----- Arduino Functions -----//
void setup() {
  Serial.begin(9600);
  
  //Setup the timer object
  gTimer.Begin();
  gTimer.SetCallback(Timer_1, Timer1Handler);
  gTimer.SetCallback(Timer_2, Timer2Handler);

  gTimer.SetTick(Timer_1, PWM_PERIOD * 10);

  gnNextPWMIdx = PWM_COUNT; //Set invalid time so we restart the sequence

  //Set all PWM information structures and setup the GPIO pins
  gaPWMTimes[0].nGPIONum = GPIO_PWM1;
  gaPWMTimes[0].tPulseDuration = PWM_MINPULSE;
  gaPWMTimes[0].bIsHigh = false; 
  pinMode(gaPWMTimes[0].nGPIONum, OUTPUT);
  digitalWrite(gaPWMTimes[0].nGPIONum, LOW);
  
  gaPWMTimes[1].nGPIONum = GPIO_PWM2;
  gaPWMTimes[1].tPulseDuration = PWM_MINPULSE;
  gaPWMTimes[1].bIsHigh = false;
  pinMode(gaPWMTimes[1].nGPIONum, OUTPUT);
  digitalWrite(gaPWMTimes[1].nGPIONum, LOW);
  
  gaPWMTimes[2].nGPIONum = GPIO_PWM3;
  gaPWMTimes[2].tPulseDuration = PWM_MINPULSE;
  gaPWMTimes[2].bIsHigh = false;
  pinMode(gaPWMTimes[2].nGPIONum, OUTPUT);
  digitalWrite(gaPWMTimes[2].nGPIONum, LOW);
  
  gaPWMTimes[3].nGPIONum = GPIO_PWM4;
  gaPWMTimes[3].tPulseDuration = PWM_MINPULSE;
  gaPWMTimes[3].bIsHigh = false;
  pinMode(gaPWMTimes[3].nGPIONum, OUTPUT);
  digitalWrite(gaPWMTimes[3].nGPIONum, LOW);

  //Setup the output for the piezzo speaker
  pinMode(GPIO_SPEAKER, OUTPUT);
  digitalWrite(GPIO_SPEAKER, LOW);

  //All setup is done, now start the timer
  gTimer.Enable(Timer_1, true);
  return;
}

void loop() {
  bool bResult;
  uint32_t tCurrTime = millis();
  uint16_t nPatternNum = (tCurrTime / 750) % 10;

  if (gMotorInfo.eCurrMove == MOVE_STANDSTILL) {
    Serial.print("No Move\n\n");
    SetMove(MOVE_LEFT_STEPFWRD);
    return;
  } else {
    Serial.print("\n\nEND\n\n");
    delay(1000);
  }

  RunMotors(tCurrTime);
  delay(100);
  
  return;
}

bool RunMotors(uint32_t tCurrTime) {
	uint32_t tTimeEllapsed = tCurrTime - gMotorInfo.tPrevTime;

	gMotorInfo.tPrevTime = tCurrTime;  //Record the time to check next pass
	
	if (gMotorInfo.pMoveList == NULL) { //No moves to carry out, go to stand still to await next move
		//MoveToStandStill();
		return true;
	}
	
	if (tTimeEllapsed < gMotorInfo.tNextEvent) { //Haven't reached the time for the next position
		gMotorInfo.tNextEvent -= tTimeEllapsed;
		return true; //Nothing to do, give up time slice
	}
	
	//Time to move to the next position
	SetLeftFootPos(gMotorInfo.pMoveList[gMotorInfo.nNextMovIdx].eLFootPos);
	SetRightFootPos(gMotorInfo.pMoveList[gMotorInfo.nNextMovIdx].eRFootPos);
	SetLeftLegPos(gMotorInfo.pMoveList[gMotorInfo.nNextMovIdx].eLLegPos);
	SetRightLegPos(gMotorInfo.pMoveList[gMotorInfo.nNextMovIdx].eRLegPos);

	gMotorInfo.tNextEvent = gMotorInfo.pMoveList[gMotorInfo.nNextMovIdx].tInstrDur;
	gMotorInfo.nNextMovIdx += 1; //Advance to the next move in the list
	
	if (gMotorInfo.nNextMovIdx >= gMotorInfo.nMoveCnt) {//Ran out of instructions, move complete
		gMotorInfo.tPrevTime = 0;
		gMotorInfo.tNextEvent = 0;
		
		gMotorInfo.eCurrMove = MOVE_STANDSTILL;
		gMotorInfo.nNextMovIdx = 0;
		
		gMotorInfo.nMoveCnt = 0;
		gMotorInfo.pMoveList = NULL;
	}

	return true;
}

bool SetMove(eMove_t eNextMove) {
	gMotorInfo.tPrevTime = 0;
	gMotorInfo.tNextEvent = 0;
	gMotorInfo.eCurrMove = eNextMove;
	gMotorInfo.nNextMovIdx = 0;
	
	switch (eNextMove) {
		case MOVE_TIPTOE:
			gMotorInfo.nMoveCnt = sizeof(gaMoveTipToe) / sizeof (sMoveInstr_t);
			gMotorInfo.pMoveList = (sMoveInstr_t *)gaMoveTipToe;
			break;
			
		case MOVE_SIT:
			gMotorInfo.nMoveCnt = sizeof(gaMoveSit) / sizeof (sMoveInstr_t);
			gMotorInfo.pMoveList = (sMoveInstr_t *)gaMoveSit;
			break;
		
		case MOVE_LEFT_STEPFWRD:
			gMotorInfo.nMoveCnt = sizeof(gaMoveLStepFwd) / sizeof (sMoveInstr_t);
			gMotorInfo.pMoveList = (sMoveInstr_t *)gaMoveLStepFwd;
			break;
			
		case MOVE_RIGHT_STEPFWD:
		case MOVE_LEFT_STEPBACK:
		case MOVE_RIGHT_STEPBACK:
		
		case MOVE_STANDSTILL:
		default: //Unknown instruction, go to default position
			gMotorInfo.nMoveCnt = 0;
			gMotorInfo.pMoveList = NULL;
			break;
	}

  return true; 
}

bool MoveToToesIn() {
  SetLeftFootPos(FOOT_FLAT);
  SetRightFootPos(FOOT_FLAT);
  SetLeftLegPos(LEG_INALL);
  SetRightLegPos(LEG_INALL);
}

bool MoveToToesOut() {
  SetLeftFootPos(FOOT_FLAT);
  SetRightFootPos(FOOT_FLAT);
  SetLeftLegPos(LEG_OUTALL);
  SetRightLegPos(LEG_OUTALL);
}

bool MoveToStandStill() {
  SetLeftFootPos(FOOT_FLAT);
  SetRightFootPos(FOOT_FLAT);
  SetLeftLegPos(LEG_CENTER);
  SetRightLegPos(LEG_CENTER);

  return true;
}

bool MoveToTipToe() {
  SetLeftFootPos(FOOT_TOEDOWNHALF);
  SetRightFootPos(FOOT_TOEDOWNHALF);
  SetLeftLegPos(LEG_CENTER);
  SetRightLegPos(LEG_CENTER);

  return true;
}

bool MoveToSit() {
  SetLeftFootPos(FOOT_TOEUPALL);
  SetRightFootPos(FOOT_TOEUPALL);
  SetLeftLegPos(LEG_CENTER);
  SetRightLegPos(LEG_CENTER);

  return true;
}

bool SetLeftFootPos(eFootPos_t ePos) {
  bool bRetVal = true;
  uint32_t nPulseUSec;
  
  if (ePos >= FOOT_NUMPOS) {
    return false;
  }

  nPulseUSec = ganLeftFootPos[ePos];

  if (nPulseUSec < PWM_MINPULSE) { //Motors won't accept a pulse this small
    nPulseUSec = PWM_MINPULSE;
    return false;
  } else if (nPulseUSec > PWM_MAXPULSE) { //Motors won't accept a pulse this large
    nPulseUSec = PWM_MAXPULSE;
    return false;
  }

  gaPWMTimes[PWMIDX_LEFTFOOT].tPulseDuration = nPulseUSec;

  return bRetVal;
}

bool SetRightFootPos(eFootPos_t ePos) {
  bool bRetVal = true;
  uint32_t nPulseUSec;
  
  if (ePos >= FOOT_NUMPOS) {
    return false;
  }

  nPulseUSec = ganRightFootPos[ePos];

  if (nPulseUSec < PWM_MINPULSE) { //Motors won't accept a pulse this small
    nPulseUSec = PWM_MINPULSE;
    bRetVal = false;
  } else if (nPulseUSec > PWM_MAXPULSE) { //Motors won't accept a pulse this large
    nPulseUSec = PWM_MAXPULSE;
    bRetVal = false;
  }

  gaPWMTimes[PWMIDX_RIGHTFOOT].tPulseDuration = nPulseUSec;

  return bRetVal;
}

bool SetLeftLegPos(eLegPos_t ePos) {
  bool bRetVal = true;
  uint32_t nPulseUSec;
  
  if (ePos >= LEG_NUMPOS) {
    return false;
  }

  nPulseUSec = ganLeftLegPos[ePos];

  if (nPulseUSec < PWM_MINPULSE) { //Motors won't accept a pulse this small
    nPulseUSec = PWM_MINPULSE;
    return false;
  } else if (nPulseUSec > PWM_MAXPULSE) { //Motors won't accept a pulse this large
    nPulseUSec = PWM_MAXPULSE;
    return false;
  }

  gaPWMTimes[PWMIDX_LEFTLEG].tPulseDuration = nPulseUSec;

  return bRetVal;
}

bool SetRightLegPos(eLegPos_t ePos) {
  bool bRetVal = true;
  uint32_t nPulseUSec;
  
  if (ePos >= LEG_NUMPOS) {
    return false;
  }

  nPulseUSec = ganRightLegPos[ePos];

  if (nPulseUSec < PWM_MINPULSE) { //Motors won't accept a pulse this small
    nPulseUSec = PWM_MINPULSE;
    return false;
  } else if (nPulseUSec > PWM_MAXPULSE) { //Motors won't accept a pulse this large
    nPulseUSec = PWM_MAXPULSE;
    return false;
  }

  gaPWMTimes[PWMIDX_RIGHTLEG].tPulseDuration = nPulseUSec;

  return bRetVal;
}

bool SetMotorPulseTime(uint8_t nMotorNum, uint32_t nPulseUSec) {
  bool bRetVal = true;

  if (nMotorNum >= PWM_COUNT) { //Invalid motor specified
    return false;
  }
  
  if (nPulseUSec < PWM_MINPULSE) { //Motors won't accept a pulse this small
    nPulseUSec = PWM_MINPULSE;
    bRetVal = false;
  } else if (nPulseUSec > PWM_MAXPULSE) { //Motors won't accept a pulse this large
    nPulseUSec = PWM_MAXPULSE;
    bRetVal = false;
  }

  gaPWMTimes[nMotorNum].tPulseDuration = nPulseUSec;

  return bRetVal;
}

//----- Callback Functions -----//

/** Timer 1 handles all of the PWM outputs */
void Timer1Handler() {
  uint32_t tCurrTime, tNextTime;
  uint8_t nNewNextPWMIdx;
  uint8_t nCtr;
  
  //Determine the current time, and update outputs for that
  if (gnNextPWMIdx >= PWM_COUNT) { //Restarting the cycle, start and time = 0
    tCurrTime = 0;

    for (nCtr = 0; nCtr < PWM_COUNT; nCtr++) { //Reset all PWM's to begin their pulse
      if (gaPWMTimes[nCtr].tPulseDuration > 0) {
        digitalWrite(gaPWMTimes[nCtr].nGPIONum, HIGH);
        gaPWMTimes[nCtr].bIsHigh = true;
      } else { //This PWM has no pulse
        digitalWrite(gaPWMTimes[nCtr].nGPIONum, LOW);
        gaPWMTimes[nCtr].bIsHigh = false;
      }
    }
  } else {  //Reach the end of high time for some PWM, that's the current time
    tCurrTime = gaPWMTimes[gnNextPWMIdx].tPulseDuration;

    digitalWrite(gaPWMTimes[gnNextPWMIdx].nGPIONum, LOW);
    gaPWMTimes[gnNextPWMIdx].bIsHigh = false;
  }

  //Find the next time actions need to be taken
  nNewNextPWMIdx = PWM_COUNT;
  tNextTime = PWM_PERIOD;
  for (nCtr = 0; nCtr < PWM_COUNT; nCtr++) {
    if (gaPWMTimes[nCtr].bIsHigh == true) { //Pin is high, check for change needed
      if (tCurrTime >= gaPWMTimes[nCtr].tPulseDuration) { //This one was missed on its time, fix it now
        digitalWrite(gaPWMTimes[nCtr].nGPIONum, LOW);
        gaPWMTimes[nCtr].bIsHigh = false;
      } else if (tNextTime > gaPWMTimes[nCtr].tPulseDuration) { //This time hasn't ellapsed, use it as the next wait
        nNewNextPWMIdx = nCtr;
        tNextTime = gaPWMTimes[nCtr].tPulseDuration;
      }
    }
  }

  //Set the next tick time
  if ((nNewNextPWMIdx >= PWM_COUNT) || (tNextTime >= PWM_PERIOD)) { //No more timers need updated, finish out the period
    tNextTime = PWM_PERIOD - tCurrTime;
    gnNextPWMIdx = PWM_COUNT;
  } else {
    tNextTime = tNextTime - tCurrTime;
    gnNextPWMIdx = nNewNextPWMIdx;
  }

  gTimer.SetTick(Timer_1, tNextTime);
  
  return;
}

/** Timer 2 will handle the piezzo speaker */
void Timer2Handler() {
  if (digitalRead(GPIO_SPEAKER) == HIGH) {
    digitalWrite(GPIO_SPEAKER, LOW);
  } else {
    digitalWrite(GPIO_SPEAKER, HIGH);
  }
  
  return;
}
