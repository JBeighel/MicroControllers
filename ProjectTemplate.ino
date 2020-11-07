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

//----- Definitions       -----//

//----- Globals           -----//
  sGPIOIface_t gGPIO;
  sSPIIface_t gSpi;

//----- Arduino Functions -----//
void setup() {
  //Arduino hardware setup
  Serial.begin(9600);

  //General Interface setup
  GPIO_INIT(&gGPIO, GPIO_HWINFO);
  SPI_1_INIT(&gSpi, SPI_1_HWINFO, 5000000, SPI_MSBFirst, SPI_Mode0);

  return;
}

void loop() {
  
  return;
}

//----- Callback Functions -----//

//----- Application Code   -----//
