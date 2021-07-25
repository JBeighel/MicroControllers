/*	#File Information
		File:	I2C_NucleoL412KB.c
		Author:	J. Beighel
		Date:	2021-05-10
*/

/*****	Includes	*****/
	#include "I2C_NucleoL412KB.h"

/*****	Constants	*****/


/*****	Definitions		*****/


/*****	Globals			*****/
	sSTI2CInfo_t gSTI2C1 = {
		.pHWInfo = &hi2c1,
		.pfAddrHandler = NULL,
		.pfRecvCompHandler = NULL,
		.pfSendCompHandler = NULL,
		.pfTransCompHandler = NULL,
		.pfInitFunc = &MX_I2C1_Init,
		.pIFace = NULL,
	};

	sSTI2CInfo_t gSTI2C3 = {
		.pHWInfo = &hi2c3,
		.pfAddrHandler = NULL,
		.pfRecvCompHandler = NULL,
		.pfSendCompHandler = NULL,
		.pfTransCompHandler = NULL,
		.pfInitFunc = &MX_I2C3_Init,
		.pIFace = NULL,
	};

/*****	Prototypes		*****/

	
/*****	Function Code	*****/

eI2CReturn_t NucleoI2CInitialize(sI2CIface_t *pI2CIface, bool bActAsMaster, uint32_t nClockFreq, void *pHWInfo) {
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
	
	pI2CIface->pfSlaveListenEnable = NucleoI2CSlaveListenEnable;
	pI2CIface->pfSlaveSetAddrHandler = NucleoI2CSlaveSetAddrHandler;
	pI2CIface->pfSlaveSetRecvCompHandler = NucleoI2CSlaveSetRecvCompHandler;
	pI2CIface->pfSlaveSetSendCompHandler = NucleoI2CSlaveSetSendCompHandler;
	pI2CIface->pfSlaveSetTransCompHandler = NucleoI2CSlaveSetTransCompHandler;
	pI2CIface->pfSlaveSendData = NucleoI2CSlaveSendData;
	pI2CIface->pfSlaveRecvData = NucleoI2CSlaveRecvData;

	pI2CIface->eCapabilities = I2C_1_CAPS; //All hardware has the same capabilities
	
	((sSTI2CInfo_t *)pHWInfo)->pIFace = pI2CIface;

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
	I2C_HandleTypeDef *pSTI2C = ((sSTI2CInfo_t *)pI2CIface->pHWInfo)->pHWInfo;

	(*pnBytesRead) = 0; //Set a default return value

	nDevAddr <<= 1; //Values are set to have the address by itself, reposition to add space for read/write bit
	
	nResult = HAL_I2C_Master_Receive(pSTI2C, nDevAddr, pDataBuff, nNumBytes, I2C_TIMEOUT);
	if (nResult != HAL_OK) {
		//HAL_ERROR, HAL_BUSY, or HAL_TIMEOUT
		return I2C_Fail_Unknown; //Only unknown because I'm not handling the actual result
	}

	*pnBytesRead = nNumBytes;
	return I2C_Success;
}

eI2CReturn_t NucleoI2CWriteUint8Reg (sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nRegAddr, uint8_t nValue) {
	HAL_StatusTypeDef nResult;
	uint8_t aBytes[2];
	I2C_HandleTypeDef *pSTI2C = ((sSTI2CInfo_t *)pI2CIface->pHWInfo)->pHWInfo;
	
	nDevAddr <<= 1; //Values are set to have the address by itself, reposition to add space for read/write bit

	aBytes[0] = nRegAddr;
	aBytes[1] = nValue;

	nResult = HAL_I2C_Master_Transmit(pSTI2C, nDevAddr, aBytes, 2, I2C_TIMEOUT);

	if (nResult != HAL_OK) {
		//HAL_ERROR, HAL_BUSY, or HAL_TIMEOUT
		return I2C_Fail_Unknown; //Only unknown because I'm not handling the actual result
	}
	
	return I2C_Success;
}

eI2CReturn_t NucleoI2CWriteData (sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nNumBytes, void *pDataBuff) {
	HAL_StatusTypeDef nResult;
	I2C_HandleTypeDef *pSTI2C = ((sSTI2CInfo_t *)pI2CIface->pHWInfo)->pHWInfo;

	nDevAddr <<= 1; //Values are set to have the address by itself, reposition to add space for read/write bit
	nResult = HAL_I2C_Master_Transmit(pSTI2C, nDevAddr, pDataBuff, nNumBytes, I2C_TIMEOUT);

	if (nResult != HAL_OK) {
		//HAL_ERROR, HAL_BUSY, or HAL_TIMEOUT
		return I2C_Fail_Unknown; //Only unknown because I'm not handling the actual result
	}

	return I2C_Success;
}

eI2CReturn_t NucleoI2CGeneralCall (sI2CIface_t *pI2CIface, uint8_t nValue) {
	I2C_HandleTypeDef *pSTI2C = ((sSTI2CInfo_t *)pI2CIface->pHWInfo)->pHWInfo;

	HAL_I2C_Master_Transmit(pSTI2C, I2C_GENERALCALLADDR, &nValue, 1, I2C_TIMEOUT);

	return I2C_Success;
}

eI2CReturn_t NucleoI2CSlaveListenEnable(sI2CIface_t *pI2CIface, uint8_t nAddr, bool bEnable) {
	HAL_StatusTypeDef nResult;
	I2C_HandleTypeDef *pSTI2C = ((sSTI2CInfo_t *)pI2CIface->pHWInfo)->pHWInfo;

	pI2CIface->nSlaveAddress = nAddr;
	pI2CIface->bMaster = false;

	if (bEnable == true) {
		if (pI2CIface->nSlaveAddress != nAddr) {
			pSTI2C->Init.OwnAddress1 = nAddr;

			HAL_I2C_DeInit(pSTI2C);
			((sSTI2CInfo_t *)pI2CIface->pHWInfo)->pfInitFunc();
		}

		nResult = HAL_I2C_EnableListen_IT(pSTI2C);

	} else {
		nResult = HAL_I2C_DisableListen_IT(pSTI2C);
		pI2CIface->nSlaveAddress = 0; //Reset these to detect config changes later
		pI2CIface->bMaster = true;
	}

	if (nResult == HAL_OK) {
		return I2C_Success;
	} else {
		return I2C_Fail_Unknown; //Only unknown because I'm not handling the actual result
	}
}

eI2CReturn_t NucleoI2CSlaveSetAddrHandler(sI2CIface_t *pI2CIface, pfI2CSlaveAddrHandler_t pfHandler) {
	sSTI2CInfo_t *pSTI2CInfo = (sSTI2CInfo_t *)pI2CIface->pHWInfo;

	pSTI2CInfo->pfAddrHandler = pfHandler;

	return I2C_Success;
}

eI2CReturn_t NucleoI2CSlaveSetRecvCompHandler(sI2CIface_t *pI2CIface, pfI2CSlaveCompHandler_t pfHandler) {
	sSTI2CInfo_t *pSTI2CInfo = (sSTI2CInfo_t *)pI2CIface->pHWInfo;

	pSTI2CInfo->pfRecvCompHandler = pfHandler;

	return I2C_Success;
}

eI2CReturn_t NucleoI2CSlaveSetTransCompHandler(sI2CIface_t *pI2CIface, pfI2CSlaveCompHandler_t pfHandler) {
	sSTI2CInfo_t *pSTI2CInfo = (sSTI2CInfo_t *)pI2CIface->pHWInfo;

	pSTI2CInfo->pfTransCompHandler = pfHandler;

	return I2C_Success;
}

eI2CReturn_t NucleoI2CSlaveSetSendCompHandler(sI2CIface_t *pI2CIface, pfI2CSlaveCompHandler_t pfHandler) {
	sSTI2CInfo_t *pSTI2CInfo = (sSTI2CInfo_t *)pI2CIface->pHWInfo;

	pSTI2CInfo->pfSendCompHandler = pfHandler;

	return I2C_Success;
}

eI2CReturn_t NucleoI2CSlaveSendData(sI2CIface_t *pI2CIface, uint16_t nDataLen, uint8_t *pData) {
	HAL_StatusTypeDef nResult;
	I2C_HandleTypeDef *pSTI2C = ((sSTI2CInfo_t *)pI2CIface->pHWInfo)->pHWInfo;

	nResult = HAL_I2C_Slave_Seq_Transmit_IT(pSTI2C, pData, nDataLen, I2C_NEXT_FRAME);

	if (nResult == HAL_OK) {
		return I2C_Success;
	} else {
		return I2C_Fail_Unknown; //Only unknown because I'm not handling the actual result
	}
}

eI2CReturn_t NucleoI2CSlaveRecvData(sI2CIface_t *pI2CIface, uint16_t nDataLen, uint8_t *pData) {
	HAL_StatusTypeDef nResult;
	I2C_HandleTypeDef *pSTI2C = ((sSTI2CInfo_t *)pI2CIface->pHWInfo)->pHWInfo;

	nResult = HAL_I2C_Slave_Seq_Receive_IT(pSTI2C, pData, nDataLen, I2C_NEXT_FRAME);

	if (nResult == HAL_OK) {
		return I2C_Success;
	} else {
		return I2C_Fail_Unknown; //Only unknown because I'm not handling the actual result
	}
}

void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode) {
	sSTI2CInfo_t *pSTI2CInfo;

	//Determine the port the interrupt is for
	if (hi2c == &hi2c1) {
		pSTI2CInfo = &gSTI2C1;
	} else if (hi2c == &hi2c3) {
		pSTI2CInfo = &gSTI2C3;
	} else {
		return;
	}

	if (TransferDirection == I2C_DIRECTION_RECEIVE) {
		if (pSTI2CInfo->pfAddrHandler != NULL) {
			pSTI2CInfo->pfAddrHandler(pSTI2CInfo->pIFace, I2CSlave_MasterReceive, AddrMatchCode);
		}
	} else {
		if (pSTI2CInfo->pfAddrHandler != NULL) {
			pSTI2CInfo->pfAddrHandler(pSTI2CInfo->pIFace, I2CSlave_MasterTransmit, AddrMatchCode);
		}
	}

	return;
}

void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c) {
	sSTI2CInfo_t *pSTI2CInfo;

	//Determine the port the interrupt is for
	if (hi2c == &hi2c1) {
		pSTI2CInfo = &gSTI2C1;
	} else if (hi2c == &hi2c3) {
		pSTI2CInfo = &gSTI2C3;
	} else {
		return;
	}

	if (pSTI2CInfo->pfSendCompHandler != NULL) {
		pSTI2CInfo->pfSendCompHandler(pSTI2CInfo->pIFace);
	}

	return;
}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c) {
	sSTI2CInfo_t *pSTI2CInfo;

	//Determine the port the interrupt is for
	if (hi2c == &hi2c1) {
		pSTI2CInfo = &gSTI2C1;
	} else if (hi2c == &hi2c3) {
		pSTI2CInfo = &gSTI2C3;
	} else {
		return;
	}

	if (pSTI2CInfo->pfRecvCompHandler != NULL) {
		pSTI2CInfo->pfRecvCompHandler(pSTI2CInfo->pIFace);
	}

	return;
}

void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c) {
	sSTI2CInfo_t *pSTI2CInfo;

	//Determine the port the interrupt is for
	if (hi2c == &hi2c1) {
		pSTI2CInfo = &gSTI2C1;
	} else if (hi2c == &hi2c3) {
		pSTI2CInfo = &gSTI2C3;
	} else {
		return;
	}

	if (pSTI2CInfo->pfTransCompHandler != NULL) {
		pSTI2CInfo->pfTransCompHandler(pSTI2CInfo->pIFace);
	}

	return;
}
