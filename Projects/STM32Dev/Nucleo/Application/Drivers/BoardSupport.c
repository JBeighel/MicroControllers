/**	File:	BoardSupport.c
	Author:	J. Beighel
	Date:	2021-03-10
*/

/*****	Includes	*****/
	#include "BoardSupport.h"

/*****	Defines		*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/
	sAPA102Info_t gLEDString;

	sXBeeObject_t gXBee1;
	sXBeeObject_t gXBee2;

/*****	Prototypes 	*****/


/*****	Functions	*****/
eReturn_t BoardSetup() {
	APA102Initialize(&gLEDString, &gSpi1);

	//This doesn't really work, the Inputs and Outputs are on different GPIO ports
	//Need to remap ports to have a mix of inputs and outputs
	//Fortunately the driver doesn't support these pins yet so no harm yet
	XBeeInitialize(&gXBee1, &gUart1, &gGpioB, &gTime, GPO_B00_0_Pin, GPI_A04_2_Pin);
	XBeeInitialize(&gXBee2, &gUart2, &gGpioB, &gTime, GPO_B01_1_Pin, GPI_A05_3_Pin);

	return Success;
}
