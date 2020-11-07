/**	
		File:	MLX90393Driver.c
		Author:	J. Beighel
		Created:10-14-2020
*/

/*****	Includes	*****/
	#include "MLX90393Driver.h"

/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	#define MLX90393SendCommand(pDev, eCmd, nSendBytes, paSendBuff, nRecvBytes, nRecvBuff) ((pDev->pSpi != NULL) ? MLX90393SPISendCommand((pDev), (eCmd), (nSendBytes), (paSendBuff), (nRecvBytes), (nRecvBuff)) : MLX90393I2CSendCommand((pDev), (eCmd), (nSendBytes), (paSendBuff), (nRecvBytes), (nRecvBuff)))

	eMLX90393Return_t MLX90393Init(sMLX90393Dev_t *pDev);

	eMLX90393Return_t MLX90393SPISendCommand(sMLX90393Dev_t *pDev, eMLX90393Command_t eCmd, uint8_t nSendBytes, uint8_t *paSendBuff, uint8_t nRecvBytes, uint8_t *paRecvBuff);
	
	eMLX90393Return_t MLX90393I2CSendCommand(sMLX90393Dev_t *pDev, eMLX90393Command_t eCmd, uint8_t nSendBytes, uint8_t *paSendBuff, uint8_t nRecvBytes, uint8_t *paRecvBuff);
	
	eMLX90393Return_t MLX90393WriteRegister(sMLX90393Dev_t *pDev, eMLX90393MemMap_t eRegAddr, uint16_t nRegValue);

	eMLX90393Return_t MLX90393ReadRegister(sMLX90393Dev_t *pDev, eMLX90393MemMap_t eRegAddr, uint16_t *pnRegValue);

/*****	Functions	*****/
eMLX90393Return_t MLX90393SpiInit(sMLX90393Dev_t *pDev, sGPIOIface_t *pGpio, sSPIIface_t *pSpi, uint16_t nCSPin) {
	eMLX90393Return_t eRetVal = MLX90393_Success;
	uint16_t nCfgValue;
	
	
	//Setup the device object
	pDev->pGpio = pGpio;
	pDev->pSpi = pSpi;
	pDev->pI2C = NULL;
	pDev->nCSPin = nCSPin;
	pDev->nTriggerPin = GPIO_NOPIN;
	pDev->nInterruptPin = GPIO_NOPIN;
	pDev->nI2CAddr = MLX90393_None;
	
	return MLX90393Init(pDev);
}

eMLX90393Return_t MLX90393I2CInit(sMLX90393Dev_t *pDev, sGPIOIface_t *pGpio, sI2CIface_t *pI2C, eMLX90393I2CAddr_t eAddr, uint8_t nTrigPin, uint8_t nIntPin) {
	eMLX90393Return_t eRetVal = MLX90393_Success;
	uint16_t nCfgValue;
	
	
	//Setup the device object
	pDev->pGpio = pGpio;
	pDev->pSpi = NULL;
	pDev->pI2C = pI2C;
	pDev->nCSPin = GPIO_NOPIN;
	pDev->nTriggerPin = nTrigPin;
	pDev->nInterruptPin = nIntPin;
	pDev->nI2CAddr = eAddr;
	
	return MLX90393Init(pDev);
}

eMLX90393Return_t MLX90393Init(sMLX90393Dev_t *pDev) {
	eMLX90393Return_t eResult, eRetVal = MLX90393_Success;
	uint16_t nCfgValue;
	
	//Configure the select pin
	if (pDev->nCSPin != GPIO_NOPIN) {
		pDev->pGpio->pfSetModeByPin(pDev->pGpio, pDev->nCSPin, GPIO_DigitalOutput);
		pDev->pGpio->pfDigitalWriteByPin(pDev->pGpio, pDev->nCSPin, true);
	}
	
	//Configure the trigger pin
	if (pDev->nTriggerPin != GPIO_NOPIN) {
		pDev->pGpio->pfSetModeByPin(pDev->pGpio, pDev->nTriggerPin, GPIO_DigitalOutput);
		pDev->pGpio->pfDigitalWriteByPin(pDev->pGpio, pDev->nTriggerPin, false);
	}
	
	//Configure the interrupt pin
	if (pDev->nInterruptPin != GPIO_NOPIN) {
		pDev->pGpio->pfSetModeByPin(pDev->pGpio, pDev->nInterruptPin, GPIO_DigitalInput);
	}
	
	//Setup the sensor (device does not have any self identification options)
	MLX90393Reset(pDev);
	DELAYMICROSEC(5000); //Delay to let the chip start up
	
	nCfgValue = MLX90393Cfg2_BstSelX | MLX90393Cfg2_BstSelY | MLX90393Cfg2_BstSelZ | MLX90393Cfg2_BstSelT | MLX90393Cfg2_BstRateMask | MLX90393Cfg2_WOCDiff | MLX90393Cfg2_ExtTrig | MLX90393Cfg2_TCmpEn | MLX90393Cfg2_TrigInt;
	if (pDev->pSpi == NULL) {
		nCfgValue |= MLX90393Cfg2_CommI2COnly;
	} else {
		nCfgValue |= MLX90393Cfg2_CommSPIOnly;
	}
	
	eResult = MLX90393WriteRegister(pDev, MLX90393Mem_Config2, nCfgValue);
	
	eRetVal = MLX90393SendCommand(pDev, MLX90393_SingleMeasure, 0, NULL, 0, NULL);
	
	if (eRetVal == MLX90393_Success) {
		eRetVal = eResult;
	}
	
	return eRetVal;
}

eMLX90393Return_t MLX90393TakeReading(sMLX90393Dev_t *pDev, uint16_t *pnXDir, uint16_t *pnYDir, uint16_t *pnZDir, uint16_t *pnTemp) {
	eMLX90393Return_t eRetVal = MLX90393_Success;
	eRetVal = MLX90393SendCommand(pDev, MLX90393_SingleMeasure, 0, NULL, 0, NULL);
	bool bIntVal;
	uint8_t nCtr;
	
	pDev->pGpio->pfDigitalWriteByPin(pDev->pGpio, pDev->nTriggerPin, true);
	DELAYMICROSEC(1000);
	pDev->pGpio->pfDigitalWriteByPin(pDev->pGpio, pDev->nTriggerPin, false);
	
	nCtr = 0;
	pDev->pGpio->pfDigitalReadByPin(pDev->pGpio, pDev->nInterruptPin, &bIntVal);
	while ((bIntVal == false) && (nCtr < 250)) {
		DELAYMICROSEC(10);
		pDev->pGpio->pfDigitalReadByPin(pDev->pGpio, pDev->nInterruptPin, &bIntVal);
		nCtr += 1;
	}
	
	if (nCtr >= 250) {
		eRetVal = MLX90393Fail_MeasTimeout;
	}
	
	//DELAYMICROSEC(1000); //1 msec to convert reading
	eMLX90393Command_t eCmd = (eMLX90393Command_t)(MLX90393_ReadMeasure | MLX90393_XAxis | MLX90393_YAxis | MLX90393_ZAxis | MLX90393_Temp);
	uint8_t aData[8]; //2 bytes per measurement
	
	//2 byte per value in the order T, X, Y,Z
	eRetVal = MLX90393SendCommand(pDev, eCmd, 0, NULL, 8, aData);
	
	(*pnTemp) = (aData[0] << 8) | aData[1];
	(*pnXDir) = (aData[2] << 8) | aData[3];
	(*pnYDir) = (aData[4] << 8) | aData[5];
	(*pnZDir) = (aData[6] << 8) | aData[7];
	
	return eRetVal;
}

eMLX90393Return_t MLX90393Reset(sMLX90393Dev_t *pDev) {
	uint8_t nReadVal;
	
	//Send exit mode before the reset
	MLX90393SendCommand(pDev, MLX90393_ExitMode, 0, NULL, 0, NULL);
	
	if (pDev->pSpi != NULL) {
		//There's no response given, otherwise MLX90393SendCommand() would be used
		//Begin the transfer
		pDev->pGpio->pfDigitalWriteByPin(pDev->pGpio, pDev->nCSPin, false);
		pDev->pSpi->pfBeginTransfer(pDev->pSpi);
		
		//Send the reset command
		pDev->pSpi->pfTransferByte(pDev->pSpi, MLX90393_Reset, &nReadVal);
		
		//End the transfer
		pDev->pSpi->pfEndTransfer(pDev->pSpi);
		pDev->pGpio->pfDigitalWriteByPin(pDev->pGpio, pDev->nCSPin, true);
	} else {
		nReadVal = MLX90393_Reset;
		pDev->pI2C->pfI2CWriteData(pDev->pI2C, pDev->nI2CAddr, 1, &nReadVal);
	}
	
	return MLX90393_Success;
}

eMLX90393Return_t MLX90393WriteRegister(sMLX90393Dev_t *pDev, eMLX90393MemMap_t eRegAddr, uint16_t nRegValue) {
	eMLX90393Return_t eRetVal;
	uint8_t anData[3];
	uint16_t nDataCheck;
	
	anData[0] = nRegValue >> 8;
	anData[1] = nRegValue & 0x00FF;
	anData[2] = eRegAddr;
	
	eRetVal = MLX90393SendCommand(pDev, MLX90393_WriteReg, 3, anData, 0, NULL);
	DELAYMICROSEC(20);
	eRetVal = MLX90393ReadRegister(pDev, eRegAddr, &nDataCheck);
	
	if (nDataCheck != nRegValue) {
		return MLX90393Fail_DataVerify;
	} else {
		return eRetVal;
	}
}

eMLX90393Return_t MLX90393ReadRegister(sMLX90393Dev_t *pDev, eMLX90393MemMap_t eRegAddr, uint16_t *pnRegValue) {
	eMLX90393Return_t eRetVal;
	uint8_t anData[2];
	
	eRetVal = MLX90393SendCommand(pDev, MLX90393_ReadReg, 1, (uint8_t *)(&eRegAddr), 2, anData);
	
	(*pnRegValue) = ((uint16_t)anData[0]) << 8;
	(*pnRegValue) |= ((uint16_t)anData[1]);
	
	return eRetVal;
}

eMLX90393Return_t MLX90393SPISendCommand(sMLX90393Dev_t *pDev, eMLX90393Command_t eCmd, uint8_t nSendBytes, uint8_t *paSendBuff, uint8_t nRecvBytes, uint8_t *paRecvBuff) {
	uint8_t nReadVal, nCtr, nBytesWaiting;
	eMLX90393Return_t eRetVal = MLX90393_Success;
	
	//Begin the transfer
	pDev->pGpio->pfDigitalWriteByPin(pDev->pGpio, pDev->nCSPin, false);
	pDev->pSpi->pfBeginTransfer(pDev->pSpi);
	
	//Send the command
	pDev->pSpi->pfTransferByte(pDev->pSpi, eCmd, &nReadVal);
	
	//Send any data
	for (nCtr = 0; nCtr < nSendBytes && paSendBuff != NULL; nCtr++) {
		pDev->pSpi->pfTransferByte(pDev->pSpi, paSendBuff[nCtr], &nReadVal);
	}
	
	//Receive status byte
	pDev->pSpi->pfTransferByte(pDev->pSpi, MLX90393_NOP, &nReadVal);
	
	nBytesWaiting = (nReadVal & MLX90393Stat_D1) | (nReadVal & MLX90393Stat_D2);
	nBytesWaiting = (2 * nBytesWaiting) + 2;
	
	if (CheckAllBitsInMask(nReadVal, MLX90393Stat_SED) == true) {
		eRetVal = MLX90393Warn_SingleErr;
	} else if (CheckAllBitsInMask(nReadVal, MLX90393Stat_Error) == true) {
		eRetVal = MLX90393Fail_DeviceErr;
	}
	
	//Receive any data
	if (paRecvBuff == NULL) {
		nRecvBytes = 0;
	}
	
	for (nCtr = 0; nCtr < nRecvBytes; nCtr++) {
		pDev->pSpi->pfTransferByte(pDev->pSpi, MLX90393_NOP, &(paRecvBuff[nCtr]));
	}
	
	//Read any bytes waiting that the calling function ignored
	if ((eCmd == MLX90393_ReadReg) || (eCmd == MLX90393_ReadMeasure)) {
		for (nCtr = nRecvBytes; nCtr < nBytesWaiting; nCtr++) {
			pDev->pSpi->pfTransferByte(pDev->pSpi, MLX90393_NOP, &nReadVal);
		}
	}
	
	//End the transfer
	pDev->pSpi->pfEndTransfer(pDev->pSpi);
	pDev->pGpio->pfDigitalWriteByPin(pDev->pGpio, pDev->nCSPin, true);
	
	return eRetVal;
}

eMLX90393Return_t MLX90393I2CSendCommand(sMLX90393Dev_t *pDev, eMLX90393Command_t eCmd, uint8_t nSendBytes, uint8_t *paSendBuff, uint8_t nRecvBytes, uint8_t *paRecvBuff) {
	uint8_t nCtr, nStatus, nBytesRead, aData[9];
	eMLX90393Return_t eRetVal = MLX90393_Success;
	eI2CReturns_t eReturn;
	
	//Send command and data together
	aData[0] = eCmd;
	if ((nSendBytes > 0) && (paSendBuff != NULL)) {
		for (nCtr = 0; nCtr < nSendBytes; nCtr++) {
			aData[1 + nCtr] = paSendBuff[nCtr];
		}
		
		nSendBytes += 1; //Add 1 for the command byte
	} else {
		nSendBytes = 1; //Send 1 command byte
	}
	
	eReturn = pDev->pI2C->pfI2CWriteData(pDev->pI2C, pDev->nI2CAddr, nSendBytes, aData);
	if ((eReturn != I2C_Success) && (eRetVal == MLX90393_Success)) {
		eRetVal = MLX90393Fail_BusError;
	}
	
	//Read all expected data
	if ((nRecvBytes > 0) && (paRecvBuff != NULL)) {
		nRecvBytes += 1; //Add 1 for the status byte
	} else {
		nRecvBytes = 1; //Receive 1 status byte
	}

	eReturn = pDev->pI2C->pfI2CReadData(pDev->pI2C, pDev->nI2CAddr, nRecvBytes, aData, &nStatus);
	
	if ((nStatus != nRecvBytes) && (eRetVal == MLX90393_Success)) {
		eRetVal = MLX90393Warn_PartialData;
	}
	
	if ((eReturn != I2C_Success) && (eRetVal == MLX90393_Success)) {
		eRetVal = MLX90393Fail_BusError;
	}
	
	//Pull out status byte
	nStatus = aData[0];
	
	if (CheckAllBitsInMask(nStatus, MLX90393Stat_SED) == true) {
		eRetVal = MLX90393Warn_SingleErr;
	} else if (CheckAllBitsInMask(nStatus, MLX90393Stat_Error) == true) {
		eRetVal = MLX90393Fail_DeviceErr;
	}
	
	for (nCtr = 1; nCtr < nRecvBytes; nCtr++) {
		paRecvBuff[nCtr - 1] = aData[nCtr];
	}
	
	return eRetVal;
}