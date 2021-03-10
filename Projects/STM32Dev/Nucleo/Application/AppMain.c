/**	File:	AppMain.c
	Author:	J. Beighel
	Date:	2021-03-10
*/

/*****	Includes	*****/
	#include "AppMain.h"

/*****	Defines		*****/


/*****	Definitions	*****/
	#define PATTERN_LEN		(sizeof(manPattern) / sizeof(uint32_t))

/*****	Constants	*****/
	const uint32_t manPattern[] = {
		APA102_DimBlue,
		APA102_DimWhite,
		APA102_DimBlue,
		APA102_Black,
		APA102_Black,
		APA102_Black,
		APA102_Black,
		APA102_DimGreen,
		APA102_DimWhite,
		APA102_DimGreen,
		APA102_Black,
		APA102_Black,
		APA102_Black,
		APA102_Black,
		APA102_DimRed,
		APA102_DimWhite,
		APA102_DimRed,
		APA102_Black,
		APA102_Black,
		APA102_Black,
		APA102_Black,
	};

/*****	Globals		*****/


/*****	Prototypes 	*****/
void CycleLEDColors(void);

/*****	Functions	*****/

void BootstrapTask(void const * argument) {
	//Initialize the processor
	PinSetup();

	//Initialize the peripherals
	BoardSetup();

	//Starts the output compare timer in interrupt mode
	gTime.pfInterruptStart(TIMEINT_2_HWINFO);

	//Begin the application
	while (1) {
		//Toggle the single LED
		gGpioB.pfDigitalWriteByPin(&gGpioB, GPO_B00_0_Pin, true);

		gTime.pfWatchdogRefresh();
		CycleLEDColors();
		osDelay(250);
		CycleLEDColors();
		osDelay(250);

		gGpioB.pfDigitalWriteByPin(&gGpioB, GPO_B00_0_Pin, false);

		//Feed the watchdog
		gTime.pfWatchdogRefresh();
		CycleLEDColors();
		osDelay(250);
		CycleLEDColors();
		osDelay(250);
	}

	return;
}

void CycleLEDColors(void) {
	static uint8_t nPatternStart = 0;
	uint8_t nLightCtr, nPatternCtr;

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

	return;
}
