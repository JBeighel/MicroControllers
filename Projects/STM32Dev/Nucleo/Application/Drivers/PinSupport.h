/**	@defgroup	pinsupport
	@brief		Processor pin support information
	@details	v0.1
	#Description

	#File Information
		File:	PinSupport.c
		Author:	J. Beighel
		Date:	2021-03-02
*/

#ifndef __PINSUPPORT_H
	#define __PINSUPPORT_H

/*****	Includes	*****/
	#include "CommonUtils.h"
	#include "GPIOGeneralInterface.h"
	#include "I2CGeneralInterface.h"
	#include "UARTGeneralInterface.h"
	#include "SPIGeneralInterface.h"

	#include "GPIO_NucleoL412KB.h"
	#include "I2C_NucleoL412KB.h"
	#include "UART_NucleoL412KB.h"
	#include "SPI_NucleoL412KB.h"

/*****	Defines		*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/
	extern sGPIOIface_t gGpioA;
	extern sGPIOIface_t gGpioB;
	extern sGPIOIface_t gGpioH;

	extern sI2CIface_t gI2C1;
	extern sI2CIface_t gI2C3;

	extern sUARTIface_t gUart1;
	extern sUARTIface_t gUart2;

	extern sSPIIface_t gSpi1;

	extern sTimeIface_t gTime;

/*****	Prototypes 	*****/


/*****	Functions	*****/
	eReturn_t PinSetup();

#endif

