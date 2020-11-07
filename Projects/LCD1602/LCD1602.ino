//----- Include files     -----//
  #include "CommonUtils.h"
  #include "CommonUtils.c"
  #include "GPIOGeneralInterface.h"
  #include "GPIOGeneralInterface.c"
  #include "I2CGeneralInterface.h"
  #include "I2CGeneralInterface.c"
  
  #include "GPIO_Arduino.h"
  #include "I2C_Arduino.h"

  #include "TC1602ADriver.h"
  #include "TC1602ADriver.c"

//----- Constants         -----//
  #define LCD_ENABLEPIN   6
  #define LCD_RDWRPIN     7
  #define LCD_REGSELPIN   8  
  #define LCD_DATA4PIN    5
  #define LCD_DATA5PIN    4
  #define LCD_DATA6PIN    3
  #define LCD_DATA7PIN    2

//----- Definitions       -----//

//----- Globals           -----//
  sTimeIface_t gTime;
  sGPIOIface_t gGPIO;
  sI2CIface_t gI2C;

  sTC1602AInfo_t gLCD;

//----- Arduino Functions -----//
void setup() {
  //Arduino hardware setup
  Serial.begin(9600);

  //General Interface setup
  TIME_INIT(&gTime);
  GPIO_INIT(&gGPIO, GPIO_HWINFO);
  I2C_1_INIT(&gI2C, true, 100000, I2C_1_HWINFO);

  //Setup peripherals
  TC1602AInit4Data(&gLCD, &gTime, &gGPIO, true, 20, 4, LCD_ENABLEPIN, LCD_RDWRPIN, LCD_REGSELPIN, LCD_DATA4PIN, LCD_DATA5PIN, LCD_DATA6PIN, LCD_DATA7PIN);

  return;
}

void loop() {
  static uint8_t nColPos = 0, nCycleCol = 0, nCycleRow = 2, nCycleChg = 1;
  uint8_t nCtr;
  
  TC1602AInitClearDisplay(&gLCD);

  TC1602AInitSetCursorPosition(&gLCD, nColPos, 0);

  TC1602AInitPrintCharacter(&gLCD, 'A');
  TC1602AInitPrintCharacter(&gLCD, 'b');
  TC1602AInitPrintCharacter(&gLCD, 'C');

  nColPos += 1;
  if (nColPos > 17) {
    nColPos = 0;
  }

  TC1602AInitSetCursorPosition(&gLCD, 0, 1);
  for (nCtr = 0; nCtr < 20; nCtr++) {
    if (nCtr < 10) {
      TC1602AInitPrintCharacter(&gLCD, '0' + nCtr);
    } else {
      TC1602AInitPrintCharacter(&gLCD, '0' + (nCtr - 10));
    }
  }

  TC1602AInitSetCursorPosition(&gLCD, nCycleCol, nCycleRow);
  TC1602AInitPrintCharacter(&gLCD, '<');
  TC1602AInitPrintCharacter(&gLCD, '>');

  nCycleCol += nCycleChg;
  if ((nCycleCol == 0) && (nCycleChg == 0)) { //Reached end of bottom row
    nCycleRow = 2;
    nCycleChg = 1;
  } else if ((nCycleCol == 0) && (nCycleChg != 0)) { //1 more move to end of bottom row
    nCycleChg = 0;
  } else if ((nCycleCol >= 18)  && (nCycleChg == 0)) { //Reached end of top row
    nCycleRow = 3;
    nCycleChg = -1;
  } else if ((nCycleCol >= 18)  && (nCycleChg != 0)) { //1 more to end of top row
    nCycleChg = 0;
  }

  gTime.pfDelayMilliSeconds(250);
  
  return;
}

//----- Callback Functions -----//

//----- Application Code   -----//
