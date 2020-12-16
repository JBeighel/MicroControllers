/**	File:	SSD1675Driver.c
	Author:	J. Beighel
	Date:	12-16-2026
*/

/*****	Includes	*****/
	#include "SSD1675Driver.h"

/*****	Defines		*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	eSSD1675Return_t SSD1675ReadRegister(sSSD1675Info_t *pObj, eSSD1675Cmd_t eCmd, uint8_t nDataBytes, uint32_t *pnValue);
	
	eSSD1675Return_t SSD1675WriteRegister(sSSD1675Info_t *pObj, eSSD1675Cmd_t eCmd, uint8_t nDataBytes, uint32_t nValue);
	
	eSSD1675Return_t SSD1675ReadBlock(sSSD1675Info_t *pObj, eSSD1675Cmd_t eCmd, uint8_t nDataBytes, uint8_t *paValue);
	
	eSSD1675Return_t SSD1675WriteBlock(sSSD1675Info_t *pObj, eSSD1675Cmd_t eCmd, uint8_t nDataBytes, const uint8_t *paValue);

/*****	Functions	*****/

eSSD1675Return_t SSD1675Initialize(sSSD1675Info_t *pObj, sTimeIface_t *pTime, sGPIOIface_t *pGpio, sSPIIface_t *pSpi, GPIOID_t nPinChipSel, GPIOID_t nPinDataCmd) {
	//Setup the object
	pObj->pTime = pTime;
	pObj->pGpio = pGpio;
	pObj->pSpi = pSpi;
	pObj->nPinChipSel = nPinChipSel;
	pObj->nPinDataCmd = nPinDataCmd;
	
	//SPI must be setup elsewhere since other stuff can be on the bus
	//GPIO can be setup elsewhere, but these calls are safe
	pObj->pGpio->pfSetModeByPin(pObj->pGpio, pObj->nPinChipSel, GPIO_DigitalOutput);
	pObj->pGpio->pfSetModeByPin(pObj->pGpio, pObj->nPinDataCmd, GPIO_DigitalOutput);
	
	pObj->pGpio->pfDigitalWriteByPin(pObj->pGpio, pObj->nPinChipSel, true); //Chip not selected
	pObj->pGpio->pfDigitalWriteByPin(pObj->pGpio, pObj->nPinDataCmd, true); //Some default state
	
	//Initialize the peripheral
	//Pin BS1 set low defines 4-wire SPI interface, this is assumed to be the case
	
	//Software reset, then wait 10 ms
	
	//Set gate driver, RAM size, and panel border
	uint8_t aBuff[15];
	int nCtr;
	
	printf("Pre:\r\n");
	SSD1675ReadBlock(pObj, SSD1675Cmd_UserIDRd, 11, aBuff);
	for (nCtr = 0; nCtr < 11; nCtr++) {
		printf("UserID %02d: 0x%02x\r\n", nCtr, aBuff[nCtr]);
	}
	
	for (nCtr = 0; nCtr < 11; nCtr++) {
		aBuff[nCtr] = (uint8_t)nCtr;
	}
	SSD1675WriteBlock(pObj, SSD1675Cmd_UserIDWrReg, 11, aBuff);
	
	printf("Post:\r\n");
	SSD1675ReadBlock(pObj, SSD1675Cmd_UserIDRd, 11, aBuff);
	for (nCtr = 0; nCtr < 11; nCtr++) {
		printf("UserID %02d: 0x%02x\r\n", nCtr, aBuff[nCtr]);
	}
	
	return SSD1675_Success;
}

eSSD1675Return_t SSD1675ReadCommand(sSSD1675Info_t *pObj, eSSD1675Cmd_t eCmd, uint8_t nDataBytes, uint32_t *pnValue) {
	uint8_t nReadByte, nCtr;
	eSPIReturn_t eResult;
	eSSD1675Return_t eRetVal = SSD1675_Success;
	
	//Set CS pin low for whole exchange
	pObj->pGpio->pfDigitalWriteByPin(pObj->pGpio, pObj->nPinChipSel, false);
	eResult = pObj->pSpi->pfBeginTransfer(pObj->pSpi);
	
	//D/C pin is low for command bytes
	pObj->pGpio->pfDigitalWriteByPin(pObj->pGpio, pObj->nPinDataCmd, false);
	
	eResult = pObj->pSpi->pfTransferByte(pObj->pSpi, eCmd, &nReadByte); //Ignore byte read during command
	if (eResult != SPI_Success) {
		eRetVal = SSD1675Fail_SPIError;
	}
	
	//D/C pin is high for data bytes
	pObj->pGpio->pfDigitalWriteByPin(pObj->pGpio, pObj->nPinDataCmd, true);
	
	*pnValue = 0;
	for (nCtr = 0; nCtr < nDataBytes && eRetVal == SSD1675_Success; nCtr++) {
		eResult = pObj->pSpi->pfTransferByte(pObj->pSpi, SSD1675Cmd_NOP, &nReadByte);
		if (eResult != SPI_Success) {
			eRetVal = SSD1675Fail_SPIError;
			break;
		}
		
		*pnValue |= nReadByte << (8 * nCtr); //Readign least to most significant
	}
	
	//Clear CS pin after command
	eResult = pObj->pSpi->pfEndTransfer(pObj->pSpi);
	pObj->pGpio->pfDigitalWriteByPin(pObj->pGpio, pObj->nPinChipSel, true);
	
	return eRetVal;
}

eSSD1675Return_t SSD1675ReadBlock(sSSD1675Info_t *pObj, eSSD1675Cmd_t eCmd, uint8_t nDataBytes, uint8_t *paValue) { 
	uint8_t nReadByte, nCtr;
	eSPIReturn_t eResult;
	eSSD1675Return_t eRetVal = SSD1675_Success;
	
	//Set CS pin low for whole exchange
	pObj->pGpio->pfDigitalWriteByPin(pObj->pGpio, pObj->nPinChipSel, false);
	eResult = pObj->pSpi->pfBeginTransfer(pObj->pSpi);
	
	//D/C pin is low for command bytes
	pObj->pGpio->pfDigitalWriteByPin(pObj->pGpio, pObj->nPinDataCmd, false);
	
	eResult = pObj->pSpi->pfTransferByte(pObj->pSpi, eCmd, &nReadByte); //Ignore byte read during command
	if (eResult != SPI_Success) {
		eRetVal = SSD1675Fail_SPIError;
	}
	
	//D/C pin is high for data bytes
	pObj->pGpio->pfDigitalWriteByPin(pObj->pGpio, pObj->nPinDataCmd, true);
	
	for (nCtr = 0; nCtr < nDataBytes && eRetVal == SSD1675_Success; nCtr++) {
		eResult = pObj->pSpi->pfTransferByte(pObj->pSpi, SSD1675Cmd_NOP, &nReadByte);
		if (eResult != SPI_Success) {
			eRetVal = SSD1675Fail_SPIError;
			break;
		}
		
		paValue[nCtr] = nReadByte;
	}
	
	//Clear CS pin after command
	eResult = pObj->pSpi->pfEndTransfer(pObj->pSpi);
	pObj->pGpio->pfDigitalWriteByPin(pObj->pGpio, pObj->nPinChipSel, true);
	
	return eRetVal;
}

eSSD1675Return_t SSD1675WriteBlock(sSSD1675Info_t *pObj, eSSD1675Cmd_t eCmd, uint8_t nDataBytes, const uint8_t *paValue) { 
	uint8_t nReadByte, nCtr;
	eSPIReturn_t eResult;
	eSSD1675Return_t eRetVal = SSD1675_Success;
	
	//Set CS pin low for whole exchange
	pObj->pGpio->pfDigitalWriteByPin(pObj->pGpio, pObj->nPinChipSel, false);
	eResult = pObj->pSpi->pfBeginTransfer(pObj->pSpi);
	
	//D/C pin is low for command bytes
	pObj->pGpio->pfDigitalWriteByPin(pObj->pGpio, pObj->nPinDataCmd, false);
	
	eResult = pObj->pSpi->pfTransferByte(pObj->pSpi, eCmd, &nReadByte); //Ignore byte read during command
	if (eResult != SPI_Success) {
		eRetVal = SSD1675Fail_SPIError;
	}
	
	//D/C pin is high for data bytes
	pObj->pGpio->pfDigitalWriteByPin(pObj->pGpio, pObj->nPinDataCmd, true);
	
	for (nCtr = 0; nCtr < nDataBytes && eRetVal == SSD1675_Success; nCtr++) {
		eResult = pObj->pSpi->pfTransferByte(pObj->pSpi, paValue[nCtr], &nReadByte);
		if (eResult != SPI_Success) {
			eRetVal = SSD1675Fail_SPIError;
			break;
		}
	}
	
	//Clear CS pin after command
	eResult = pObj->pSpi->pfEndTransfer(pObj->pSpi);
	pObj->pGpio->pfDigitalWriteByPin(pObj->pGpio, pObj->nPinChipSel, true);
	
	return eRetVal;
}