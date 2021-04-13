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
  #define   PIN_ETHRST      6

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

  sTCPServ_t gTCPServ;

  uint16_t gnPortNum;

  char gaLastMsg[25];

//----- Arduino Functions -----//
void setup() {
  IN_ADDR Addr;
  char aMessage[25];
  sConnInfo_t ConnInfo;
  eNetReturn_t eResult;
  eW5500Return_t eWizResult;
  bool bGoodInit;
  
  //Arduino hardware setup
  Serial.begin(9600);

  //General Interface setup
  TIME_INIT(&gTime);
  GPIO_INIT(&gGPIO, GPIO_HWINFO);
  I2C_INIT(&gI2C, true, 100000, I2C_1_HWINFO);
  SPI_INIT(&gSpi, SPI_1_HWINFO, 5000000, SPI_MSBFirst, SPI_Mode3);
  UART_INIT(&gUart, 38400, UART_8None1, UART_2_HWINFO);
  Serial.println("Gen Iface init done");

  gGPIO.pfSetModeByPin(&gGPIO, PIN_TESTPT, GPIO_DigitalOutput);
  gGPIO.pfDigitalWriteByPin(&gGPIO, PIN_TESTPT, true);
  gGPIO.pfSetModeByPin(&gGPIO, PIN_ETHRST, GPIO_DigitalOutput);
  gGPIO.pfDigitalWriteByPin(&gGPIO, PIN_ETHRST, true);
  Serial.println("GPIO init done");

  //Peripheral setup
  US2066InitSPI(&gScreen, &gTime, &gGPIO, &gSpi, 20, 4, PIN_SCREENCS, PIN_SCREENRST);
  US2066ClearDisplay(&gScreen);
  US2066DisplayOn(&gScreen);
  Serial.println("Screen init done");

  do {
    bGoodInit = true;

    gGPIO.pfDigitalWriteByPin(&gGPIO, PIN_ETHRST, false);
    gTime.pfDelayMilliSeconds(5);
    gGPIO.pfDigitalWriteByPin(&gGPIO, PIN_ETHRST, true);
    gTime.pfDelayMilliSeconds(1000);
    Serial.println("Eth reset done");
    
    eWizResult = W5500Initialize(&gWiznet, &gSpi, &gGPIO, PIN_ETHCS);
    if (eWizResult != W5500_Success) {
      ScreenWriteString(0, 3, "Init Fail");
      bGoodInit = false;
    } else {
      ScreenWriteString(0, 3, "W5500 Init");
    }
    Serial.println("W5500 init done");
    
    eWizResult = W5500VerifyPart(&gWiznet);
    if (eWizResult != W5500_Success) {
      ScreenWriteString(0, 3, "Bad Part");
      bGoodInit = false;
    }

    gTime.pfDelayMilliSeconds(100);
  } while (bGoodInit == false);

/*
  while(1) {
    eWizResult = W5500VerifyPart(&gWiznet);
    gTime.pfDelayMilliSeconds(100);
  }
*/
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
  Addr.S_un.S_un_b.s_b4 = 250;
  W5500SetDefaultGateway(&gWiznet, &Addr);

  W5500CreateTCPServer(&gWiznet, &gTCPServ);

  //Setup host socket
  ConnInfo.Addr.nNetLong = 0;
  ConnInfo.Port = 23;
  eResult = gTCPServ.pfBind(&gTCPServ, &ConnInfo);
  if (eResult == Net_Success) {
    gnPortNum = ConnInfo.Port;
  } else {
    ConnInfo.Port = -1;
  }

  gaLastMsg[0] = '\0';

  return;
}

void loop() {
  IN_ADDR IPAddr;
  SOCKADDR_IN RemoteIPAddr;
  bool bStatus;
  char aMessage[4][25];
  sSocket_t ClientSck;
  eNetReturn_t eResult;
  eW5500SckProt_t eSckProt;
  eW5500SckStat_t eSckState;
  uint16_t nBytesWaiting;

  memset(aMessage, ' ', sizeof(aMessage));

  gGPIO.pfDigitalWriteByPin(&gGPIO, PIN_TESTPT, false);
  W5500ReadIPAddress(&gWiznet, &IPAddr);
  gGPIO.pfDigitalWriteByPin(&gGPIO, PIN_TESTPT, true);
  snprintf(aMessage[0], 21, "IP %d.%d.%d.%d", IPAddr.S_un.S_un_b.s_b1, IPAddr.S_un.S_un_b.s_b2, IPAddr.S_un.S_un_b.s_b3, IPAddr.S_un.S_un_b.s_b4);
  Serial.println(aMessage[0]);
  aMessage[0][strlen(aMessage[0])] = ' ';

  W5500LinkStatus(&gWiznet, &bStatus);

  if (bStatus == true) {
    sprintf(aMessage[1], "Status: ON");
  } else {
    sprintf(aMessage[1], "Status: OFF");
  }
  aMessage[1][strlen(aMessage[1])] = ' ';

  if (W5500VerifyPart(&gWiznet) == W5500_Success) {
    sprintf(&(aMessage[1][12]), "Good");
  } else {
    sprintf(&(aMessage[1][12]), "Bad Part");
  }
  aMessage[1][strlen(aMessage[1])] = ' ';
  

  eResult = gTCPServ.pfAcceptClient(&gTCPServ, &ClientSck);
  if (eResult == Net_Success) {
    snprintf(aMessage[2], 21, "Client: %d.%d.%d.%d", ClientSck.Conn.Addr.Octets.b3, ClientSck.Conn.Addr.Octets.b2, ClientSck.Conn.Addr.Octets.b1, ClientSck.Conn.Addr.Octets.b0);
  } else {
    snprintf(aMessage[2], 21, "Fail: %d %d", eResult, millis());
  }
  aMessage[2][strlen(aMessage[1])] = ' ';

  W5500SocketStatus(&gWiznet, gTCPServ.HostSck.nSocket, &eSckProt, &eSckState, &nBytesWaiting, &RemoteIPAddr, sizeof(SOCKADDR_IN));
  snprintf(aMessage[3], 21, "Port: %d Sck:%d %02x", gnPortNum, gTCPServ.HostSck.nSocket);
  aMessage[3][strlen(aMessage[1])] = ' ';

  aMessage[0][20] = '\0';
  aMessage[1][20] = '\0';
  aMessage[2][20] = '\0';
  aMessage[3][20] = '\0';
  ScreenWriteString(0, 0, aMessage[0]);
  ScreenWriteString(0, 1, aMessage[1]);
  ScreenWriteString(0, 2, aMessage[2]);
  ScreenWriteString(0, 3, aMessage[3]);

  delay(500);
  
  return;
}

//----- Callback Functions -----//

//----- Application Code   -----//
void ScreenWriteString(uint8_t nCol, uint8_t nRow, const char *aText) {
  uint8_t nCtr, nLen;
  
  US2066SetCursorPosition(&gScreen, nCol, nRow);

  nCtr = 0;
  while (aText[nCtr] != '\0') {
    US2066PrintCharacter(&gScreen, aText[nCtr]);
    nCtr += 1;  
  }

  return;
}
