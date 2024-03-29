//----- Include files     -----//
  //General libraries
  #include "CommonUtils.h"
  #include "CommonUtils.c"
  #include "TimeGeneralInterface.h"
  #include "TimeGeneralInterface.c"
  #include "GPIOGeneralInterface.h"
  #include "GPIOGeneralInterface.c"
  #include "SPIGeneralInterface.h"
  #include "SPIGeneralInterface.c"
  //#include "UARTGeneralInterface.h"
  //#include "UARTGeneralInterface.c"

  //Arduino libraries
  #include "GPIO_Arduino.h"
  #include "SPI_Arduino.h"
  //#include "UART_Arduino.h"

  //Peripheral libraries
  #include "APA102Driver.h"
  #include "APA102Driver.c"

//----- Constants         -----//
  #define PIN_BUTTON      0

  #define TICKS_LEDCHANGE 200

  #define TIME_DEBOUNCE   50

  #define NUM_LEDS        28

  #define PATTERN_NUM     9
  #define PATTERN_MAXLEN  64
  #define PATTERN_ORDHOLD 28

  #define PATTERN_END     0xFFFFFFFF
  
//----- Definitions       -----//

//----- Globals           -----//
  //General interface objects
  sTimeIface_t gTime;
  sGPIOIface_t gGPIO;
  sSPIIface_t gSpi;
  //sUARTIface_t gUart;
  
  //Peripheral objects
  sAPA102Info_t gLEDs;

  eAPA102Color_t gaPatterns[PATTERN_NUM][PATTERN_MAXLEN] = {  
    {
      APA102_DimRed,
      APA102_DimOrange,
      APA102_DimYellow,
      APA102_DimGreen,
      APA102_DimAqua,
      APA102_DimBlue,
      APA102_DimPurple,
      APA102_DimPink,
      
      (eAPA102Color_t)PATTERN_END
    },
    {
      APA102_DimRed,
      APA102_DimRed,
      APA102_DimRed,
      APA102_DimRed,
      APA102_DimRed,
      APA102_DimOrange,
      APA102_DimOrange,
      APA102_DimOrange,
      APA102_DimOrange,
      APA102_DimOrange,
      APA102_DimYellow,
      APA102_DimYellow,
      APA102_DimYellow,
      APA102_DimYellow,
      APA102_DimYellow,
      APA102_DimGreen,
      APA102_DimGreen,
      APA102_DimGreen,
      APA102_DimGreen,
      APA102_DimGreen,
      APA102_DimAqua,
      APA102_DimAqua,
      APA102_DimAqua,
      APA102_DimAqua,
      APA102_DimAqua,
      APA102_DimBlue,
      APA102_DimBlue,
      APA102_DimBlue,
      APA102_DimBlue,
      APA102_DimBlue,
      APA102_DimPurple,
      APA102_DimPurple,
      APA102_DimPurple,
      APA102_DimPurple,
      APA102_DimPurple,
      APA102_DimPink,
      APA102_DimPink,
      APA102_DimPink,
      APA102_DimPink,
      APA102_DimPink,

      (eAPA102Color_t)PATTERN_END
    },
    {
      APA102_DimPink,
      APA102_DimCyan,
      APA102_DimBlue,
      APA102_DimPurple,
      
      (eAPA102Color_t)PATTERN_END
    },
    {
      APA102_DimRed,
      APA102_DimOrange,
      APA102_DimYellow,
      APA102_DimGreen,
      APA102_DimBlue,
      APA102_DimPurple,
      APA102_DimPink,
      
      (eAPA102Color_t)PATTERN_END
    },
    {
      APA102_DimRed,
      APA102_DimOrange,
      APA102_DimYellow,
      APA102_DimGreen,
      APA102_DimBlue,
      APA102_DimPurple,
      APA102_DimPink,
      
      (eAPA102Color_t)PATTERN_END
    },
    {
      APA102_DimRed,
      APA102_DimOrange,
      APA102_DimYellow,
      APA102_DimGreen,
      APA102_DimBlue,
      APA102_DimPurple,
      APA102_DimPink,
      
      (eAPA102Color_t)PATTERN_END
    },
    {
      APA102_DimRed,
      APA102_DimOrange,
      APA102_DimYellow,
      APA102_DimGreen,
      APA102_DimBlue,
      APA102_DimPurple,
      APA102_DimPink,
      
      (eAPA102Color_t)PATTERN_END
    },
    {
      APA102_DimRed,
      APA102_DimOrange,
      APA102_DimYellow,
      APA102_DimGreen,
      APA102_DimBlue,
      APA102_DimPurple,
      APA102_DimPink,
      
      (eAPA102Color_t)PATTERN_END
    },
    {
      APA102_DimCyan,
      APA102_DimCyan,
      APA102_DimCyan,
      APA102_DimCyan,
      APA102_DimCyan,
      APA102_DimCyan,
      APA102_DimCyan,
      APA102_DimBlue,
      APA102_DimBlue,
      APA102_DimBlue,
      APA102_DimBlue,
      APA102_DimBlue,
      APA102_DimBlue,
      APA102_DimBlue,
      APA102_DimPurple,
      APA102_DimPurple,
      APA102_DimPurple,
      APA102_DimPurple,
      APA102_DimPurple,
      APA102_DimPurple,
      APA102_DimPurple,
      APA102_DimPink,
      APA102_DimPink,
      APA102_DimPink,
      APA102_DimPink,
      APA102_DimPink,
      APA102_DimPink,
      APA102_DimPink,
      
      (eAPA102Color_t)PATTERN_END
    },
  };

  uint8_t ganOrders[PATTERN_NUM][NUM_LEDS + 1] = {
    {
      0,  1,  2,  3,  4,  5,  6, 
      7,  8,  9,  10, 11, 12, 13,
      14, 15, 16, 17, 18, 19, 20, 
      21, 22, 23, 24, 25, 26, 27,
      1, //Hold of 1 means every light gets a new color
    },
    {
      0,  1,  2,  3,  4,  5,  6, 
      7,  8,  9,  10, 11, 12, 13,
      14, 15, 16, 17, 18, 19, 20, 
      21, 22, 23, 24, 25, 26, 27,
      1,
    },
    {
      0,  6,  10,    16, 18, 22, 26, //This order does chevrons
      1,  5,  7, 13, 17,     23, 25,
      2,  4,  8, 12, 14, 20, 24,
      3,      9, 11, 15, 19, 21, 27, 
      7,
    },
    {
      0,  1,  2,  3,  4,  5,  6, 
      7,  8,  9,  10, 11, 12, 13,
      14, 15, 16, 17, 18, 19, 20, 
      21, 22, 23, 24, 25, 26, 27,
      7, //Hold of 7 means every row gets a color
    },
    {
      0,  7, 14, 21,
      1,  8, 15, 22,
      2,  9, 16, 23,
      3, 10, 17, 24,
      4, 11, 18, 25,
      5, 12, 19, 26,
      6, 13, 20, 27,
      4, //Hold of 4 means every column gets a color (with this order)
    },
    {
      3, 10, 17, 24,
      4, 11, 18, 25,
      2,  9, 16, 23,
      5, 12, 19, 26,
      1,  8, 15, 22,
      6, 13, 20, 27,
      0,  7, 14, 21,
      8, //Hold of 8 means every two columns gets a color
    },
    {
      0,  7, 14, 21,
      6, 13, 20, 27,
      
      1,  8, 15, 22,
      5, 12, 19, 26,
      
      4, 11, 18, 25,
      2,  9, 16, 23,
      
      3, 10, 17, 24,
      8, //Hold of 8 means every two columns gets a color
    },
    {
      0,  7, 20, 27, //This order pairs the rows
      1,  8, 19, 26, //But moves the paris in opposite directions
      2,  9, 18, 25,
      3, 10, 17, 24,
      4, 11, 16, 23,
      5, 12, 15, 22,
      6, 13, 14, 21,
      4, 
    },
    {
      0,  1,  2,  3,  4,  5,  6,
      13, 20, 
      27, 26, 25, 24, 23, 22, 21,
      14, 15, 16, 17, 18, 19,
      12, 11, 10,  9,  8,  7,
      1,
    },
  };
  
//----- Arduino Functions -----//
void setup() {
  //Arduino hardware setup
  Serial.begin(9600);

  //General Interface setup
  TIME_INIT(&gTime);
  GPIO_INIT(&gGPIO, GPIO_HWINFO);
  SPI_INIT(&gSpi, SPI_1_HWINFO, 5000000, SPI_MSBFirst, SPI_Mode3);
  //UART_INIT(&gUart, 38400, UART_8None1, UART_1_HWINFO);

  //Peripheral setup
  APA102Initialize(&gLEDs, &gSpi);

  gGPIO.pfSetModeByPin(&gGPIO, PIN_BUTTON, GPIO_DigitalInput);

  return;
}

#define GetRed(nColor)    ((nColor & APA102_RedMask) >> APA102_RedShift)
#define GetGreen(nColor)  ((nColor & APA102_GreenMask) >> APA102_GreenShift)
#define GetBlue(nColor)   ((nColor & APA102_BlueMask) >> APA102_BlueShift)

void loop() {
  static bool bLastBtn = false, bBtnPressed = false;
  static uint32_t tLastChange = 0;
  static uint32_t tDebounce = 0;
  static uint32_t nPatternStartIdx = 0;
  static uint32_t nPatternIdx = 0;
  static uint32_t nPatternLen = 0;
  
  bool bBtnDown;
  uint32_t tCurrTime, nLedCtr, nPatternCtr;

  tCurrTime = gTime.pfGetTicks();
  
  gGPIO.pfDigitalReadByPin(&gGPIO, PIN_BUTTON, &bBtnDown);
  if (bBtnDown == false) {
    bLastBtn = false;
    tDebounce = 0;
  }
  
  if ((bBtnDown == true) && (bBtnDown != bLastBtn)) {
    tDebounce = tCurrTime;
    bLastBtn = bBtnDown;
  }

  if ((tDebounce - tCurrTime >= TIME_DEBOUNCE) && (tDebounce != 0) && (bBtnDown == true) && (bLastBtn == true)) {
    bBtnPressed = true; // debounce time is up, send press event
    tDebounce = 0; //No debounce time means we're not debouncing
  }

  if (bBtnPressed == true) {
    Serial.println("Button pressed!");
    nPatternIdx += 1;
    nPatternStartIdx = 0;

    if (nPatternIdx >= PATTERN_NUM) {
      nPatternIdx = 0;
    }

    nPatternLen = 0; //mark the length unknown
    
    bBtnPressed = false; //Button handled, clear the event
  }

  if (nPatternLen == 0) {//Find out how long this pattern is
    for (nPatternCtr = 0; nPatternCtr < PATTERN_MAXLEN; nPatternCtr += 1) {
      if (gaPatterns[nPatternIdx][nPatternCtr] == PATTERN_END) {
        nPatternLen = nPatternCtr;
        break;
      }
    }
  }

  if (tCurrTime - tLastChange > TICKS_LEDCHANGE) {
    nPatternCtr = nPatternStartIdx;

    //Update all the light colors
    for (nLedCtr = 0; nLedCtr < NUM_LEDS; nLedCtr += 1) {
      APA102SetLightColor(&gLEDs, ganOrders[nPatternIdx][nLedCtr], gaPatterns[nPatternIdx][nPatternCtr]);

      if (((nLedCtr + 1) % ganOrders[nPatternIdx][PATTERN_ORDHOLD]) == 0) {
        nPatternCtr += 1;
        if (nPatternCtr >= nPatternLen) {
          nPatternCtr = 0;
        }
      }
    }

    APA102UpdateLights(&gLEDs);

    //Incremment the starting point of the pattern
    nPatternStartIdx += 1;
    if (nPatternStartIdx >= nPatternLen) {
      nPatternStartIdx = 0;
    }

    //Remember when we last updated the lights
    tLastChange = tCurrTime;
  }

  
  return;
}

//----- Callback Functions -----//

//----- Application Code   -----//
