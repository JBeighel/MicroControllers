//----- Include files     -----//
  #include "CommonUtils.h"
  #include "CommonUtils.c"
  #include "GPIOGeneralInterface.h"
  #include "GPIOGeneralInterface.c"
  #include "SPIGeneralInterface.h"
  #include "SPIGeneralInterface.c"
  
  #include "GPIO_Arduino.h"
  #include "SPI_Arduino.h"

  #include "AS5147Driver.h"
  #include "AS5147Driver.c"

//----- Constants         -----//
  #define PIN_POSCS     2

//----- Definitions       -----//

//----- Globals           -----//
  sGPIOIface_t gGPIO;
  sSPIIface_t gSpi;

  sAS5147Dev_t gPosSens;

//----- Arduino Functions -----//
void setup() {
  //Arduino hardware setup
  Serial.begin(9600);

  //General Interface setup
  GPIO_INIT(&gGPIO, GPIO_HWINFO);
  SPI_1_INIT(&gSpi, SPI_1_HWINFO, 2000000, SPI_MSBFirst, SPI_Mode1);

  //Setup peripherals
  AS5147Init(&gPosSens, &gGPIO, &gSpi, PIN_POSCS);

  return;
}

void loop() {
  eAS5147Return_t eReturn;
  uint16_t nUncAngle, nComAngle, nMag, nAGC;
  
  eReturn = AS5147TakeReading(&gPosSens, &nUncAngle, &nComAngle, &nMag, &nAGC);
  if (eReturn == AS5147_Success) {
    Serial.print("U,");
    Serial.print(nUncAngle);
    Serial.print(",C,");
    Serial.print(nComAngle);
    Serial.print(",M,");
    Serial.print(nMag);
    Serial.print(",G,");
    Serial.print(nAGC);
    Serial.print("\n");
  } else {
    Serial.print("Reading FAIL: ");
    Serial.print(eReturn);
    Serial.print("\n");
  }

  delay(500);
  return;
}

//----- Callback Functions -----//

//----- Application Code   -----//
