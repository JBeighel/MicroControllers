/**	File:	AppMain.c
	Author:	J. Beighel
	Date:	2021-03-10
*/

/*****	Includes	*****/
	#include "AppMain.h"

/*****	Defines		*****/


/*****	Definitions	*****/
	#define PATTERN_LEN		(sizeof(manPattern) / sizeof(uint32_t))

	#define I2CSLAVE_ADDR	0x5A

/*****	Constants	*****/
	const uint32_t manPattern[] = {
		APA102_DimBlue,
		APA102_DimWhite,
		APA102_DimBlue,
		APA102_Black,
		APA102_Black,
		APA102_Black,
		APA102_Black,
		APA102_DimGreen,
		APA102_DimWhite,
		APA102_DimGreen,
		APA102_Black,
		APA102_Black,
		APA102_Black,
		APA102_Black,
		APA102_DimRed,
		APA102_DimWhite,
		APA102_DimRed,
		APA102_Black,
		APA102_Black,
		APA102_Black,
		APA102_Black,
	};

	typedef enum eTaskID_t {
		Task_Main,
		Task_Terminal,
		Task_I2C,
	} eTaskID_t;

/*****	Globals		*****/
	sDNPMsgBuffer_t gDNPBuild, gDNPParse;
	sDNPDataValue_t gDNPValue;
	sTerminal_t gTerminal;

	uint8_t gaI2CData[16];

/*****	Prototypes 	*****/
void TerminalTask(void *pParams);
void I2CTask(void *pParams);
void CycleLEDColors(void);
eReturn_t TerminalCommandHandler(sTerminal_t *pTerminal, const char *pCmd);

eI2CReturn_t I2CSlaveAddrHandler(sI2CIface_t *pI2CIface, eI2CSlaveDirection_t eDirect, uint16_t nAddrMatch);
eI2CReturn_t I2CSlaveRecvHandler(sI2CIface_t *pI2CIface);
eI2CReturn_t I2CSlaveSendHandler(sI2CIface_t *pI2CIface);
eI2CReturn_t I2CSlaveTransHandler(sI2CIface_t *pI2CIface);

/*****	Functions	*****/

void BootstrapTask(void const * argument) {
	sIOConnect_t IOObject;
	uint32_t nUsed;

	//Initialize the processor
	PinSetup();

	//Initialize the peripherals
	BoardSetup();

	//Starts the output compare timer in interrupt mode
	gTime.pfInterruptStart(TIMEINT_2_HWINFO);

	IOCnctCreateFromUART(&gUart1, &IOObject);
	TerminalInitialize(&gTerminal, &IOObject);

	SysModeInitialize(&gTime);

	DBInitialize();

	gTerminal.pfAddCmdHandler(&gTerminal, &TerminalCommandHandler);

	xTaskCreate(&TerminalTask, "UART Terminal", 512, (void *)Task_Terminal, osPriorityAboveNormal, NULL);
	xTaskCreate(&I2CTask, "I2C Controller", 256, (void *)Task_I2C, osPriorityAboveNormal, NULL);

	//Begin the application
	SysModeTransitionMode(SysMode_Normal);

	while (1) {
		//Toggle the single LED
		gGpioB.pfDigitalWriteByPin(&gGpioB, GPO_B00_0_Pin, true);

		CycleLEDColors();
		gTime.pfWatchdogRefresh();
		osDelay(250);
		CycleLEDColors();
		gTime.pfWatchdogRefresh();
		osDelay(250);

		gGpioB.pfDigitalWriteByPin(&gGpioB, GPO_B00_0_Pin, false);

		CycleLEDColors();
		gTime.pfWatchdogRefresh();
		osDelay(250);
		CycleLEDColors();
		gTime.pfWatchdogRefresh();
		osDelay(250);

		//DNP Testing stuff
		DNPBufferNewMessage(&gDNPBuild);
		gDNPBuild.nDestAddr = 0xFFFC;
		gDNPBuild.nSourceAddr = 0x1234;
		gDNPBuild.eDataControl = DNPData_Direction | DNPData_Primary | DNPData_UnconfirmData;

		/*
		gDNPBuild.eControlCode = DNPCtrl_Read;
		DNPBuilderAddDataObjectRequest(&gDNPBuild, DNPGrp_BinaryInput, 2, 0, 0);
		*/

		/*
		gDNPBuild.eControlCode = DNPCtrl_DirectOperate;
		DNPBuilderAddBinaryOutputCommandDataObject(&gDNPBuild, 0x01, 0, DNPBinOutCtrl_PulseOn, 1, 500, 0, 0);
		*/

		/*
		gDNPBuild.eControlCode = DNPCtrl_Response;
		uint8_t aFlags[] = {0x80, 0x81, 0x81, 0x80};
		DNPBuilderAddBinaryInputDataObject(&gDNPBuild, 1, 4, aFlags, true, 500);
		*/

		gDNPBuild.eControlCode = DNPCtrl_Response;
		DNPBuilderAddDeviceAttributeValue(&gDNPBuild, DNPDevAttr_Manufacturer, "Blah");

		DNPBuilderGenerateDNP(&gDNPBuild);
		gTime.pfWatchdogRefresh();

		DNPBufferNewMessage(&gDNPParse);
		DNPParserReceivedData(&gDNPParse, gDNPBuild.aDNPMessage, 0, gDNPBuild.nDNPMsgLen, &nUsed);

		DNPParserNextDataObject(&gDNPParse);
		DNPParserNextDataValue(&gDNPParse, &gDNPValue);
		DNPParserNextDataValue(&gDNPParse, &gDNPValue);
		DNPParserNextDataValue(&gDNPParse, &gDNPValue);
		DNPParserNextDataValue(&gDNPParse, &gDNPValue);
		DNPParserNextDataValue(&gDNPParse, &gDNPValue);

		//TerminalCommandHandler(&gTerminal, " aaabbbccc a*(b+)(c(c)c?) ");

		//System verification stuff goes last
		SysModeTaskCheckIn(Task_Main);
		SysModeSystemVerify();
	}

	return;
}

void TerminalTask(void *pParams) {
	SysModeTaskID_t eTaskID = (SysModeTaskID_t)pParams;
	gTerminal.pfWriteTextLine(&gTerminal, "Hello");

	while (1) {
		SysModeTaskCheckIn(eTaskID);
		gTerminal.pfReadInput(&gTerminal);
		gTime.pfDelayMilliSeconds(1);
	}

	return; //Shouldn't reach here
}

void I2CTask(void *pParams) {
	SysModeTaskID_t eTaskID = (SysModeTaskID_t)pParams;
	uint8_t aData[] = { 0xB6, 0x6B };
	uint8_t nBytesRead;

	//gI2C1 this is the master
	//gI2C3 this is the slave

	gI2C3.pfSlaveSetAddrHandler(&gI2C3, &I2CSlaveAddrHandler);
	gI2C3.pfSlaveSetRecvCompHandler(&gI2C3, &I2CSlaveRecvHandler);
	gI2C3.pfSlaveSetSendCompHandler(&gI2C3, &I2CSlaveSendHandler);
	gI2C3.pfSlaveSetTransCompHandler(&gI2C3, &I2CSlaveTransHandler);
	gI2C3.pfSlaveListenEnable(&gI2C3, I2CSLAVE_ADDR, true);


	while (1) {
		SysModeTaskCheckIn(eTaskID);
		gTime.pfDelayMilliSeconds(500);


		//gI2C1.pfI2CReadData(&gI2C1, I2CSLAVE_ADDR, 2, gaI2CData, &nBytesRead);
		//gI2C1.pfI2CWriteData(&gI2C1, I2CSLAVE_ADDR, 1, aData);
		gI2C1.pfI2CGeneralCall(&gI2C1, aData[0]);
		//gTime.pfDelayMilliSeconds(50);
		//gI2C3.pfSlaveListenEnable(&gI2C3, I2CSLAVE_ADDR, true);
	}

	return; //Shouldn't reach here
}

eI2CReturn_t I2CSlaveAddrHandler(sI2CIface_t *pI2CIface, eI2CSlaveDirection_t eDirect, uint16_t nAddrMatch) {
	gaI2CData[0] = 0xA5;
	gaI2CData[1] = 0x5A;

	//Heard slave address from the master, check what the master wants to
	//do next and queue that action up
	if (eDirect == I2CSlave_MasterReceive) {
		pI2CIface->pfSlaveSendData(pI2CIface, 1, gaI2CData);
	} else if (eDirect == I2CSlave_MasterTransmit) {
		pI2CIface->pfSlaveRecvData(pI2CIface, 2, gaI2CData);
	}

	return Success;
}

eI2CReturn_t I2CSlaveRecvHandler(sI2CIface_t *pI2CIface) {
	//gaI2CData[0] = 0x84; //Modifying the buffer will overwrite what has read over the bus
	//gaI2CData[1] = 0x48;

	//Expected data was read, prepare to receive more?
	pI2CIface->pfSlaveRecvData(pI2CIface, 1, gaI2CData);

	return Success;
}

eI2CReturn_t I2CSlaveSendHandler(sI2CIface_t *pI2CIface) {
	gaI2CData[0] = 0xC3;
	gaI2CData[1] = 0x3C;

	//Queued data was sent, queue up more?
	pI2CIface->pfSlaveSendData(pI2CIface, 1, gaI2CData);

	return Success;
}

eI2CReturn_t I2CSlaveTransHandler(sI2CIface_t *pI2CIface) {
	//Master has ended the conversation
	//Go back to listening for more transactions
	pI2CIface->pfSlaveListenEnable(pI2CIface, I2CSLAVE_ADDR, true);

	return Success;
}

void CycleLEDColors(void) {
	static uint8_t nPatternStart = 0;
	uint8_t nLightCtr, nPatternCtr;

	//Cycle the colored LEDs
	nPatternCtr = nPatternStart;
	for (nLightCtr = 0; nLightCtr < 5; nLightCtr++) {
		APA102SetLightColor(&gLEDString, nLightCtr, manPattern[nPatternCtr]);

		nPatternCtr += 1;
		if (nPatternCtr >= PATTERN_LEN) {
			nPatternCtr = 0;
		}
	}
	APA102UpdateLights(&gLEDString);

	nPatternStart += 1;
	if (nPatternStart >= PATTERN_LEN) {
		nPatternStart = 0;
	}

	return;
}

eReturn_t TerminalCommandHandler(sTerminal_t *pTerminal, const char *pCmd) {
	char strInput[TERMINAL_BUFFERSIZE];
	char strCmd[40], strParam[40];
	char *pStart;
	uint32_t nIdx;
	sRegExResult_t RXResult;

	//Make a copy to avoid hurting the original
	strcpy(strInput, pCmd);
	pStart = strInput;

	StrTrimStart(pStart, " \t\r\n");
	StrTrimEnd(pStart, " \t\r\n");

	//Pull out the command word
	nIdx = StrFindFirstChar(pStart, " \t\r\n");
	if (nIdx == UINT32_MAXVALUE) {
		nIdx = strlen(pStart);
	}

	StrPiece(pStart, 0, nIdx, strCmd); //Copy out the word

	pStart = &pStart[nIdx]; //Advance past that word
	StrTrimStart(pStart, " \t\r\n");

	//Pull out the parameter
	nIdx = StrFindFirstChar(pStart, " \t\r\n");
	if (nIdx == UINT32_MAXVALUE) {
		nIdx = strlen(pStart);
	}

	StrPiece(pStart, 0, nIdx, strParam); //Copy out the word

	StrRegEx(strCmd, strParam, RX_IgnoreCase, &RXResult);

	snprintf(strInput, TERMINAL_BUFFERSIZE, "Source: %s", strCmd);
	gTerminal.pfWriteTextLine(&gTerminal, strInput);
	snprintf(strInput, TERMINAL_BUFFERSIZE, "Reg Ex: %s", strParam);
	gTerminal.pfWriteTextLine(&gTerminal, strInput);
	snprintf(strInput, TERMINAL_BUFFERSIZE, "Result: %u Start: %lu Length: %lu", RXResult.eResult, RXResult.nStart, RXResult.nLength);
	gTerminal.pfWriteTextLine(&gTerminal, strInput);

	return Success;
}
