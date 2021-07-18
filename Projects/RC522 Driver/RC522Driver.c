/*
		File:	RC522Driver.c
		Author:	J. Beighel
		Created:2021-07-22
*/

/*****	Includes	*****/
	#include "RC522Driver.h"

/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	eRC522Return_t RC522ReadRegister(sRC522Obj_t *pRC522, uint8_t nRegAddr, uint8_t *pnRegVal);

	eRC522Return_t RC522WriteRegister(sRC522Obj_t *pRC522, uint8_t nRegAddr, uint8_t nRegVal);

	eRC522Return_t RC522InitDevice(sRC522Obj_t *pRC522);

	eRC522Return_t RC522AntennaEnable(sRC522Obj_t *pRC522, bool bTurnOn);

/*****	Functions	*****/
eRC522Return_t RC522InitializeSPI(sRC522Obj_t *pRC522, sSPIIface_t *pSpiObj, sGPIOIface_t *pGPIOObj, uint16_t nChipSelectPin, GPIOID_t nResetPin) {
	eRC522Return_t eReturn;
	uint8_t nRegVal;

	pRC522->pGPIO = pGPIOObj;
	pRC522->pSpi = pSpiObj;
	pRC522->pI2C = NULL;
	pRC522->nCSPin = nChipSelectPin;
	pRC522->nRstPin = nResetPin;

	pRC522->pGPIO->pfDigitalWriteByPin(pRC522->pGPIO, pRC522->nRstPin, true); //Positive edge to power on
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

	return RC522InitDevice(pRC522);
}

eRC522Return_t RC522InitializeI2C(sRC522Obj_t *pRC522, sI2CIface_t *pI2CObj, sGPIOIface_t *pGPIOObj, eRC522Addr_t eAddrPins, GPIOID_t nResetPin) {
	eI2CReturn_t eReturn;
	uint8_t nRegVal;

	pRC522->pGPIO = pGPIOObj;
	pRC522->pSpi = NULL;
	pRC522->pI2C = pI2CObj;
	pRC522->nCSPin = 0;
	pRC522->nRstPin = nResetPin;
	pRC522->nI2CAddr = RC522AddrBase | eAddrPins;

	pRC522->pGPIO->pfDigitalWriteByPin(pRC522->pGPIO, pRC522->nRstPin, true); //Positive edge to power on
	eReturn = pRC522->pI2C->pfI2CReadUint8Reg(pRC522->pI2C, pRC522->nI2CAddr, RC522Reg_Version, &nRegVal);
	if (eReturn != I2C_Success) {
		return RC522Fail_BusFailure;
	}

	if ((nRegVal & RC522RegVer_ChipMask) != RC522RegVer_ChipMFRC522) {
		//Not identified as the correct part
		return RC522Fail_NotFound;
	}

	return RC522InitDevice(pRC522);
}

eRC522Return_t RC522ReadRegister(sRC522Obj_t *pRC522, uint8_t nRegAddr, uint8_t *pnRegVal) {
	if (pRC522->pSpi != NULL) {
		//If I2C/UART the register address is unchanged, for SPI some extra framing is needed
		nRegAddr = RC522Reg_SPIRead | (nRegAddr << RC522Reg_SPILShift);

		pRC522->pGPIO->pfDigitalWriteByPin(pRC522->pGPIO, pRC522->nCSPin, false);
		pRC522->pSpi->pfBeginTransfer(pRC522->pSpi);

		pRC522->pSpi->pfTransferByte(pRC522->pSpi, nRegAddr, pnRegVal);
		pRC522->pSpi->pfTransferByte(pRC522->pSpi, 0x00, pnRegVal); //The device continually reads registers until it gets 0x00

		pRC522->pSpi->pfEndTransfer(pRC522->pSpi);
		pRC522->pGPIO->pfDigitalWriteByPin(pRC522->pGPIO, pRC522->nCSPin, true);
	} else if (pRC522->pI2C != NULL) {
		pRC522->pI2C->pfI2CReadUint8Reg(pRC522->pI2C, pRC522->nI2CAddr, nRegAddr, pnRegVal);
	} else {
		return RC522Fail_BusFailure;
	}

	return RC522_Success;
}

eRC522Return_t RC522WriteRegister(sRC522Obj_t *pRC522, uint8_t nRegAddr, uint8_t nRegVal) {
	uint8_t nReadByte;

	if (pRC522->pSpi != NULL) {
		//If I2C/UART the register address is unchanged, for SPI some extra framing is needed
		nRegAddr = RC522Reg_SPIWrite | (nRegAddr << RC522Reg_SPILShift);

		pRC522->pGPIO->pfDigitalWriteByPin(pRC522->pGPIO, pRC522->nCSPin, false);
		pRC522->pSpi->pfBeginTransfer(pRC522->pSpi);

		pRC522->pSpi->pfTransferByte(pRC522->pSpi, nRegAddr, &nReadByte);
		pRC522->pSpi->pfTransferByte(pRC522->pSpi, nRegVal, &nReadByte);
		pRC522->pSpi->pfTransferByte(pRC522->pSpi, 0x00, &nReadByte); //The device continually reads registers until it gets 0x00

		pRC522->pSpi->pfEndTransfer(pRC522->pSpi);
		pRC522->pGPIO->pfDigitalWriteByPin(pRC522->pGPIO, pRC522->nCSPin, true);

		if (nReadByte != nRegVal) {
			return RC522Fail_WriteVerify;
		}
	} else if (pRC522->pI2C != NULL) {
		pRC522->pI2C->pfI2CWriteUint8Reg(pRC522->pI2C, pRC522->nI2CAddr, nRegAddr, nRegVal);
	} else {
		return RC522Fail_BusFailure;
	}

	return RC522_Success;
}

eRC522Return_t RC522InitDevice(sRC522Obj_t *pRC522) {
	//Set Tx and Rx mode registers to 0x00
	RC522WriteRegister(pRC522, RC522Reg_TxMode, RC522RegTxRxMode_106kBd);
	RC522WriteRegister(pRC522, RC522Reg_TxMode, RC522RegTxRxMode_106kBd);

	RC522WriteRegister(pRC522, RC522Reg_ModWidth, 0x26); //Sets the modulation width

	// When communicating with a PICC we need a timeout if something goes wrong.
	// Timer = 13.56 MHz / (2 * PreScaler + 1)
	// TPrescaler_Hi are four low bits in TModeReg. TPrescaler_Lo is TPrescalerReg.
	RC522WriteRegister(pRC522, RC522Reg_TMode, RC522RegTMode_Auto); //0x0A9 = 169 => Timer=40kHz, or period of 25μs.
	RC522WriteRegister(pRC522, RC522Reg_TPrescaler, 0xA9);

	// Reload timer with 0x3E8 = 1000, 25ms before timeout
	RC522WriteRegister(pRC522, RC522Reg_TReloadMSB, 0x03);
	RC522WriteRegister(pRC522, RC522Reg_TReloadLSB, 0xE8);

	RC522WriteRegister(pRC522, RC522Reg_TxASK, RC522RegTxASK_Force100ASK);

	RC522WriteRegister(pRC522, RC522Reg_Mode, RC522RegMode_TxWaitRF | RC522RegMode_PolMFIn | RC522RegMode_CRCPre6363); //0x29 instead of 0x3D

	return RC522AntennaEnable(pRC522, true);
}

eRC522Return_t RC522AntennaEnable(sRC522Obj_t *pRC522, bool bTurnOn) {
	uint8_t nRegVal;

	RC522ReadRegister(pRC522, RC522Reg_TxControl, &nRegVal);

	if (bTurnOn == true) {
		nRegVal |= 0x03;
	} else {
		nRegVal &= ~0x03;
	}

	return RC522WriteRegister(pRC522, RC522Reg_TxControl, nRegVal);
}
