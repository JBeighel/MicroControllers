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
  #define SERIAL_OUT    Serial1

//----- Definitions       -----//

//----- Globals           -----//
  sGPIOIface_t gGPIO;
  sSPIIface_t gSpi;

  sAS5147Dev_t gPosSens;

//----- Arduino Functions -----//
void setup() {
  //Arduino hardware setup
  SERIAL_OUT.begin(9600);

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
    SERIAL_OUT.print("U,");
    SERIAL_OUT.print(nUncAngle);
    SERIAL_OUT.print(",C,");
    SERIAL_OUT.print(nComAngle);
    SERIAL_OUT.print(",M,");
    SERIAL_OUT.print(nMag);
    SERIAL_OUT.print(",G,");
    SERIAL_OUT.print(nAGC);
    SERIAL_OUT.print("\r\n");
  } else {
    SERIAL_OUT.print("Reading FAIL: ");
    SERIAL_OUT.print(eReturn);
    SERIAL_OUT.print("\r\n");
  }

  delay(500);
  return;
}

//----- Callback Functions -----//

//----- Application Code   -----//
