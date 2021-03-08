/**	File:	TimeGeneralInterface.c
	Author:	J. Beighel
	Date:	2021-03-03
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

	/**	@brief	Watchdog timer control function not implemented
	 *	@ingroup	timeiface
	 */
	eReturn_t WatchdogNotImplemented(void);

/*****	Functions	*****/

eReturn_t TimeInterfaceInitialize(sTimeIface_t *pIface) {
	pIface->pfGetTicks = &TimeCurrentTicksNotImplemented;
	pIface->pfDelaySeconds = &TimeDelayNotImplemented;
	pIface->pfDelayMilliSeconds = &TimeDelayNotImplemented;
	pIface->pfDelayMicroSeconds = &TimeDelayNotImplemented;
	pIface->pfDelay100NanoSeconds = &TimeDelayNotImplemented;
	
	pIface->pfWatchdogStart = &WatchdogNotImplemented;
	pIface->pfWatchdogStop = &WatchdogNotImplemented;
	pIface->pfWatchdogRefresh = &WatchdogNotImplemented;

	pIface->eCapabilities = TimeCap_None;
	
	return Success;
}
	
uint32_t TimeCurrentTicksNotImplemented(void) {
	return 0;
}

eReturn_t TimeDelayNotImplemented(uint32_t nDelayAmount) {
	return Fail_NotImplem;
}

eReturn_t WatchdogNotImplemented(void) {
	return Fail_NotImplem;
}