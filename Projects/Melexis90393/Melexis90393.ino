//----- Include files     -----//
  #include "CommonUtils.h"
  #include "CommonUtils.c"
  #include "GPIOGeneralInterface.h"
  #include "GPIOGeneralInterface.c"
  #include "SPIGeneralInterface.h"
  #include "SPIGeneralInterface.c"
  #include "I2CGeneralInterface.h"
  #include "I2CGeneralInterface.c"
  
  #include "GPIO_Arduino.h"
  #include "SPI_Arduino.h"
  #include "I2C_Arduino.h"

  #include "MLX90393Driver.h"
  #include "MLX90393Driver.c"

//----- Constants         -----//
  #define PIN_POSCS       2
  #define PIN_TRIGGER     4
  #define PIN_INTERRUPT   3
  #define OUT_SERIAL    Serial

  #define COM_SPI         1
  #define COM_I2C         2
  #define COM_BUS         COM_I2C

//----- Definitions       -----//

//----- Globals           -----//
  sGPIOIface_t gGPIO;
  sSPIIface_t gSpi;
  sI2CIface_t gI2C;

  sMLX90393Dev_t gSensor;

  eMLX90393Return_t geInitResult;

//----- Arduino Functions -----//
void setup() {
  //Arduino hardware setup
  OUT_SERIAL.begin(9600);

  //General Interface setup
  GPIO_INIT(&gGPIO, GPIO_HWINFO);
  SPI_1_INIT(&gSpi, SPI_1_HWINFO, 2000000, SPI_MSBFirst, SPI_Mode3);
  I2C_1_INIT(&gI2C, true, 100000, I2C_1_HWINFO);

  //Setup peripherals
  #if COM_BUS == COM_I2C
    geInitResult = MLX90393I2CInit(&gSensor, &gGPIO, &gI2C, MLX90393_LW_ABA_011Base, PIN_TRIGGER, PIN_INTERRUPT);
  #else
    geInitResult = MLX90393SpiInit(&gSensor, &gGPIO, &gSpi, PIN_POSCS);
  #endif

  return;
}

void loop() {
  eMLX90393Return_t eReturn;
  uint16_t nXDir, nYDir, nZDir, nTemp;
  uint16_t tCurrTime = millis();

  if (geInitResult != MLX90393_Success) {
    OUT_SERIAL.print("Init FAIL ");
    OUT_SERIAL.print(geInitResult);
    OUT_SERIAL.print("\r\n");
  }

  OUT_SERIAL.print("T,");
  OUT_SERIAL.print(tCurrTime);
  OUT_SERIAL.print(",");

/*
  eReturn = MLX90393Init(&gSensor, &gGPIO, &gSpi, PIN_POSCS);
  OUT_SERIAL.print("Init:");
  OUT_SERIAL.print(eReturn);
  OUT_SERIAL.print("\r\n");
 */
 uint16_t nRegVal = MLX90393Cfg2_CommSPIOnly | MLX90393Cfg2_BstSelX | MLX90393Cfg2_BstSelY | MLX90393Cfg2_BstSelZ | MLX90393Cfg2_BstRateMask | MLX90393Cfg2_WOCDiff | MLX90393Cfg2_ExtTrig | MLX90393Cfg2_TCmpEn;

eReturn = MLX90393WriteRegister(&gSensor, MLX90393Mem_Config2, nRegVal);
if (eReturn != MLX90393_Success) {
  OUT_SERIAL.print("RegWrite FAIL ");
  OUT_SERIAL.print(eReturn);
  OUT_SERIAL.print(" Val:");
  OUT_SERIAL.print(nRegVal, HEX);
  OUT_SERIAL.print("\r\n");
  //return;
}


eReturn = MLX90393ReadRegister(&gSensor, MLX90393Mem_Config1, &nRegVal);
OUT_SERIAL.print("RegRead1:");
OUT_SERIAL.print(eReturn);
OUT_SERIAL.print(" Val:");
OUT_SERIAL.print(nRegVal, HEX);
OUT_SERIAL.print("\r\n");


eReturn = MLX90393ReadRegister(&gSensor, MLX90393Mem_Config2, &nRegVal);
OUT_SERIAL.print("RegRead2:");
OUT_SERIAL.print(eReturn);
OUT_SERIAL.print(" Val:");
OUT_SERIAL.print(nRegVal, HEX);
OUT_SERIAL.print("\r\n");

/*
  eReturn = MLX90393TakeReading(&gSensor, &nXDir, &nYDir, &nZDir, &nTemp);
  if (eReturn == MLX90393_Success) {
    OUT_SERIAL.print("X,");
    OUT_SERIAL.print(nXDir);
    OUT_SERIAL.print(",Y,");
    OUT_SERIAL.print(nYDir);
    OUT_SERIAL.print(",Z,");
    OUT_SERIAL.print(nZDir);
    OUT_SERIAL.print(",T,");
    OUT_SERIAL.print(nTemp);
    OUT_SERIAL.print("\r\n");
  } else {
    OUT_SERIAL.print("Reading FAIL: ");
    OUT_SERIAL.print(eReturn);
    OUT_SERIAL.print("\r\n");

    if (eReturn == MLX90393Fail_DeviceErr) {
      MLX90393Reset(&gSensor);
      delay(500);
      #if COM_BUS == COM_I2C
        MLX90393I2CInit(&gSensor, &gGPIO, &gI2C, MLX90393_LW_ABA_011Base, PIN_TRIGGER, PIN_INTERRUPT);
      #else
        MLX90393SpiInit(&gSensor, &gGPIO, &gSpi, PIN_POSCS);
      #endif
    }
  }
*/
  delay(500);
  return;
}

//----- Callback Functions -----//

//----- Application Code   -----//
