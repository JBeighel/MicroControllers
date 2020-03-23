//----- Include files     -----//
  #include <Servo.h>

  #include <IRremote.h>
  #include <IRremoteInt.h>
  
  #include "CommonUtils.h"
  #include "HCSR04Driver.h"

//----- Constants         -----//
  #define PIN_SERVO         3

  #define PIN_IR_RMT        2
  
  #define PIN_DIST_TRIG     A3
  #define PIN_DIST_ECHO     4

  #define PIN_FL_FWD        5
  #define PIN_FL_BCK        7
  #define PIN_FR_FWD        6
  #define PIN_FR_BCK        8
  #define PIN_BL_FWD        11
  #define PIN_BL_BCK        10
  #define PIN_BR_FWD        12
  #define PIN_BR_BCK        9

  #define PIN_LIGHT_LEFT    A0
  #define PIN_LIGHT_MID     A1
  #define PIN_LIGHT_RIGHT   A2
  
//----- Definitions       -----//
  typedef enum eRemoteCmds_t {
    BTN_UPARROW     = 0xFF629D,
    BTN_LEFTARROW   = 0xFF22DD,
    BTN_RIGHTARROW  = 0xFFC23D,
    BTN_DOWNARROW   = 0xFFA857,
    BTN_OK          = 0xFFD2FD,

    SIGNAL_REPEAT   = 0xFFFFFFFF,
  } eRemoteCmds_t;
  
//----- Globals           -----//
  cHCSR04Driver_t gDistSensor(PIN_DIST_TRIG, PIN_DIST_ECHO);
  IRrecv gIRRemote(PIN_IR_RMT);

  eRemoteCmds_t gLastRemoteCmd;
  
//----- Arduino Functions -----//
void setup() {
  //Initialize arduino hardware
  Serial.begin(9600);   //Debug output, and also the BT input

  pinMode(PIN_SERVO, OUTPUT);
  pinMode(PIN_FL_FWD, OUTPUT);
  pinMode(PIN_FL_BCK, OUTPUT);
  pinMode(PIN_FR_FWD, OUTPUT);
  pinMode(PIN_FR_BCK, OUTPUT);
  pinMode(PIN_BL_FWD, OUTPUT);
  pinMode(PIN_BL_BCK, OUTPUT);
  pinMode(PIN_BR_FWD, OUTPUT);
  pinMode(PIN_BR_BCK, OUTPUT);

  digitalWrite(PIN_FL_FWD, LOW);
  digitalWrite(PIN_FR_FWD, LOW);
  digitalWrite(PIN_BL_FWD, LOW);
  digitalWrite(PIN_BR_FWD, LOW);
  
  digitalWrite(PIN_FL_BCK, LOW);
  digitalWrite(PIN_FR_BCK, LOW);
  digitalWrite(PIN_BL_BCK, LOW);
  digitalWrite(PIN_BR_BCK, LOW);

  pinMode(PIN_DIST_ECHO, INPUT);
  pinMode(PIN_LIGHT_LEFT, INPUT);
  pinMode(PIN_LIGHT_MID, INPUT);
  pinMode(PIN_LIGHT_RIGHT, INPUT);

  //Initialize peripherals
  gIRRemote.enableIRIn();
  gIRRemote.blink13(true);
  gDistSensor.SetReadInterval(HCSR04_NOINTERVAL);

  gLastRemoteCmd = BTN_OK; //Ok is the stop button

  return;
}

void loop() {
  uint32_t nCurrTime = millis();
  decode_results IRResult;
  
  //Check distance sensor
  if (gDistSensor.SingleReading() != false) { //Have a good reading, make sure we're not about to crash
    //Serial.println(gDistSensor.cnDistanceCM);
    
    if (gDistSensor.cnDistanceCM < 10) { //Too close, stop moving
      AllWheelsStop();

      return;
    }
  }

  //Check the remote
  if (gIRRemote.decode(&IRResult)){
    Serial.println(IRResult.value, HEX);
    gIRRemote.resume();

    if ((IRResult.value != SIGNAL_REPEAT) && ((IRResult.value != gLastRemoteCmd))) { //Got a brand new command
      AllWheelsStop();

      gLastRemoteCmd = IRResult.value;
    } else { //Remote is saying to repeat the last command
      switch (gLastRemoteCmd) {
        case BTN_UPARROW:
          AllWheelsForward();
          break;
        case BTN_LEFTARROW:
          AllWheelsTurnLeft();
          break;
        case BTN_RIGHTARROW:
          AllWheelsTurnRight();
          break;
        case BTN_DOWNARROW:
          AllWheelsBackward();
          break;
        case BTN_OK:
        default: //Unrecognized button
          AllWheelsStop();
          break;
      }
    }
  }

  return;
}

//----- Callback Functions -----//

//----- Application Code   -----//
bool SetWheelSpeed(uint32_t nWheelPin, float nSpeedPct) {
  uint8_t nPWMVal;

  if (nSpeedPct > 1) {
    nSpeedPct * 1;
  } else if (nSpeedPct < 0) {
    nSpeedPct = 0;
  }

  nPWMVal = (uint8_t)(nSpeedPct * 255);

  switch (nWheelPin) {
    case (PIN_FL_FWD) :
    case (PIN_FR_FWD) :
    case (PIN_BL_FWD) :
    case (PIN_BR_FWD) : //Forward motors are connected to PWM
      analogWrite(nWheelPin, nPWMVal);
      return true;
    case (PIN_FL_BCK) :
    case (PIN_FR_BCK) :
    case (PIN_BL_BCK) :
    case (PIN_BR_BCK) : //Reverse motors are straight binary outs
      if (nPWMVal > 0) { //Some forward value is requested
        digitalWrite(nWheelPin, HIGH);
      } else { //Tell the motor to stop
        digitalWrite(nWheelPin, LOW);
      }
      return true;
    default: //Unrecognized wheel pin
      return false;
  }
}

bool AllWheelsStop() {
  SetWheelSpeed(PIN_FL_FWD, 0);
  SetWheelSpeed(PIN_FR_FWD, 0);
  SetWheelSpeed(PIN_BL_FWD, 0);
  SetWheelSpeed(PIN_BR_FWD, 0);

  SetWheelSpeed(PIN_FL_BCK, 0);
  SetWheelSpeed(PIN_FR_BCK, 0);
  SetWheelSpeed(PIN_BL_BCK, 0);
  SetWheelSpeed(PIN_BR_BCK, 0);

  return true;
}

bool AllWheelsForward() {
  SetWheelSpeed(PIN_FL_FWD, 1);
  SetWheelSpeed(PIN_FR_FWD, 1);
  SetWheelSpeed(PIN_BL_FWD, 1);
  SetWheelSpeed(PIN_BR_FWD, 1);

  SetWheelSpeed(PIN_FL_BCK, 0);
  SetWheelSpeed(PIN_FR_BCK, 0);
  SetWheelSpeed(PIN_BL_BCK, 0);
  SetWheelSpeed(PIN_BR_BCK, 0);

  return true;
}

bool AllWheelsBackward() {
  SetWheelSpeed(PIN_FL_FWD, 0);
  SetWheelSpeed(PIN_FR_FWD, 0);
  SetWheelSpeed(PIN_BL_FWD, 0);
  SetWheelSpeed(PIN_BR_FWD, 0);

  SetWheelSpeed(PIN_FL_BCK, 1);
  SetWheelSpeed(PIN_FR_BCK, 1);
  SetWheelSpeed(PIN_BL_BCK, 1);
  SetWheelSpeed(PIN_BR_BCK, 1);

  return true;
}

bool AllWheelsTurnLeft() {
  SetWheelSpeed(PIN_FL_FWD, 0);
  SetWheelSpeed(PIN_FR_FWD, 1);
  SetWheelSpeed(PIN_BL_FWD, 0);
  SetWheelSpeed(PIN_BR_FWD, 1);

  SetWheelSpeed(PIN_FL_BCK, 1);
  SetWheelSpeed(PIN_FR_BCK, 0);
  SetWheelSpeed(PIN_BL_BCK, 1);
  SetWheelSpeed(PIN_BR_BCK, 0);

  return true;
}

bool AllWheelsTurnRight() {
  SetWheelSpeed(PIN_FL_FWD, 1);
  SetWheelSpeed(PIN_FR_FWD, 0);
  SetWheelSpeed(PIN_BL_FWD, 1);
  SetWheelSpeed(PIN_BR_FWD, 0);

  SetWheelSpeed(PIN_FL_BCK, 0);
  SetWheelSpeed(PIN_FR_BCK, 1);
  SetWheelSpeed(PIN_BL_BCK, 0);
  SetWheelSpeed(PIN_BR_BCK, 1);

  return true;
}
