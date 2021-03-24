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
	sTerminal_t gTerminal;

/*****	Prototypes 	*****/
void TerminalTask(void *pParams);
void CycleLEDColors(void);

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
		gDNPBuild.eControlCode = DNPCtrl_Read;
		gDNPBuild.eDataControl = DNPData_Direction | DNPData_Primary | DNPData_UnconfirmData;
		DNPBuilderAddDataObjectRequest(&gDNPBuild, DNPGrp_BinaryInput, 2, 0, 0);
		DNPBuilderGenerateDNP(&gDNPBuild);
		gTime.pfWatchdogRefresh();

		DNPBufferNewMessage(&gDNPParse);
		DNPParserReceivedData(&gDNPParse, gDNPBuild.aDNPMessage, 0, gDNPBuild.nDNPMsgLen, &nUsed);

		DNPParserNextDataObject(&gDNPParse);

		//gTerminal.pfWriteTextLine(&gTerminal, "56");
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
