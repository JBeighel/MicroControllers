/*****	Includes	*****/
	#include <string.h>

	//Pin supported (micro hardware and buses)
	#include "CommonUtils.h"
	#include "GPIOGeneralInterface.h"
	#include "I2CGeneralInterface.h"
	#include "SPIGeneralInterface.h"
	
	#include "GPIO_RaspberryPi.h"
	#include "I2C_RaspberryPi.h"
	#include "SPI_RaspberryPi.h"
	
	//Board support (peripherals)
	#include "MCP23017Driver.h"

/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/
sGPIOIface_t gGPIO;
sI2CIface_t gI2C;
sSPIIface_t gSPI;

sMCP23017Info_t gExp;

/*****	Prototypes 	*****/
	int32_t GPIOWork(void);
	int32_t I2CWork(void);
	int32_t SPIWork(void);

/*****	Functions	*****/
int32_t main(int32_t nArgCnt, char *aArgVals) {
	SPIWork();
	
	
	return 0;
}

int32_t GPIOWork(void) {
	eGPIOReturn_t eResult;
	eGPIOModes_t ePinMode;
	uint16_t nCtr;
	bool bPinState;
	
	eResult = GPIO_INIT(&gGPIO, GPIO_HWINFO);
	
	if (eResult != GPIO_Success) {
		printf("GPIO Init failed %d\r\n", eResult);
		return 0;
	} else {
		printf("GPIO Init succeeded\r\n");
	}
	
	for (nCtr = 4; nCtr < 10; nCtr++) {
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
	
	return 1;
}

int32_t I2CWork(void) {
	eI2CReturns_t eI2CResult;
	eMCP23017Returns_t eExpResult;
	bool bLevel;
	uint16_t nCtr;
	
	eI2CResult = I2C_1_PORTINIT(&gI2C, true, 100000, I2C_1_HWINFO);
	if (eI2CResult != I2C_Success) {
		printf("I2C Failed to Initialize: %d\r\n", eI2CResult);
		return 0;
	}
	
	eExpResult = MCP23017Initialize(&gExp, (eMCP23017Address_t)(MCP23017_Base | MCP23017_A0 | MCP23017_A1 | MCP23017_A2), &gI2C);
	if (eExpResult != MCP23017_Success) {
		printf("Failed to initialize GPIO expander\r\n");
		return 0;
	}

	for (nCtr = 0; nCtr < 8; nCtr++) {
		eExpResult = MCP23017PinModeByPin(&gExp, nCtr, MCP23017_Input);
		if (eExpResult != MCP23017_Success) {
			printf("Setting mode for pin %d failed, code: %d\r\n", nCtr, gI2CHWInfo[0].nLastErr);
			return 0;
		}
		
		eExpResult = MCP23017PinModeByPin(&gExp, nCtr + 8, MCP23017_Output);
		if (eExpResult != MCP23017_Success) {
			printf("Setting mode for pin %d failed, code: %d\r\n", nCtr, gI2CHWInfo[0].nLastErr);
			return 0;
		}
	}
	
	eExpResult = MCP23017WriteOutputByPin(&gExp, 8, true);
	eExpResult = MCP23017WriteOutputByPin(&gExp, 9, true);
	eExpResult = MCP23017WriteOutputByPin(&gExp, 10, false);
	eExpResult = MCP23017WriteOutputByPin(&gExp, 11, false);
	if (eExpResult != MCP23017_Success) {
		printf("Writing value for for pin 8 failed, code: %d\r\n", gI2CHWInfo[0].nLastErr);
		return 0;
	}
	
	eExpResult = MCP23017ReadInputByPin(&gExp, 8, &bLevel);
	if (eExpResult != MCP23017_Success) {
		printf("Reading value for for pin 8 failed, code: %d / %d\r\n", eExpResult, gI2CHWInfo[0].nLastErr);
		printf("%s\r\n", strerror(gI2CHWInfo[0].nLastErr));
		return 0;
	}
	
	if (bLevel == true) {
		printf("Pin B0 set HIGH\r\n");
	} else {
		printf("Pin B0 set low\r\n");
	}
	
	eExpResult = MCP23017ReadInputByPin(&gExp, 0, &bLevel);
	if (eExpResult != MCP23017_Success) {
		printf("Reading value for for pin 0 failed, code: %d\r\n", gI2CHWInfo[0].nLastErr);
		return 0;
	}
	
	if (bLevel == true) {
		printf("Pin A0 reads HIGH\r\n");
	} else {
		printf("Pin A0 reads low\r\n");
	}
	
	eExpResult = MCP23017ReadInputByPin(&gExp, 1, &bLevel);
	if (eExpResult != MCP23017_Success) {
		printf("Reading value for for pin 1 failed, code: %d\r\n", gI2CHWInfo[0].nLastErr);
		return 0;
	}
	
	if (bLevel == true) {
		printf("Pin A1 reads HIGH\r\n");
	} else {
		printf("Pin A1 reads low\r\n");
	}
	
	gI2C.pfShutdown(&gI2C);
	
	return 1;
}

int32_t SPIWork(void) {
	eSPIReturn_t eResult;
	uint8_t nReadByte;
	
	eResult = SPI_1_PORTINIT(&gSPI, SPI_1_HWINFO, 5000000, SPI_MSBFirst, SPI_Mode0);
	if (eResult != SPI_Success) {
		printf("SPI Failed to Initialize: %d\r\n", eResult);
		return 0;
	}
	
	eResult = gSPI.pfTransferByte(&gSPI, 0x00, &nReadByte);
	if (eResult != SPI_Success) {
		printf("SPI Transfer Failed: %d / %d\r\n", eResult, ((sRasPiSPIHWInfo_t *)gSPI.pHWInfo)->nLastErr);
		return 0;
	}
	
	printf("Read value: %x\r\n", nReadByte);
	
	return 1;
}
