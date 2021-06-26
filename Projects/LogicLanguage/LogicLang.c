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
	
	//Push constant 5 on the stack
	Param.eType = LGCVar_Int16;
	Param.nInteger = 5;
	LogicSetProgramInstruction(&RunTime, 0, 0, LGCIns_CmdLoad | LGCIns_ParamConstNumber, &Param);
	
	//Pop stack value (5) into local variabble 0
	Param.eType = LGCVar_Int16;
	Param.nInteger = 0;
	LogicSetProgramInstruction(&RunTime, 0, 1, LGCIns_CmdStore | LGCIns_ParamLocalVar, &Param);
	
	//Push local variable 0 on the stack
	Param.eType = LGCVar_Int16;
	Param.nInteger = 0;
	LogicSetProgramInstruction(&RunTime, 0, 2, LGCIns_CmdLoad | LGCIns_ParamLocalVar, &Param);
	
	//Pop stack value and add it to local 0
	Param.eType = LGCVar_Int16;
	Param.nInteger = 0;
	LogicSetProgramInstruction(&RunTime, 0, 3, LGCIns_CmdAdd | LGCIns_ParamLocalVar, &Param);
	
	//Return to end the program
	Param.eType = LGCVar_Unspecified;
	Param.nInteger = 0;
	LogicSetProgramInstruction(&RunTime, 0, 3, LGCIns_CmdReturn | LGCIns_ParamConstNumber, &Param);
	
	eResult = LogicRunProgram(&RunTime, 0);
	
	printf("Done: %d\n", eResult);
	printf("Local 0: %d\n", RunTime.aProgramUnits[0].aMemory[0].nInteger);
	printf("Stack 0: %d %d\n", RunTime.aStack[0].eType, RunTime.aStack[0].nInteger);
	
	return 0;
}