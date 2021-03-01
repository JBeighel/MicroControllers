/**	File:	???.c
	Author:	J. Beighel
	Date:	MM-DD-YYYY
*/

/*****	Includes	*****/
	#include "AppMain.h"

/*****	Defines		*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/


/*****	Functions	*****/

void BootstrapTask(void const * argument) {
	GPIO_INIT(&gGpioA, GPIO_A_HWINFO);
	GPIO_INIT(&gGpioB, GPIO_B_HWINFO);
	GPIO_INIT(&gGpioH, GPIO_H_HWINFO);

	I2C_INIT(&gI2C1, true, 100000, I2C_1_HWINFO);
	I2C_INIT(&gI2C3, true, 100000, I2C_3_HWINFO);

	UART_INIT(&gUart1, 115200, UART_8None1, UART_1_HWINFO);
	UART_INIT(&gUart2, 115200, UART_8None1, UART_2_HWINFO);

	TIME_INIT(&gTime);

	while (1) {
		gGpioB.pfDigitalWriteByPin(&gGpioB, GPO_B03_2_Pin, true);

		HAL_IWDG_Refresh(&hiwdg);
		osDelay(100);

		gGpioB.pfDigitalWriteByPin(&gGpioB, GPO_B03_2_Pin, false);

		HAL_IWDG_Refresh(&hiwdg);
		osDelay(100);
	}

	return;
}
