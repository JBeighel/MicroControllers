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
	eLogicReturn_t LogicStackPop(sLogicRunTime_t *pRunTime, sLogicVariable_t *pValue);
	
	eLogicReturn_t LogicStackPush(sLogicRunTime_t *pRunTime, sLogicVariable_t *pValue);
	
	eLogicReturn_t LogicRunInstruction(sLogicRunTime_t *pRunTime);

/*****	Functions	*****/
eLogicReturn_t LogicRunTimeInitialize(sLogicRunTime_t *pRunTime) {
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
	pRunTime->nStackTopIdx = 0; //Nothing is on the stack
	
	//Set the current program to something
	pRunTime->pCurrProgram = &(pRunTime->aProgramUnits[0]);
	
	return LogicSuccess;
}

eLogicReturn_t LogicSetProgramInstruction(sLogicRunTime_t *pRunTime, uint32_t nProgramIdx, uint32_t nInstIdx, eLogicInstType_t eInst, sLogicVariable_t *pParam) {
	pRunTime->aProgramUnits[nProgramIdx].aProgram[nInstIdx].eCommand = eInst;
	pRunTime->aProgramUnits[nProgramIdx].aProgram[nInstIdx].Param.eType = pParam->eType;
	pRunTime->aProgramUnits[nProgramIdx].aProgram[nInstIdx].Param.nInteger = pParam->nInteger;
	pRunTime->aProgramUnits[nProgramIdx].aProgram[nInstIdx].Param.nDecimal = pParam->nDecimal;
	
	return LogicSuccess;
}

eLogicReturn_t LogicRunProgram(sLogicRunTime_t *pRunTime, uint32_t nProgramIdx) {
	uint32_t nCtr, nParamIdx;
	eLogicReturn_t eResult;
	
	if (nProgramIdx > LOGIC_PROGRAMUNITS) {
		return LogicFail_InvalidProg;
	}
	
	//Select the program to run
	pRunTime->pCurrProgram = &(pRunTime->aProgramUnits[nProgramIdx]);
	//Begin with the first instruction
	pRunTime->pCurrProgram->nProgIdx = 0;
	//Entry point has nothing to return to
	pRunTime->pCurrProgram->pReturnTo = NULL;
	
	//Pop all the input values from the stack
	for (nCtr = 0; nCtr < pRunTime->pCurrProgram->nNumInputs; nCtr++) {
		nParamIdx = nCtr + pRunTime->pCurrProgram->nNumOutputs;
		eResult = LogicStackPop(pRunTime, &(pRunTime->pCurrProgram->aMemory[nParamIdx]));
		
		if (eResult != LogicSuccess) {
			return eResult;
		}
	}
	
	//Program space should be ready, start running instructions
	eResult = LogicSuccess;
	while ((eResult == LogicSuccess) || (eResult == LogicWarn_ProgramReturn)) {
		eResult = LogicRunInstruction(pRunTime);
	}
	
	if (eResult == LogicWarn_ProgramEnded) {
		return LogicSuccess;
	} else {
		return eResult;
	}
}

eLogicReturn_t LogicStackPop(sLogicRunTime_t *pRunTime, sLogicVariable_t *pValue) {
	if (pRunTime->nStackTopIdx == 0) {
		//No values on the stack to pop
		pValue->eType = LGCVar_Unspecified;
		pValue->nInteger = 0;
		pValue->nDecimal = 0;
		
		return LogicFail_StackUnderflow;
	}
	
	pRunTime->nStackTopIdx -= 1; //Reduce the stack contents by 1
	
	//Copy the popped information over
	pValue->eType = pRunTime->aStack[pRunTime->nStackTopIdx].eType;
	pValue->nInteger = pRunTime->aStack[pRunTime->nStackTopIdx].nInteger;
	pValue->nDecimal = pRunTime->aStack[pRunTime->nStackTopIdx].nDecimal;
	
	return LogicSuccess;
}
	
eLogicReturn_t LogicStackPush(sLogicRunTime_t *pRunTime, sLogicVariable_t *pValue) {
	if (pRunTime->nStackTopIdx > LOGIC_STACKDEPTH) {
		//No space in the stack for more
		return LogicFail_StackOverflow;
	}
	
	//Add the information to the stack
	pRunTime->aStack[pRunTime->nStackTopIdx].eType = pValue->eType;
	pRunTime->aStack[pRunTime->nStackTopIdx].nInteger = pValue->nInteger;
	pRunTime->aStack[pRunTime->nStackTopIdx].nDecimal = pValue->nDecimal;
	
	pRunTime->nStackTopIdx += 1; //Update the top spot
	
	return LogicSuccess;
}

eLogicReturn_t LogicRunInstruction(sLogicRunTime_t *pRunTime) {
	sLogicInstruction_t *pInstr;
	sLogicVariable_t *pParam;
	eLogicReturn_t eResult;
	
	if (pRunTime->pCurrProgram->nProgIdx >= LOGIC_PROGRAMINSTRS) {
		//Ran out of instruction space without returning
		return LogicFail_NoReturn;
	}
	
	//Find the current instruction
	pInstr = &(pRunTime->pCurrProgram->aProgram[pRunTime->pCurrProgram->nProgIdx]);
	
	//advance to next instruction
	pRunTime->pCurrProgram->nProgIdx += 1;
	
	//Pull out the parameter
	switch (pInstr->eCommand & LGCIns_ParamTypeMask) {
		case LGCIns_ParamConstNumber: //Instruction parameter used as is
			pParam = &(pInstr->Param);
			break;
		case LGCIns_ParamLocalVar: //Use program's local memory space
		default:
			return LogicFail_ParamType;
	}
	
	//process the instruction
	switch (pInstr->eCommand & LGCIns_CommandMask) {
		case LGCIns_CmdNoOp: //No operation, do nothing
			break;
		case LGCIns_CmdLoad: //Push a value onto the stack
			if ((pInstr->eCommand & LGCIns_ParamTypeMask) == LGCIns_ParamLabel) {
				//Can't load a label into the stack
				return LogicFail_InvalidParam;
			}
			
			//Parameter value gets pushed onto the stack
			eResult = LogicStackPush(pRunTime, pParam);
			if (eResult != LogicSuccess) {
				return eResult;
			}
			
			break;
		default:
			return LogicFail_InvalidInstr;
	}
}