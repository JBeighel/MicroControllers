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
		APA102_DimWhite,
		APA102_Black,
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
	gTime.pfInterruptStart(TIMEINT_2_HWINFO);

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
		gGpioB.pfDigitalWriteByPin(&gGpioB, GPO_B00_0_Pin, true);

		gTime.pfWatchdogRefresh();
		osDelay(500);

		gGpioB.pfDigitalWriteByPin(&gGpioB, GPO_B00_0_Pin, false);

		//Feed the watchdog
		gTime.pfWatchdogRefresh();
		osDelay(500);
	}

	return;
}
