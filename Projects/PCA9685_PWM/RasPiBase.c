/**	File:	RasPiBase.c
	Author:	J. Beighel
	Date:	12-17-2020
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
	
	#include "PCA9685Driver.h"

/*****	Defines		*****/
	#if ((TIME_CAPS & SSD1675_TIMECAPS) != PCA9685_TIMECAPS)
		#error	Selected Time interface can not be used with the SSD1675 peripheral
	#endif
	
	#if ((GPIO_1_CAPS & PCA9685_GPIOCAPS) != PCA9685_GPIOCAPS)
		#error	Selected GPIO port can not be used with the SSD1675 peripheral
	#endif
	
	#if ((I2C_1_CAPS & PCA9685_SPICAPS) != PCA9685_SPICAPS)
		#error	Selected SPI port can not be used with the SSD1675 peripheral
	#endif
	
	#define PWM_PINOutEn		4

/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/
	sTimeIface_t gTime;
	sGPIOIface_t gGPIO;
	sI2CIface_t gI2C;
	sSPIIface_t gSPI;
	sUARTIface_t gUART;
	
	sPCA9685Info_t gPWM;

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
	gGPIO.pfSetModeByPin(&gGPIO, PWM_PINOutEn, GPIO_DigitalOutput);
	
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
	eResult = PCA9685Initialize(&gPWM, &gGPIO, &gI2C, PCA9685_None, PWM_PINOutEn);
	if (eResult != PCA9685_Success) {
		return Fail_Unknown;
	}
	
	return Success;
}

int main(int nArgCnt, char **aArgVals) {
	ePCA9685Return_t eResult;
	uint32_t nFreqHz;
	
	if (BoardInit() != Success) {
		printf("Board initialization failed.\r\n");
		return 1;
	}
	
	eResult = PCA9685GetPWMFrequency(&gPWM, &nFreqHz);
	if (eResult != PCA9685_Success) {
		printf("Failed to read Frequency: %d\r\n", eResult);
		return 1;
	}
	printf("Frequency: %d Hz\r\n", nFreqHz);
	
	eResult = PCA9685SetPWMFrequency(&gPWM, 50); //Serve is 50 Hz, Min on 150, Max on 450
	if (eResult != PCA9685_Success) {
		printf("Failed to read Frequency: %d\r\n", eResult);
		return 1;
	}
	
	eResult = PCA9685GetPWMFrequency(&gPWM, &nFreqHz);
	if (eResult != PCA9685_Success) {
		printf("Failed to read Frequency: %d\r\n", eResult);
		return 1;
	}
	printf("Frequency: %d Hz\r\n", nFreqHz);
	
	PCA9685SetOutput(&gPWM, 0, 0, 150);
	gTime.pfDelayMilliSeconds(500);
	PCA9685SetOutput(&gPWM, 0, 0, 250);
	gTime.pfDelayMilliSeconds(500);
	PCA9685SetOutput(&gPWM, 0, 0, 350);
	gTime.pfDelayMilliSeconds(500);
	PCA9685SetOutput(&gPWM, 0, 0, 450);
	
	
	//PCA9685SetOutput(&gPWM, 0, 0, 4090);
	
	return 0;
}