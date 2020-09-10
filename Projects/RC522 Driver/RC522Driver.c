/*
		File:	RC522Driver.c
		Author:	J. Beighel
		Created:09-04-2020
*/

/*****	Includes	*****/
	#include "RC522Driver.h"

/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	eRC522Return_t RC522ReadRegister(sRC522Obj_t *pRC522, uint8_t nRegAddr, uint8_t *pnRegVal);

	eRC522Return_t RC522WriteRegister(sRC522Obj_t *pRC522, uint8_t nRegAddr, uint8_t nRegVal);

/*****	Functions	*****/
eRC522Return_t RC522Initialize(sRC522Obj_t *pRC522, sSPIIface_t *pSpiObj, sGPIOIface_t *pGPIOObj, uint16_t nChipSelectPin) {
	eRC522Return_t eReturn;
	uint8_t nRegVal;

	pRC522->pGPIO = pGPIOObj;
	pRC522->pSpi = pSpiObj;
	pRC522->nCSPin = nChipSelectPin;

	pRC522->pGPIO->pfDigitalWriteByPin(pRC522->pGPIO, pRC522->nCSPin, true); //Chip select is normally high

	//Check the device version
	eReturn = RC522ReadRegister(pRC522, RC522Reg_Version, &nRegVal);

	if (eReturn != RC522_Success) { //SPI communications failure
		return eReturn;
	}

	if ((nRegVal & RC522RegVer_ChipMask) != RC522RegVer_ChipMFRC522) {
		//Not identified as the correct part
		return RC522Fail_NotFound;
	}

	return RC522_Success;
}

eRC522Return_t RC522ReadRegister(sRC522Obj_t *pRC522, uint8_t nRegAddr, uint8_t *pnRegVal) {
	uint8_t aData[2], aRecv[2];

	//If I2C/UART the register address is unchanged, for SPI some extra framing is needed
	nRegAddr = RC522Reg_SPIRead | (nRegAddr << RC522Reg_SPILShift);

	aData[0] = nRegAddr;
	aData[1] = 0x00; //The device continually reads registers until it gets 0x00

	pRC522->pGPIO->pfDigitalWriteByPin(pRC522->pGPIO, pRC522->nCSPin, false);
	pRC522->pSpi->pfBeginTransfer(pRC522->pSpi);

	pRC522->pSpi->pfTransfer2Bytes(pRC522->pSpi, aData, aRecv);
	*pnRegVal = aRecv[1]; //The data received with the first address is disregarded

	//pRC522->pSpi->pfTransferByte(pRC522->pSpi, nRegAddr, pnRegVal);
	//pRC522->pSpi->pfTransferByte(pRC522->pSpi, 0x00, pnRegVal); //The device continually reads registers until it gets 0x00

	pRC522->pSpi->pfEndTransfer(pRC522->pSpi);
	pRC522->pGPIO->pfDigitalWriteByPin(pRC522->pGPIO, pRC522->nCSPin, true);

	return RC522_Success;
}

eRC522Return_t RC522WriteRegister(sRC522Obj_t *pRC522, uint8_t nRegAddr, uint8_t nRegVal) {
	uint8_t nReadByte;

	//If I2C/UART the register address is unchanged, for SPI some extra framing is needed
	nRegAddr = RC522Reg_SPIWrite | (nRegAddr << RC522Reg_SPILShift);

	pRC522->pGPIO->pfDigitalWriteByPin(pRC522->pGPIO, pRC522->nCSPin, false);
	pRC522->pSpi->pfBeginTransfer(pRC522->pSpi);

	pRC522->pSpi->pfTransferByte(pRC522->pSpi, nRegAddr, &nReadByte);
	pRC522->pSpi->pfTransferByte(pRC522->pSpi, nRegVal, &nReadByte);
	pRC522->pSpi->pfTransferByte(pRC522->pSpi, 0x00, &nReadByte); //The device continually reads registers until it gets 0x00

	pRC522->pSpi->pfEndTransfer(pRC522->pSpi);
	pRC522->pGPIO->pfDigitalWriteByPin(pRC522->pGPIO, pRC522->nCSPin, true);

	return RC522_Success;
}
