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


/*****	Globals		*****/


/*****	Prototypes 	*****/


/*****	Functions	*****/

void BootstrapTask(void const * argument) {
	//Initialize the processor
	PinSetup();

	//Initialize the peripherals
	BoardSetup();

	//Begin the application
	while (1) {
		
	}

	return;
}
