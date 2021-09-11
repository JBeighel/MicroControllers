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

  //Arduino libraries
  #include "GPIO_Arduino.h"
  #include "I2C_Arduino.h"
  #include "SPI_Arduino.h"
  #include "UART_Arduino.h"

  //Peripheral libraries
  #include "LSM9DS1Driver.h"
  #include "LSM9DS1Driver.c"
  #include "MPU6050Driver.h"
  #include "MPU6050Driver.c"
  #include "PCA9685Driver.h"
  #include "PCA9685Driver.c"

//----- Constants         -----//

//----- Definitions       -----//

//----- Globals           -----//
  //General interface objects
  sTimeIface_t gTime;
  sGPIOIface_t gGPIO;
  sI2CIface_t gI2C;
  sSPIIface_t gSpi;
  sUARTIface_t gUart;
  
  //Peripheral objects
  sMPU6050Obj_t gAccel;

//----- Arduino Functions -----//
void setup() {
  //Arduino hardware setup
  Serial.begin(9600);

  //General Interface setup
  TIME_INIT(&gTime);
  //GPIO_INIT(&gGPIO, GPIO_HWINFO);
  I2C_INIT(&gI2C, true, 100000, I2C_1_HWINFO);
  //SPI_INIT(&gSpi, SPI_1_HWINFO, 5000000, SPI_MSBFirst, SPI_Mode0);
  //UART_INIT(&gUart, 38400, UART_8None1, UART_1_HWINFO);

  //Peripheral setup
  MPU6050Initialize(&gAccel, &gI2C, (eMPU6050Addr_t)0);
  gTime.pfDelayMilliSeconds(100);
  
  return;
}

void loop() {
  int16_t anGyro[3], anAccel[3];
  eMPU6050Return_t eResult;

  //MPU6050ReadGyro(&gAccel, &anGyro[0], &anGyro[1], &anGyro[2]);
  eResult = MPU6050ReadAccel(&gAccel, &anAccel[0], &anAccel[1], &anAccel[2]);

  /*
  Serial.print("G X:");
  Serial.print(anGyro[0]);
  Serial.print(" Y:");
  Serial.print(anGyro[1]);
  Serial.print(" Z:");
  Serial.print(anGyro[2]);
  */
  Serial.print(" - A X,");
  Serial.print(anAccel[0]);
  Serial.print(",Y,");
  Serial.print(anAccel[1]);
  Serial.print(",Z,");
  Serial.print(anAccel[2]);
  
  Serial.print("\r\n");

  gTime.pfDelayMilliSeconds(100);
  
  return;
}

//----- Callback Functions -----//

//----- Application Code   -----//
