/*****	Includes	*****/
	//Pin supported (micro hardware and buses)
	#include "CommonUtils.h"
	#include "GPIOGeneralInterface.h"
	#include "I2CGeneralInterface.h"
	
	#include "GPIO_RaspberryPi.h"
	#include "I2C_RaspberryPi.h"
	
	//Board support (peripherals)
	#include "MCP23017Driver.h"

/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/
sGPIOIface_t gGPIO;
sI2CIface_t gI2C;

sMCP23017Info_t gExp;

/*****	Prototypes 	*****/


/*****	Functions	*****/
int32_t main(int32_t nArgCnt, char *aArgVals) {
	eGPIOReturn_t eResult;
	 eI2CReturns_t eI2CResult;
	eGPIOModes_t ePinMode;
	bool bPinState, bLevel;
	uint16_t nCtr;
	
	eResult = GPIO_INIT(&gGPIO, GPIO_HWINFO);
	
	if (eResult != GPIO_Success) {
		printf("GPIO Init failed %d\r\n", eResult);
		return 0;
	} else {
		printf("GPIO Init succeeded\r\n");
	}
	
	for (nCtr = 0; nCtr < 10; nCtr++) {
		eResult = gGPIO.pfSetModeByPin(&gGPIO, nCtr, GPIO_DigitalOutput);
		if (eResult != GPIO_Success) {
			printf("Failed to set mode for pin %d: %d\r\n", nCtr, eResult);
		}
		
		eResult = gGPIO.pfDigitalWriteByPin(&gGPIO, nCtr, false);
		if (eResult != GPIO_Success) {
			printf("Failed to output set state for pin %d: %d\r\n", nCtr, eResult);
		}
	}
	
	for (nCtr = 10; nCtr < 20; nCtr++) {
		eResult = gGPIO.pfSetModeByPin(&gGPIO, nCtr, GPIO_DigitalInput);
		
		if (eResult != GPIO_Success) {
			printf("Failed to set mode for pin %d: %d\r\n", nCtr, eResult);
		}
	}
	
	for (nCtr = 0; nCtr < 20; nCtr++) {
		gGPIO.pfReadModeByPin(&gGPIO, nCtr, &ePinMode);
		
		switch (ePinMode) {
			case (GPIO_DigitalInput):
				gGPIO.pfDigitalReadByPin(&gGPIO, nCtr, &bPinState);
				printf("Pin %d is Input, value %d\r\n", nCtr, bPinState);
				break;
			case (GPIO_DigitalOutput):
				printf("Pin %d is Output\r\n", nCtr);
				break;
			default:
				printf("Pin %d is unknown %d\r\n", nCtr, ePinMode);
				break;
		}
	}
	
	eI2CResult = I2C_1_PORTINIT(&gI2C, true, 100000, I2C_1_HWINFO);
	if (eI2CResult != I2C_Success) {
		printf("I2C Failed to Initialize: %d\r\n", eI2CResult);
		return 0;
	}
	
	MCP23017Initialize(&gExp, (eMCP23017Address_t)(MCP23017_Base | MCP23017_A0 | MCP23017_A1 | MCP23017_A2), &gI2C);

	for (nCtr = 0; nCtr < 8; nCtr++) {
		MCP23017PinModeByPin(&gExp, nCtr, MCP23017_Input);
		MCP23017PinModeByPin(&gExp, nCtr + 8, MCP23017_Output);
	}
	
	MCP23017WriteOutputByPin(&gExp, 8, true);
	MCP23017ReadInputByPin(&gExp, 8, &bLevel);
	if (bLevel == true) {
		printf("Pin B0 set HIGH\r\n");
	} else {
		printf("Pin B0 set low\r\n");
	}
	
	MCP23017ReadInputByPin(&gExp, 0, &bLevel);
	if (bLevel == true) {
		printf("Pin A0 reads HIGH\r\n");
	} else {
		printf("Pin A0 reads low\r\n");
	}
	
	gI2C.pfShutdown(&gI2C);
	
	return 0;
}
