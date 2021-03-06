/*	#File Information
		File:	I2C_NucleoL412KB.c
		Author:	J. Beighel
		Date:	2021-03-01
*/

/*****	Includes	*****/
	#include "I2C_NucleoL412KB.h"

/*****	Constants	*****/


/*****	Definitions		*****/


/*****	Globals			*****/


/*****	Prototypes		*****/

	
/*****	Function Code	*****/

eI2CReturn_t NucleoI2CInitialize(sI2CIface_t *pI2CIface, bool bActAsMaster, uint32_t nClockFreq, void *pHWInfo) {
	//pHWInfo is I2C_HandleTypeDef *
	
	//Ensure a base sanity in the object
	I2CInterfaceInitialize(pI2CIface);

	//Verify the requested configuration
	if (bActAsMaster != true) { //Not supporting slave operation just yet
		return I2C_Fail_Unsupported;
	}
	
	if ((nClockFreq != 100000) && (nClockFreq != 1000000) && (nClockFreq != 3400000)) {
		//Anything outside of this the nucleo libraries don't support
		return I2C_Fail_Unsupported;
	}
	
	//The Cube will configure the hardware interface
	
	//Copy parameters into I@C Objects
	pI2CIface->bMaster = bActAsMaster;
	pI2CIface->nClockFreq = nClockFreq; //Cube is setting this, so hope it matches?
	pI2CIface->pHWInfo = pHWInfo;
	
	//Setup function pointers
	pI2CIface->pfInitialize = &NucleoI2CInitialize;
	pI2CIface->pfI2CReadUint8Reg = &NucleoI2CReadUint8Reg;
	pI2CIface->pfI2CReadData = &NucleoI2CReadData;
	pI2CIface->pfI2CWriteUint8Reg = &NucleoI2CWriteUint8Reg;
	pI2CIface->pfI2CWriteData = &NucleoI2CWriteData;
	pI2CIface->pfI2CGeneralCall = &NucleoI2CGeneralCall;
	
	return I2C_Success;
}
	
eI2CReturn_t NucleoI2CReadUint8Reg (sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nRegAddr, uint8_t *pnValue) {
	//pHWInfo is I2C_HandleTypeDef *

	nDevAddr <<= 1; //Values are set to have the address by itself, reposition to add space for read/write bit
	//Write the register to read from
	HAL_I2C_Master_Transmit((I2C_HandleTypeDef *)pI2CIface->pHWInfo, nDevAddr, &nRegAddr, 1, I2C_TIMEOUT); //Send the address

	//Read the value
	HAL_I2C_Master_Receive((I2C_HandleTypeDef *)pI2CIface->pHWInfo, nDevAddr, pnValue, 1, I2C_TIMEOUT);

	return I2C_Success;
}

eI2CReturn_t NucleoI2CReadData (sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nNumBytes, void *pDataBuff, uint8_t *pnBytesRead) {
	HAL_StatusTypeDef nResult;

	(*pnBytesRead) = 0; //Set a default return value

	nDevAddr <<= 1; //Values are set to have the address by itself, reposition to add space for read/write bit
	
	nResult = HAL_I2C_Master_Receive((I2C_HandleTypeDef *)pI2CIface->pHWInfo, nDevAddr, pDataBuff, nNumBytes, I2C_TIMEOUT);
	if (nResult != HAL_OK) {
		//HAL_ERROR, HAL_BUSY, or HAL_TIMEOUT
		return I2C_Fail_Unknown; //Only unknown because I'm not handling the actual result
	}

	return I2C_Success;
}

eI2CReturn_t NucleoI2CWriteUint8Reg (sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nRegAddr, uint8_t nValue) {
	HAL_StatusTypeDef nResult;
	uint8_t aBytes[2];
	
	nDevAddr <<= 1; //Values are set to have the address by itself, reposition to add space for read/write bit

	aBytes[0] = nRegAddr;
	aBytes[1] = nValue;

	nResult = HAL_I2C_Master_Transmit((I2C_HandleTypeDef *)pI2CIface->pHWInfo, nDevAddr, aBytes, 2, I2C_TIMEOUT);

	if (nResult != HAL_OK) {
		//HAL_ERROR, HAL_BUSY, or HAL_TIMEOUT
		return I2C_Fail_Unknown; //Only unknown because I'm not handling the actual result
	}
	
	return I2C_Success;
}

eI2CReturn_t NucleoI2CWriteData (sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nNumBytes, void *pDataBuff) {
	HAL_StatusTypeDef nResult;

	nDevAddr <<= 1; //Values are set to have the address by itself, reposition to add space for read/write bit
	nResult = HAL_I2C_Master_Transmit((I2C_HandleTypeDef *)pI2CIface->pHWInfo, nDevAddr, pDataBuff, nNumBytes, I2C_TIMEOUT);

	if (nResult != HAL_OK) {
		//HAL_ERROR, HAL_BUSY, or HAL_TIMEOUT
		return I2C_Fail_Unknown; //Only unknown because I'm not handling the actual result
	}

	return I2C_Success;
}

eI2CReturn_t NucleoI2CGeneralCall (sI2CIface_t *pI2CIface, uint8_t nValue) {
	HAL_I2C_Master_Transmit((I2C_HandleTypeDef *)pI2CIface->pHWInfo, I2C_GENERALCALLADDR, &nValue, 1, I2C_TIMEOUT);

	return I2C_Success;
}
