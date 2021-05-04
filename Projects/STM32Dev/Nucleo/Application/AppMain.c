/**	File:	AppMain.c
	Author:	J. Beighel
	Date:	2021-03-10
*/

/*****	Includes	*****/
	#include "AppMain.h"

/*****	Defines		*****/


/*****	Definitions	*****/
	#define PATTERN_LEN		(sizeof(manPattern) / sizeof(uint32_t))

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

/*****	Globals		*****/
	sDNPMsgBuffer_t gDNPBuild, gDNPParse;
	sDNPDataValue_t gDNPValue;
	sTerminal_t gTerminal;

/*****	Prototypes 	*****/
void TerminalTask(void *pParams);
void CycleLEDColors(void);
eReturn_t TerminalCommandHandler(sTerminal_t *pTerminal, const char *pCmd);

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

	gTerminal.pfAddCmdHandler(&gTerminal, &TerminalCommandHandler);

	xTaskCreate(&TerminalTask, "UART Terminal", 512, NULL, osPriorityAboveNormal, NULL);

	//Begin the application
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
	}

	return;
}

void TerminalTask(void *pParams) {
	gTerminal.pfWriteTextLine(&gTerminal, "Hello");

	while (1) {
		gTerminal.pfReadInput(&gTerminal);
		gTime.pfDelayMilliSeconds(1);
	}

	return; //Shouldn't reach here
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
