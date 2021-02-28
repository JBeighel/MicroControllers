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
