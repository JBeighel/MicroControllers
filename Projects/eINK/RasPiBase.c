/**	File:	RasPiBase.c
	Author:	J. Beighel
	Date:	12-16-2020
*/

/*****	Includes	*****/
	#include "CommonUtils.h"
	#include "TimeGeneralInterface.h"
	#include "GPIOGeneralInterface.h"
	#include "I2CGeneralInterface.h"
	#include "SPIGeneralInterface.h"
	#include "UARTGeneralInterface.h"
	
	#include "GPIO_RaspberryPi.h"
	#include "I2C_RaspberryPi.h"
	#include "SPI_RaspberryPi.h"
	#include "UART_RaspberryPi.h"
	
	#include "SSD1675Driver.h"

/*****	Defines		*****/
	#if ((TIME_CAPS & SSD1675_TIMECAPS) != SSD1675_TIMECAPS)
		#error	Selected Time interface can not be used with the SSD1675 peripheral
	#endif
	
	#if ((GPIO_1_CAPS & SSD1675_GPIOCAPS) != SSD1675_GPIOCAPS)
		#error	Selected GPIO port can not be used with the SSD1675 peripheral
	#endif
	
	#if ((SPI_1_CAPS & SSD1675_SPICAPS) != SSD1675_SPICAPS)
		#error	Selected SPI port can not be used with the SSD1675 peripheral
	#endif
	
	#define EINK_PINChipSel		25
	#define EINK_PINDataCmd		24

/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/
	sTimeIface_t gTime;
	sGPIOIface_t gGPIO;
	sI2CIface_t gI2C;
	sSPIIface_t gSPI;
	sUARTIface_t gUART;
	
	sSSD1675Info_t gEInk;

/*****	Prototypes 	*****/


/*****	Functions	*****/
eReturn_t BoardInit(void) {
	int eResult;
	
	//Init processor
	eResult = TIME_INIT(&gTime);
	if (eResult != Success) {
		return Fail_Unknown;
	}
	
	eResult = GPIO_INIT(&gGPIO, GPIO_HWINFO);
	if (eResult != Success) {
		return Fail_Unknown;
	}
	
	//Set GPIO pin modes
	gGPIO.pfSetModeByPin(&gGPIO, EINK_PINChipSel, GPIO_DigitalOutput);
	gGPIO.pfSetModeByPin(&gGPIO, EINK_PINDataCmd, GPIO_DigitalOutput);
	
	eResult = I2C_1_PORTINIT(&gI2C, true, 100000, I2C_1_HWINFO);
	if (eResult != GPIO_Success) {
		return Fail_Unknown;
	}
	
	eResult = SPI_1_PORTINIT(&gSPI, SPI_1_HWINFO, 5000000, SPI_MSBFirst, SPI_Mode0);
	if (eResult != SPI_Success) {
		return Fail_Unknown;
	}
	
	eResult = UART_PORTINITIALIZE(&gUART, 9600, UART_8None1, UART_1_HWINFO);
	if (eResult != UART_Success) {
		return Fail_Unknown;
	}
	
	//Init peripherals
	eResult = SSD1675Initialize(&gEInk, &gTime, &gGPIO, &gSPI, EINK_PINChipSel, EINK_PINDataCmd);
	if (eResult != SSD1675_Success) {
		return Fail_Unknown;
	}
	
	return Success;
}

int main(int nArgCnt, char **aArgVals) {
	
	if (BoardInit() != Success) {
		printf("Board initialization failed.\r\n");
		return 1;
	}
	
	
	return 0;
}