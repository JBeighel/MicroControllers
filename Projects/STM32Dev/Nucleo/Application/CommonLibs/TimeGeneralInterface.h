/**	@defgroup	timeiface
	@brief		General interface for basic time functions
	@details	v0.3
	# Intent #
		This module is to create a common interface to provide basic time functions.  
		Applications that need time functions can use this interface to access this 
		functionality wihtout environment or processor dependencies.
		
		The interface itself will provide no functionality, it only offers a standardized 
		set of functions to use for time management.  Each environemnt will require 
		its own implementation of these functions.
		
	# Usage #
		Each processor or environment that will support this interface must create 
		its own copies of the interface functions.

		Each implementation must call TimeInterfaceInitialize() to ensure that the
		time interface object has correct definitions for each function pointer.  
		This will give each function a valid definition so that accidental calls 
		to non-implemented functions won't cause a crash.

		In addition the implementation must define a value to reach the
		TimeInterfaceInitialize() function. This should be called via a definition
		of TIME_INIT.

		Having these defined gives a very consistent and generic means of establishing
		the interface object in the application that looks like this:

		sTimeIface_t TimeObj;

		TIME_INIT(&TimeObj);

		The last thing the driver must do is create a define of the capabilities that
		it provides.  This define should be options from the eTimeCapabilities_t 
		enumeration ORed together.  This value should be a define of TIME_CAPS.  The 
		application can check the to see if the implementation has the capabilities 
		it requires by checking this defined value.
		
	#File Information
		File:	TimeGeneralInterface.h
		Author:	J. Beighel
		Date:	2021-03-03
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

	typedef void (*pfTimerInterrupt_t)(sTimeIface_t *pTime);

	/**	@brief		Enumeration of time interface capabilities
		@ingroup	timeiface
	*/
	typedef enum eTimeCapabilities_t {
		TimeCap_None			= 0x0000,	/**< No capabilities */
		TimeCap_GetTicks		= 0x0001,	/**< Can return a system tick count */
		TimeCap_DelaySec		= 0x0002,	/**< Can Delay for seconds */
		TimeCap_DelayMilliSec	= 0x0004,	/**< Can delay for milliseconds */
		TimeCap_DelayMicroSec	= 0x0008,	/**< Can delay for microseconds */
		TimeCap_Delay100NanoSec	= 0x0010,	/**< Can delay for 100s of nanoseconds */

		TimeCap_WatchdogStart	= 0x0020,	/**< Can start the watchdog timer after initialization */
		TimeCap_WatchdogStop	= 0x0020,	/**< Can stop the watchdog timer during runtime */
		TimeCap_WatchdogRefresh	= 0x0040,	/**< Can refresh the watchdog timer */

		TimeCap_TimerStart		= 0x0080,
		TimeCap_TimerStop		= 0x0100,
		TimeCap_TimerInterrupt	= 0x0200,
		TimeCap_TimerSetCount	= 0x0400,
	} eTimeCapabilities_t;
	
	/**	@brief		Function type to use in order to get the current system tick count
		@return		The current cound of system ticks, this number will overflow and wrap 
						at some point
		@ingroup	timeiface
	*/
	typedef uint32_t (*pfGetCurrentTicks_t)(void);
	
	/**	@brief		Function type to use in order to delay for a specified time period
		@details	The time period can be in any units
		@param		nDelayTime		Number of milliseconds to delay for
		@ingroup	timeiface
	*/
	typedef eReturn_t (*pfTimeDelay_t)(uint32_t nDelayAmount);
	
	typedef eReturn_t (*pfTimeIfaceInitialize_t)(sTimeIface_t *pTime);
	
	/**	@brief		Function type to use for controlling the watchdog
	 *	@return		Success if the operation succeeds or a code indicating the failure
	 *	@ingroup	timeiface
	 */
	typedef eReturn_t (*pfWatchdogControl_t)(void);

	typedef eReturn_t (*pfTimerStart_t)(void *pTimerHW);

	typedef eReturn_t (*pfTimerStop_t)(void *pTimerHW);

	typedef eReturn_t (*pfTimerSetCount_t)(uint32_t nCountVal);

	/**	@brief		Interface object for time methods
		@ingroup	timeiface
	*/
	typedef struct sTimeIface_t {
		pfGetCurrentTicks_t pfGetTicks;		/**< Function pointer for getting tick count */
		pfTimeDelay_t pfDelaySeconds;		/**< Function pointer for delay by seconds */
		pfTimeDelay_t pfDelayMilliSeconds;	/**< Function pointer for delay by milliseconds */
		pfTimeDelay_t pfDelayMicroSeconds;	/**< Function pointer for delay by microseconds */
		pfTimeDelay_t pfDelay100NanoSeconds;	/**< Function pointer for delay by 100s nanoseconds */
		
		pfWatchdogControl_t pfWatchdogStart;	/**< Function pointer to start watchdog timer */
		pfWatchdogControl_t pfWatchdogStop;		/**< Function pointer to stop watchdog timer */
		pfWatchdogControl_t pfWatchdogRefresh;	/**< Function pointer for watchdog timer refresh */

		pfTimerInterrupt_t pfTimerInterrupt;
		pfTimerStart_t pfTImerStart;
		pfTimerStop_t pfTimerStop;
		pfTimerSetCount_t pfTimerSetInterruptCount;

		eTimeCapabilities_t eCapabilities;	/**< Capabilities of this implementation */
	} sTimeIface_t;
	

/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	eReturn_t TimeInterfaceInitialize(sTimeIface_t *pIface);

/*****	Functions	*****/


#endif

