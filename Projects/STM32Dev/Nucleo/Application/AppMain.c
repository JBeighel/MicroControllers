/**	File:	???.c
	Author:	J. Beighel
	Date:	MM-DD-YYYY
*/

/*****	Includes	*****/
	#include "AppMain.h"

/*****	Defines		*****/


/*****	Definitions	*****/
#define PATTERN_LEN		(sizeof(manPattern) / sizeof(uint32_t))

/*****	Constants	*****/
const uint32_t manPattern[] = {
	APA102_DimRed,
	APA102_DimGreen,
	APA102_DimBlue,
};

/*****	Globals		*****/


/*****	Prototypes 	*****/


/*****	Functions	*****/

void BootstrapTask(void const * argument) {
	uint8_t nLightCtr, nPatternCtr, nPatternStart;
	sAPA102Info_t LEDString;

	GPIO_INIT(&gGpioA, GPIO_A_HWINFO);
	GPIO_INIT(&gGpioB, GPIO_B_HWINFO);
	GPIO_INIT(&gGpioH, GPIO_H_HWINFO);

	I2C_INIT(&gI2C1, true, 100000, I2C_1_HWINFO);
	I2C_INIT(&gI2C3, true, 100000, I2C_3_HWINFO);

	UART_INIT(&gUart1, 115200, UART_8None1, UART_1_HWINFO);
	UART_INIT(&gUart2, 115200, UART_8None1, UART_2_HWINFO);

	SPI_INIT(&gSpi1, SPI_1_HWINFO, 4000000, SPI_MSBFirst, SPI_Mode3);

	TIME_INIT(&gTime);

	APA102Initialize(&LEDString, &gSpi1);

	nPatternStart = 0;
	while (1) {
		nPatternCtr = nPatternStart;
		for (nLightCtr = 0; nLightCtr < 5; nLightCtr++) {
			APA102SetLightColor(&LEDString, nLightCtr, manPattern[nPatternCtr]);

			nPatternCtr += 1;
			if (nPatternCtr >= PATTERN_LEN) {
				nPatternCtr = 0;
			}
		}
		APA102UpdateLights(&LEDString);

		nPatternStart += 1;
		if (nPatternStart >= PATTERN_LEN) {
			nPatternStart = 0;
		}

		gGpioB.pfDigitalWriteByPin(&gGpioB, GPO_B03_2_Pin, true);
		gGpioB.pfDigitalWriteByPin(&gGpioB, GPO_H03_5_Pin, true);
		gGpioB.pfDigitalWriteByPin(&gGpioB, GPO_B01_1_Pin, false);

		HAL_IWDG_Refresh(&hiwdg);
		osDelay(200);

		gGpioB.pfDigitalWriteByPin(&gGpioB, GPO_B03_2_Pin, false);
		gGpioB.pfDigitalWriteByPin(&gGpioB, GPO_H03_5_Pin, false);
		gGpioB.pfDigitalWriteByPin(&gGpioB, GPO_B01_1_Pin, true);

		HAL_IWDG_Refresh(&hiwdg);
		osDelay(200);
	}

	return;
}
