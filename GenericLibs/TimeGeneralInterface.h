/**	@defgroup	timeiface
	@brief		General interface for basic time functions
	@details	v0.1
	#Description
	
	#File Information
		File:	TimeGeneralInterface.h
		Author:	J. Beighel
		Date:	2021-01-22
*/

#ifndef __TIMEIFACE
	#define __TIMEIFACE

/*****	Includes	*****/
	#include <stdint.h>
	#include <stdbool.h>
	
	#include "CommonUtils.h"

/*****	Defines		*****/


/*****	Definitions	*****/
	typedef struct sTimeIface_t sTimeIface_t;

	/**	@brief		Enumeration of time interface capabilities
		@ingroup	timeiface
	*/
	typedef enum eTimeCapabilities_t {
		TimeCap_None			= 0x0000,	/**< No capabilities */
		TimeCap_GetTicks		= 0x0001,	/**< Can return a system tick count */
		TimeCap_DelaySec		= 0x0002,	/**< Can Delay for seconds */
		TimeCap_DelayMilliSec	= 0x0004,	/**< Can delay for milliseconds */
		TimeCap_DelayMicroSec	= 0x0008,	/**< Can delay for microseconds */
	} eTimeCapabilities_t;
	
	/**	@brief		Function type to use in order to get the current system tick count
		@return		The current cound of system ticks, this number will overflow and wrap 
						at some point
		@ingroup	gpioiface
	*/
	typedef uint32_t (*pfGetCurrentTicks_t)(void);
	
	/**	@brief		Function type to use in order to delay for a specified time period
		@details	The time period can be in any units
		@param		nDelayTime		Number of milliseconds to delay for
		@ingroup	gpioiface
	*/
	typedef eReturn_t (*pfTimeDelay_t)(uint32_t nDelayAmount);
	
	typedef eReturn_t (*pfTimeIfaceInitialize_t)(sTimeIface_t *pTime);
	
	/**	@brief		Interface object for time methods
		@ingroup	timeiface
	*/
	typedef struct sTimeIface_t {
		pfGetCurrentTicks_t pfGetTicks;		/**< Function pointer for getting tick count */
		pfTimeDelay_t pfDelaySeconds;		/**< Function pointer for delay by seconds */
		pfTimeDelay_t pfDelayMilliSeconds;	/**< Function pointer for delay by milliseconds */
		pfTimeDelay_t pfDelayMicroSeconds;	/**< Function pointer for delay by microseconds */
	} sTimeIface_t;
	

/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	eReturn_t TimeInterfaceInitialize(sTimeIface_t *pIface);

/*****	Functions	*****/


#endif

