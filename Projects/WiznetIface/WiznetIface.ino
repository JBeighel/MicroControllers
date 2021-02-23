//----- Include files     -----//
  //General libraries
  #include "CommonUtils.h"
  #include "CommonUtils.c"
  #include "TimeGeneralInterface.h"
  #include "TimeGeneralInterface.c"
  #include "GPIOGeneralInterface.h"
  #include "GPIOGeneralInterface.c"
  #include "I2CGeneralInterface.h"
  #include "I2CGeneralInterface.c"
  #include "SPIGeneralInterface.h"
  #include "SPIGeneralInterface.c"
  #include "UARTGeneralInterface.h"
  #include "UARTGeneralInterface.c"
  #include "NetworkGeneralInterface.h"
  #include "NetworkGeneralInterface.c"

  //Arduino libraries
  #include "GPIO_Arduino.h"
  #include "I2C_Arduino.h"
  #include "SPI_Arduino.h"
  #include "UART_Arduino.h"

  //Peripheral libraries
  #include "US2066Driver.h"
  #include "US2066Driver.c"
  #include "W5500Driver.h"
  #include "W5500Driver.c"

//----- Constants         -----//
  #define   PIN_SCREENCS    8
  #define   PIN_SCREENRST   9

  #define   PIN_ETHCS       5

  #define   PIN_TESTPT      4

  uint8_t gaMacAddr[6] = {0x02, 0x00, 0x00, 0x00, 0x00, 0x01};

//----- Definitions       -----//

//----- Globals           -----//
  //General interface objects
  sTimeIface_t gTime;
  sGPIOIface_t gGPIO;
  sI2CIface_t gI2C;
  sSPIIface_t gSpi;
  sUARTIface_t gUart;
  
  //Peripheral objects
  sUS2066Info_t gScreen;
  sW5500Obj_t gWiznet;

  uint8_t gnHostSck;
  char gaLastMsg[25];

//----- Arduino Functions -----//
void setup() {
  IN_ADDR Addr;
  char aMessage[25];
  
  //Arduino hardware setup
  Serial.begin(9600);

  //General Interface setup
  TIME_INIT(&gTime);
  GPIO_INIT(&gGPIO, GPIO_HWINFO);
  I2C_INIT(&gI2C, true, 100000, I2C_1_HWINFO);
  SPI_INIT(&gSpi, SPI_1_HWINFO, 5000000, SPI_MSBFirst, SPI_Mode3);
  UART_INIT(&gUart, 38400, UART_8None1, UART_2_HWINFO);

  gGPIO.pfSetModeByPin(&gGPIO, PIN_TESTPT, GPIO_DigitalOutput);
  gGPIO.pfDigitalWriteByPin(&gGPIO, PIN_TESTPT, true);

  //Peripheral setup
  US2066InitSPI(&gScreen, &gTime, &gGPIO, &gSpi, 20, 4, PIN_SCREENCS, PIN_SCREENRST);
  US2066ClearDisplay(&gScreen);
  US2066DisplayOn(&gScreen);
  
  W5500Initialize(&gWiznet, &gSpi, &gGPIO, PIN_ETHCS);

  W5500SetMACAddress(&gWiznet, gaMacAddr);

  Addr.S_un.S_un_b.s_b1 = 10;
  Addr.S_un.S_un_b.s_b2 = 10;
  Addr.S_un.S_un_b.s_b3 = 0;
  Addr.S_un.S_un_b.s_b4 = 20;
  W5500SetIPAddress(&gWiznet, &Addr);
  sprintf(aMessage, "IP %d.%d.%d.%d", Addr.S_un.S_un_b.s_b1, Addr.S_un.S_un_b.s_b2, Addr.S_un.S_un_b.s_b3, Addr.S_un.S_un_b.s_b4);
  ScreenWriteString(0, 0, aMessage);

  Addr.S_un.S_un_b.s_b1 = 255;
  Addr.S_un.S_un_b.s_b2 = 255;
  Addr.S_un.S_un_b.s_b3 = 255;
  Addr.S_un.S_un_b.s_b4 = 0;
  W5500SetSubnetMask(&gWiznet, &Addr);

  Addr.S_un.S_un_b.s_b1 = 10;
  Addr.S_un.S_un_b.s_b2 = 10;
  Addr.S_un.S_un_b.s_b3 = 0;
  Addr.S_un.S_un_b.s_b4 = 1;
  W5500SetDefaultGateway(&gWiznet, &Addr);

  W5500SocketListen(&gWiznet, &gnHostSck, 23, IPPROTO_TCP);

  gaLastMsg[0] = '\0';
  
  return;
}

void loop() {
  IN_ADDR IPAddr;
  SOCKADDR_IN RemoteIPAddr;
  bool bStatus;
  char aMessage[25];
  eW5500SckProt_t eSckProt;
  eW5500SckStat_t eSckState;
  uint16_t nBytesWaiting, nBytesRead;
  
  US2066ClearDisplay(&gScreen);

  gGPIO.pfDigitalWriteByPin(&gGPIO, PIN_TESTPT, false);
  W5500ReadIPAddress(&gWiznet, &IPAddr);
  gGPIO.pfDigitalWriteByPin(&gGPIO, PIN_TESTPT, true);
  sprintf(aMessage, "IP %d.%d.%d.%d", IPAddr.S_un.S_un_b.s_b1, IPAddr.S_un.S_un_b.s_b2, IPAddr.S_un.S_un_b.s_b3, IPAddr.S_un.S_un_b.s_b4);
  ScreenWriteString(0, 0, aMessage);
  Serial.println(aMessage);

  W5500LinkStatus(&gWiznet, &bStatus);
  W5500SocketStatus(&gWiznet, gnHostSck, &eSckProt, &eSckState,& nBytesWaiting, &RemoteIPAddr, sizeof(SOCKADDR_IN));

  if (bStatus == true) {
    sprintf(aMessage, "Status: ON");
  } else {
    sprintf(aMessage, "Status: OFF");
  }
  ScreenWriteString(0, 1, aMessage);

  switch (eSckState) {
    case W5500SckStat_Closed :
      sprintf(aMessage, "Closed");
      W5500SocketListen(&gWiznet, &gnHostSck, 23, IPPROTO_TCP);
      break;
    case W5500SckStat_Init :
      sprintf(aMessage, "Init");
      break;
    case W5500SckStat_Listen :
      sprintf(aMessage, "Listen");
      break;
    case W5500SckStat_Establish :
      sprintf(aMessage, "Estab");
      break;
    case W5500SckStat_CloseWait :
      sprintf(aMessage, "ClsWait");
      W5500SocketListen(&gWiznet, &gnHostSck, 23, IPPROTO_TCP);
      break;
    case W5500SckStat_Closing :
      sprintf(aMessage, "Closing");
      break;
    default :
      sprintf(aMessage, "? %02x", eSckState);
      break;
  }
  ScreenWriteString(12, 1, aMessage);

  if (nBytesWaiting > 0) {
    if (nBytesWaiting > 19) {
      nBytesWaiting = 19;
    }
    
    W5500SocketReceive(&gWiznet, gnHostSck, (uint8_t *)gaLastMsg, nBytesWaiting, &nBytesRead);
    gaLastMsg[nBytesRead + 1] = '\0';
    gaLastMsg[nBytesWaiting] = '\0';
    gaLastMsg[19] = '\0';
  }

  ScreenWriteString(0, 2, gaLastMsg);

  delay(500);
  
  return;
}

//----- Callback Functions -----//

//----- Application Code   -----//
void ScreenWriteString(uint8_t nCol, uint8_t nRow, char *aText) {
  uint8_t nCtr, nLen;
  
  US2066SetCursorPosition(&gScreen, nCol, nRow);

  nCtr = 0;
  while (aText[nCtr] != '\0') {
    US2066PrintCharacter(&gScreen, aText[nCtr]);
    nCtr += 1;  
  }

  return;
}
