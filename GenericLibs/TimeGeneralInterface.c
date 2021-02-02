/**	File:	TimeGeneralInterface.c
	Author:	J. Beighel
	Date:	2021-02-02
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
	pIface->pfDelay100NanoSeconds = &TimeDelayNotImplemented;
	
	pIface->eCapabilities = TimeCap_None;
	
	return Success;
}
	
uint32_t TimeCurrentTicksNotImplemented(void) {
	return 0;
}

eReturn_t TimeDelayNotImplemented(uint32_t nDelayAmount) {
	return Fail_NotImplem;
}
