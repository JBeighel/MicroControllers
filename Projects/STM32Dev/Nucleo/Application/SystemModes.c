/**	File:	SystemModes.c
	Author:	J. Beighel
	Date:	2021-05-03
*/

/*****	Includes	*****/
	#include "SystemModes.h"

/*****	Defines		*****/


/*****	Definitions	*****/
	typedef struct sSysModeTaskInfo_t {
		SysModeTaskID_t nID;
	} sSysModeTaskInfo_t;

	typedef struct sSysModeModeInfo_t {
		pfSysModeChangeHandler_t pfOnEnter;
		pfSysModeChangeHandler_t pfOnExit;
		pfSysModeChangeHandler_t pfOnEllapse;
		uint32_t tTimeInMode;
	} sSysModeModeInfo_t;

	typedef struct sSysModeInfo_t {
		eSysModeModes_t eCurrentMode;
		uint32_t tModeEnter;
		sSysModeModeInfo_t aModes[SysMode_NumModes];
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

	for (nCtr = 0; nCtr < SysMode_NumModes; nCtr++) {
		mSysInfo.aModes[nCtr].pfOnEnter = NULL;
		mSysInfo.aModes[nCtr].pfOnExit = NULL;
		mSysInfo.aModes[nCtr].pfOnEllapse = NULL;
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
