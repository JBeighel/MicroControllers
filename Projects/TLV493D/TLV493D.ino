//----- Include files     -----//
  #include "CommonUtils.h"
  #include "CommonUtils.c"
  #include "GPIOGeneralInterface.h"
  #include "GPIOGeneralInterface.c"
  #include "I2CGeneralInterface.h"
  #include "I2CGeneralInterface.c"
  
  #include "GPIO_Arduino.h"
  #include "I2C_Arduino.h"

  #include "TLV493DDriver.h"
  #include "TLV493DDriver.c"

//----- Constants         -----//
  #define OUT_SERIAL    Serial1
  
//----- Definitions       -----//

//----- Globals           -----//
  sGPIOIface_t gGPIO;
  sI2CIface_t gI2C;

  sTLV493DDev_t gSensor;

//----- Arduino Functions -----//
void setup() {
  //Arduino hardware setup
  OUT_SERIAL.begin(9600);

  //General Interface setup
  GPIO_INIT(&gGPIO, GPIO_HWINFO);
  I2C_1_INIT(&gI2C, true, 100000, I2C_1_HWINFO);

  //Setup peripherals
  TLV493DInit(&gSensor, &gI2C, TLV493DAddr_Base);

  return;
}

void loop() {
  eTLV493DReturn_t eReturn;
  float nXVal, nYVal, nZVal, nTemp;
  uint16_t tCurrTime = millis();
  static int nCtr = 0;

  delay(700);
  OUT_SERIAL.print("Addr: ");
  OUT_SERIAL.print(gSensor.nI2CAddr, HEX);
  OUT_SERIAL.print("\r\n");

  eReturn = TLV493DReadSingleMeasurement(&gSensor, &nXVal, &nYVal, &nZVal, &nTemp);
  if (eReturn != TLV493D_Success) {
    //nCtr += 1;
    //if (nCtr >= 5) {
      //TLV493DInit(&gSensor, &gI2C, TLV493DAddr_Base);
      //OUT_SERIAL.print("RE-Init\r\n");
      //nCtr = 0;
    //}
    
    OUT_SERIAL.print("T,");
    OUT_SERIAL.print(tCurrTime);
    OUT_SERIAL.print(" Reading FAIL ");
    OUT_SERIAL.print(eReturn);
    OUT_SERIAL.print(", I2C ");
    OUT_SERIAL.print(gSensor.eLastI2CErr);
    OUT_SERIAL.print("\r\n");
  } else {
    OUT_SERIAL.print("T,");
    OUT_SERIAL.print(tCurrTime);
    OUT_SERIAL.print(",X,");
    OUT_SERIAL.print(nXVal);
    OUT_SERIAL.print(",Y,");
    OUT_SERIAL.print(nYVal);
    OUT_SERIAL.print(",Z,");
    OUT_SERIAL.print(nZVal);
    OUT_SERIAL.print("\r\n");
  }
  
  return;
}

//----- Callback Functions -----//

//----- Application Code   -----//
