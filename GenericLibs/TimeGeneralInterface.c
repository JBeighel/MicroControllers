/**	File:	TimeGeneralInterface.c
	Author:	J. Beighel
	Date:	12-10-2020
*/

/*****	Includes	*****/
	#include "TimeGeneralInterface.h"

/*****	Defines		*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	/**	@brief		Get current ticks not implemented
		@ingroup	timeiface
	*/
	uint32_t TimeCurrentTicksNotImplemented(void);
	
	/**	@brief		Delay not implemented
		@ingroup	timeiface
	*/
	bool TimeDelayNotImplemented(uint32_t nDelayAmount);

/*****	Functions	*****/

bool TimeInterfaceInitialize(sTimeIface_t *pIface) {
	pIface->pfGetTicks = &TimeCurrentTicksNotImplemented;
	pIface->pfDelaySeconds = &TimeDelayNotImplemented;
	pIface->pfDelayMilliSeconds = &TimeDelayNotImplemented;
	pIface->pfDelayMicroSeconds = &TimeDelayNotImplemented;
	
	return true;
}
	
uint32_t TimeCurrentTicksNotImplemented(void) {
	return 0;
}

bool TimeDelayNotImplemented(uint32_t nDelayAmount) {
	return false;
}
