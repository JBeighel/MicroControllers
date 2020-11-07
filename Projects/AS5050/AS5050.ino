//----- Include files     -----//
  #include "CommonUtils.h"
  #include "CommonUtils.c"
  #include "GPIOGeneralInterface.h"
  #include "GPIOGeneralInterface.c"
  #include "SPIGeneralInterface.h"
  #include "SPIGeneralInterface.c"
  
  #include "GPIO_Arduino.h"
  #include "SPI_Arduino.h"

  #include "AS5050Driver.h"
  #include "AS5050Driver.c"

//----- Constants         -----//
  #define PIN_POSCS     2
  #define OUT_SERIAL    Serial

//----- Definitions       -----//

//----- Globals           -----//
  sGPIOIface_t gGPIO;
  sSPIIface_t gSpi;

  sAS5050Dev_t gPosSens;

//----- Arduino Functions -----//
void setup() {
  //Arduino hardware setup
  OUT_SERIAL.begin(9600);

  //General Interface setup
  GPIO_INIT(&gGPIO, GPIO_HWINFO);
  SPI_1_INIT(&gSpi, SPI_1_HWINFO, 2000000, SPI_MSBFirst, SPI_Mode1);

  //Setup peripherals
  AS5050Init(&gPosSens, &gGPIO, &gSpi, PIN_POSCS);

  return;
}

void loop() {
  eAS5050Return_t eReturn;
  uint16_t nAngle, nAGC;
  uint16_t tCurrTime = millis();

  OUT_SERIAL.print("T,");
  OUT_SERIAL.print(tCurrTime);
  OUT_SERIAL.print(",");
  
  eReturn = AS5050TakeReading(&gPosSens, &nAngle, &nAGC);
  if (eReturn == AS5050_Success) {
    OUT_SERIAL.print("A,");
    OUT_SERIAL.print(nAngle);
    OUT_SERIAL.print(",G,");
    OUT_SERIAL.print(nAGC);
    OUT_SERIAL.print("\r\n");
  } else {
    OUT_SERIAL.print("Reading FAIL: ");
    OUT_SERIAL.print(eReturn);
    OUT_SERIAL.print(" Last Err: 0x");
    OUT_SERIAL.print(gPosSens.nLastErr, HEX);
    OUT_SERIAL.print("\r\n");
  }

  delay(500);
  return;
}

//----- Callback Functions -----//

//----- Application Code   -----//
