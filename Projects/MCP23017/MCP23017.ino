//----- Include files     -----//
  #include "CommonUtils.h"
  #include "CommonUtils.c"
  #include "GPIOGeneralInterface.h"
  #include "GPIOGeneralInterface.c"
  #include "I2CGeneralInterface.h"
  #include "I2CGeneralInterface.c"
  //#include "SPIGeneralInterface.h"
  //#include "SPIGeneralInterface.c"
  
  //#include "GPIO_Arduino.h"
  //#include "SPI_Arduino.h"
  #include "I2C_Arduino.h"

  #include "MCP23017Driver.h"
  #include "MCP23017Driver.c"

//----- Constants         -----//

//----- Definitions       -----//

//----- Globals           -----//
  sGPIOIface_t gGPIO;
  sI2CIface_t gI2C;
  //sSPIIface_t gSpi;

  sMCP23017Info_t gExp;

//----- Arduino Functions -----//
void setup() {
  int nCtr;
  
  //Arduino hardware setup
  Serial.begin(9600);

  //General Interface setup
  //GPIO_INIT(&gGPIO, GPIO_HWINFO);
  I2C_1_INIT(&gI2C, true, 100000, I2C_1_HWINFO);
  //SPI_1_INIT(&gSpi, SPI_1_HWINFO, 5000000, SPI_MSBFirst, SPI_Mode0);

  MCP23017Initialize(&gExp, (eMCP23017Address_t)(MCP23017_Base | MCP23017_A0 | MCP23017_A1 | MCP23017_A2), &gI2C);

  for (nCtr = 0; nCtr < 8; nCtr++) {
    MCP23017PinModeByPin(&gExp, nCtr, MCP23017_Input);
    MCP23017PinModeByPin(&gExp, nCtr + 8, MCP23017_Output);
  }

  return;
}

void loop() {
  static uint32_t tLast = 0;
  uint32_t tNow;
  bool bLevel;

  tNow = millis();

  if (tLast == 0) { //Starting up, set an output
    MCP23017WriteOutputByPin(&gExp, 8, true);
    tLast = tNow;
  }

  if (tNow - tLast > 5000) { //Every 5 seconds toggle the output
    MCP23017ReadInputByPin(&gExp, 8, &bLevel);

    if (bLevel == true) {
      MCP23017WriteOutputByPin(&gExp, 8, false);
    } else {
      MCP23017WriteOutputByPin(&gExp, 8, true);
    }
    
    tLast = tNow;
  }

  MCP23017ReadInputByPin(&gExp, 0, &bLevel);
  if (bLevel == true) {
    Serial.println("Pin 0: HIGH");
  } else {
    Serial.println("Pin 0: low");
  }

  MCP23017ReadInputByPin(&gExp, 1, &bLevel);
  if (bLevel == true) {
    Serial.println("Pin 1: HIGH");
  } else {
    Serial.println("Pin 1: low");
  }

  delay(50);
  
  return;
}

//----- Callback Functions -----//

//----- Application Code   -----//
