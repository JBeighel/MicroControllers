/**	File:	I2C_RaspberryPi.c
	Author:	J. Beighel
	Date:	2021-05-02
*/

/*****	Includes	*****/
	#include "I2C_RaspberryPi.h"

/*****	Constants	*****/


/*****	Definitions	*****/


/*****	Constants	*****/
	/**	@brief		Path and file name of the filesystem object for the I2C bus
		@ingroup	i2craspberrypi
	*/
	const char cgI2C1File[] = "/dev/i2c-1";

	/**	@brief		Array of all I2C buses for the Raspberry Pi
		@ingroup	i2craspberrypi
	*/
	sRasPiI2CHWInfo_t gI2CHWInfo [] = {
		{ .pcFilePath = cgI2C1File, .I2CFile = -1, },
	};

/*****	Globals		*****/


/*****	Prototypes 	*****/
	eI2CReturn_t RasPiI2CShutdown(sI2CIface_t *pI2CIface);
	
	eI2CReturn_t RasPiI2CReadUint8Reg(sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nRegAddr, uint8_t *pnValue);
	
	eI2CReturn_t RasPiI2CWriteUint8Reg(sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nRegAddr, uint8_t nValue);

/*****	Functions	*****/

eI2CReturn_t RasPiInitializeI2CBus(sI2CIface_t *pI2CIface, bool bActAsMaster, uint32_t nClockFreq, void *pHWInfo) {
	sRasPiI2CHWInfo_t *pI2C = (sRasPiI2CHWInfo_t *)pHWInfo;
	
	//Setup the interface structure
	I2CInterfaceInitialize(pI2CIface);
	
	pI2CIface->bMaster = bActAsMaster;
	pI2CIface->nClockFreq = nClockFreq;
	pI2CIface->pHWInfo = pHWInfo;
	
	//Set all the interface functions
	pI2CIface->pfInitialize = &RasPiInitializeI2CBus;
	pI2CIface->pfShutdown = &RasPiI2CShutdown;
	pI2CIface->pfI2CReadUint8Reg = &RasPiI2CReadUint8Reg;
	pI2CIface->pfI2CWriteUint8Reg = &RasPiI2CWriteUint8Reg;
	
	//Set up the hardware
	pI2C->I2CFile = open(pI2C->pcFilePath, O_RDWR);
	
	if (pI2C->I2CFile < 0) {
		pI2C->nLastErr = errno;
		return I2C_Fail_Unknown;
	}
	
	pI2CIface->eCapabilities = I2C_1_CAPS;
	
	return I2C_Success;
}

eI2CReturn_t RasPiI2CShutdown(sI2CIface_t *pI2CIface) {
	sRasPiI2CHWInfo_t *pI2C = (sRasPiI2CHWInfo_t *)(pI2CIface->pHWInfo);
	
	if (pI2C->I2CFile >= 0) {
		close(pI2C->I2CFile);
	}
	
	pI2C->I2CFile = -1;
	
	return I2C_Success;
}

eI2CReturn_t RasPiI2CReadUint8Reg(sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nRegAddr, uint8_t *pnValue) {
	sRasPiI2CHWInfo_t *pI2C = (sRasPiI2CHWInfo_t *)(pI2CIface->pHWInfo);
	int32_t nReturn;
	
	*pnValue = 0;
	
	nReturn = ioctl(pI2C->I2CFile, I2C_SLAVE, nDevAddr);
	if (nReturn < 0) { //Faired to aquire the bus or reach the slave
		//Global errno holds the error code
		pI2C->nLastErr = errno;
		return I2C_Fail_Unknown;
	}
	
	//Write the address we want to read
	nReturn = write(pI2C->I2CFile, &nRegAddr, 1); //Length is one for uint8 registers
	if (nReturn != 1) { //Didn't write the correct number of bytes
		//Global errno holds the error code
		pI2C->nLastErr = errno;
		return I2C_Fail_Unknown;
	}
	
	//Read the byte value from the address
	nReturn = read(pI2C->I2CFile, pnValue, 1); //Length is one for uint8 registers
	if (nReturn != 1) { //Didn't write the correct number of bytes
		//Global errno holds the error code
		pI2C->nLastErr = errno;
		return I2C_Fail_Unknown;
	}
	
	return I2C_Success;
}

eI2CReturn_t RasPiI2CWriteUint8Reg(sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nRegAddr, uint8_t nValue) {
	sRasPiI2CHWInfo_t *pI2C = (sRasPiI2CHWInfo_t *)(pI2CIface->pHWInfo);
	int32_t nReturn;
	uint8_t aData[2];
	
	aData[0] = nRegAddr;
	aData[1] = nValue;
	
	nReturn = ioctl(pI2C->I2CFile, I2C_SLAVE, nDevAddr);
	if (nReturn < 0) { //Faired to aquire the bus or reach the slave
		//Global errno holds the error code
		pI2C->nLastErr = errno;
		return I2C_Fail_Unknown;
	}
	
	//Write the address and data in one transaction
	nReturn = write(pI2C->I2CFile, aData, 2); //Length is one for uint8 registers
	if (nReturn != 2) { //Didn't write the correct number of bytes
		//Global errno holds the error code
		pI2C->nLastErr = errno;
		return I2C_Fail_Unknown;
	}
	
	return I2C_Success;
}

eI2CReturn_t RasPiI2CReadData(sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nNumBytes, void *pDataBuff, uint8_t *pnBytesRead) {
	sRasPiI2CHWInfo_t *pI2C = (sRasPiI2CHWInfo_t *)(pI2CIface->pHWInfo);
	int32_t nReturn;
	
	nReturn = ioctl(pI2C->I2CFile, I2C_SLAVE, nDevAddr);
	if (nReturn < 0) { //Faired to aquire the bus or reach the slave
		//Global errno holds the error code
		pI2C->nLastErr = errno;
		return I2C_Fail_Unknown;
	}
	
	//Try reading from the peripheral
	nReturn = read(pI2C->I2CFile, pDataBuff, nNumBytes); //Length is one for uint8 registers
	*pnBytesRead = nReturn;
	if (nReturn != nNumBytes) { //Didn't write the correct number of bytes
		//Global errno holds the error code
		pI2C->nLastErr = errno;
		return I2C_Fail_Unknown;
	}
	
	return I2C_Success;
}

eI2CReturn_t RasPiI2CWriteData(sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nNumBytes, void *pDataBuff) {
	sRasPiI2CHWInfo_t *pI2C = (sRasPiI2CHWInfo_t *)(pI2CIface->pHWInfo);
	int32_t nReturn;
	
	nReturn = ioctl(pI2C->I2CFile, I2C_SLAVE, nDevAddr);
	if (nReturn < 0) { //Faired to aquire the bus or reach the slave
		//Global errno holds the error code
		pI2C->nLastErr = errno;
		return I2C_Fail_Unknown;
	}
	
	//Write the data to the peripheral
	nReturn = write(pI2C->I2CFile, pDataBuff, nNumBytes); //Length is one for uint8 registers
	if (nReturn != nNumBytes) { //Didn't write the correct number of bytes
		//Global errno holds the error code
		pI2C->nLastErr = errno;
		return I2C_Fail_Unknown;
	}
	
	return I2C_Success;
}