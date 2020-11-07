//----- Include files     -----//
  #include "CommonUtils.h"
  #include "CommonUtils.c"
  #include "GPIOGeneralInterface.h"
  #include "GPIOGeneralInterface.c"
  #include "I2CGeneralInterface.h"
  #include "I2CGeneralInterface.c"
  
  #include "GPIO_Arduino.h"
  #include "I2C_Arduino.h"

  #include "MMC3630Driver.h"
  #include "MMC3630Driver.c"

//----- Constants         -----//
  #define OUT_SERIAL    Serial1
  
//----- Definitions       -----//

//----- Globals           -----//
  sGPIOIface_t gGPIO;
  sI2CIface_t gI2C;

  sMMC3630Dev_t gSensor;

//----- Arduino Functions -----//
void setup() {
  //Arduino hardware setup
  OUT_SERIAL.begin(9600);

  //General Interface setup
  GPIO_INIT(&gGPIO, GPIO_HWINFO);
  I2C_1_INIT(&gI2C, true, 100000, I2C_1_HWINFO);

  //Setup peripherals
  MMC3630Init(&gSensor, &gI2C, MMC3630Addr_Base);

  return;
}

void loop() {
  eMMC3630Return_t eReturn;
  int16_t nXVal, nYVal, nZVal;
  float nTemp;
  uint16_t tCurrTime = millis();

  delay(500);

  eReturn = MMC3630ReadSingleMeasurement(&gSensor, &nXVal, &nYVal, &nZVal, &nTemp);
  if (eReturn != MMC3630_Success) {
    OUT_SERIAL.print("T,");
    OUT_SERIAL.print(tCurrTime);
    OUT_SERIAL.print(" Reading FAIL ");
    OUT_SERIAL.print(eReturn);
    //OUT_SERIAL.print(", I2C ");
    //OUT_SERIAL.print(gSensor.eLastI2CErr);
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
