#include "UARTGeneralInterface.h"
#include "UARTGeneralInterface.c"
#include "UART_Arduino.h"

#include "TF02Driver.h"
#include "TF02Driver.c"

sUARTIface_t gUART1;
sTF02Device_t gTF02;

void setup() {
  Serial.begin(115200);
  UART_PORTINITIALIZE(&gUART1, 115200, UART_8None1, UART_2_HWINFO);

  TF02Initialize(&gTF02, &gUART1);
}

void loop() {

}
