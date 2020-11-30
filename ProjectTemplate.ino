//----- Include files     -----//
  #include "CommonUtils.h"
  #include "CommonUtils.c"
  #include "GPIOGeneralInterface.h"
  #include "GPIOGeneralInterface.c"
  #include "I2CGeneralInterface.h"
  #include "I2CGeneralInterface.c"
  #include "SPIGeneralInterface.h"
  #include "SPIGeneralInterface.c"
  
  #include "GPIO_Arduino.h"
  #include "I2C_Arduino.h"
  #include "SPI_Arduino.h"

  #include "Driver.h"
  #include "Driver.c"

//----- Constants         -----//

//----- Definitions       -----//

//----- Globals           -----//
  //Generla interface objects
  sGPIOIface_t gGPIO;
  sI2CIface_t gI2C;
  sSPIIface_t gSpi;
  
  //Peripheral objects

//----- Arduino Functions -----//
void setup() {
  //Arduino hardware setup
  Serial.begin(9600);

  //General Interface setup
  GPIO_INIT(&gGPIO, GPIO_HWINFO);
  I2C_1_PORTINIT(&gI2C, true, 100000, I2C_1_HWINFO);
  SPI_1_INIT(&gSpi, SPI_1_HWINFO, 5000000, SPI_MSBFirst, SPI_Mode0);

  //Peripheral setup

  return;
}

void loop() {
  
  return;
}

//----- Callback Functions -----//

//----- Application Code   -----//
