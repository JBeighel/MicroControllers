/**	@defgroup	tf02driver
	@brief		
	@details	v0.1
	#Description
	
	#File Information
		File:	TF02Driver.c
		Author:	J. Beighel
		Date:	09-15-2020
*/

#ifndef __TF02DRIVER_H
	#define __TF02DRIVER_H

/*****	Includes	*****/
	#include "UARTGeneralInterface.h"

/*****	Constants	*****/


/*****	Definitions	*****/
	typedef enum eTF02Returns_t {
		TFO2Warn_Unknown		= -1,	/**< An unknown but recoverable error occurred */
		TF02_Success			= 0,	/**< Operation completed successfully */
		TF02Fail_Unknown		= 1,	/**< An unknown and unrecoverable error occurred */
	} eTF02Returns_t;
	
	typedef struct sTF02Device_t {
		sUARTIface_t *pUART;
	} sTF02Device_t;

/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	eTF02Returns_t TF02Initialize(sTF02Device_t *pTF02Obj, sUARTIface_t *pUART);

/*****	Functions	*****/


#endif

