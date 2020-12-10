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
	eReturn_t TimeDelayNotImplemented(uint32_t nDelayAmount);

/*****	Functions	*****/

eReturn_t TimeInterfaceInitialize(sTimeIface_t *pIface) {
	pIface->pfGetTicks = &TimeCurrentTicksNotImplemented;
	pIface->pfDelaySeconds = &TimeDelayNotImplemented;
	pIface->pfDelayMilliSeconds = &TimeDelayNotImplemented;
	pIface->pfDelayMicroSeconds = &TimeDelayNotImplemented;
	
	return Success;
}
	
uint32_t TimeCurrentTicksNotImplemented(void) {
	return 0;
}

eReturn_t TimeDelayNotImplemented(uint32_t nDelayAmount) {
	return Fail_NotImplem;
}
