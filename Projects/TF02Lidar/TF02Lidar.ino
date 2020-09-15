#include "UARTGeneralInterface.h"
#include "UARTGeneralInterface.c"
#include "UART_Arduino.h"

#include "TF02Driver.h"
#include "TF02Driver.c"

#if ((TF02_UARTCAPS & UART_2_CAPS) != TF02_UARTCAPS)
  #error "UART 2 Capabilities won't support TFO2 driver
#endif

sUARTIface_t gUART1;
sTF02Device_t gTF02;

void setup() {
  Serial.begin(115200);
  UART_PORTINITIALIZE(&gUART1, 115200, UART_8None1, UART_2_HWINFO);

  TF02Initialize(&gTF02, &gUART1);

  return;
}

void loop() {
  eTF02Returns_t eReturn;
  uint16_t nDist, nAmp;
  
  Serial.print("Reading...");

  eReturn = TF02GetReading(&gTF02);

  if (eReturn != TF02_Success) {
    Serial.print("Fail ");
    Serial.print(eReturn);
    Serial.print("\n");
  } else {
    Serial.print("Pass\n");

    nDist = gTF02.uData.sParse.nDistLow;
    nDist |= gTF02.uData.sParse.nDistHigh << 8;
  
    nAmp = gTF02.uData.sParse.nSigStrLow;
    nAmp |= gTF02.uData.sParse.nSigStrHigh << 8;
  
    Serial.print("Dist: ");
    Serial.print(nDist);
    Serial.print(" SigStr: ");
    Serial.print(nAmp);
    Serial.print(" Relia: ");
    Serial.print(gTF02.uData.sParse.nReliability);
    Serial.print("\n");
  }

  delay(75);
  return;
}
