/**	@defgroup	boardsupport
	@brief		Support for peripherals attached to processor
	@details	v0.1
	#Description

	#File Information
		File:	BoardSupport.h
		Author:	J. Beighel
		Date:	2021-03-10
*/

#ifndef __BOARDSUPPORT_H
	#define __BOARDSUPPORT_H

/*****	Includes	*****/
	#include "PinSupport.h"

	#include "APA102Driver.h"

/*****	Defines		*****/
	#if (APA102_SPICAPS & SPI_CAPS) != APA102_SPICAPS
		#error "APA102 driver SPI capabilities are not met by current implementation"
	#endif

/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/
	extern sAPA102Info_t gLEDString;

/*****	Prototypes 	*****/


/*****	Functions	*****/
	eReturn_t BoardSetup();

#endif

