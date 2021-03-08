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

	//Initialize the processor
	PinSetup();

	//Initialize the peripherals
	BoardSetup();

	//Starts the output compare timer in interrupt mode
	HAL_TIM_OC_Start_IT(&htim2, TIM_CHANNEL_1);
	//Stops the timer in interrupt mode
	//HAL_TIM_OC_Stop_IT(&htim2, TIM_CHANNEL_1);

	//Begin the application
	nPatternStart = 0;
	while (1) {
		//Cycle the colored LEDs
		nPatternCtr = nPatternStart;
		for (nLightCtr = 0; nLightCtr < 5; nLightCtr++) {
			APA102SetLightColor(&gLEDString, nLightCtr, manPattern[nPatternCtr]);

			nPatternCtr += 1;
			if (nPatternCtr >= PATTERN_LEN) {
				nPatternCtr = 0;
			}
		}
		APA102UpdateLights(&gLEDString);

		nPatternStart += 1;
		if (nPatternStart >= PATTERN_LEN) {
			nPatternStart = 0;
		}

		//Toggle the single LED
		//gGpioB.pfDigitalWriteByPin(&gGpioB, GPO_B03_2_Pin, true);

		gTime.pfWatchdogRefresh();
		osDelay(200);

		//gGpioB.pfDigitalWriteByPin(&gGpioB, GPO_B03_2_Pin, false);

		//Feed the watchdog
		gTime.pfWatchdogRefresh();
		osDelay(200);
	}

	return;
}


bool gbLightOn = false;
//Timer 2 Channel 1 counter call back (auto resets to trigger again)
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim) {

	if (gbLightOn == false) {
		gGpioB.pfDigitalWriteByPin(&gGpioB, GPO_B03_2_Pin, true);
		gbLightOn = true;
		htim2.Init.Period = 32000000;
		htim2.Instance->ARR = htim2.Init.Period;
	} else {
		gGpioB.pfDigitalWriteByPin(&gGpioB, GPO_B03_2_Pin, false);
		gbLightOn = false;
		htim2.Init.Period = 4 * 32000000;
		htim2.Instance->ARR = htim2.Init.Period;
	}

	//Instance registers
	//ARR is the auto-reset register.  It will be used as the match
	//to the next interrupt
	//PSC is the prescaler
	//TimerClockFreq = APB 1 timer clocks?
	//(ARR + 1) * (PSC + 1) / TimerClockFreq = Timer period

	return;
}
