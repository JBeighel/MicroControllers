/**	@defgroup	sysmodes
	@brief		Module to track the operating mode and what permissions it has
	@details	v0.1
	#Description
		This module will maintain a list of operating modes for the system as
		well as permissions for what can be done in each mode.  Similarly
		transitions from one mode to another will be controlled to only allow
		valid changes.

		This is intended to function as a global enable/disable for any module
		or hardware component.  In addition it should also enforce safe
		transitions between operating states.

		Functions are provided to test the current mode and available permissions
		that all other modules will need to call in order to obey the rules
		established.

	#File Information
		File:	SystemModes.h
		Author:	J. Beighel
		Date:	2021-05-03
*/

#ifndef __SYSTEMMODES_H
	#define __SYSTEMMODES_H

/*****	Includes	*****/
	#include "CommonUtils.h"
	#include "TimeGeneralInterface.h"

/*****	Defines		*****/
	/**	@brief		Time Interface capabilities required by this module
	 *	@ingroup	sysmodes
	 */
	#define SYSMODE_TIMEIFACECAPS	TimeCap_GetTicks

	/**	@brief		Constant to use for system modes that may not change to another mode
	 *	@ingroup	sysmodes
	 */
	#define	SYSMODE_NOTRANSITIONS	0

	/**	@brief		The maximum number of tasks that will check in routinely
	 *	@ingroup	sysmodes
	 */
	#define SYSMODE_NUMTASKS	4

	/**	@brief		Number of system ticks between verifications that all tasks checked in
	 *	@ingroup	sysmodes
	 */
	#define SYSMODE_TASKCHECK	100

	/**	@brief		Macro to convert a mode into a transition flag
	 *	@ingroup	sysmodes
	 */
	#define SystemModeFlag(eMode)	(1 << eMode)

/*****	Definitions	*****/
	/**	@brief		Enumeration of all operating modes in the system
	 *	@ingroup	sysmodes
	 */
	typedef enum eSysModeModes_t {
		SysMode_Initialize,		/**< System is starting up */
		SysMode_Configure,		/**< Configuration is being updated */
		SysMode_Normal,			/**< Normal operation */
		SysMode_Reset,			/**< System is shutting down to restart */
		SysMode_Shutdown,		/**< System is shutting down to power off */
		SysMode_Error,			/**< System encountered an error condition */

		SysMode_NumModes,		/**< Number of modes, must be last in enum */
	} eSysModeModes_t;

	/**	@brief		Enumeration of all system level permissions
	 *	@ingroup	sysmodes
	 */
	typedef enum eSysModePermissions_t {
		SysPerm_None			= 0x0000,	/**< No valid permissions */
		SysPerm_Shutdown		= 0x0001,	/**< Request system shutdown */
		SysPerm_DebugTerminal	= 0x0002,	/**< Read/Write to debug terminal */
		SysPerm_ConfigEdit		= 0x0004,	/**< Modify system configuration */
	} eSysModePermissions_t;

	/**	@brief		Type used to identify Task Identifiers
	 *	@details	This type is used for convenience by the module.  The
	 *		actual value can be cast to or from any compatible data size.
	 *	@ingroup	sysmodes
	 */
	typedef uint32_t SysModeTaskID_t;

	/**	@brief		Prototype for all mode change event handler functions
	 *	@ingroup	sysmodes
	 */
	typedef eReturn_t (*pfSysModeChangeHandler_t)();

	/**	@brief		Prototype for all event handlers for tasks that don't check in
	 *	@ingroup	sysmodes
	 */
	typedef void (*pfSysModeTaskCheckFail_t)(SysModeTaskID_t TaskID);


/*****	Constants	*****/
	/**	@brief		List of all valid transitions from one mode to another
	 *	@ingroup	sysmodes
	 */
	const eSysModeModes_t gSysModeTransitions[SysMode_NumModes] = {
		[SysMode_Initialize]	= SystemModeFlag(SysMode_Normal) | SystemModeFlag(SysMode_Error),
		[SysMode_Configure]		= SystemModeFlag(SysMode_Reset),
		[SysMode_Normal]		= SystemModeFlag(SysMode_Reset) | SystemModeFlag(SysMode_Shutdown) | SystemModeFlag(SysMode_Error),
		[SysMode_Reset]			= SYSMODE_NOTRANSITIONS,
		[SysMode_Shutdown]		= SYSMODE_NOTRANSITIONS,
		[SysMode_Error]			= SystemModeFlag(SysMode_Reset),
	};

	/**	@brief		List of all system permissions allowed in each mode
	 *	@ingroup	sysmodes
	 */
	const eSysModePermissions_t gSysModePermissions[SysMode_NumModes] = {
		[SysMode_Initialize]	= SysPerm_None,
		[SysMode_Configure]		= SysPerm_ConfigEdit,
		[SysMode_Normal]		= SysPerm_None,
		[SysMode_Reset]			= SysPerm_Shutdown,
		[SysMode_Shutdown]		= SysPerm_Shutdown,
		[SysMode_Error]			= SysPerm_None,
	};

/*****	Globals		*****/


/*****	Prototypes 	*****/
	/**	@brief		Prepares the system modes information, puts the device in Initialize mode
	 *	@return		Success upon completion, or a code indicating the error encountered
	 *	@ingroup	sysmodes
	 */
	eReturn_t SysModeInitialize(sTimeIface_t *pTimeObj);

	/**	@brief		Retrieves the current operating mode the system is in
	 *	@return		The current mode the system is in
	 *	@ingroup	sysmodes
	 */
	eSysModeModes_t SysModeGetCurrentMode();

	/**	@brief		Retrieves the number of system ticks the system was in its current mode
	 *	@details	This is the amount of time since the system was last told
	 *		to enter that mode.  If a request is made to enter the mode it is
	 *		currently in then the time will be from the most recent request.
	 *		This time is intended as a reset-able trimer where setting the same
	 *		mode is the reset.
	 *		If the mode changes then the exit handler for the prior mode will
	 *		be called as well as the entrance handler for the new mode.
	 *	@return		The number of system ticks the system was in the current mode
	 *	@ingroup	sysmodes
	 */
	uint32_t SysModeTimeInMode();

	/**	@brief		Requests the system to switch to a specific mode
	 *	@details	Obeys the transition rules defined in gSysModeTransitions
	 *		and will update the system operating mode.
	 *		This function will permit transitions into the current mode.  This
	 *		will reset the timer for how long the system has been in that mode.
	 *	@param		eNewMode	The mode
	 *	@return		Success upon completion, or a code indicating the error encountered
	 *	@ingroup	sysmodes
	 */
	eReturn_t SysModeTransitionMode(eSysModeModes_t eNewMode);

	/**	@brief		Check if a system permission is allowed in the current mode
	 *	@param		ePermission		The permission to check on
	 *	@return		True if the permission is allowed, false if it is not
	 *	@ingroup	sysmodes
	 */
	bool SysModeCheckPermission(eSysModePermissions_t ePermission);

	/**	@brief		Set an event handler for when the system enters a specific mode
	 *	@param		eMode		The mode to set the handler for
	 *	@param		pfHandler	Pointer to the handler function
	 *	@return		Success upon completion, or a code indicating the error encountered
	 *	@ingroup	sysmodes
	 */
	eReturn_t SysModeSetModeEnterHandler(eSysModeModes_t eMode, pfSysModeChangeHandler_t pfHandler);

	/**	@brief		Set an event handler for when the system exits a specific mode
	 *	@param		eMode		The mode to set the handler for
	 *	@param		pfHandler	Pointer to the handler function
	 *	@return		Success upon completion, or a code indicating the error encountered
	 *	@ingroup	sysmodes
	 */
	eReturn_t SysModeSetModeExitHandler(eSysModeModes_t eMode, pfSysModeChangeHandler_t pfHandler);

	eReturn_t SysModeSystemVerify();

	eReturn_t SysModeTaskCheckIn(SysModeTaskID_t TaskID);

/*****	Functions	*****/


#endif

