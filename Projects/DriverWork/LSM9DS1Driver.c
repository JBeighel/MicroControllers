/** # File Information #
		File:	LSM9DS1Driver.c
		Author:	J. Beighel
		Date:	2021-09-05
*/

#include "LSM9DS1Driver.h"

/***** Protected Constants   *****/


/***** Protected Definitions *****/


/***** Module Variables      *****/


/***** Protected Prototypes  *****/


/***** Function Code         *****/
eReturn_t LSM9DS1Initialize(sI2CIface_t *pI2CIface, sLSM9DS1_t *pDev) {
	memset(pDev, 0, sizeof(sLSM9DS1_t));
	
	pDev->pI2CBus = pI2CIface;
	pDev->nAGDevAddr = LSM9DS1_AG_ADDRESS;
	pDev->nMDevAddr = LSM9DS1_M_ADDRESS;
	
	return Success;
}
	
eReturn_t LSM9DS1DetectDevice(sLSM9DS1_t *pDev) {
	sI2CIface_t *pI2CIface = pDev->pI2CBus;
	uint8_t nValue;
	eI2CReturn_t eRetVal;
	
	eRetVal = pI2CIface->pfI2CReadUint8Reg(pI2CIface, pDev->nAGDevAddr, LSM9DS1_WHO_AM_I, &nValue);
	
	if ((eRetVal <= 0) && (nValue == LSM9DS1_SELFID)) {
		return Success;
	} else {
		return Fail_Unknown;
	}
}

eReturn_t LSM9DS1Configure(sLSM9DS1_t *pDev) {
	sI2CIface_t *pI2CIface = pDev->pI2CBus;
	eI2CReturn_t eResult;
	
	eResult = pI2CIface->pfI2CWriteUint8Reg(pI2CIface, pDev->nAGDevAddr, LSM9DS1_CTRL_REG1_G, LSM9DS1_CTRL1G_G_14);
	if (eResult > I2C_Success) {
		return Fail_CommError;
	}
	
	return Success;
}

eReturn_t LSM9DS1ReadGyroscope(sLSM9DS1_t *pDev, int16_t *pnXVal, int16_t *pnYVal, int16_t *pnZVal) {
	sI2CIface_t *pI2CIface = pDev->pI2CBus;
	uint8_t nRegVal, nTimeOut;
	eI2CReturn_t eResult;
	bool bIsDataReady;
	
	(*pnXVal) = 0;
	(*pnYVal) = 0;
	(*pnZVal) = 0;
	
	//Check if there's gyroscope data waiting
	bIsDataReady = false;
	nTimeOut = 255;
	while ((bIsDataReady == false) && (nTimeOut > 0)) {
		eResult = pI2CIface->pfI2CReadUint8Reg(pI2CIface, pDev->nAGDevAddr, LSM9DS1_STATUS_REG, &nRegVal);
		if (eResult > I2C_Success) {
			return Fail_CommError;
		}
		
		if ((nRegVal & LSM9DS1_STATUS_GDA) == LSM9DS1_STATUS_GDA) {
			bIsDataReady = true;
		} else {
			/** @todo	Should have a delay here, but a generic method for that is needed */
			nTimeOut -= 1;
		}
	}
	
	if (bIsDataReady == false) { //Device says no data is waiting, nothing to read
		return Fail_Unknown;
	}
	
	//Data is waiting, read it out
	eResult = pI2CIface->pfI2CReadUint8Reg(pI2CIface, pDev->nAGDevAddr, LSM9DS1_OUT_X_L_G, &nRegVal);
	if (eResult > I2C_Success) {
		return Fail_CommError;
	} else {
		(*pnXVal) = 0x0000 | nRegVal;
	}
	
	eResult = pI2CIface->pfI2CReadUint8Reg(pI2CIface, pDev->nAGDevAddr, LSM9DS1_OUT_X_H_G, &nRegVal);
	if (eResult > I2C_Success) {
		return Fail_CommError;
	} else {
		(*pnXVal) |= nRegVal << 8; //Remember, left shifts of signed ints don't change sign
	}
	
	eResult = pI2CIface->pfI2CReadUint8Reg(pI2CIface, pDev->nAGDevAddr, LSM9DS1_OUT_Y_L_G, &nRegVal);
	if (eResult > I2C_Success) {
		return Fail_CommError;
	} else {
		(*pnYVal) = 0x0000 | nRegVal;
	}
	
	eResult = pI2CIface->pfI2CReadUint8Reg(pI2CIface, pDev->nAGDevAddr, LSM9DS1_OUT_Y_H_G, &nRegVal);
	if (eResult > I2C_Success) {
		return Fail_CommError;
	} else {
		(*pnYVal) |= nRegVal << 8; //Remember, left shifts of signed ints don't change sign
	}
	
	eResult = pI2CIface->pfI2CReadUint8Reg(pI2CIface, pDev->nAGDevAddr, LSM9DS1_OUT_Z_L_G, &nRegVal);
	if (eResult > I2C_Success) {
		return Fail_CommError;
	} else {
		(*pnZVal) = 0x0000 | nRegVal;
	}
	
	eResult = pI2CIface->pfI2CReadUint8Reg(pI2CIface, pDev->nAGDevAddr, LSM9DS1_OUT_Z_H_G, &nRegVal);
	if (eResult > I2C_Success) {
		return Fail_CommError;
	} else {
		(*pnZVal) |= nRegVal << 8; //Remember, left shifts of signed ints don't change sign
	}
	
	return Success;
}
	
eReturn_t LSM9DS1ReadAccelerometer(sLSM9DS1_t *pDev, int16_t *pnXVal, int16_t *pnYVal, int16_t *pnZVal) {
	sI2CIface_t *pI2CIface = pDev->pI2CBus;
	uint8_t nRegVal, nTimeOut;
	eI2CReturn_t eResult;
	bool bIsDataReady;
	
	(*pnXVal) = 0;
	(*pnYVal) = 0;
	(*pnZVal) = 0;
	
	//Check if there's gyroscope data waiting
	bIsDataReady = false;
	nTimeOut = 255;
	while ((bIsDataReady == false) && (nTimeOut > 0)) {
		eResult = pI2CIface->pfI2CReadUint8Reg(pI2CIface, pDev->nAGDevAddr, LSM9DS1_STATUS_REG, &nRegVal);
		if (eResult > I2C_Success) {
			return Fail_CommError;
		}
		
		if ((nRegVal & LSM9DS1_STATUS_XLDA) == LSM9DS1_STATUS_XLDA) {
			bIsDataReady = true;
		} else {
			/** @todo	Should have a delay here, but a generic method for that is needed */
			nTimeOut -= 1;
		}
	}
	
	if (bIsDataReady == false) { //Device says no data is waiting, nothing to read
		return Fail_Unknown;
	}
	
	//Data is waiting, read it out
	eResult = pI2CIface->pfI2CReadUint8Reg(pI2CIface, pDev->nAGDevAddr, LSM9DS1_OUT_X_L_XL, &nRegVal);
	if (eResult > I2C_Success) {
		return Fail_CommError;
	} else {
		(*pnXVal) = 0x0000 | nRegVal;
	}
	
	eResult = pI2CIface->pfI2CReadUint8Reg(pI2CIface, pDev->nAGDevAddr, LSM9DS1_OUT_X_H_XL, &nRegVal);
	if (eResult > I2C_Success) {
		return Fail_CommError;
	} else {
		(*pnXVal) |= nRegVal << 8; //Remember, left shifts of signed ints don't change sign
	}
	
	eResult = pI2CIface->pfI2CReadUint8Reg(pI2CIface, pDev->nAGDevAddr, LSM9DS1_OUT_Y_L_XL, &nRegVal);
	if (eResult > I2C_Success) {
		return Fail_CommError;
	} else {
		(*pnYVal) = 0x0000 | nRegVal;
	}
	
	eResult = pI2CIface->pfI2CReadUint8Reg(pI2CIface, pDev->nAGDevAddr, LSM9DS1_OUT_Y_H_XL, &nRegVal);
	if (eResult > I2C_Success) {
		return Fail_CommError;
	} else {
		(*pnYVal) |= nRegVal << 8; //Remember, left shifts of signed ints don't change sign
	}
	
	eResult = pI2CIface->pfI2CReadUint8Reg(pI2CIface, pDev->nAGDevAddr, LSM9DS1_OUT_Z_L_XL, &nRegVal);
	if (eResult > I2C_Success) {
		return Fail_CommError;
	} else {
		(*pnZVal) = 0x0000 | nRegVal;
	}
	
	eResult = pI2CIface->pfI2CReadUint8Reg(pI2CIface, pDev->nAGDevAddr, LSM9DS1_OUT_Z_H_XL, &nRegVal);
	if (eResult > I2C_Success) {
		return Fail_CommError;
	} else {
		(*pnZVal) |= nRegVal << 8; //Remember, left shifts of signed ints don't change sign
	}
	
	return Success;
}