/**	File:	LogicInterpreter.c
	Author:	J. Beighel
	Date:	2021-06-26
*/

/*****	Includes	*****/
	#include "LogicInterpreter.h"

/*****	Defines		*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	/**	@brief		Pop a variable from the stack and into the variable space
		@param		pRunTime	Run time execution environment to operate in
		@param		pValue		Variable space to copy to popped value into
		@ingroup	logic
	*/
	eLogicReturn_t LogicStackPop(sLogicRunTime_t *pRunTime, sLogicVariable_t *pValue);
	
	/**	@brief		Push a variable onto the stack
		@param		pRunTime	Run time execution environment to operate in
		@param		pValue		Variable space to push onto the stack
		@ingroup	logic
	*/
	eLogicReturn_t LogicStackPush(sLogicRunTime_t *pRunTime, sLogicVariable_t *pValue);
	
	/**	@brief		Run a single instruction
		@details	Perform the command that is next to be executed.  All 
			sections of the run time environment will be updated accordingly.
		@param		pRunTime	Run time execution environment to operate in
		@return		LogicSuccess if the command was an instruction that doesn't
			change the program unit that is being executed.
			LogicWarn_ProgramBranch if the command branches to a new program
			unit.
			LogicWarn_ProgramReturn if the command returns from the current 
			program unit and resumes execution in the calling program unit
			LogicWarn_ProgramEnded if the entry program unit returns and all 
			execution is completed.
		@ingroup	logic
	*/
	eLogicReturn_t LogicRunInstruction(sLogicRunTime_t *pRunTime);
	
	/**	@brief		Add a pair of variables
		@details	Adds the two variables passed in.  The sum is then stored
			in the pAddendSum parameter.
			For boolean variables the value is set true if the sum is  non-zero,
			otherwise false.
		@param		pAddendSum		First value to be added, also receives the 
			result of the arithmetic
		@param		pAddend			Second value to be added.
		@return		LogicSuccess on successful completion, or an error code 
			indicating the problem encountered
		@ingroup	logic
	*/
	eLogicReturn_t LogicVariableAdd(sLogicVariable_t *pAddendSum, sLogicVariable_t *pAddend);
	
	/**	@brief		Subtract a pair of variables
		@details	Subtracts the two variables passed in.  The sum is then 
			stored in the pDifference parameter.
			pDifference = pDifference - pNumber
			For boolean variables the value is set true if the difference is
			non-zero, otherwise false.
		@param		pDifference		First value to be subtracted, also receives
			the result of the arithmetic
		@param		pNumber			Second value to be subtracted
		@return		LogicSuccess on successful completion, or an error code 
			indicating the problem encountered
		@ingroup	logic
	*/
	eLogicReturn_t LogicVariableSubtract(sLogicVariable_t *pDifference, sLogicVariable_t *pNumber);
	
	/**	@brief		Multiplies a pair of variables
		@details	Multiplies the two variables passed in.  The product is then 
			stored in the pProduct parameter.
			pProduct = pProduct * pNumber
			For boolean variables the value is set true if the product is
			non-zero, otherwise false.
		@param		pDifference		First value to be multiplied, also receives
			the result of the arithmetic
		@param		pNumber			Second value to be multiplied
		@return		LogicSuccess on successful completion, or an error code 
			indicating the problem encountered
		@ingroup	logic
	*/
	eLogicReturn_t LogicVariableMultiply(sLogicVariable_t *pProduct, sLogicVariable_t *pNumber);
	
	/**	@brief		Bit shifts a variable
		@details	Bit shifts the integer vale of the variable passed in.  The
			result is stored back in that variable.  The inserted bits will 
			always be zeros.
			Paramters can set the direction and number of bits to shift.  
		@param		pNumber			The variable to operate on
		@param		bShiftRight		True to right shift, false to left shift
		@param		nShiftSpaces	Number of bits to shift
		@return		LogicSuccess on successful completion, or an error code 
			indicating the problem encountered
		@ingroup	logic
	*/
	eLogicReturn_t LogicVariableShift(sLogicVariable_t *pNumber, bool bShiftRight, uint64_t nShiftSpaces);
	
	/**	@brief		Bitwise Ands a pair of variables
		@details	Bitwise Ands the two variables passed in.  The result is then 
			stored in the pOperand parameter.
			pOperand = pOperand & pNumber
			For boolean variables the value is set true if the result is
			non-zero, otherwise false.
		@param		pOperand		First value to be Anded, also receives
			the result of the arithmetic
		@param		pNumber			Second value to be Anded
		@return		LogicSuccess on successful completion, or an error code 
			indicating the problem encountered
		@ingroup	logic
	*/
	eLogicReturn_t LogicVariableBitAnd(sLogicVariable_t *pOperand, sLogicVariable_t *pNumber);
	
	/**	@brief		Bitwise Ors a pair of variables
		@details	Bitwise Ors the two variables passed in.  The result is then 
			stored in the pOperand parameter.
			pOperand = pOperand | pNumber
			For boolean variables the value is set true if the result is
			non-zero, otherwise false.
		@param		pOperand		First value to be Ored, also receives
			the result of the arithmetic
		@param		pNumber			Second value to be Ored
		@return		LogicSuccess on successful completion, or an error code 
			indicating the problem encountered
		@ingroup	logic
	*/
	eLogicReturn_t LogicVariableBitOr(sLogicVariable_t *pOperand, sLogicVariable_t *pNumber);

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
	sLogicVariable_t StackVal;
	eLogicReturn_t eResult;
	eLogicInstType_t eCmd, eVarType;
	
	if (pRunTime->pCurrProgram->nProgIdx >= LOGIC_PROGRAMINSTRS) {
		//Ran out of instruction space without returning
		return LogicFail_NoReturn;
	}
	
	//Find the current instruction
	pInstr = &(pRunTime->pCurrProgram->aProgram[pRunTime->pCurrProgram->nProgIdx]);
	
	//advance to next instruction
	pRunTime->pCurrProgram->nProgIdx += 1;
	
	//Pull out the parameter
	eVarType = pInstr->eCommand & LGCIns_ParamTypeMask;
	switch (eVarType) {
		case LGCIns_ParamNone: //No paramter, but don't leave invalid pointer
			pParam = &StackVal;
			StackVal.eType = LGCVar_Unspecified;
			StackVal.nInteger = 0;
			StackVal.nDecimal = 0;
			
			break;
		case LGCIns_ParamConstNumber: //Instruction parameter used as is
			pParam = &(pInstr->Param);
			break;
		case LGCIns_ParamLocalVar: //Use program's local memory space
			if (pInstr->Param.nInteger >= LOGIC_MEMBLOCKSIZE) {
				return LogicFail_MemoryIndex;
			}
			
			//The param has the index of local memory space
			pParam = &(pRunTime->pCurrProgram->aMemory[pInstr->Param.nInteger]);
			
			break;
		case LGCIns_ParamGlobalVar: //Use runtime's global memory space
			if (pInstr->Param.nInteger >= LOGIC_MEMBLOCKSIZE) {
				return LogicFail_MemoryIndex;
			}
			
			//The param has the index of global memory space
			pParam = &(pRunTime->aGlobals[pInstr->Param.nInteger]);
			
			break;
		case LGCIns_ParamRegister: //Use runtime's register memory space
			if (pInstr->Param.nInteger >= LOGIC_REGISTERCOUNT) {
				return LogicFail_MemoryIndex;
			}
			
			//The param has the index of register memory space
			pParam = &(pRunTime->aRegisters[pInstr->Param.nInteger]);
			
			break;
		case LGCIns_ParamLabel: //Instruction parameter is execution index
			pParam = &(pInstr->Param);
			break;
		default:
			return LogicFail_ParamType;
	}
	
	//process the instruction
	eCmd = pInstr->eCommand & LGCIns_CommandMask;
	switch (eCmd) {
		case LGCIns_CmdNoOp: //No operation, do nothing
			break;
		case LGCIns_CmdLoad: //Push a value onto the stack
			if (eVarType == LGCIns_ParamLabel) {
				//Can't load a label into the stack
				return LogicFail_InvalidParam;
			}
			
			//Parameter value gets pushed onto the stack
			eResult = LogicStackPush(pRunTime, pParam);
			if (eResult != LogicSuccess) {
				return eResult;
			}
			
			break;
		case LGCIns_CmdStore: //Pop a value and put it in memory
			if ((eVarType == LGCIns_ParamLabel) || (eVarType == LGCIns_ParamConstNumber)) {
				//Can't store into labels or constants
				return LogicFail_InvalidParam;
			}
		
			//Get the stack value
			eResult = LogicStackPop(pRunTime, &StackVal);
			if (eResult != LogicSuccess) {
				return eResult;
			}
			
			//Copy it into the parameter
			pParam->eType = StackVal.eType;
			pParam->nInteger = StackVal.nInteger;
			pParam->nDecimal = StackVal.nDecimal;
			
			break;
		case LGCIns_CmdAdd: //Pop avalue and add it to param
			if ((eVarType == LGCIns_ParamLabel) || (eVarType == LGCIns_ParamConstNumber)) {
				//Can't store into labels or constants
				return LogicFail_InvalidParam;
			}
			
			//Get the stack value
			eResult = LogicStackPop(pRunTime, &StackVal);
			if (eResult != LogicSuccess) {
				return eResult;
			}
			
			//Add it into the parameter based on var type
			LogicVariableAdd(pParam, &StackVal);
			
			break;
		case LGCIns_CmdSub: //Pop avalue and subtract it from param
			if ((eVarType == LGCIns_ParamLabel) || (eVarType == LGCIns_ParamConstNumber)) {
				//Can't store into labels or constants
				return LogicFail_InvalidParam;
			}
			
			//Get the stack value
			eResult = LogicStackPop(pRunTime, &StackVal);
			if (eResult != LogicSuccess) {
				return eResult;
			}
			
			//Add it into the parameter based on var type
			LogicVariableSubtract(pParam, &StackVal);
			
			break;
		case LGCIns_CmdMult: //Pop avalue and subtract it from param
			if ((eVarType == LGCIns_ParamLabel) || (eVarType == LGCIns_ParamConstNumber)) {
				//Can't store into labels or constants
				return LogicFail_InvalidParam;
			}
			
			//Get the stack value
			eResult = LogicStackPop(pRunTime, &StackVal);
			if (eResult != LogicSuccess) {
				return eResult;
			}
			
			//Add it into the parameter based on var type
			LogicVariableMultiply(pParam, &StackVal);
			
			break;
		case LGCIns_CmdRSh: //Pop avalue and right shift param that many bits
			if ((eVarType == LGCIns_ParamLabel) || (eVarType == LGCIns_ParamConstNumber)) {
				//Can't store into labels or constants
				return LogicFail_InvalidParam;
			}
			
			//Get the stack value
			eResult = LogicStackPop(pRunTime, &StackVal);
			if (eResult != LogicSuccess) {
				return eResult;
			}
			
			if (StackVal.eType == LGCVar_Decimal) {
				//Can't shift by fractions
				return LogicFail_InvalidParam;
			}
			
			//Add it into the parameter based on var type
			LogicVariableShift(pParam, true, StackVal.nInteger);
			
			break;
		case LGCIns_CmdLSh: //Pop avalue and left shift param that many bits
			if ((eVarType == LGCIns_ParamLabel) || (eVarType == LGCIns_ParamConstNumber)) {
				//Can't store into labels or constants
				return LogicFail_InvalidParam;
			}
			
			//Get the stack value
			eResult = LogicStackPop(pRunTime, &StackVal);
			if (eResult != LogicSuccess) {
				return eResult;
			}
			
			if (StackVal.eType == LGCVar_Decimal) {
				//Can't shift by fractions
				return LogicFail_InvalidParam;
			}
			
			//Add it into the parameter based on var type
			LogicVariableShift(pParam, false, StackVal.nInteger);
			
			break;
		case LGCIns_CmdBitAnd: //Pop avalue and bit And it with param 
			if ((eVarType == LGCIns_ParamLabel) || (eVarType == LGCIns_ParamConstNumber)) {
				//Can't store into labels or constants
				return LogicFail_InvalidParam;
			}
			
			//Get the stack value
			eResult = LogicStackPop(pRunTime, &StackVal);
			if (eResult != LogicSuccess) {
				return eResult;
			}
			
			if (StackVal.eType == LGCVar_Decimal) {
				//Can't bit operate on by fractions
				return LogicFail_InvalidParam;
			}
			
			//Add it into the parameter based on var type
			LogicVariableBitAnd(pParam, &StackVal);
			
			break;
		case LGCIns_CmdBitOr: //Pop a value and bit Or it with param 
			if ((eVarType == LGCIns_ParamLabel) || (eVarType == LGCIns_ParamConstNumber)) {
				//Can't store into labels or constants
				return LogicFail_InvalidParam;
			}
			
			//Get the stack value
			eResult = LogicStackPop(pRunTime, &StackVal);
			if (eResult != LogicSuccess) {
				return eResult;
			}
			
			if (StackVal.eType == LGCVar_Decimal) {
				//Can't bit operate on by fractions
				return LogicFail_InvalidParam;
			}
			
			//Add it into the parameter based on var type
			LogicVariableBitOr(pParam, &StackVal);
			
			break;
		case LGCIns_CmdJump: //Move execution to param instruction index
			if (eVarType != LGCIns_ParamLabel) {
				//Can't jump to non-label parameters
				return LogicFail_InvalidParam;
			}
			
			if (pParam->nInteger >= LOGIC_PROGRAMINSTRS) {
				return LogicFail_InstrIndex;
			}
			
			//Change the program index to the index specified
			pRunTime->pCurrProgram->nProgIdx = pParam->nInteger;
			
			break;
		case LGCIns_CmdJumpZero: //If Popped value zero move execution to param instruction index
			if (eVarType != LGCIns_ParamLabel) {
				//Can't jump to non-label parameters
				return LogicFail_InvalidParam;
			}
			
			if (pParam->nInteger >= LOGIC_PROGRAMINSTRS) {
				return LogicFail_InstrIndex;
			}
			
			//Get the stack value
			eResult = LogicStackPop(pRunTime, &StackVal);
			if (eResult != LogicSuccess) {
				return eResult;
			}
			
			if (((StackVal.eType == LGCVar_Decimal) && (StackVal.nDecimal == 0)) || (StackVal.nInteger == 0)) {
				//Change the program index to the index specified
				pRunTime->pCurrProgram->nProgIdx = pParam->nInteger;
			}
			
			break;
		case LGCIns_CmdJumpNonZero: //If popped value nonzero move execution to param index
			if (eVarType != LGCIns_ParamLabel) {
				//Can't jump to non-label parameters
				return LogicFail_InvalidParam;
			}
			
			if (pParam->nInteger >= LOGIC_PROGRAMINSTRS) {
				return LogicFail_InstrIndex;
			}
			
			//Get the stack value
			eResult = LogicStackPop(pRunTime, &StackVal);
			if (eResult != LogicSuccess) {
				return eResult;
			}
			
			if (((StackVal.eType == LGCVar_Decimal) && (StackVal.nDecimal != 0)) || (StackVal.nInteger != 0)) {
				//Change the program index to the index specified
				pRunTime->pCurrProgram->nProgIdx = pParam->nInteger;
			}
			
			break;
		case LGCIns_CmdReturn: //End this program unit and return to caller
			pRunTime->pCurrProgram->nProgIdx = 0; //Reset index for next run
			
			if (pRunTime->pCurrProgram->pReturnTo == NULL) {
				//In the original program unit, nothing to return to
				pRunTime->pCurrProgram = NULL;
				return LogicWarn_ProgramEnded;
			} else {
				//Update the current program
				pRunTime->pCurrProgram = pRunTime->pCurrProgram->pReturnTo;
				return LogicWarn_ProgramReturn;
			}
		default:
			return LogicFail_InvalidInstr;
	}
	
	//Command completed correctly
	return LogicSuccess;
}

eLogicReturn_t LogicVariableAdd(sLogicVariable_t *pAddendSum, sLogicVariable_t *pAddend) {
	if (pAddendSum->eType == LGCVar_Unspecified) { //No value type, just copy into
		pAddendSum->eType = pAddend->eType;
		pAddendSum->nInteger == pAddend->nInteger;
		pAddendSum->nDecimal == pAddend->nDecimal;
	} else if (pAddendSum->eType == LGCVar_Bool) {
		//Adding to one remains true
		if (pAddendSum->nInteger == 0) {
			if ((pAddend->eType == LGCVar_Decimal) && (pAddend->nDecimal != 0)) {
				pAddendSum->nInteger = 1;
			} else if (pAddend->nInteger != 0) { //All other values are integer
				pAddendSum->nInteger = 1;
			}
		}
	} else if (pAddendSum->eType == LGCVar_Decimal) {
		//Add to decimal value
		if (pAddend->eType == LGCVar_Decimal) {
			pAddendSum->nDecimal += pAddend->nDecimal;
		} else { //All other values are integer
			pAddendSum->nDecimal += pAddend->nInteger;
		}
	} else {
		//Add to integer value
		if (pAddend->eType == LGCVar_Decimal) {
			pAddendSum->nInteger += pAddend->nDecimal;
		} else { //All other values are integer
			pAddendSum->nInteger += pAddend->nInteger;
		}
		
		//Truncate small int types
		if (pAddendSum->eType == LGCVar_Int32) {
			pAddendSum->nInteger &= 0xFFFFFFFF;
		} else if (pAddendSum->eType == LGCVar_Int16) {
			pAddendSum->nInteger &= 0xFFFF;
		} else if (pAddendSum->eType == LGCVar_Int8) {
			pAddendSum->nInteger &= 0xFF;
		}
	}
	
	return LogicSuccess;
}

eLogicReturn_t LogicVariableSubtract(sLogicVariable_t *pDifference, sLogicVariable_t *pNumber) {
	if (pDifference->eType == LGCVar_Unspecified) { //No value type, just copy into
		pDifference->eType = pNumber->eType;
		pDifference->nInteger == pNumber->nInteger;
		pDifference->nDecimal == pNumber->nDecimal;
	} else if (pDifference->eType == LGCVar_Bool) {
		//Subttracting from zero remains false
		if (pDifference->nInteger == 1) {
			if ((pNumber->eType == LGCVar_Decimal) && (pNumber->nDecimal != 0)) {
				pDifference->nInteger = 0;
			} else if (pNumber->nInteger != 0) { //All other values are integer
				pDifference->nInteger = 0;
			}
		}
	} else if (pDifference->eType == LGCVar_Decimal) {
		//Subtract from decimal value
		if (pNumber->eType == LGCVar_Decimal) {
			pDifference->nDecimal -= pNumber->nDecimal;
		} else { //All other values are integer
			pDifference->nDecimal -= pNumber->nInteger;
		}
	} else {
		//Subtract from integer value
		if (pNumber->eType == LGCVar_Decimal) {
			pDifference->nInteger -= pNumber->nDecimal;
		} else { //All other values are integer
			pDifference->nInteger -= pNumber->nInteger;
		}
		
		//Truncate small int types
		if (pDifference->eType == LGCVar_Int32) {
			pDifference->nInteger &= 0xFFFFFFFF;
		} else if (pDifference->eType == LGCVar_Int16) {
			pDifference->nInteger &= 0xFFFF;
		} else if (pDifference->eType == LGCVar_Int8) {
			pDifference->nInteger &= 0xFF;
		}
	}
	
	return LogicSuccess;
}

eLogicReturn_t LogicVariableMultiply(sLogicVariable_t *pProduct, sLogicVariable_t *pNumber) {
	if (pProduct->eType == LGCVar_Unspecified) { //No value type, just copy into
		pProduct->eType = pNumber->eType;
		pProduct->nInteger == pNumber->nInteger;
		pProduct->nDecimal == pNumber->nDecimal;
	} else if (pProduct->eType == LGCVar_Bool) {
		//Miltiplying anything with zero remains false
		if (pProduct->nInteger == 1) {
			if ((pNumber->eType == LGCVar_Decimal) && (pNumber->nDecimal == 0)) {
				pProduct->nInteger = 0;
			} else if (pNumber->nInteger == 0) { //All other values are integer
				pProduct->nInteger = 0;
			}
		}
	} else if (pProduct->eType == LGCVar_Decimal) {
		//Subtract from decimal value
		if (pNumber->eType == LGCVar_Decimal) {
			pProduct->nDecimal *= pNumber->nDecimal;
		} else { //All other values are integer
			pProduct->nDecimal *= pNumber->nInteger;
		}
	} else {
		//Subtract from integer value
		if (pNumber->eType == LGCVar_Decimal) {
			pProduct->nInteger *= pNumber->nDecimal;
		} else { //All other values are integer
			pProduct->nInteger *= pNumber->nInteger;
		}
		
		//Truncate small int types
		if (pProduct->eType == LGCVar_Int32) {
			pProduct->nInteger &= 0xFFFFFFFF;
		} else if (pProduct->eType == LGCVar_Int16) {
			pProduct->nInteger &= 0xFFFF;
		} else if (pProduct->eType == LGCVar_Int8) {
			pProduct->nInteger &= 0xFF;
		}
	}
	
	return LogicSuccess;
}

eLogicReturn_t LogicVariableShift(sLogicVariable_t *pNumber, bool bShiftRight, uint64_t nShiftSpaces) {
	if (pNumber->eType == LGCVar_Decimal) { //Can't shift floats
		return LogicFail_InvalidParam;
	}
	
	if (bShiftRight == true) {
		pNumber->nInteger >>= nShiftSpaces;
	} else {
		pNumber->nInteger <<= nShiftSpaces;
	}
	
	if (pNumber->eType == LGCVar_Bool) {
		if (pNumber->nInteger != 0) {
			pNumber->nInteger = 1;
		}
	} else if (pNumber->eType == LGCVar_Int32) {
		pNumber->nInteger &= 0xFFFFFFFF;
	} else if (pNumber->eType == LGCVar_Int16) {
		pNumber->nInteger &= 0xFFFF;
	} else if (pNumber->eType == LGCVar_Int8) {
		pNumber->nInteger &= 0xFF;
	}
	
	return LogicSuccess;
}

eLogicReturn_t LogicVariableBitAnd(sLogicVariable_t *pOperand, sLogicVariable_t *pNumber) {
	if (pOperand->eType == LGCVar_Decimal) { //Can't shift floats
		return LogicFail_InvalidParam;
	}
	
	pOperand->nInteger &= pNumber->nInteger;
	
	if (pOperand->eType == LGCVar_Bool) {
		if (pNumber->nInteger != 0) {
			pNumber->nInteger = 1;
		}
	} else if (pNumber->eType == LGCVar_Int32) {
		pNumber->nInteger &= 0xFFFFFFFF;
	} else if (pNumber->eType == LGCVar_Int16) {
		pNumber->nInteger &= 0xFFFF;
	} else if (pNumber->eType == LGCVar_Int8) {
		pNumber->nInteger &= 0xFF;
	}
	
	return LogicSuccess;
}

eLogicReturn_t LogicVariableBitOr(sLogicVariable_t *pOperand, sLogicVariable_t *pNumber) {
	if (pOperand->eType == LGCVar_Decimal) { //Can't shift floats
		return LogicFail_InvalidParam;
	}
	
	pOperand->nInteger |= pNumber->nInteger;
	
	if (pOperand->eType == LGCVar_Bool) {
		if (pNumber->nInteger != 0) {
			pNumber->nInteger = 1;
		}
	} else if (pNumber->eType == LGCVar_Int32) {
		pNumber->nInteger &= 0xFFFFFFFF;
	} else if (pNumber->eType == LGCVar_Int16) {
		pNumber->nInteger &= 0xFFFF;
	} else if (pNumber->eType == LGCVar_Int8) {
		pNumber->nInteger &= 0xFF;
	}
	
	return LogicSuccess;
}