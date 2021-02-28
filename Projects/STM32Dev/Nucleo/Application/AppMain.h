/**	@defgroup	GROUPNAME
	@brief
	@details	v
	#Description

	#File Information
		File:	???.c
		Author:	J. Beighel
		Date:	MM-DD-YYYY
*/

#ifndef __NAME_H
	#define __NAME_H

/*****	Includes	*****/
	#include "FreeRTOS.h"
	#include "task.h"
	#include "main.h"
	#include "cmsis_os.h"
	#include "iwdg.h"

	#include "CommonUtils.h"
	#include "GPIOGeneralInterface.h"
	#include "I2CGeneralInterface.h"
	#include "UARTGeneralInterface.h"

	#include "GPIO_NucleoL412KB.h"
	#include "I2C_NucleoL412KB.h"
	#include "UART_NucleoL412KB.h"

/*****	Defines		*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/
	sGPIOIface_t gGpioA;
	sGPIOIface_t gGpioB;
	sGPIOIface_t gGpioH;

/*****	Prototypes 	*****/


/*****	Functions	*****/


#endif

