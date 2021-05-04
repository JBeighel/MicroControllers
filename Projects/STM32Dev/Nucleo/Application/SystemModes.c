/**	File:	SystemModes.c
	Author:	J. Beighel
	Date:	2021-05-03
*/

/*****	Includes	*****/
	#include "SystemModes.h"

/*****	Defines		*****/
	/**	@brief		Macro to convert a task number into a check in flag
	 *	@ingroup	sysmodes
	 */
	#define SystemTaskFlag(nTask)	(1 << nTask)

/*****	Definitions	*****/
	typedef struct sSysModeTaskInfo_t {
		SysModeTaskID_t nID;
		uint32_t tLastCheckIn;
		pfSysModeTaskCheckFail_t pfRecovery;
	} sSysModeTaskInfo_t;

	typedef struct sSysModeModeInfo_t {
		pfSysModeChangeHandler_t pfOnEnter;
		pfSysModeChangeHandler_t pfOnExit;
		pfSysModeChangeHandler_t pfOnElapse;
		uint32_t tTimeInMode;
	} sSysModeModeInfo_t;

	typedef struct sSysModeInfo_t {
		eSysModeModes_t eCurrentMode;
		uint32_t tModeEnter;
		uint32_t tLastTaskCheck;
		uint32_t TaskFlags;
		uint32_t nTasksTracked;
		sSysModeModeInfo_t aModes[SysMode_NumModes];
		sSysModeTaskInfo_t aTasks[SYSMODE_NUMTASKS];
		sTimeIface_t *pTimeIface;
	} sSysModeInfo_t;

/*****	Constants	*****/


/*****	Globals		*****/
	sSysModeInfo_t mSysInfo;

/*****	Prototypes 	*****/


/*****	Functions	*****/
eReturn_t SysModeInitialize(sTimeIface_t *pTimeObj) {
	uint32_t nCtr;

	mSysInfo.eCurrentMode = SysMode_Initialize;
	mSysInfo.tModeEnter = 0;
	mSysInfo.pTimeIface = pTimeObj;
	mSysInfo.nTasksTracked = 0;
	mSysInfo.TaskFlags = 0;
	mSysInfo.tLastTaskCheck = 0;

	for (nCtr = 0; nCtr < SysMode_NumModes; nCtr++) {
		mSysInfo.aModes[nCtr].pfOnEnter = NULL;
		mSysInfo.aModes[nCtr].pfOnExit = NULL;
		mSysInfo.aModes[nCtr].pfOnElapse = NULL;
		mSysInfo.aModes[nCtr].tTimeInMode = 0;
	}

	return Success;
}

eSysModeModes_t SysModeGetCurrentMode() {
	return mSysInfo.eCurrentMode;
}

uint32_t SysModeTimeInMode() {
	//Current ticks - mode enter ticks
	return mSysInfo.pTimeIface->pfGetTicks() - mSysInfo.tModeEnter;
}

eReturn_t SysModeTransitionMode(eSysModeModes_t eNewMode) {
	eReturn_t eResult, eRetVal = Success;

	//Make sure its a valid mode
	if (eNewMode >= SysMode_NumModes) {
		return Fail_Invalid;
	}

	if (eNewMode == mSysInfo.eCurrentMode) {
		//We're already in that mode, just refresh the timer
		mSysInfo.tModeEnter = mSysInfo.pTimeIface->pfGetTicks();
		return Success;
	}

	//See if this transition is allowed
	if (CheckAllBitsInMask(gSysModeTransitions[mSysInfo.eCurrentMode], SystemModeFlag(eNewMode)) == true) {
		//Call the handler for exiting the current mode
		if (mSysInfo.aModes[mSysInfo.eCurrentMode].pfOnExit != NULL) {
			eResult = mSysInfo.aModes[mSysInfo.eCurrentMode].pfOnExit();

			if (eResult > Success) { //Save off warnings
				eRetVal = eResult;
			} else { //On failure abort mode change
				return eResult;
			}
		}

		//Call the handler for entering the new mode
		if (mSysInfo.aModes[mSysInfo.eCurrentMode].pfOnEnter != NULL) {
			eResult = mSysInfo.aModes[mSysInfo.eCurrentMode].pfOnEnter();

			if (eResult > Success) { //Save off warnings
				eRetVal = eResult;
			} else { //On failure abort mode change
				return eResult;
			}
		}

		//Set the new mode
		mSysInfo.eCurrentMode = eNewMode;
		mSysInfo.tModeEnter = mSysInfo.pTimeIface->pfGetTicks();

		return eRetVal;
	} else {
		return Fail_Blocked;
	}
}

bool SysModeCheckPermission(eSysModePermissions_t ePermission) {
	if (CheckAllBitsInMask(gSysModePermissions[mSysInfo.eCurrentMode], ePermission) == true) {
		return true;
	} else {
		return false;
	}
}

eReturn_t SysModeSetModeEnterHandler(eSysModeModes_t eMode, pfSysModeChangeHandler_t pfHandler) {
	//Make sure its a valid mode
	if (eMode >= SysMode_NumModes) {
		return Fail_Invalid;
	}

	//Update the pointer
	mSysInfo.aModes[eMode].pfOnEnter = pfHandler;

	return Success;
}

eReturn_t SysModeSetModeExitHandler(eSysModeModes_t eMode, pfSysModeChangeHandler_t pfHandler) {
	//Make sure its a valid mode
	if (eMode >= SysMode_NumModes) {
		return Fail_Invalid;
	}

	//Update the pointer
	mSysInfo.aModes[eMode].pfOnExit = pfHandler;

	return Success;
}

eReturn_t SysModeSystemVerify() {
	eReturn_t eRetVal = Success;
	uint32_t tCurrTime, ExpFlags, nCtr;

	tCurrTime = mSysInfo.pTimeIface->pfGetTicks();

	if (tCurrTime - mSysInfo.tLastTaskCheck > SYSMODE_TASKCHECK) { //Time to verify that all tasks have checked in
		//Figure out what the flags should look like
		ExpFlags = UINT32_MAXVALUE;
		ExpFlags >>= 32 - mSysInfo.nTasksTracked;

		//Compare with the check in flags
		ExpFlags &= mSysInfo.TaskFlags;
		if (ExpFlags != 0) {
			//ExpFlags has flags for the tasks that didn't check in
			for (nCtr = 0; nCtr < mSysInfo.nTasksTracked; nCtr++) {
				if (CheckAllBitsInMask(ExpFlags, 0x01) == true) {
					//Found a task index that didn't check in
					if (mSysInfo.aTasks[nCtr].pfRecovery != NULL) {
						//It has a recovery function so call that
						mSysInfo.aTasks[nCtr].pfRecovery(mSysInfo.aTasks[nCtr].nID);
					}
				}

				//Shift the flags down to test the next index
				ExpFlags >>= 1;
			}
		}
	}

	//See if time in mode has elapsed
	if ((mSysInfo.aModes[mSysInfo.eCurrentMode].tTimeInMode > 0) && (mSysInfo.aModes[mSysInfo.eCurrentMode].pfOnElapse != NULL)) {
		if (SysModeTimeInMode() > mSysInfo.aModes[mSysInfo.eCurrentMode].tTimeInMode) {
			eRetVal = mSysInfo.aModes[mSysInfo.eCurrentMode].pfOnElapse();
		}
	}

	return eRetVal;
}

eReturn_t SysModeTaskCheckIn(SysModeTaskID_t TaskID) {
	uint32_t nCtr, nIdx;

	nIdx = SYSMODE_NUMTASKS; //Start with this invalid

	//Look for this task in the list
	for (nCtr = 0; nCtr < mSysInfo.nTasksTracked; nCtr++) {
		if (mSysInfo.aTasks[nCtr].nID == TaskID) {
			nIdx = nCtr;
			break;
		}
	}

	if (nIdx == SYSMODE_NUMTASKS) { //Never found this task in the list
		if (mSysInfo.nTasksTracked < SYSMODE_NUMTASKS) { //Still have space, add this one
			nIdx = mSysInfo.nTasksTracked;
			mSysInfo.nTasksTracked += 1;
			mSysInfo.aTasks[nIdx].nID = TaskID;
		} else { //No room to track more, this is bad
			return Fail_Invalid;
		}
	}

	//Mark this task as checked in
	mSysInfo.aTasks[nIdx].tLastCheckIn = mSysInfo.pTimeIface->pfGetTicks();
	mSysInfo.TaskFlags |= SystemTaskFlag(nIdx);

	return Success;
}

