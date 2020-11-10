/*		File:	TLV493DDriver.c
		Author:	J. Beighel
		Date:	10-21-2020
*/


/*****	Includes	*****/
	#include "TLV493DDriver.h"

/*****	Constants	*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	eTLV493DReturn_t TLV493DReadRegisters(sTLV493DDev_t *pDev, int16_t *pnXVal, int16_t *pnYVal, int16_t *pnZVal, int16_t *pnTempVal);
	
	eTLV493DReturn_t TLV493DWriteRegisters(sTLV493DDev_t *pDev, eTLV493DMode1_t eMode1, eTLV493DMode2_t eMode2);

/*****	Functions	*****/

eTLV493DReturn_t TLV493DInit(sTLV493DDev_t *pDev, sI2CIface_t *pI2C, eTLV493DI2CAddr_t eI2CAddr) {
	uint8_t nBytesRead, anReadData[TLV493DReg_RdInitCount];
	eI2CReturns_t eResult;
	eTLV493DMode1_t eMode1;
	eTLV493DMode2_t eMode2;
	
	uint8_t nCtr;
	uint8_t anAddrList[] = { TLV493DAddr_Bus0SDAHigh, TLV493DAddr_Bus1SDAHigh, TLV493DAddr_Bus2SDAHigh, TLV493DAddr_Bus3SDAHigh, TLV493DAddr_Bus4SDALow, TLV493DAddr_Bus5SDALow, TLV493DAddr_Bus6SDALow, TLV493DAddr_Bus7SDALow};
	
	//Setup the object
	pDev->nI2CAddr = eI2CAddr;
	pDev->pI2C = pI2C;
	
	pDev->pI2C->pfI2CGeneralCall(pDev->pI2C, 0xFF); //A general call should reset the device
	
	//Get inital values from the peripheral
	eResult = pDev->pI2C->pfI2CReadData(pDev->pI2C, pDev->nI2CAddr, TLV493DReg_RdInitCount, anReadData, &nBytesRead);
	if ((eResult != I2C_Success) && (nBytesRead != TLV493DReg_RdInitCount)) {
		pDev->pI2C->pfI2CGeneralCall(pDev->pI2C, 0xFF); //A general call should reset the device
		
		nCtr = 0;
		do {
			pDev->nI2CAddr = anAddrList[nCtr];
			nCtr++;
			
			eResult = pDev->pI2C->pfI2CReadData(pDev->pI2C, pDev->nI2CAddr, TLV493DReg_RdInitCount, anReadData, &nBytesRead);
		} while ((eResult != I2C_Success) && (nBytesRead != TLV493DReg_RdInitCount) && (nCtr < 8));
		
		if ((eResult != I2C_Success) && (nBytesRead != TLV493DReg_RdInitCount)) {
			return TLV493DFail_BusError;
		}
	}
	
	pDev->nRdReg8 = anReadData[TLV493DReg_RdReg8];
	pDev->nRdReg9 = anReadData[TLV493DReg_RdReg9];
	
	//Set initial configuration
	eMode1 = (eTLV493DMode1_t)(TLV493DM1_InterruptDis | TLV493DM1_FastEnable | TLV493DM1_LowPowerEn);
	eMode2 = (eTLV493DMode2_t)(TLV493DM2_TempDisable | TLV493DM2_LowPow12ms | TLV493DM2_ParityTestEn);
	
	return TLV493DWriteRegisters(pDev, eMode1, eMode2);
}

eTLV493DReturn_t TLV493DReadSingleMeasurement(sTLV493DDev_t *pDev, float *pnXmT, float *pnYmT, float *pnZmT, float *pnTempC) {
	eTLV493DReturn_t eRetVal = TLV493D_Success;
	int16_t nXVal, nYVal, nZVal, nTemp;
	
	eRetVal = TLV493DReadRegisters(pDev, &nXVal, &nYVal, &nZVal, &nTemp);
	
	(*pnXmT) = ((float)nXVal) * TLV493D_12BITFACTOR;
	(*pnYmT) = ((float)nYVal) * TLV493D_12BITFACTOR;
	(*pnZmT) = ((float)nZVal) * TLV493D_12BITFACTOR;
	
	(*pnTempC) = (((float)nTemp) * TLV493D_TEMPFACTOR) + TLV493D_TEMPOFFSET;
	
	return eRetVal;
}

eTLV493DReturn_t TLV493DReadRegisters(sTLV493DDev_t *pDev, int16_t *pnXVal, int16_t *pnYVal, int16_t *pnZVal, int16_t *pnTempVal) {
	eI2CReturns_t eResult;
	eTLV493DReturn_t eRetVal = TLV493D_Success;
	uint8_t nBytesRead, anReadData[TLV493DReg_RdCount];
	
	//Read the data
	eResult = pDev->pI2C->pfI2CReadData(pDev->pI2C, pDev->nI2CAddr, TLV493DReg_RdCount, anReadData, &nBytesRead);
	if ((eResult != I2C_Success) && (nBytesRead != TLV493DReg_RdCount)) {
		pDev->eLastI2CErr = eResult;
		return TLV493DFail_BusError;
	}
	
	//Parse out the readings
	(*pnXVal) = (anReadData[TLV493DReg_RdXHigh] & TLV493DHigh_XYZMask) << TLV493DHigh_XYZLShift;
	(*pnXVal) |= (anReadData[TLV493DReg_RdXYLow] & TLV493DLow_XMask) >> TLV493DLow_XRShift;
	
	(*pnYVal) = (anReadData[TLV493DReg_RdYHigh] & TLV493DHigh_XYZMask) << TLV493DHigh_XYZLShift;
	(*pnYVal) |= (anReadData[TLV493DReg_RdXYLow] & TLV493DLow_YMask) >> TLV493DLow_YRShift;
	
	(*pnZVal) = (anReadData[TLV493DReg_RdZHigh] & TLV493DHigh_XYZMask) << TLV493DHigh_XYZLShift;
	(*pnZVal) |= (anReadData[TLV493DReg_RdZLow] & TLV493DLow_ZMask) >> TLV493DLow_ZRShift;
	
	(*pnTempVal) = (anReadData[TLV493DReg_RdTempHigh] & TLV493DHigh_TempTempMask) << TLV493DHigh_TempLShift;
	(*pnTempVal) |= (anReadData[TLV493DReg_RdTempLow] & TLV493DLow_TempMask) >> TLV493DLow_TempRShift;
	
	//Check if all conversions are complete
	anReadData[TLV493DReg_RdTempHigh] &= TLV493DHigh_TempChMask;
	if (CheckAllBitsInMask(anReadData[TLV493DReg_RdTempHigh], TLV493DHigh_TempChConvX) != true) {
	//if (CheckAllBitsInMask(anReadData[TLV493DReg_RdZLow], TLV493DLow_ZPowerDownFlag) == false) {
		eRetVal = TLV493DWarn_ConvInPg;
	}
	
	//Check if device is reporting any problems
	if ((CheckAllBitsInMask(anReadData[TLV493DReg_RdZLow], TLV493DLow_ZTestMode) == true) || (CheckAllBitsInMask(anReadData[TLV493DReg_RdZLow], TLV493DLow_ZFuseFlag) == false)) {
		eRetVal = TLV493DFail_DeviceErr;
	}
	
	return eRetVal;
}

eTLV493DReturn_t TLV493DWriteRegisters(sTLV493DDev_t *pDev, eTLV493DMode1_t eMode1, eTLV493DMode2_t eMode2) {
	uint8_t anData[TLV493DReg_WrCount];
	uint32_t nParTest;
	eI2CReturns_t eResult;
	
	anData[TLV493DReg_WrRes1] = 0x00; //First reserved register ust always be 0x00
	anData[TLV493DReg_WrMode1] = eMode1 & (~TLV493DM1_ParitySet); //Leave out the parity bit, that will be set later
	anData[TLV493DReg_WrRes2] = pDev->nRdReg8; //Reserved 2 must be a value read from the chip
	anData[TLV493DReg_WrMode2] = eMode2 & (~TLV493DM2_Rd9Mask); //Only include the valid mode bits
	anData[TLV493DReg_WrMode2] |= pDev->nRdReg9 & TLV493DM2_Rd9Mask;
	
	//Set the parity bit for odd parity
	nParTest = (anData[0] << 24) || (anData[1] << 16) || (anData[2] << 8) || anData[0];
	nParTest = CountSetBitsInInt32(nParTest);
	
	if ((nParTest % 2) == 0) {
		anData[TLV493DReg_WrMode1] |= TLV493DM1_ParitySet;
	}
	
	//Data should be ready, ship it over
	eResult = pDev->pI2C->pfI2CWriteData(pDev->pI2C, pDev->nI2CAddr, TLV493DReg_WrCount, anData);
	if (eResult != I2C_Success) {
		return TLV493DFail_BusError;
	}
	
	return TLV493D_Success;
}