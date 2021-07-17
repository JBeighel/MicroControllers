/*	File:	I2CGeneralInterface.c
	Author:	J. Beighel
	Date:	2021-05-07
*/

#include "I2CGeneralInterface.h"

/***** Protected Constants   *****/


/***** Protected Definitions *****/


/***** Module Variables      *****/


/***** Protected Prototypes  *****/
	/**	@brief		I2C Interface function to use if this feature is not implemented in the hardware driver
		@ingroup	i2ciface_priv
	*/
	eI2CReturn_t I2CIfaceNoInitializePort(sI2CIface_t *pI2CIface, bool bActAsMaster, uint32_t nClockFreq, void *pHWInfo);

	/**	@brief		I2C Interface function to use if this feature is not implemented in the hardware driver
		@ingroup	i2ciface_priv
	*/
	eI2CReturn_t I2CIfaceNoShutdownPort (sI2CIface_t *pI2CIface);

	/**	@brief		I2C Interface function to use if this feature is not implemented in the hardware driver
		@ingroup	i2ciface_priv
	*/	
	eI2CReturn_t I2CIfaceNoReadUint8Reg (sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nRegAddr, uint8_t *pnValue);

	/**	@brief		I2C Interface function to use if this feature is not implemented in the hardware driver
		@ingroup	i2ciface_priv
	*/
	eI2CReturn_t I2CIfaceNoReadData (sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nNumBytes, void *pDataBuff, uint8_t *pnBytesRead);

	/**	@brief		I2C Interface function to use if this feature is not implemented in the hardware driver
		@ingroup	i2ciface_priv
	*/
	eI2CReturn_t I2CIfaceNoWriteUint8Reg (sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nRegAddr, uint8_t nValue);

	/**	@brief		I2C Interface function to use if this feature is not implemented in the hardware driver
		@ingroup	i2ciface_priv
	*/
	eI2CReturn_t I2CIfaceNoWriteData (sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nNumBytes, void *pDataBuff);

	/**	@brief		I2C Interface function to use if this feature is not implemented in the hardware driver
		@ingroup	i2ciface_priv
	*/
	eI2CReturn_t I2CIfaceNoGeneralCall (sI2CIface_t *pI2CIface, uint8_t nValue);

	eI2CReturn_t I2CIfaceNoSlaveListenEnable(sI2CIface_t *pI2CIface, uint8_t nAddr, bool bEnable);

	eI2CReturn_t I2CIfaceNoSlaveSetAddrHandler(sI2CIface_t *pI2CIface, pfI2CSlaveAddrHandler_t pfHandler);

	eI2CReturn_t I2CIfaceNoSlaveSetRecvCompHandler(sI2CIface_t *pI2CIface, pfI2CSlaveCompHandler_t pfHandler);

	eI2CReturn_t I2CIfaceNoSlaveSetSendCompHandler(sI2CIface_t *pI2CIface, pfI2CSlaveCompHandler_t pfHandler);

	eI2CReturn_t I2CIfaceNoSlaveSetTransCompHandler(sI2CIface_t *pI2CIface, pfI2CSlaveCompHandler_t pfHandler);

	eI2CReturn_t I2CIfaceNoSlaveSendData(sI2CIface_t *pI2CIface, uint16_t nDataLen, uint8_t *pData);

	eI2CReturn_t I2CIfaceNoSlaveRecvData(sI2CIface_t *pI2CIface, uint16_t nBuffLen, uint8_t *pData);

/***** Function Code         *****/
eI2CReturn_t I2CInterfaceInitialize(sI2CIface_t *pI2CIface) {
	memset(pI2CIface, 0, sizeof(sI2CIface_t));
	
	pI2CIface->pfInitialize = &I2CIfaceNoInitializePort;
	pI2CIface->pfShutdown = &I2CIfaceNoShutdownPort;
	pI2CIface->pfI2CReadUint8Reg = &I2CIfaceNoReadUint8Reg;
	pI2CIface->pfI2CReadData = &I2CIfaceNoReadData;
	pI2CIface->pfI2CWriteUint8Reg = &I2CIfaceNoWriteUint8Reg;
	pI2CIface->pfI2CWriteData = &I2CIfaceNoWriteData;
	pI2CIface->pfI2CGeneralCall = &I2CIfaceNoGeneralCall;
	
	pI2CIface->pfSlaveListenEnable = I2CIfaceNoSlaveListenEnable;
	pI2CIface->pfSlaveSetAddrHandler = I2CIfaceNoSlaveSetAddrHandler;
	pI2CIface->pfSlaveSetRecvCompHandler = I2CIfaceNoSlaveSetRecvCompHandler;
	pI2CIface->pfSlaveSetSendCompHandler = I2CIfaceNoSlaveSetSendCompHandler;
	pI2CIface->pfSlaveSetTransCompHandler = I2CIfaceNoSlaveSetSendCompHandler;
	pI2CIface->pfSlaveSendData = I2CIfaceNoSlaveSendData;
	pI2CIface->pfSlaveRecvData = I2CIfaceNoSlaveRecvData;

	pI2CIface->eCapabilities = I2CCap_None;
	pI2CIface->nSlaveAddress = 0;
	
	return I2C_Success;
}


eI2CReturn_t I2CIfaceNoInitializePort(sI2CIface_t *pI2CIface, bool bActAsMaster, uint32_t nClockFreq, void *pHWInfo) {
	return I2C_Fail_Unsupported;
}

eI2CReturn_t I2CIfaceNoShutdownPort (sI2CIface_t *pI2CIface) {
	return I2C_Fail_Unsupported;
}
		
eI2CReturn_t I2CIfaceNoReadUint8Reg (sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nRegAddr, uint8_t *pnValue) {
	return I2C_Fail_Unsupported;
}

eI2CReturn_t I2CIfaceNoReadData (sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nNumBytes, void *pDataBuff, uint8_t *pnBytesRead) {
	return I2C_Fail_Unsupported;
}

eI2CReturn_t I2CIfaceNoWriteUint8Reg (sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nRegAddr, uint8_t nValue) {
	return I2C_Fail_Unsupported;
}

eI2CReturn_t I2CIfaceNoWriteData (sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nNumBytes, void *pDataBuff) {
	return I2C_Fail_Unsupported;
}

eI2CReturn_t I2CIfaceNoGeneralCall (sI2CIface_t *pI2CIface, uint8_t nValue) {
	return I2C_Fail_Unsupported;
}

eI2CReturn_t I2CIfaceNoSlaveListenEnable(sI2CIface_t *pI2CIface, uint8_t nAddr, bool bEnable) {
	return I2C_Fail_Unsupported;
}

eI2CReturn_t I2CIfaceNoSlaveSetAddrHandler(sI2CIface_t *pI2CIface, pfI2CSlaveAddrHandler_t pfHandler) {
	return I2C_Fail_Unsupported;
}

eI2CReturn_t I2CIfaceNoSlaveSetRecvCompHandler(sI2CIface_t *pI2CIface, pfI2CSlaveCompHandler_t pfHandler) {
	return I2C_Fail_Unsupported;
}

eI2CReturn_t I2CIfaceNoSlaveSetTransCompHandler(sI2CIface_t *pI2CIface, pfI2CSlaveCompHandler_t pfHandler) {
	return I2C_Fail_Unsupported;
}

eI2CReturn_t I2CIfaceNoSlaveSetSendCompHandler(sI2CIface_t *pI2CIface, pfI2CSlaveCompHandler_t pfHandler) {
	return I2C_Fail_Unsupported;
}

eI2CReturn_t I2CIfaceNoSlaveSendData(sI2CIface_t *pI2CIface, uint16_t nDataLen, uint8_t *pData) {
	return I2C_Fail_Unsupported;
}

eI2CReturn_t I2CIfaceNoSlaveRecvData(sI2CIface_t *pI2CIface, uint16_t nBuffLen, uint8_t *pData) {
	return I2C_Fail_Unsupported;
}
