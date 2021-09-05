/**	File:	MPU6050Driver.c
	Author:	J. Beighel
	Date:	2021-09-05
*/

/*****	Includes	*****/


/*****	Defines		*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/


/*****	Functions	*****/
eMPU6050Return_t MPU6050Initialize(sMPU6050Obj_t pObj, sI2CIface_t pI2C, eMPU6050Addr_t eI2CAddr) {
	uint8_t nValue;
	eI2CReturn_t eResult;
	
	//Initialize the driver object
	pObj->pI2C = pI2C;
	pObj->eAddr = MPU6050Addr_Base | eI2CAddr;
	
	//Verify the I2C bus will work
	if (CheckAllBitsInMask(pObj->pI2C->eCapabilities, MPU6050_I2CCAPS) == false) {
		//Bus is missing needed capabilities
		return Fail_Invalid;
	}
	
	if (pObj->pI2C->bMaster == false) {
		//Bus must be configured as a master
		return Fail_Invalid;
	}
	
	if (pObj->pI2c->nClockFreq > MPU6050_MAXCLOCK) {
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
	
	eResult = pObj->pI2C->pfI2CWriteUint8Reg(pObj->pI2C, pObj->eAddr, MPU6050Reg_IntPinCfg,  MPU6050RegIntCfg_None); //Disable the I2C master
	if (eResult != I2C_Success) { //Failed to talk on the bus
		return Fail_CommError;
	}
	
	eResult = pObj->pI2C->pfI2CWriteUint8Reg(pObj->pI2C, pObj->eAddr, MPU6050Reg_IntEnable,  MPU6050RegIntEn_None); //Disable the I2C master
	if (eResult != I2C_Success) { //Failed to talk on the bus
		return Fail_CommError;
	}
	
	//user ctrl
	//power mgmt 1
	//power mgmt 2
	
	//signal path reset
	
	return Success;
}
