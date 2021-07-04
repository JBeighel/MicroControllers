/**	File:	LogicLang.c
	Author:	J. Beighel
	Date:	2021-06-24
*/

/*****	Includes	*****/
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
	
	//Return to end the program
	Param.eType = LGCVar_Unspecified;
	Param.nInteger = 0;
	LogicSetProgramInstruction(&RunTime, 0, 6, LGCIns_CmdReturn | LGCIns_ParamNone, &Param);
	
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
	
	eResult = LogicRunProgram(&RunTime, 0);
	
	printf("Done: %d\n", eResult);
	printf("Local 0: %d %d\n", RunTime.aProgramUnits[0].aMemory[0].eType, RunTime.aProgramUnits[0].aMemory[0].nInteger);
	printf("Stack Size: %d\n", RunTime.nStackTopIdx);
	printf("Stack 0: %d %d\n", RunTime.aStack[0].eType, RunTime.aStack[0].nInteger);
	
	return 0;
}