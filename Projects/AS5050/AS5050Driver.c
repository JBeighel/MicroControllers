/**	
		File:	AS5147Driver.c
		Author:	J. Beighel
		Created:09-28-2020
*/

/*****	Includes	*****/
	#include "AS5050Driver.h"

/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/

	/**	@brief		Read a value from a register in the AS5050 device
		@ingroup	as5050driver
	*/
	eAS5050Return_t AS5050RegRead(sAS5050Dev_t *pDev, eAS5050Registers_t eReg, uint16_t *pnRegValue);

	/**	@brief		Write a value to a register in the AS5050 device
		@ingroup	as5050driver
	*/
	eAS5050Return_t AS5050RegWrite(sAS5050Dev_t *pDev, eAS5050Registers_t eReg, uint16_t nRegValue);

/*****	Functions	*****/
eAS5050Return_t AS5050Init(sAS5050Dev_t *pDev, sGPIOIface_t *pGpio, sSPIIface_t *pSpi, uint16_t nCSPin) {
	//Setup the device object
	pDev->pGpio = pGpio;
	pDev->pSpi = pSpi;
	pDev->nCSPin = nCSPin;
	
	//Configure the device
	pDev->pGpio->pfSetModeByPin(pDev->pGpio, pDev->nCSPin, GPIO_DigitalOutput);
	pDev->pGpio->pfDigitalWriteByPin(pDev->pGpio, pDev->nCSPin, true);
	
	uint16_t nRegVal;
	AS5050RegRead(pDev, AS5050Reg_SysConfig, &nRegVal);
	
	if (nRegVal != 1) { //Default value is one, so it should be that?
		return AS5050Fail_Unknown;
	}
	
	return AS5050_Success;
}

eAS5050Return_t AS5050TakeReading(sAS5050Dev_t *pDev, uint16_t *pnAngle, uint16_t *pnAGC) {
	uint16_t nReadVal;
	eAS5050Return_t eRetVal = AS5050_Success;
	
	//Read all the values from the device
	if (AS5050RegRead(pDev, AS5050Reg_Angle, pnAngle) == AS5050Warn_DevError) {
		AS5050RegRead(pDev, AS5050Reg_ErrStatus, &pDev->nLastErr);
		AS5050RegRead(pDev, AS5050Reg_ClearEF, &nReadVal);
		eRetVal = AS5050Warn_DevError;
	}

	if (AS5050RegRead(pDev, AS5050Reg_AGC, pnAGC) == AS5050Warn_DevError) {
		AS5050RegRead(pDev, AS5050Reg_ErrStatus, &pDev->nLastErr);
		AS5050RegRead(pDev, AS5050Reg_ClearEF, &nReadVal);
		eRetVal = AS5050Warn_DevError;
	}
	
	//Strip off all extraneous bits
	(*pnAngle) &= AS5050Reg_DataMask;
	(*pnAGC) &= AS5050RegAGC_Mask;
	
	return eRetVal;
}

eAS5050Return_t AS5050RegRead(sAS5050Dev_t *pDev, eAS5050Registers_t eReg, uint16_t *pnRegValue) {
	uint16_t nParity, nCmd;
	uint8_t nCmdFrame[2], nReadVal[2];
	eAS5050Return_t eRetVal = AS5050_Success;
	
	nCmd = (eReg << AS5050Reg_CmdAddrLShift) & AS5050Reg_CmdAddrMask;
	nCmd |= AS5050Reg_ReadBit;
	
	nParity = CountSetBitsInInt16(nCmd);
	if ((nParity % 2) == 1) {
		nCmd |= AS5050Reg_ParitySet;
	} else {
		nCmd |= AS5050Reg_ParityClear;
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
	nCmd = (AS5050Reg_NOP << AS5050Reg_CmdAddrLShift)  & AS5050Reg_CmdAddrMask;
	nCmd |= AS5050Reg_ReadBit;
	
	nParity = CountSetBitsInInt16(nCmd);
	if ((nParity % 2) == 1) {
		nCmd |= AS5050Reg_ParitySet;
	} else {
		nCmd |= AS5050Reg_ParityClear;
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
		eRetVal = AS5050Warn_Parity;
	}
	
	if (CheckAllBitsInMask((*pnRegValue), AS5050Reg_ErrFlag) == true) { //Device reporting an error
		eRetVal = AS5050Warn_DevError;
	}
	
	//Extract the data from the framing
	(*pnRegValue) &= AS5050Reg_DataMask;
	(*pnRegValue) >>= AS5050Reg_DataShift;
	
	return eRetVal;
}

eAS5050Return_t AS5050RegWrite(sAS5050Dev_t *pDev, eAS5050Registers_t eReg, uint16_t nRegValue) {
	uint16_t nParity, nCmd;
	uint8_t nCmdFrame[2], nReadVal[2];
	
	nCmd = (eReg << AS5050Reg_CmdAddrLShift) & AS5050Reg_CmdAddrMask;
	nCmd |= AS5050Reg_WriteBit;
	
	nParity = CountSetBitsInInt16(nCmd);
	if ((nParity % 2) == 1) {
		nCmd |= AS5050Reg_ParitySet;
	} else {
		nCmd |= AS5050Reg_ParityClear;
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
	nCmd = (nRegValue << AS5050Reg_DataShift) & AS5050Reg_DataMask;
	nCmd|= AS5050Reg_DataBit;
	
	nParity = CountSetBitsInInt16(nCmd);
	if ((nParity % 2) == 1) {
		nCmd |= AS5050Reg_ParitySet;
	} else {
		nCmd |= AS5050Reg_ParityClear;
	}
	
	nCmdFrame[0] = nCmd >> 8;
	nCmdFrame[1] = nCmd & 0xFF;
	
	pDev->pSpi->pfTransferByte(pDev->pSpi, nCmdFrame[0], &(nReadVal[0]));
	pDev->pSpi->pfTransferByte(pDev->pSpi, nCmdFrame[1], &(nReadVal[1]));
	
	pDev->pSpi->pfEndTransfer(pDev->pSpi);
	pDev->pGpio->pfDigitalWriteByPin(pDev->pGpio, pDev->nCSPin, true);
	
	//Can send a nop and teh device will echo back the data sent for confirmation
	
	return AS5050_Success;
}
