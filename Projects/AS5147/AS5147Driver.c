/**	
		File:	AS5147Driver.c
		Author:	J. Beighel
		Created:09-28-2020
*/

/*****	Includes	*****/
	#include "AS5147Driver.h"

/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/

	/**	@brief		Read a value from a register in the AS5147 device
		@ingroup	as5147driver
	*/
	eAS5147Return_t AS5147RegRead(sAS5147Dev_t *pDev, eAS5147Registers_t eReg, uint16_t *pnRegValue);

	/**	@brief		Write a value to a register in the AS5147 device
		@ingroup	as5147driver
	*/
	eAS5147Return_t AS5147RegWrite(sAS5147Dev_t *pDev, eAS5147Registers_t eReg, uint16_t nRegValue);

/*****	Functions	*****/
eAS5147Return_t AS5147Init(sAS5147Dev_t *pDev, sGPIOIface_t *pGpio, sSPIIface_t *pSpi, uint16_t nCSPin) {
	//Setup the device object
	pDev->pGpio = pGpio;
	pDev->pSpi = pSpi;
	pDev->nCSPin = nCSPin;
	
	//Configure the device
	pDev->pGpio->pfSetModeByPin(pDev->pGpio, pDev->nCSPin, GPIO_DigitalOutput);
	pDev->pGpio->pfDigitalWriteByPin(pDev->pGpio, pDev->nCSPin, true);
	
	uint16_t nRegVal;
	AS5147RegRead(pDev, AS5147Reg_SETTINGS1, &nRegVal);
	
	if (nRegVal != 1) { //Default value is one, so it should be that?
		return AS5147Fail_Unknown;
	}
	
	return AS5147_Success;
}

eAS5147Return_t AS5147TakeReading(sAS5147Dev_t *pDev, uint16_t *pnAngleUnc, uint16_t *pnAngleCom, uint16_t *pnMag, uint16_t *pnAGC) {
	uint16_t nReadVal;
	
	//Read all the values from the device
	AS5147RegRead(pDev, AS5147Reg_ANGLEUNC, pnAngleUnc);
	AS5147RegRead(pDev, AS5147Reg_ANGLECOM, pnAngleCom);
	AS5147RegRead(pDev, AS5147Reg_MAG, pnMag);
	AS5147RegRead(pDev, AS5147Reg_DIAAGC, pnAGC);
	
	//Strip off all extraneous bits
	(*pnAngleUnc) &= AS5147Reg_DataMask;
	(*pnAngleCom) &= AS5147Reg_DataMask;
	(*pnMag) &= AS5147Reg_DataMask;
	(*pnAGC) &= AS5147RegDiag_AGCMask;
	
	return AS5147_Success;
}

eAS5147Return_t AS5147RegRead(sAS5147Dev_t *pDev, eAS5147Registers_t eReg, uint16_t *pnRegValue) {
	uint16_t nParity, nCmd;
	uint8_t nCmdFrame[2], nReadVal[2];
	eAS5147Return_t eRetVal = AS5147_Success;
	
	nCmd = eReg & AS5147Reg_DataMask;
	nCmd |= AS5147Reg_ReadBit;
	
	nParity = CountSetBitsInInt16(nCmd);
	if ((nParity % 2) == 1) {
		nCmd |= AS5147Reg_ParitySet;
	} else {
		nCmd |= AS5147Reg_ParityClear;
	}
	
	nCmdFrame[0] = nCmd >> 8;
	nCmdFrame[1] = nCmd & 0xFF;
	
	pDev->pGpio->pfDigitalWriteByPin(pDev->pGpio, pDev->nCSPin, false);
	pDev->pSpi->pfBeginTransfer(pDev->pSpi);
	
	//Send the read request (The read data is unknown and discardable)
	pDev->pSpi->pfTransferByte(pDev->pSpi, nCmdFrame[0], &(nReadVal[0]));
	pDev->pSpi->pfTransferByte(pDev->pSpi, nCmdFrame[1], &(nReadVal[1]));
	pDev->pGpio->pfDigitalWriteByPin(pDev->pGpio, pDev->nCSPin, true); //After every frame flip the CS Pin
	pDev->pGpio->pfDigitalWriteByPin(pDev->pGpio, pDev->nCSPin, false);
	
	//Read back the data (send nop to just read data)
	nCmd = AS5147Reg_NOP  & AS5147Reg_DataMask;
	nCmd |= AS5147Reg_ReadBit;
	
	nParity = CountSetBitsInInt16(nCmd);
	if ((nParity % 2) == 1) {
		nCmd |= AS5147Reg_ParitySet;
	} else {
		nCmd |= AS5147Reg_ParityClear;
	}
	
	nCmdFrame[0] = nCmd >> 8;
	nCmdFrame[1] = nCmd & 0xFF;
	
	pDev->pSpi->pfTransferByte(pDev->pSpi, nCmdFrame[0], &(nReadVal[0]));
	pDev->pSpi->pfTransferByte(pDev->pSpi, nCmdFrame[1], &(nReadVal[1]));
	
	pDev->pSpi->pfEndTransfer(pDev->pSpi);
	pDev->pGpio->pfDigitalWriteByPin(pDev->pGpio, pDev->nCSPin, true);
	
	(*pnRegValue) = nReadVal[1];
	(*pnRegValue) |= nReadVal[0] << 8;
	
	nParity = CountSetBitsInInt16(*pnRegValue);
	if ((nParity % 2) == 1) {
		eRetVal = AS5147Warn_Parity;
	}
	
	//Extract the data from the framing
	(*pnRegValue) &= AS5147Reg_DataMask;
	
	return eRetVal;
}

eAS5147Return_t AS5147RegWrite(sAS5147Dev_t *pDev, eAS5147Registers_t eReg, uint16_t nRegValue) {
	uint16_t nParity, nCmd;
	uint8_t nCmdFrame[2], nReadVal[2];
	
	nCmd = eReg & AS5147Reg_DataMask;
	nCmd |= AS5147Reg_WriteBit;
	
	nParity = CountSetBitsInInt16(nCmd);
	if ((nParity % 2) == 1) {
		nCmd |= AS5147Reg_ParitySet;
	} else {
		nCmd |= AS5147Reg_ParityClear;
	}
	
	nCmdFrame[0] = nCmd >> 8;
	nCmdFrame[1] = nCmd & 0xFF;
	
	pDev->pGpio->pfDigitalWriteByPin(pDev->pGpio, pDev->nCSPin, false);
	pDev->pSpi->pfBeginTransfer(pDev->pSpi);
	
	//Send the write request (The read data is unknown and discardable)
	pDev->pSpi->pfTransferByte(pDev->pSpi, nCmdFrame[0], &(nReadVal[0]));
	pDev->pSpi->pfTransferByte(pDev->pSpi, nCmdFrame[1], &(nReadVal[1]));
	pDev->pGpio->pfDigitalWriteByPin(pDev->pGpio, pDev->nCSPin, true); //After every frame flip the CS Pin
	pDev->pGpio->pfDigitalWriteByPin(pDev->pGpio, pDev->nCSPin, false);
	
	//Send over the data to put in the register (the read data echo's the address to write)
	nCmd = nRegValue & AS5147Reg_DataMask;
	nCmd|= AS5147Reg_DataBit;
	
	nParity = CountSetBitsInInt16(nCmd);
	if ((nParity % 2) == 1) {
		nCmd |= AS5147Reg_ParitySet;
	} else {
		nCmd |= AS5147Reg_ParityClear;
	}
	
	nCmdFrame[0] = nCmd >> 8;
	nCmdFrame[1] = nCmd & 0xFF;
	
	pDev->pSpi->pfTransferByte(pDev->pSpi, nCmdFrame[0], &(nReadVal[0]));
	pDev->pSpi->pfTransferByte(pDev->pSpi, nCmdFrame[1], &(nReadVal[1]));
	
	pDev->pSpi->pfEndTransfer(pDev->pSpi);
	pDev->pGpio->pfDigitalWriteByPin(pDev->pGpio, pDev->nCSPin, true);
	
	//Can send a nop and teh device will echo back the data sent for confirmation
	
	return AS5147_Success;
}
