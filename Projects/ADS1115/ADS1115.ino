//----- Include files     -----//
  #include "CommonUtils.h"
  #include "CommonUtils.c"
  #include "GPIOGeneralInterface.h"
  #include "GPIOGeneralInterface.c"
  #include "I2CGeneralInterface.h"
  #include "I2CGeneralInterface.c"
  
  #include "GPIO_Arduino.h"
  #include "I2C_Arduino.h"

  #include "ADS1115Driver.h"
  #include "ADS1115Driver.c"

//----- Constants         -----//
  #define PIN_ALERT       2
  #define PIN_READY       4
  #define OUT_SERIAL    Serial1
  
//----- Definitions       -----//

//----- Globals           -----//
  sGPIOIface_t gGPIO;
  sI2CIface_t gI2C;

  sADS1115Dev_t gSensor;

//----- Arduino Functions -----//
void setup() {
  //Arduino hardware setup
  OUT_SERIAL.begin(9600);

  //General Interface setup
  GPIO_INIT(&gGPIO, GPIO_HWINFO);
  I2C_1_INIT(&gI2C, true, 100000, I2C_1_HWINFO);

  //Setup peripherals
  ADS1115Init(&gSensor, &gGPIO, &gI2C, ADS1115Addr_Base, PIN_ALERT);

  return;
}

void loop() {
  eADS1115Return_t eReturn;
  int16_t nADCVal = 0;
  uint16_t tCurrTime = millis();

  eReturn = ADS1115TakeSingleReading(&gSensor, &nADCVal);
  if (eReturn != ADS1115_Success) {
    OUT_SERIAL.print("Reading FAIL ");
    OUT_SERIAL.print(eReturn);
    OUT_SERIAL.print("\r\n");
  }

  OUT_SERIAL.print("T,");
  OUT_SERIAL.print(tCurrTime);
  OUT_SERIAL.print(",V,");
  OUT_SERIAL.print(nADCVal);
  OUT_SERIAL.print("\r\n");
  
  delay(500);
  return;
}

//----- Callback Functions -----//

//----- Application Code   -----//
