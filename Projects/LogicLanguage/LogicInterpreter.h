/**	@defgroup	logic
	@brief		Functions for implementing a logic runtime
	@details	v0.1
	#Description
	
	#File Information
		File:	LogicInterpreter.h
		Author:	J. Beighel
		Date:	2021-06-24
*/

#ifndef __LOGICINTERPRETER_H
	#define __LOGICINTERPRETER_H

/*****	Includes	*****/
	#include "CommonUtils.h"

/*****	Defines		*****/
	#define LOGIC_MEMBLOCKSIZE	16
	
	#define LOGIC_REGISTERCOUNT	4
	
	#define LOGIC_STACKDEPTH	64
	
	#define LOGIC_PROGRAMUNITS	2
	
	#define LOGIC_PROGRAMINSTRS	20

/*****	Definitions	*****/
	typedef enum eLogicVarType_t {
		LGCVar_Unspecified,
		LGCVar_Bool,
		LGCVar_Int8,
		LGCVar_Int16,
		LGCVar_Int32,
		LGCVar_Int64,
		LGCVar_Decimal,
		LGCVar_Date,
	} eLogicVarType_t;
	
	typedef enum eLogicInstType_t {
		LGCIns_CommandMask		= 0xFFF0,
		LGCIns_ParamTypeMask	= 0x000F,
		
		LGCIns_ParamConstNumber	= 0x0001,
		LGCIns_ParamRegister	= 0x0002,
		LGCIns_ParamLabel		= 0x0003,
		LGCIns_ParamLocalVar	= 0x0004,
		LGCIns_ParamGlobalVar	= 0x0005,
		
		LGCIns_CmdNoOp			= 0x0000,
		LGCIns_CmdLoad			= 0x0010,
		LGCIns_CmdStore			= 0x0020,
		LGCIns_CmdAdd			= 0x0030,
		LGCIns_CmdSub			= 0x0040,
		LGCIns_CmdMult			= 0x0050,
		LGCIns_CmdDiv			= 0x0060,
		LGCIns_CmdRSh			= 0x0070,
		LGCIns_CmdLSh			= 0x0080,
		LGCIns_CmdBitAnd		= 0x0090,
		LGCIns_CmdBitOr			= 0x00A0,
		LGCIns_CmdBitXor		= 0x00B0,
		LGCIns_CmdBitNeg		= 0x00C0,
		LGCIns_CmdLogicAnd		= 0x00D0,
		LGCIns_CmdLogicOr		= 0x00E0,
		LGCIns_CmdLogicNeg		= 0x00F0,
		LGCIns_CmdJump			= 0x0100,
		LGCIns_CmdJumpZero		= 0x0110,
		LGCIns_CmdJumpNonZero	= 0x0120,
		LGCIns_CmdBranch		= 0x0130,
		LGCIns_CmdReturn		= 0x0140,
		LGCIns_CmdExternal		= 0x0150,
	} eLogicInstType_t;
	
	typedef struct sLogicVariable_t {
		eLogicVarType_t eType;	/**< Type of variable this is holding */
		uint64_t nInteger;		/**< Integer or Date value */
		double nDecimal;		/**< Decimal value */
	} sLogicVariable_t;
	
	typedef struct sLogicInstruction_t {
		eLogicInstType_t eCommand;	/**< Command and parameter type */
		sLogicVariable_t Param;		/**< Parameter value */
	} sLogicInstruction_t;
	
	typedef struct sLogicProgEnv_t {
		struct sLogicProgEnv_t *pReturnTo;	/**< Program unit to return to upon completion */
		/** @breif		Local variable memory space 
			@details	The first nNumOutputs spaces are used to hold the 
				output variables.  These will be pushed onto the stack when 
				the program returns.
				The next nNumInputs spaces will hold the input variables.  When
				the program starts they will be popped from the stack.
				The remaining space will hold any other local variables the 
				program needs.
		*/
		sLogicVariable_t aMemory[LOGIC_MEMBLOCKSIZE];
		/** Instruction block to execute */
		sLogicInstruction_t aProgram[LOGIC_PROGRAMINSTRS];
		uint32_t nProgIdx;			/**< Instruction index to run next */
		uint32_t nNumInputs;		/**< Number of inputs pulled from the stack at start */
		uint32_t nNumOutputs;		/**< Number of outputs to push onto the stack */
	} sLogicProgEnv_t;
	
	typedef struct sLogicRunTime_t {
		/**	Global variable space */
		sLogicVariable_t aGlobals[LOGIC_MEMBLOCKSIZE];
		/** Space for runtime registers, or temporary value space */
		sLogicVariable_t aRegisters[LOGIC_REGISTERCOUNT];
		/** Stack for holding values used by instructions or to pass values to 
			other programs
		*/
		sLogicVariable_t aStack[LOGIC_STACKDEPTH];
		uint32_t nStackTopIdx;			/**< Index of the top of the stack */
		/**< Array of program units that can be run */
		sLogicProgEnv_t aProgramUnits[LOGIC_PROGRAMUNITS];
		sLogicProgEnv_t *pCurrProgram;	/**< Pointer to current program being executed */
	} sLogicRunTime_t;

/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	eReturn_t LogicRunTimeInitialize(sLogicRunTime_t *pRunTime);
	
	eReturn_t LogicSetProgramInstruction(sLogicRunTime_t *pRunTime, uint32_t nProgramIdx, uint32_t nInstIdx, sLogicInstruction_t eInst, sLogicVariable_t *pParam);
	
	eReturn_t LogicRunProgram(sLogicRunTime_t *pRunTime, uint32_t nProgramIdx);

/*****	Functions	*****/


#endif

