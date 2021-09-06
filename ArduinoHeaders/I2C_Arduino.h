/**	@defgroup i2ciface_arduino
	@brief
	@details	v0.2
	# Intent #


	# Design #
	
	
	# File Info #
		File:	I2C_Arduino.h
		Author:	J. Beighel
		Date:	2021-09-05
*/

/**	@defgroup i2ciface_arduino_priv : Private Objects
	@brief
	@ingroup i2ciface_arduino
*/

#ifndef __I2CIFACE_ARDUINO
	#define __I2CIFACE_ARDUINO

/*****	Includes	*****/
	#include <Wire.h>

	#include "I2CGeneralInterface.h"

/*****	Constants	*****/
	#define		I2C_1_HWINFO	(&Wire)
	
	#define		I2C_1_CAPS		(eI2CCapabilities_t)(I2CCap_ReadUint8Reg | I2CCap_ReadData | I2CCap_WriteUint8Reg | I2CCap_WriteData | I2CCap_GeneralCall | I2CCap_Shutdown)
	
	#define		I2C_INIT		ArduinoI2CInitialize

	#define		BUILD_DEBUG		1
	#ifdef BUILD_DEBUG
		#define	DEBUG_PRINT	Serial.print
	#endif

/*****	Definitions		*****/


/*****	Globals			*****/


/*****	Prototypes		*****/
	eI2CReturn_t ArduinoI2CInitialize(sI2CIface_t *pI2CIface, bool bActAsMaster, uint32_t nClockFreq, void *pHWInfo);
	
	eI2CReturn_t ArduinoI2CShutdownPort (sI2CIface_t *pI2CIface);
	
	eI2CReturn_t ArduinoI2CReadUint8Reg (sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nRegAddr, uint8_t *pnValue);

	eI2CReturn_t ArduinoI2CReadData (sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nNumBytes, void *pDataBuff, uint8_t *pnBytesRead);

	eI2CReturn_t ArduinoI2CWriteUint8Reg (sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nRegAddr, uint8_t nValue);

	eI2CReturn_t ArduinoI2CWriteData (sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nNumBytes, void *pDataBuff);

	eI2CReturn_t ArduinoI2CGeneralCall (sI2CIface_t *pI2CIface, uint8_t nValue);
	
/*****	Function Code	*****/

eI2CReturn_t ArduinoI2CInitialize(sI2CIface_t *pI2CIface, bool bActAsMaster, uint32_t nClockFreq, void *pHWInfo) {
	TwoWire *pI2CBus = (TwoWire *)pHWInfo;
	
	//Verify the requested configuration
	if (bActAsMaster != true) { //Not supporting slave operation just yet
		return I2C_Fail_Unsupported;
	}
	
	if ((nClockFreq != 100000) && (nClockFreq != 1000000) && (nClockFreq != 3400000)) {
		//Anything outside of this the arduino liraries don't support
		return I2C_Fail_Unsupported;
	}
	
	//Configure the hardware interface
	pI2CBus->begin();
	pI2CBus->setClock(nClockFreq);
	
	//Copy parameters into I@C Objects
	pI2CIface->bMaster = bActAsMaster;
	pI2CIface->nClockFreq = nClockFreq;
	pI2CIface->pHWInfo = pHWInfo;
	
	//Setup function pointers
	pI2CIface->pfInitialize = &ArduinoI2CInitialize;
	pI2CIface->pfShutdown = &ArduinoI2CShutdownPort;
	pI2CIface->pfI2CReadUint8Reg = &ArduinoI2CReadUint8Reg;
	pI2CIface->pfI2CReadData = &ArduinoI2CReadData;
	pI2CIface->pfI2CWriteUint8Reg = &ArduinoI2CWriteUint8Reg;
	pI2CIface->pfI2CWriteData = &ArduinoI2CWriteData;
	pI2CIface->pfI2CGeneralCall = &ArduinoI2CGeneralCall;
	
	pI2CIface->eCapabilities = I2C_1_CAPS;
	
	return I2C_Success;
}
	
eI2CReturn_t ArduinoI2CShutdownPort (sI2CIface_t *pI2CIface) {
	//Arduino does not require any shutdown
	return I2C_Success;
}

eI2CReturn_t ArduinoI2CReadUint8Reg (sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nRegAddr, uint8_t *pnValue) {
	uint8_t nResult;
	TwoWire *pI2CDevice = (TwoWire *)pI2CIface->pHWInfo;

	//Write the register to read from
	pI2CDevice->beginTransmission(nDevAddr);
	pI2CDevice->write(nRegAddr);
	nResult = pI2CDevice->endTransmission();
	
	if (nResult != 0) {
		switch (nResult) {
			case 1:
				return I2C_Fail_WriteBuffOver;
			case 2:
				return I2C_Fail_NackAddr;
			case 3:
				return I2C_Fail_NackData;
			case 4:
			default:
				return I2C_Fail_Unknown;
		}
	}

	//Read the value
	pI2CDevice->requestFrom(nDevAddr, (size_t)1); //This function assumes registers have 1 byte
	(*pnValue) = pI2CDevice->read();

	return I2C_Success;
}

eI2CReturn_t ArduinoI2CReadData (sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nNumBytes, void *pDataBuff, uint8_t *pnBytesRead) {
	uint8_t nCtr, nRead;
	uint8_t *pData = (uint8_t *)pDataBuff;
	TwoWire *pI2CDevice = (TwoWire *)pI2CIface->pHWInfo;
	
	nRead = pI2CDevice->requestFrom(nDevAddr, (size_t)nNumBytes);

	(*pnBytesRead) = 0;
	for (nCtr = 0; nCtr < nNumBytes && pI2CDevice->available(); nCtr++) {
		pData[nCtr] = pI2CDevice->read();
		(*pnBytesRead)++;
	}
	
	if ((*pnBytesRead) < nNumBytes) {
		return I2C_Warn_PartialRead;
	} else {
		return I2C_Success;
	}
}

eI2CReturn_t ArduinoI2CWriteUint8Reg (sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nRegAddr, uint8_t nValue) {
	uint8_t nResult;
	TwoWire *pI2CDevice = (TwoWire *)pI2CIface->pHWInfo;
	
	pI2CDevice->beginTransmission(nDevAddr);
	pI2CDevice->write(nRegAddr);
	pI2CDevice->write(nValue);
	nResult = pI2CDevice->endTransmission();
	
	if (nResult == 0) {
		return I2C_Success;
	} else {
		switch (nResult) {
			case 1:
				return I2C_Fail_WriteBuffOver;
			case 2:
				return I2C_Fail_NackAddr;
			case 3:
				return I2C_Fail_NackData;
			case 4:
			default:
				return I2C_Fail_Unknown;
		}
	}
}

eI2CReturn_t ArduinoI2CWriteData (sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nNumBytes, void *pDataBuff) {
	uint8_t nCtr;
	uint8_t *pData = (uint8_t *)pDataBuff;
	TwoWire *pI2CDevice = (TwoWire *)pI2CIface->pHWInfo;
	
	pI2CDevice->beginTransmission(nDevAddr);
	
	for (nCtr = 0; nCtr < nNumBytes; nCtr++) {
		pI2CDevice->write(pData[nCtr]);
	}
	
	pI2CDevice->endTransmission();
	
	return I2C_Success;
}

eI2CReturn_t ArduinoI2CGeneralCall (sI2CIface_t *pI2CIface, uint8_t nValue) {
	TwoWire *pI2CDevice = (TwoWire *)pI2CIface->pHWInfo;
	
	pI2CDevice->beginTransmission(I2C_GENERALCALLADDR);
	pI2CDevice->write(nValue);
	pI2CDevice->endTransmission();
	
	return I2C_Success;
}

#endif
