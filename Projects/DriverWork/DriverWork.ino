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
  int16_t anAccel[3];
  eMPU6050Return_t eResult;
  float nVelocity;
  
  nVelocity = GetVelocity(&gAccel);
  nVelocity -= 0; //Apply some calibration?
  
  Serial.print("V,");
  Serial.print(nVelocity);  
  Serial.print("\r\n");

  gTime.pfDelayMilliSeconds(100);
  
  return;
}

//----- Callback Functions -----//

//----- Application Code   -----//
float GetVelocity(sMPU6050Obj_t *pAccel) {
  static float nLastVelocity = 0;
  
  int16_t anAccelTry[3], anAccelGood[3];
  uint8_t nGoodSamples;
  float nValue;
  eMPU6050Return_t eResult;

  //Set flags to say we have no good samples
  nGoodSamples = 0;
  while (nGoodSamples != 7) {
    eResult = MPU6050ReadAccel(pAccel, &anAccelTry[0], &anAccelTry[1], &anAccelTry[2]);
    if (eResult != Success) { //Bad read, try again?
      continue;
    }

    //Check X sample
    if ((anAccelTry[0] < -150) || (anAccelTry[0] > 0)) { //Found a good sample
      nGoodSamples |= 0x01,
      anAccelGood[0] = anAccelTry[0];
    }

    //Check Y sample
    if ((anAccelTry[1] < -150) || (anAccelTry[1] > 0)) { //Found a good sample
      nGoodSamples |= 0x02,
      anAccelGood[1] = anAccelTry[1];
    }

    //Check Z sample
    if ((anAccelTry[2] < -150) || (anAccelTry[2] > 0)) { //Found a good sample
      nGoodSamples |= 0x04,
      anAccelGood[2] = anAccelTry[2];
    }
  }

  //Have good samples, calculate the magnitude of the acceleration vector
  //Each sample is the length along some axis
  nValue = anAccelGood[0] * anAccelGood[0];
  nValue += anAccelGood[1] * anAccelGood[1];
  nValue += anAccelGood[2] * anAccelGood[2];

  nValue = sqrt(nValue);
  nValue -= 16000; //Subtract the effect of gravity
  nValue /= 8; //Attenuate the acceleration 

  //Have the acceleration, integrate it to get the current velocity
  nLastVelocity *= 0.90; //Leak some from the accumulator
  nLastVelocity += nValue;

  Serial.print("X,");
  Serial.print(anAccelGood[0]);
  Serial.print(",Y,");
  Serial.print(anAccelGood[1]);
  Serial.print(",Z,");
  Serial.print(anAccelGood[2]);
  Serial.print(",M,");
  Serial.print(nValue);  
  Serial.print("\r\n");

  return nLastVelocity;
}
