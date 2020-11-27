/**	File:	I2C_RaspberryPi.c
	Author:	J. Beighel
	Date:	11-25-2020
*/

/*****	Includes	*****/
	#include "I2C_RaspberryPi.h"

/*****	Constants	*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	eI2CReturns_t RasPiI2CShutdown(sI2CIface_t *pI2CIface);

/*****	Functions	*****/

eI2CReturns_t RasPiInitializeI2CBus(sI2CIface_t *pI2CIface, bool bActAsMaster, uint32_t nClockFreq, void *pHWInfo) {
	sRasPiI2CHWInfo_t *pI2C = (sRasPiI2CHWInfo_t *)pHWInfo;
	
	//Setup the interface structure
	I2CInterfaceInitialize(pI2CIface);
	
	pI2CIface->bMaster = bActAsMaster;
	pI2CIface->nClockFreq = nClockFreq;
	pI2CIface->pHWInfo = pHWInfo;
	
	//Set all the interface functions
	pI2CIface->pfInitialize = &RasPiInitializeI2CBus;
	pI2CIface->pfShutdown = &RasPiI2CShutdown;
	
	//Set up the hardware
	pI2C->I2CFile = open(pI2C->pcFilePath, O_RDWR);
	
	if (pI2C->I2CFile < 0) {
		return I2C_Fail_Unknown;
	}
	
	return I2C_Success;
}

eI2CReturns_t RasPiI2CShutdown(sI2CIface_t *pI2CIface) {
	sRasPiI2CHWInfo_t *pI2C = (sRasPiI2CHWInfo_t *)(pI2CIface->pHWInfo);
	
	if (pI2C->I2CFile >= 0) {
		close(pI2C->I2CFile);
	}
	
	return I2C_Success;
}