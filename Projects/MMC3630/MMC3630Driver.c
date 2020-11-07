/*		File:	MMC3630Driver.c
		Author:	J. Beighel
		Date:	10-22-2020
*/


/*****	Includes	*****/
	#include "MMC3630Driver.h"

/*****	Constants	*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/


/*****	Functions	*****/

eMMC3630Return_t MMC3630Init(sMMC3630Dev_t *pDev, sI2CIface_t *pI2C, eMMC3630I2CAddr_t eI2CAddr) {
	eI2CReturns_t eResult;
	uint8_t nReadVal, nCtr;
	int16_t nXPre, nYPre, nZPre, nXPost, nYPost, nZPost;
	float nTemp;
	eMMC3630Cont0_t eControl0 = MMC3630Cont0_Default;
	eMMC3630Cont1_t eControl1 = MMC3630Cont1_BW10mSec; //(eMMC3630Cont1_t)(MMC3630Cont1_Default | MMC3630Cont1_BW10mSec);
	eMMC3630Cont2_t eControl2 = MMC3630Cont2_CMOff;
	
	//Setup the object
	pDev->nI2CAddr = eI2CAddr;
	pDev->pI2C = pI2C;
	
	//Identify device
	eResult = pDev->pI2C->pfI2CReadUint8Reg(pDev->pI2C, pDev->nI2CAddr, MMC3630Reg_ProductID, &nReadVal);
	if (eResult != I2C_Success) {
		return MMC3630Fail_BusError;
	} else if (nReadVal != MMC3630Reg_ProdIDVal) {
		return MMC3630Fail_NotFound;
	}
	
	//Set initial configuration
	eResult = pDev->pI2C->pfI2CWriteUint8Reg(pDev->pI2C, pDev->nI2CAddr, MMC3630Reg_Control0, eControl0);
	if (eResult != I2C_Success) {
		return MMC3630Fail_BusError;
	}
	
	eResult = pDev->pI2C->pfI2CWriteUint8Reg(pDev->pI2C, pDev->nI2CAddr, MMC3630Reg_Control1, eControl1);
	if (eResult != I2C_Success) {
		return MMC3630Fail_BusError;
	}
	
	eResult = pDev->pI2C->pfI2CWriteUint8Reg(pDev->pI2C, pDev->nI2CAddr, MMC3630Reg_Control2, eControl2);
	if (eResult != I2C_Success) {
		return MMC3630Fail_BusError;
	}
	//Obtain offset values, set field and take reading
	eControl0 = (eMMC3630Cont0_t)(MMC3630Cont1_BW10mSec | MMC3630Cont0_Set);
	eResult = pDev->pI2C->pfI2CWriteUint8Reg(pDev->pI2C, pDev->nI2CAddr, MMC3630Reg_Control0, eControl0);
	if (eResult != I2C_Success) {
		return MMC3630Fail_BusError;
	}
	
	nReadVal = 0;
	nCtr = 0;
	while ((CheckAllBitsInMask(nReadVal, MMC3630Stat_MeasMDone | MMC3630Stat_MeasTDone) == false) && (nCtr < 100)) {
		eResult = pDev->pI2C->pfI2CReadUint8Reg(pDev->pI2C, pDev->nI2CAddr, MMC3630Reg_Status, &nReadVal);
		if (eResult != I2C_Success) {
			return MMC3630Fail_BusError;
		}
		nCtr += 1;
	}
	
	MMC3630ReadSingleMeasurement(pDev, &nXPre, &nYPre, &nZPre, &nTemp);
	
	//Obtain offset values, reset field and take reading
	eControl0 = (eMMC3630Cont0_t)(MMC3630Cont1_BW10mSec | MMC3630Cont0_Reset);
	eResult = pDev->pI2C->pfI2CWriteUint8Reg(pDev->pI2C, pDev->nI2CAddr, MMC3630Reg_Control0, eControl0);
	
	MMC3630ReadSingleMeasurement(pDev, &nXPost, &nYPost, &nZPost, &nTemp);
	
	pDev->nXOffset = (nXPre + nXPost) / 2;
	pDev->nYOffset = (nYPre + nYPost) / 2;
	pDev->nZOffset = (nZPre + nZPost) / 2;
	
	return MMC3630_Success;
}

eMMC3630Return_t MMC3630ReadSingleMeasurement(sMMC3630Dev_t *pDev, int16_t *pnXVal, int16_t *pnYVal, int16_t *pnZVal, float *pnTempC) {
	eI2CReturns_t eResult;
	uint16_t nTemp;
	uint8_t nStatus, nCtr, anVals[7];
	
	//Request the measurements be taken
	eResult = pDev->pI2C->pfI2CWriteUint8Reg(pDev->pI2C, pDev->nI2CAddr, MMC3630Reg_Control0, MMC3630Cont0_MeasTemp | MMC3630Cont0_MeasMagnet);
	if (eResult != I2C_Success) {
		return MMC3630Fail_BusError;
	}
	
	//Wait for measurements to complete
	nStatus = 0;
	nCtr = 0;
	while ((CheckAllBitsInMask(nStatus, MMC3630Stat_MeasMDone | MMC3630Stat_MeasTDone) == false) && (nCtr < 100)) {
		eResult = pDev->pI2C->pfI2CReadUint8Reg(pDev->pI2C, pDev->nI2CAddr, MMC3630Reg_Status, &nStatus);
		if (eResult != I2C_Success) {
			return MMC3630Fail_BusError;
		}
		nCtr += 1;
	}
	
	//Instruct read to begin at first measurement value
	anVals[0] = MMC3630Reg_XoutLow;
	eResult = pDev->pI2C->pfI2CWriteData(pDev->pI2C, pDev->nI2CAddr, 1, anVals);
	if (eResult != I2C_Success) {
		return MMC3630Fail_BusError;
	}
	
	//Read out all measurement values
	eResult = pDev->pI2C->pfI2CReadData(pDev->pI2C, pDev->nI2CAddr, 7, anVals, &nCtr);
	if (eResult != I2C_Success) {
		return MMC3630Fail_BusError;
	}
	
	//Parse out the read data
	(*pnXVal) = (anVals[MMC3630Reg_XoutHigh] << 8) | anVals[MMC3630Reg_XoutLow]; 
	(*pnYVal) = (anVals[MMC3630Reg_YoutHigh] << 8) | anVals[MMC3630Reg_YoutLow]; 
	(*pnZVal) = (anVals[MMC3630Reg_ZoutHigh] << 8) | anVals[MMC3630Reg_ZoutLow];
	
	(*pnXVal) -= pDev->nXOffset;
	(*pnYVal) -= pDev->nYOffset;
	(*pnZVal) -= pDev->nZOffset;
	
	nTemp = anVals[MMC3630Reg_Temperature];
	(*pnTempC) = (((float)nTemp) * MMC3630_TEMPFACTOR) + MMC3630_TEMPOFFSET;
	
	return MMC3630_Success;
}

