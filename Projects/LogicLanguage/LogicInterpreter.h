/**	@defgroup	logic
	@brief		Functions for implementing a logic runtime
	@details	v0.1
	#Description
	
	#File Information
		File:	LogicInterpreter.h
		Author:	J. Beighel
		Date:	2021-06-26
*/

#ifndef __LOGICINTERPRETER_H
	#define __LOGICINTERPRETER_H

/*****	Includes	*****/
	#include "CommonUtils.h"

/*****	Defines		*****/
	/** @brief		Number of variable spaces in a memory block
		@ingroup	logic
	*/
	#define LOGIC_MEMBLOCKSIZE	16
	
	/** @brief		Number of register spaces
		@details	Registers are intended for short term temporary value storage
		@ingroup	logic
	*/
	#define LOGIC_REGISTERCOUNT	4
	
	/** @brief		Number of variable spaces in the stack
		@ingroup	logic
	*/
	#define LOGIC_STACKDEPTH	64

	/** @brief		Number of program unit spaces in the runtime environment
		@ingroup	logic
	*/
	#define LOGIC_PROGRAMUNITS	2

	/** @brief		Number of instruction spaces in each program unit
		@ingroup	logic
	*/
	#define LOGIC_PROGRAMINSTRS	20

/*****	Definitions	*****/
	/**	@brief		Enumeration of all logic module return codes
		@ingroup	logic
	*/
	typedef enum eLogicReturn_t {
		LogicWarn_ProgramBranch	= 4,	/**< Execution branched into a new program unit */
		LogicWarn_ProgramEnded	= 3,	/**< Return with no program unit to return to, execution ended */
		LogicWarn_ProgramReturn	= 2,	/**< Current program unit returned to previous unit */
		LogicWarn_Unknown		= 1,	/**< Unknown but recoverable error encountered */
		LogicSuccess			= 0,	/**< Operation completed successfully */
		LogicFail_Unknown		= -1,	/**< Unknown and unrecoverable error encountered */
		LogicFail_InvalidProg	= -2,	/**< An invalid program unit was requested */
		LogicFail_StackUnderflow= -3,	/**< A pop request failed because the stack was empty */
		LogicFail_StackOverflow	= -4,	/**< A push request failed because all stack space was full */
		LogicFail_NoReturn		= -5,	/**< Reached end of instruction space and did not return */
		LogicFail_ParamType		= -6,	/**< An invalid parameter type was encountered */
		LogicFail_InvalidInstr	= -7,	/**< An invalid command was provided in an instruction */
		LogicFail_InvalidParam	= -8,	/**< An invalid parameter type was provided for an instruction */
		LogicFail_MemoryIndex	= -9,	/**< An invalid memory index was provided */
		LogicFail_InstrIndex	= -10,	/**< An invalid instruction index was provided */
	} eLogicReturn_t;

	/**	@brief		Enumeration of all program variable types
		@ingroup	logic
	*/
	typedef enum eLogicVarType_t {
		LGCVar_Unspecified,	/**< Unitialized memory space */
		LGCVar_Bool,		/**< Boolean value, true or false only */
		LGCVar_Int8,		/**< 8 bit signed integer */
		LGCVar_Int16,		/**< 16 bit signed integer */
		LGCVar_Int32,		/**< 32 bit signed integer */
		LGCVar_Int64,		/**< 64 bit signed integer */
		LGCVar_Decimal,		/**< Floating point decimal value */
		LGCVar_Date,		/**< Time and date, microseconds since Jan 1, 1970 */
	} eLogicVarType_t;
	
	/**	@brief		Enumeration of all instruction commands and parameter types
		@ingroup	logic
	*/
	typedef enum eLogicInstType_t {
		LGCIns_CommandMask		= 0xFFF0,	/**< Mask for command portion of instruction */
		LGCIns_ParamTypeMask	= 0x000F,	/**< Mask for parameter type bits in instruction */
		
		LGCIns_ParamNone		= 0x0000,	/**< No parameter */
		LGCIns_ParamConstNumber	= 0x0001,	/**< Parameter is a constant number value */
		LGCIns_ParamRegister	= 0x0002,	/**< Parameter is index of register to operate on */
		LGCIns_ParamLabel		= 0x0003,	/**< Parameter is index of program unit or instruction to change execution position to */
		LGCIns_ParamLocalVar	= 0x0004,	/**< Parameter is index of local memory to operate on */
		LGCIns_ParamGlobalVar	= 0x0005,	/**< Parameter is index of global memory to operate on */
		
		LGCIns_CmdNoOp			= 0x0000,	/**< No operation */
		LGCIns_CmdLoad			= 0x0010,	/**< Push parameter value onto stack */ 
		LGCIns_CmdStore			= 0x0020,	/**< Pop stack value into parameter */
		LGCIns_CmdAdd			= 0x0030,	/**< Pop stack and add into parameter */
		LGCIns_CmdSub			= 0x0040,	/**< Pop stack and subtract from parameter */
		LGCIns_CmdMult			= 0x0050,	/**< Pop stack and multiply with parameter */
		LGCIns_CmdDiv			= 0x0060,	/**< Pop stack and divide into parameter */
		LGCIns_CmdRSh			= 0x0070,	/**< Pop stack and right shift parameter that many bits */
		LGCIns_CmdLSh			= 0x0080,	/**< Pop stack and left shift parameter that many bits */
		LGCIns_CmdBitAnd		= 0x0090,	/**< Pop stack and bitwise And with parameter */
		LGCIns_CmdBitOr			= 0x00A0,	/**< Pop stack and bitwise Or with parameter */
		LGCIns_CmdBitXor		= 0x00B0,	/**< Pop stack and bitwise Xor with parameter */
		LGCIns_CmdBitNeg		= 0x00C0,	/**< Invert all bits in parameter */
		LGCIns_CmdLogicAnd		= 0x00D0,
		LGCIns_CmdLogicOr		= 0x00E0,
		LGCIns_CmdLogicNeg		= 0x00F0,
		LGCIns_CmdJump			= 0x0100,	/**< Jump to the instruction index in parameter */
		LGCIns_CmdJumpZero		= 0x0110,	/**< Pop stack and if its zero jump to instruction index in parameter */
		LGCIns_CmdJumpNonZero	= 0x0120,	/**< Pop stack and if its non-zero jump to instruction index in parameter */
		LGCIns_CmdBranch		= 0x0130,
		LGCIns_CmdReturn		= 0x0140,
		LGCIns_CmdExternal		= 0x0150,
	} eLogicInstType_t;
	
	/**	@brief		Structure to hold a program variable
		@ingroup	logic
	*/
	typedef struct sLogicVariable_t {
		eLogicVarType_t eType;	/**< Type of variable this is holding */
		uint64_t nInteger;		/**< Integer or Date value */
		double nDecimal;		/**< Decimal value */
	} sLogicVariable_t;
	
	/**	@brief		Structure to hold a program instruction
		@ingroup	logic
	*/
	typedef struct sLogicInstruction_t {
		eLogicInstType_t eCommand;	/**< Command and parameter type */
		sLogicVariable_t Param;		/**< Parameter value */
	} sLogicInstruction_t;
	
	/**	@brief		Structure to hold a program unit memory environment
		@ingroup	logic
	*/
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
	
	/**	@brief		Structure to hold an program's memory space
		@ingroup	logic
	*/
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
	/**	@brief		Intializes a run time environment
		@details	Sets all memory spaces as uninitialized and removes all 
			instructions from the program space.  This makes the entire run 
			time environment blank and available for use.
		@param		pRunTime	Run time environment to prepare for use
		@return		LogicSuccess on successful completion, or an error code 
			indicating the problem encountered
		@ingroup	logic
	*/
	eLogicReturn_t LogicRunTimeInitialize(sLogicRunTime_t *pRunTime);
	
	/**	@brief		Stores an instruction into the runtim environment
		@details	Places one instruction into a program unit of the run time
			environment.  Calling this function repeatedly will allow the 
			loading of an entire program to execute.
		@param		pRunTime		Runtim environment to inster the instruction
		@param		nProgramIdx		Program unt to place the instruction in
		@param		nInstIdx		Instruction index to store the instruction in
		@param		eInst			Instruction to insert, expects command bit 
			Ored with parameter type
		@param		pParam			Pointer to variable to copy into the parameter
			for this instruction
		@return		LogicSuccess on successful completion, or an error code 
			indicating the problem encountered
		@ingroup	logic
	*/
	eLogicReturn_t LogicSetProgramInstruction(sLogicRunTime_t *pRunTime, uint32_t nProgramIdx, uint32_t nInstIdx, eLogicInstType_t eInst, sLogicVariable_t *pParam);
	
	eLogicReturn_t LogicRunProgram(sLogicRunTime_t *pRunTime, uint32_t nProgramIdx);

/*****	Functions	*****/


#endif
