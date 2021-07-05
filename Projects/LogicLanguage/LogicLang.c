/**	File:	LogicLang.c
	Author:	J. Beighel
	Date:	2021-06-24
*/

/*****	Includes	*****/
	#include <stdio.h>

	//Genereral use libraries
	#include "CommonUtils.h"
	#include "TimeGeneralInterface.h"
	#include "GPIOGeneralInterface.h"
	#include "I2CGeneralInterface.h"
	#include "SPIGeneralInterface.h"
	#include "UARTGeneralInterface.h"
	#include "NetworkGeneralInterface.h"

	#include "GPIO_RaspberryPi.h"
	#include "I2C_RaspberryPi.h"
	#include "SPI_RaspberryPi.h"
	#include "UART_RaspberryPi.h"
	#include "Network_RaspberryPi.h"
	
	//Driver libraries
	#include "LogicInterpreter.h"
	
/*****	Defines		*****/


/*****	Definitions	*****/
	typedef enum __attribute__((__packed__)) eLineType_t {
		Line_ProgUnitStart,
		Line_InstrInfo,
	} eLineType_t;

	typedef struct __attribute__((__packed__)) sProgUnitInfo_t {
		uint32_t nIndex;
		uint32_t nNumInputs;
		uint32_t nNumOutputs;
		uint32_t nNumInstr;
	} sProgUnitInfo_t;
	
	typedef struct __attribute__((__packed__)) sInstrInfo_t {
		eLogicInstType_t eCmd;
		sLogicVariable_t Param;
	} sInstrInfo_t;

	typedef struct __attribute__((__packed__)) sLineInfo_t {
		eLineType_t eType;
		
		uint8_t aReserved[2];
		
		union {
			sProgUnitInfo_t Prog;
			sInstrInfo_t Instr; 
			uint8_t aSize[29];
		} Data;
	} sLineInfo_t;
	
	STATIC_ASSERT(LineIfo32Bytes, sizeof(sLineInfo_t) == 32);

/*****	Constants	*****/


/*****	Globals		*****/
	sTimeIface_t gTime;
	sGPIOIface_t gGPIO;
	sI2CIface_t gI2C;
	sSPIIface_t gSPI;
	sUARTIface_t gUART;
	sTCPServ_t gTCPServ;
	sTCPClient_t gTCPClient;
	sUDPServ_t gUDPServ;
	sUDPClient_t gUDPClient;
	
/*****	Prototypes 	*****/
	eLogicReturn_t LogicExtern(sLogicVariable_t *pInputs, sLogicVariable_t *pOutputs);
	
	bool LoadLogicProgram(const char *strFileName, sLogicRunTime_t *pRunTime);

/*****	Functions	*****/
eReturn_t BoardInit(void) {
	int eResult;
	
	//Init processor (pin support work)
	eResult = TIME_INIT(&gTime);
	if (eResult != Success) {
		return Fail_Unknown;
	}
	
	eResult = GPIO_INIT(&gGPIO, GPIO_HWINFO);
	if (eResult != Success) {
		return Fail_Unknown;
	}
	
	eResult = I2C_INIT(&gI2C, true, 100000, I2C_1_HWINFO);
	if (eResult != GPIO_Success) {
		return Fail_Unknown;
	}
	
	eResult = SPI_INIT(&gSPI, SPI_1_HWINFO, 5000000, SPI_MSBFirst, SPI_Mode0);
	if (eResult != SPI_Success) {
		return Fail_Unknown;
	}
	
	eResult = UART_INIT(&gUART, 9600, UART_8None1, UART_1_HWINFO);
	if (eResult != UART_Success) {
		return Fail_Unknown;
	}
	
	TCPSERV_INIT(&gTCPServ);
	TCPCLIENT_INIT(&gTCPClient);
	UDPSERV_INIT(&gUDPServ);
	UDPCLIENT_INIT(&gUDPClient);
	
	//Init peripherals (board support work)
	
	
	return Success;
}

int main(int nArgCnt, char **aArgVals) {
	sLogicRunTime_t RunTime;
	sLogicVariable_t Param;
	eLogicReturn_t eResult;
	
	if ((false) && (BoardInit() != Success)) {
		printf("Board initialization failed.\r\n");
		return 1;
	}
	
	LogicRunTimeInitialize(&RunTime);
	
	//Program 1: entry point for testing
	LogicSetProgramIOCounts(&RunTime, 0, 0, 0);
	
	//Push constant 6 on the stack
	Param.eType = LGCVar_Int16;
	Param.nInteger = 6;
	LogicSetProgramInstruction(&RunTime, 0, 0, LGCIns_CmdLoad | LGCIns_ParamConstNumber, &Param);
	
	//Branch to new program (pops inptus from stack, pushes outputs to stack)
	Param.eType = LGCVar_Int32;
	Param.nInteger = 1;
	LogicSetProgramInstruction(&RunTime, 0, 1, LGCIns_CmdBranch | LGCIns_ParamLabel, &Param);
	
	//Pop stack value into local variable 0
	Param.eType = LGCVar_Int16;
	Param.nInteger = 0;
	LogicSetProgramInstruction(&RunTime, 0, 2, LGCIns_CmdStore | LGCIns_ParamLocalVar, &Param);
	
	//Push constant 0 on the stack
	Param.eType = LGCVar_Int16;
	Param.nInteger = 0;
	LogicSetProgramInstruction(&RunTime, 0, 3, LGCIns_CmdLoad | LGCIns_ParamConstNumber, &Param);
	
	//Branch to new program (pops inptus from stack, pushes outputs to stack)
	Param.eType = LGCVar_Int32;
	Param.nInteger = 1;
	LogicSetProgramInstruction(&RunTime, 0, 4, LGCIns_CmdBranch | LGCIns_ParamLabel, &Param);
	
	//Pop stack value and add it to local variable 0
	Param.eType = LGCVar_Int16;
	Param.nInteger = 0;
	LogicSetProgramInstruction(&RunTime, 0, 5, LGCIns_CmdAdd | LGCIns_ParamLocalVar, &Param);
	
	//Push constant on the stack
	Param.eType = LGCVar_Int16;
	Param.nInteger = 57;
	LogicSetProgramInstruction(&RunTime, 0, 6, LGCIns_CmdLoad | LGCIns_ParamConstNumber, &Param);
	
	//Call the external handler
	Param.eType = LGCVar_Int16;
	Param.nInteger = 0;
	LogicSetProgramInstruction(&RunTime, 0, 7, LGCIns_CmdExternal | LGCIns_ParamLabel, &Param);
	
	//Return to end the program
	Param.eType = LGCVar_Unspecified;
	Param.nInteger = 0;
	LogicSetProgramInstruction(&RunTime, 0, 8, LGCIns_CmdReturn | LGCIns_ParamNone, &Param);
	
	//Program 1: if param 0 == 0 then return 10 else return 5
	LogicSetProgramIOCounts(&RunTime, 1, 1, 1);
	
	//Push local variable 1 (program input) on the stack
	Param.eType = LGCVar_Int16;
	Param.nInteger = 1;
	LogicSetProgramInstruction(&RunTime, 1, 0, LGCIns_CmdLoad | LGCIns_ParamLocalVar, &Param);
	
	//Pop stack value if zero Jump to new instruction index
	Param.eType = LGCVar_Int16;
	Param.nInteger = 5;
	LogicSetProgramInstruction(&RunTime, 1, 1, LGCIns_CmdJumpZero | LGCIns_ParamLabel, &Param);
	
	//Push constant on the stack
	Param.eType = LGCVar_Int16;
	Param.nInteger = 5;
	LogicSetProgramInstruction(&RunTime, 1, 2, LGCIns_CmdLoad | LGCIns_ParamConstNumber, &Param);
	
	//Pop stack value into local variable 0 (program output)
	Param.eType = LGCVar_Int16;
	Param.nInteger = 0;
	LogicSetProgramInstruction(&RunTime, 1, 3, LGCIns_CmdStore | LGCIns_ParamLocalVar, &Param);
	
	//Return to end the program
	Param.eType = LGCVar_Unspecified;
	Param.nInteger = 0;
	LogicSetProgramInstruction(&RunTime, 1, 4, LGCIns_CmdReturn | LGCIns_ParamNone, &Param);
	
	//Push constant on the stack
	Param.eType = LGCVar_Int16;
	Param.nInteger = 10;
	LogicSetProgramInstruction(&RunTime, 1, 5, LGCIns_CmdLoad | LGCIns_ParamConstNumber, &Param);
	
	//Pop stack value into local variable 0 (program output)
	Param.eType = LGCVar_Int16;
	Param.nInteger = 0;
	LogicSetProgramInstruction(&RunTime, 1, 6, LGCIns_CmdStore | LGCIns_ParamLocalVar, &Param);
	
	//Return to end the program
	Param.eType = LGCVar_Unspecified;
	Param.nInteger = 0;
	LogicSetProgramInstruction(&RunTime, 1, 7, LGCIns_CmdReturn | LGCIns_ParamNone, &Param);
	
	//Register externals
	LogicAddExternal(&RunTime, 0, &LogicExtern, 1, 1);
	
	eResult = LogicRunProgram(&RunTime, 0);
	
	printf("Done: %d\n", eResult);
	printf("Local 0: %d %d\n", RunTime.aProgramUnits[0].aMemory[0].eType, (uint32_t)RunTime.aProgramUnits[0].aMemory[0].nInteger);
	printf("Stack Size: %d\n", RunTime.nStackTopIdx);
	printf("Stack 0: %d %d\n", RunTime.aStack[0].eType, (uint32_t)RunTime.aStack[0].nInteger);
	
	return 0;
}

eLogicReturn_t LogicExtern(sLogicVariable_t *pInputs, sLogicVariable_t *pOutputs) {
	printf("Extern Function!  Parameter = %d\n", (uint32_t)pInputs[0].nInteger);
	
	pOutputs[0].eType = LGCVar_Int32;
	pOutputs[0].nInteger = 32;
	
	return LogicSuccess;
}

bool LoadLogicProgram(const char *strFileName, sLogicRunTime_t *pRunTime) {
	FILE *fdLoad;
	sLineInfo_t Line;
	uint32_t nRead, nProgIdx = LOGIC_PROGRAMUNITS, nInstrIdx;
	bool bRetVal = true;
	
	//Open the program file
	fdLoad = fopen(strFileName, "r");
	if (fdLoad == NULL) {
		return false;
	}
	
	//Load all data from the file
	while (feof(fdLoad) == 0) {
		nRead = fread(&Line, sizeof(sLineInfo_t), 1, fdLoad);
		if (nRead != 1) { //Failed to read a record
			bRetVal = false;
			break;
		}
		
		//Process the read data
		if (Line.eType == Line_ProgUnitStart) {
			//Beginning a new program unit
			nProgIdx = Line.Data.Prog.nIndex;
			
			nInstrIdx = 0; //Start with first instruction
			
			//Create new program unit
			LogicSetProgramIOCounts(pRunTime, Line.Data.Prog.nIndex, Line.Data.Prog.nNumInputs, Line.Data.Prog.nNumOutputs);
		} else if (Line.eType == Line_InstrInfo) {
			if (nProgIdx >= LOGIC_PROGRAMUNITS) {
				bRetVal = false;
				break;
			}
			
			//Add instruction to program
			LogicSetProgramInstruction(pRunTime, nProgIdx, nInstrIdx, Line.Data.Instr.eCmd, &(Line.Data.Instr.Param));
			nInstrIdx += 1;
		} else {
			bRetVal = false;
			break;
		}
	}

	//Close the file
	fclose(fdLoad);
	
	return bRetVal;
}
