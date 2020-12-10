//----- Include files     -----//
	  #include "CommonUtils.h"
	  #include "GPIOGeneralInterface.h"
	  //#include "I2CGeneralInterface.h"
	  //#include "SPIGeneralInterface.h"
	  
	  #include "GPIO_RaspberryPi.h"
	  //#include "I2C_Arduino.h"
	  //#include "SPI_Arduino.h"

	  #include "Keypad.h"
	  #include "US2066Driver.h"

//----- Constants         -----//

//----- Definitions       -----//

//----- Globals           -----//
	//Generla interface objects
	sTimeIface_t gTime;
	sGPIOIface_t gGPIO;
	//sI2CIface_t gI2C;
	//sSPIIface_t gSpi;

	//Peripheral objects
	sKeypadInfo_t gKeys;
	sUS2066Info_t gDisp;

//----- Arduino Functions -----//
int main(int nArg, char *pArgV) {
	uint32_t nCurrTime, nStartTime;
	bool bKeyDown;
	char Key;
	uint8_t nCol = 0, nRow = 0;
	
	//General Interface setup
	TIME_INIT(&gTime);
	GPIO_INIT(&gGPIO, GPIO_HWINFO);
	//I2C_1_PORTINIT(&gI2C, true, 100000, I2C_1_HWINFO);
	//SPI_1_INIT(&gSpi, SPI_1_HWINFO, 5000000, SPI_MSBFirst, SPI_Mode0);

	//Peripheral setup
	GPIOID_t aOuts[] = { 17, 27, 22, 18};
	GPIOID_t aIns[] = { 23, 24, 25 };
	KeypadInit(&gKeys, &gGPIO, aOuts, aIns);
	
	US2066Init4Data(&gDisp, &gTime, &gGPIO, false, 20, 4, 21, 20, 16, 5, 6, 13, 19);
	
	US2066InitClearDisplay(&gDisp);
	US2066InitDisplayOn(&gDisp);

	US2066InitSetCursorPosition(&gDisp, 0, 0);
	US2066InitPrintCharacter(&gDisp, 'H');
	US2066InitPrintCharacter(&gDisp, 'i');

	nStartTime = gTime.pfGetTicks();
	do {
		KeypadCheckPress(&gKeys, &bKeyDown, &Key);
		if (bKeyDown == true) {
			printf("Pressed Key: %c\r\n", Key);
			US2066InitSetCursorPosition(&gDisp, nCol, nRow);
			US2066InitPrintCharacter(&gDisp, Key);
			
			nCol += 1;
			if (nCol >= 20) {
				nCol = 0;
				nRow += 1;
			}
		}
		
		gTime.pfDelayMilliSeconds(10);
		nCurrTime = gTime.pfGetTicks();
	} while (nCurrTime - nStartTime < 5000);
	
	return 0;
}
