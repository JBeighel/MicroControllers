/**	File:	MPU6050Driver.c
	Author:	J. Beighel
	Date:	2021-09-05
*/

/*****	Includes	*****/
	#include "MPU6050Driver.h"

/*****	Defines		*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/


/*****	Functions	*****/
eMPU6050Return_t MPU6050Initialize(sMPU6050Obj_t *pObj, sI2CIface_t *pI2C, eMPU6050Addr_t eI2CAddr) {
	uint8_t nValue;
	eI2CReturn_t eResult;
	
	//Initialize the driver object
	pObj->pI2C = pI2C;
	pObj->eAddr = (eMPU6050Addr_t)(MPU6050Addr_Base | eI2CAddr);
	
	//Verify the I2C bus will work
	if (CheckAllBitsInMask(pObj->pI2C->eCapabilities, MPU6050_I2CCAPS) == false) {
		//Bus is missing needed capabilities
		return Fail_Invalid;
	}
	
	if (pObj->pI2C->bMaster == false) {
		//Bus must be configured as a master
		return Fail_Invalid;
	}
	
	if (pObj->pI2C->nClockFreq > MPU6050_MAXCLOCK) {
		//Bus clock is too high
		return Fail_Invalid;
	}
	
	//See if the device is there
	eResult = pObj->pI2C->pfI2CReadUint8Reg(pObj->pI2C, pObj->eAddr, MPU6050Reg_WhoAmI, &nValue);
	if (eResult != I2C_Success) { //Failed to talk on the bus
		return Fail_CommError;
	}
	
	if (nValue != MPU6050Addr_Base) { //Device gave incorrect response
		return Fail_Unknown;
	}
	
	//Apply a default configuration
	eResult = pObj->pI2C->pfI2CWriteUint8Reg(pObj->pI2C, pObj->eAddr, MPU6050Reg_SmplRateDiv, 0);
	if (eResult != I2C_Success) { //Failed to talk on the bus
		return Fail_CommError;
	}
	
	eResult = pObj->pI2C->pfI2CWriteUint8Reg(pObj->pI2C, pObj->eAddr, MPU6050Reg_Config, MPU6050RegCfg_SyncDis | MPU6050RegCfg_LPF260Hz);
	if (eResult != I2C_Success) { //Failed to talk on the bus
		return Fail_CommError;
	}
	
	eResult = pObj->pI2C->pfI2CWriteUint8Reg(pObj->pI2C, pObj->eAddr, MPU6050Reg_GyroConfig, MPU6050RegGCfg_FS2000);
	if (eResult != I2C_Success) { //Failed to talk on the bus
		return Fail_CommError;
	}
	
	eResult = pObj->pI2C->pfI2CWriteUint8Reg(pObj->pI2C, pObj->eAddr, MPU6050Reg_AccelConfig,  MPU6050RegACfg_FS16g);
	if (eResult != I2C_Success) { //Failed to talk on the bus
		return Fail_CommError;
	}
	
	eResult = pObj->pI2C->pfI2CWriteUint8Reg(pObj->pI2C, pObj->eAddr, MPU6050Reg_FIFOEnable,  MPU6050RegFIFOen_None);
	if (eResult != I2C_Success) { //Failed to talk on the bus
		return Fail_CommError;
	}
	
	eResult = pObj->pI2C->pfI2CWriteUint8Reg(pObj->pI2C, pObj->eAddr, MPU6050Reg_I2CMstrCtrl,  0x00); //Disable the I2C master
	if (eResult != I2C_Success) { //Failed to talk on the bus
		return Fail_CommError;
	}
	
	eResult = pObj->pI2C->pfI2CWriteUint8Reg(pObj->pI2C, pObj->eAddr, MPU6050Reg_IntPinCfg,  MPU6050RegIntCfg_None); 
	if (eResult != I2C_Success) { //Failed to talk on the bus
		return Fail_CommError;
	}
	
	eResult = pObj->pI2C->pfI2CWriteUint8Reg(pObj->pI2C, pObj->eAddr, MPU6050Reg_IntEnable,  MPU6050RegIntEn_None); 
	if (eResult != I2C_Success) { //Failed to talk on the bus
		return Fail_CommError;
	}
	
	eResult = pObj->pI2C->pfI2CWriteUint8Reg(pObj->pI2C, pObj->eAddr, MPU6050Reg_UserControl,  MPU6050RegUsrCtrl_6050Def); 
	if (eResult != I2C_Success) { //Failed to talk on the bus
		return Fail_CommError;
	}
	
	eResult = pObj->pI2C->pfI2CWriteUint8Reg(pObj->pI2C, pObj->eAddr, MPU6050Reg_PowerMgmt1, MPU6050RegPwr1_ClkGyroX); 
	if (eResult != I2C_Success) { //Failed to talk on the bus
		return Fail_CommError;
	}
	
	eResult = pObj->pI2C->pfI2CWriteUint8Reg(pObj->pI2C, pObj->eAddr, MPU6050Reg_PowerMgmt2, MPU6050RegPwr2_LPWake1Hz); 
	if (eResult != I2C_Success) { //Failed to talk on the bus
		return Fail_CommError;
	}
	
	eResult = pObj->pI2C->pfI2CWriteUint8Reg(pObj->pI2C, pObj->eAddr, MPU6050Reg_SigPathRest,  MPU6050RegSigRst_All); 
	if (eResult != I2C_Success) { //Failed to talk on the bus
		return Fail_CommError;
	}
	
	//Device should be ready to use
	
	return Success;
}

eMPU6050Return_t MPU6050ReadGyro(sMPU6050Obj_t *pObj, int16_t *pnXMeas, int16_t *pnYMeas, int16_t *pnZMeas) {
	int8_t anData[6];
	uint8_t nRead;
	eI2CReturn_t eResult;
	
	//Tell the device to start at Gyro data
	anData[0] = MPU6050Reg_GyroXOutH;
	eResult = pObj->pI2C->pfI2CWriteData(pObj->pI2C, pObj->eAddr, 1, anData);
	if (eResult != I2C_Success) { //Failed to talk on the bus
		return Fail_CommError;
	}
	
	//Read 6 bytes worth of samples out
	eResult = pObj->pI2C->pfI2CReadData(pObj->pI2C, pObj->eAddr, 6, anData, &nRead);
	if (eResult != I2C_Success) { //Failed to talk on the bus
		return Fail_CommError;
	}
	
	//Parse out the sample data
	*pnXMeas = ((uint16_t)anData[0] << 8) | anData[1];
	*pnYMeas = ((uint16_t)anData[2] << 8) | anData[3];
	*pnZMeas = ((uint16_t)anData[4] << 8) | anData[5];
	
	return Success;
}

eMPU6050Return_t MPU6050ReadAccel(sMPU6050Obj_t *pObj, int16_t *pnXMeas, int16_t *pnYMeas, int16_t *pnZMeas) {
	int8_t anData[6];
	uint8_t nRead;
	eI2CReturn_t eResult;
	
	//Tell the device to start at Gyro data
	anData[0] = MPU6050Reg_AccelXOutH;
	eResult = pObj->pI2C->pfI2CWriteData(pObj->pI2C, pObj->eAddr, 1, anData);
	if (eResult != I2C_Success) { //Failed to talk on the bus
		return Fail_CommError;
	}
	
	//Read 6 bytes worth of samples out
	eResult = pObj->pI2C->pfI2CReadData(pObj->pI2C, pObj->eAddr, 6, anData, &nRead);
	if (eResult != I2C_Success) { //Failed to talk on the bus
		return Fail_CommError;
	}
	
	//Parse out the sample data
	*pnXMeas = ((uint16_t)anData[0] << 8) | anData[1];
	*pnYMeas = ((uint16_t)anData[2] << 8) | anData[3];
	*pnZMeas = ((uint16_t)anData[4] << 8) | anData[5];
	
	return Success;
}
