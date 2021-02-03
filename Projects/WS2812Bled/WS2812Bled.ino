//----- Include files     -----//
  //General libraries
  #include "CommonUtils.h"
  #include "CommonUtils.c"
  #include "TimeGeneralInterface.h"
  #include "TimeGeneralInterface.c"
  #include "GPIOGeneralInterface.h"
  #include "GPIOGeneralInterface.c"

  //Arduino libraries
  #include "GPIO_Arduino.h"

  //Peripheral libraries
  #include "WS2812BDriver.h"
  #include "WS2812BDriver.c"

//----- Constants         -----//
  #define PIN_LEDCTRL    3

//----- Definitions       -----//

//----- Globals           -----//
  //General interface objects
  sTimeIface_t gTime;
  sGPIOIface_t gGPIO;
  
  //Peripheral objects
  sWS2812BInfo_t gLED;
  
//----- Arduino Functions -----//
void setup() {
  //Arduino hardware setup
  Serial.begin(9600);

  //General Interface setup
  TIME_INIT(&gTime);
  GPIO_INIT(&gGPIO, GPIO_HWINFO);

  //Peripheral setup
  gGPIO.pfSetModeByPin(&gGPIO, PIN_LEDCTRL, GPIO_DigitalOutput);
  WS2812BInitialize(&gLED, &gTime, &gGPIO, PIN_LEDCTRL);

  return;
}

void loop() {
  WS2812BSetLightColor(&gLED, 0, 255, 255, 255);
  WS2812BSetLightColor(&gLED, 1, 0, 0, 255);
  WS2812BSetLightColor(&gLED, 2, 0, 255, 0);
  WS2812BSetLightColor(&gLED, 3, 255, 0, 0);

  WS2812BUpdateLights(&gLED);  
  
  return;
}

//----- Callback Functions -----//

//----- Application Code   -----//
