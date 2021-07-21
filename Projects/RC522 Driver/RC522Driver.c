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
	
	/**	@brief		Issue a command to the peripheral and await completion
		@param		pRC522			Pointer to RC522 device object to use
		@param		eCmd			The command to tel lthe device to carry out
		@param		eDoneIRQ		Mask of any IRQ bits that indicate the command is complete
		@param		pSendData		Pointer to buffer of data bytes to write to FIFO
		@param		nSendDataLen	Number of data bytes to write to the FIFO
		@param		pReadData		Buffer to store data read from FIFO (null to skip FIFO read)
		@param		pnReadDataLen	Number of bytes of space in pReadData buffer (zero to skip
			FIFO read).  Also used to return number of bytes read from FIFO
		@param		bCheckCRC		Perform a CRC check on the data received by the device
		@return		RC522_Success on successful completion, or an error code describing the 
			failure detected
		@ingroup	rc522driver
	*/
	eRC522Return_t RC522PerformCommand(sRC522Obt_t *pRC522, eRC522RegCmd_t eCmd, eRC522RegComIrq_t eDoneIRQ, uint8_t *pSendData, uint8_t nSendDataLen, uint8_t *pReadData, uint8_t *pnReadDataLen, bool bCheckCRC);

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

eRC522Return_t RC522WriteRegisterData(sRC522Obj_t *pRC522, uint8_t nRegAddr, uint8_t nDataLen, uint8_t *pData) {
	uint8_t nReadByte, nCtr;

	if (pRC522->pSpi != NULL) {
		//If I2C/UART the register address is unchanged, for SPI some extra framing is needed
		nRegAddr = RC522Reg_SPIWrite | (nRegAddr << RC522Reg_SPILShift);

		pRC522->pGPIO->pfDigitalWriteByPin(pRC522->pGPIO, pRC522->nCSPin, false);
		pRC522->pSpi->pfBeginTransfer(pRC522->pSpi);

		//Write the address
		pRC522->pSpi->pfTransferByte(pRC522->pSpi, nRegAddr, &nReadByte);
		
		//Write all the data bytes
		for (nCtr = 0; nCtr < nDataLen; nCtr++) {
			pRC522->pSpi->pfTransferByte(pRC522->pSpi, pData[nCtr], &nReadByte);
		}

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

	RC522WriteRegister(pRC522, RC522Reg_ModWidth, RC522_DEFMODWIDTH); //Sets the modulation width

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
		nRegVal |= RC522RegTxCtrl_Tx2RFEn | RC522RegTxCtrl_Tx1RFEn;
	} else {
		nRegVal &= ~(RC522RegTxCtrl_Tx2RFEn | RC522RegTxCtrl_Tx1RFEn);
	}

	return RC522WriteRegister(pRC522, RC522Reg_TxControl, nRegVal);
}

eRC522Return_t RC522IsCardPresent(sRC522Obj_t *pRC522, bool *pbCardDetected) {
	uint8_t aATQABuff[2];
	
	
	//Reset baud rates and mod width
	RC522WriteRegister(pRC522, RC522Reg_TxMode, RC522RegTxRxMode_106kBd);
	RC522WriteRegister(pRC522, RC522Reg_RxMode, RC522RegTxRxMode_106kBd);
	
	RC522WriteRegister(pRC522, RC522Reg_ModWidth, RC522_DEFMODWIDTH);
	
	
}

eRC522Return_t RC522PerformCommand(sRC522Obt_t *pRC522, eRC522RegCmd_t eCmd, eRC522RegComIrq_t eDoneIRQ, uint8_t *pSendData, uint8_t nSendDataLen, uint8_t *pReadData, uint8_t *pnReadDataLen, bool bCheckCRC) {
	int16_t nCtr, nCRC;
	uint8_t nReadVal;
	
	//Cancel any current commands
	RC522WriteRegister(pRC522, RC522Reg_Command, RC522RegCmd_CmdIdle);
	//Clear all interrupts so they can detect command completion
	RC522WriteRegister(pRC522, RC522Reg_ComIrq, RC522RegComIrq_AllIrq);
	//Clear the FIFO for command data
	RC522WriteRegister(pRC522, RC522Reg_FIFOLevel, RC522RegFIFOLvl_FlushBuff);
	//Write the requested data into the FIFO
	RC522WriteRegisterData(pRC522, RC522Reg_FIFOData, nSendDataLen, pSendData);
	//Clear out any bit alignment
	RC522WriteRegister(pRC522, RC522Reg_BitFraming, RC522RegBitFrm_TxRx8Bits);
	
	//Everything is reset, issue the command
	RC522WriteRegister(pRC522, RC522Reg_Command, eCmd);
	if (eCmd == RC522RegCmd_CmdTransceive) { //Make sure transceive starts
		RC522WriteRegisterData(pRC522, RC522Reg_BitFraming, RC522RegBitFrm_StartSend);
	}
	
	//Now wait until the command to complete
	//Timer is set to start after transmission
	for (nCtr = 0; nCtr < 1000; nCtr++) {
		RC522ReadRegister(pRC522, RC522Reg_ComIrq, &nReadVal);
		
		if (CheckAnyBitsInMask(eDoneIRQ, nReadVal) == true) {
			//At least 1 of the done IRQ flags is set
			break;
		}
	}
	
	if (nCtr >= 1000) { //Command never completed? 
		if (pReadData != null) {
			*pReadData = 0;
		}
		return RC522Fail_Timeout;
	}
	
	//Check if the device detected any errors
	RC522ReadRegister(pRC522, RC522Reg_Error, &nReadVal);
	if (nReadVal != 0) { //Buffer overflow, parity, or protocol indicate real failures
		if (pReadData != null) {
			*pReadData = 0;
		}
		return RC522Fail_DeviceError;
	}
	
	//if data was requested, retrieve it
	if ((pReadData != NULL) && (nReadDataLen > 0)) {
		RC522ReadRegister(pRC522, RC522Reg_FIFOLevel, &nReadVal);
		nReadVal &= RC522RegFIFOLvl_LvlMask; //Get just the used space
		
		if (nReadVal > *nReadDataLen) {
			return RC522Fail_BuffSize;
		}
		
		*pReadVal = nReadVal;
		
		//Read all bytes from the FIFO
		for (nCtr = 0; nCtr < nReadVal; nCtr++) {
			RC522ReadRegister(pRC522, RC522Reg_FIFOData, &(pReadData[nCtr]));
		}
		
		RC522ReadRegister(pRC522, RC522Reg_Control, &nReadVal);
		nReadVal &= RC522RegCtrl_RxLastBits; //Get just the number of bits in the last byte
		//Not sure how to use this, but maybe its helpful?
	}
	
	//If requested, do a CRC check
	if (bCheckCRC == true) {
		//Stop any active commands
		RC522WriteRegister(pRC522, RC522Reg_Command, RC522RegCmd_CmdIdle);
		//Clear CRC interrupts to detect command completion
		RC522WriteRegister(pRC522, RC522Reg_DivIrq, RC522RegDivIrq_CRC);
		
		//Write the data into the fifo (Last two data bytes are the CRC)
		RC522WriteRegister(pRC522, RC522Reg_FIFOLevel, RC522RegFIFOLvl_FlushBuff);
		RC522WriteRegisterData(pRC522, RC522Reg_FIFOData, (*pReadVal) - 2, pReadData);
		
		//Have the device CRC the FIFO contents
		RC522WriteRegister(pRC522, RC522Reg_Command, RC522RegCmd_CmdCalcCRC);
		
		//Wait for the command to complete
		for (nCtr = 0; nCtr < 1000; nCtr++) {
			RC522ReadRegister(pRC522, RC522Reg_DivIrq, &nReadVal);
			
			if (CheckAllBitsInMask(RC522RegDivIrq_CRC, nReadVal) == true) {
				//CRC is complete
				break;
			}
		}
		
		if (nCtr >= 1000) { //Command never completed? 
			return RC522Fail_Timeout;
		}
		
		//Be sure it won't start another CRC
		RC522WriteRegister(pRC522, RC522Reg_Command, RC522RegCmd_CmdIdle);
		
		//Get the calculated CRC
		RC522ReadRegister(pRC522, RC522Reg_CRCMSB, &nReadVal);
		nCRC = nReadVal << 8;
		RC522ReadRegister(pRC522, RC522Reg_CRCLSB, &nReadVal);
		nCRC |= nReadVal;
		
		//Get the CRC from the received data
		nCtr = pReadData[(*pReadVal) - 2] << 8;
		nCtr |= pReadData[(*pReadVal) - 1];
		
		if (nCtr != nCRC) {
			return RC522Fail_CRC;
		}
	}
	
	return RC522_Success;
}