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

/*****	Prototypes 	*****/


/*****	Functions	*****/
eReturn_t BoardSetup() {
	APA102Initialize(&gLEDString, &gSpi1);

	return Success;
}
