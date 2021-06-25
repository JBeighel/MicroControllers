/**	File:	LogicInterpreter.c
	Author:	J. Beighel
	Date:	2021-06-24
*/

/*****	Includes	*****/
	#include "LogicInterpreter.h"

/*****	Defines		*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/


/*****	Functions	*****/
eReturn_t LogicRunTimeInitialize(sLogicRunTime_t *pRunTime) {
	uint32_t nCtr, nMemCtr;
	
	//Clear out all program units (functions)
	for (nCtr = 0; nCtr < LOGIC_PROGRAMUNITS; nCtr++) {
		//Wipe their local space
		for (nMemCtr = 0; nMemCtr < LOGIC_MEMBLOCKSIZE; nMemCtr++) {
			pRunTime->aProgramUnits[nCtr].aMemory[nMemCtr].eType = LGCVar_Unspecified;
			pRunTime->aProgramUnits[nCtr].aMemory[nMemCtr].nInteger = 0;
			pRunTime->aProgramUnits[nCtr].aMemory[nMemCtr].nDecimal = 0;
		}
		
		//Wipe instruction space
		for (nMemCtr = 0; nMemCtr < LOGIC_PROGRAMINSTRS; nMemCtr++) {
			pRunTime->aProgramUnits[nCtr].aProgram[nMemCtr].eCommand = LGCIns_CmdNoOp;
			pRunTime->aProgramUnits[nCtr].aProgram[nMemCtr].Param.eType = LGCVar_Unspecified;
			pRunTime->aProgramUnits[nCtr].aProgram[nMemCtr].Param.nInteger = 0;
			pRunTime->aProgramUnits[nCtr].aProgram[nMemCtr].Param.nDecimal = 0;
		}
		
		//Clear all other values
		pRunTime->aProgramUnits[nCtr].nProgIdx = 0;
		pRunTime->aProgramUnits[nCtr].nNumInputs = 0;
		pRunTime->aProgramUnits[nCtr].nNumOutputs = 0;
		pRunTime->aProgramUnits[nCtr].pReturnTo = NULL;
	}
	
	//Wipe out the global space
	for (nMemCtr = 0; nMemCtr < LOGIC_MEMBLOCKSIZE; nMemCtr++) {
		pRunTime->aGlobals[nMemCtr].eType = LGCVar_Unspecified;
		pRunTime->aGlobals[nMemCtr].nInteger = 0;
		pRunTime->aGlobals[nMemCtr].nDecimal = 0;
	}
	
	//Wipe out the register space
	for (nMemCtr = 0; nMemCtr < LOGIC_REGISTERCOUNT; nMemCtr++) {
		pRunTime->aRegisters[nMemCtr].eType = LGCVar_Unspecified;
		pRunTime->aRegisters[nMemCtr].nInteger = 0;
		pRunTime->aRegisters[nMemCtr].nDecimal = 0;
	}
	
	//Wipe out the stack space
	for (nMemCtr = 0; nMemCtr < LOGIC_STACKDEPTH; nMemCtr++) {
		pRunTime->aStack[nMemCtr].eType = LGCVar_Unspecified;
		pRunTime->aStack[nMemCtr].nInteger = 0;
		pRunTime->aStack[nMemCtr].nDecimal = 0;
	}
	
	//Set the current program to something
	pRunTime->pCurrProgram = &(pRunTime->aProgramUnits[0]);
	
	return Success;
}
