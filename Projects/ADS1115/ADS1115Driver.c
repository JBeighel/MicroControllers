/*		File:	ADS1115Driver.c
		Author:	J. Beighel
		Date:	10-20-2020
*/


/*****	Includes	*****/
	#include "ADS1115Driver.h"

/*****	Constants	*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	eADS1115Return_t ADS1115ReadRegister(sADS1115Dev_t *pDev, eADS1115Reg_t eRegAddr, uint16_t *pnRegValue);
	
	eADS1115Return_t ADS1115WriteRegister(sADS1115Dev_t *pDev, eADS1115Reg_t eRegAddr, uint16_t nRegValue);

/*****	Functions	*****/

eADS1115Return_t ADS1115Init(sADS1115Dev_t *pDev, sGPIOIface_t *pGpio, sI2CIface_t *pI2C, eADS1115I2CAddr_t eI2CAddr, uint16_t nAlertRdyPin) {
	uint16_t nRead;
	eADS1115Return_t eResult;
	
	//Setup the object
	pDev->nI2CAddr = eI2CAddr;
	pDev->nAlertPin = nAlertRdyPin;
	pDev->pI2C = pI2C;
	pDev->pGpio = pGpio;
	pDev->nConfig = ADS1115Cfg_MUXA0toA1 | ADS1115Cfg_PGA2048 | ADS1115Cfg_ModeSingle | ADS1115Cfg_DR128SPS | ADS1115Cfg_CompMdTrad | ADS1115Cfg_CompQDis;
	
	//Configure the pins
	pDev->pGpio->pfSetModeByPin(pDev->pGpio, pDev->nAlertPin, GPIO_DigitalInput);
	
	//Configure the peripheral
	eResult = ADS1115WriteRegister(pDev, ADS1115_Config, pDev->nConfig);
	if (eResult != ADS1115_Success) {
		return eResult;
	}
	
	eResult = ADS1115ReadRegister(pDev, ADS1115_Config, &nRead);
	if (eResult != ADS1115_Success) {
		return eResult;
	} else if (pDev->nConfig | ADS1115Cfg_OpStatNoConv != nRead) {
		return ADS1115Fail_NoConfig;
	}
	
	return ADS1115_Success;
}

eADS1115Return_t ADS1115TakeSingleReading(sADS1115Dev_t *pDev, int16_t *pnVal) {
	eADS1115Return_t eResult;
	uint16_t nRead, nTimeout;
	
	//Write the config to perform a conversion
	eResult = ADS1115WriteRegister(pDev, ADS1115_Config, pDev->nConfig | ADS1115Cfg_OpStatStConv);
	
	//wait for the conversion to finish
	nTimeout = 0;
	eResult = ADS1115ReadRegister(pDev, ADS1115_Config, &nRead);
	while ((eResult == ADS1115_Success) && (CheckAllBitsInMask(nRead, ADS1115Cfg_OpStatNoConv) == false) && (nTimeout < 100)) {
		eResult = ADS1115ReadRegister(pDev, ADS1115_Config, &nRead);
		nTimeout += 1;
	}
	
	if (eResult != ADS1115_Success) {
		return eResult;
	} else if (nTimeout >= 100) {
		return ADS1115Fail_Timeout;
	}
	
	//Read the conversion results
	eResult = ADS1115ReadRegister(pDev, ADS1115_Conversion, (uint16_t *)pnVal);
	if (eResult != ADS1115_Success) {
		return eResult;
	}
	
	return ADS1115_Success;
}

eADS1115Return_t ADS1115SetMeasurementInput(sADS1115Dev_t *pDev, eADS1115Measurement_t eMeas) {
	eADS1115Config_t eCfg;
	uint16_t nRead;
	eADS1115Return_t eResult;
	
	switch (eMeas) {
		ADS1115_MeasA0ToA1 :
			eCfg = ADS1115Cfg_MUXA0toA1;
			break;
		ADS1115_MeasA0ToA3 :
			eCfg = ADS1115Cfg_MUXA0toA3;
			break;
		ADS1115_MeasA1ToA3 :
			eCfg = ADS1115Cfg_MUXA1toA3;
			break;
		ADS1115_MeasA2ToA3 :
			eCfg = ADS1115Cfg_MUXA2toA3;
			break;
		ADS1115_MeasA0ToGnd :
			eCfg = ADS1115Cfg_MUXA0toGnd;
			break;
		ADS1115_MeasA1ToGnd :
			eCfg = ADS1115Cfg_MUXA1toGnd;
			break;
		ADS1115_MeasA2ToGnd :
			eCfg = ADS1115Cfg_MUXA2toGnd;
			break;
		ADS1115_MeasA3ToGnd :
			eCfg = ADS1115Cfg_MUXA3toGnd;
			break;
		default :
			return ADS1115Fail_Invalid;
	}
	
	//Clear out old MUX setting and add in new one
	pDev->nConfig &= ~ADS1115Cfg_MUXMask;
	pDev->nConfig |= eCfg;
	
	//Write the settings to the ADC
	eResult = ADS1115WriteRegister(pDev, ADS1115_Config, pDev->nConfig);
	if (eResult != ADS1115_Success) {
		return eResult;
	}
	
	eResult = ADS1115ReadRegister(pDev, ADS1115_Config, &nRead);
	if (eResult != ADS1115_Success) {
		return eResult;
	} else if (pDev->nConfig | ADS1115Cfg_OpStatNoConv != nRead) {
		return ADS1115Fail_NoConfig;
	}
	
	return ADS1115_Success;
}

eADS1115Return_t ADS1115ReadRegister(sADS1115Dev_t *pDev, eADS1115Reg_t eRegAddr, uint16_t *pnRegValue) {
	eI2CReturns_t eResult;
	uint8_t nBytesRead, anReadData[2];
	
	//Send the register to read from
	eResult = pDev->pI2C->pfI2CWriteData(pDev->pI2C, pDev->nI2CAddr, 1, (uint8_t *)&eRegAddr);
	if (eResult != I2C_Success) {
		return ADS1115Fail_BusError;
	}
	
	//Read the data
	eResult = pDev->pI2C->pfI2CReadData(pDev->pI2C, pDev->nI2CAddr, 2, anReadData, &nBytesRead);
	if ((eResult != I2C_Success) && (nBytesRead != 1)) {
		return ADS1115Fail_BusError;
	}
	
	(*pnRegValue) = anReadData[0] << 8;
	(*pnRegValue) |= anReadData[1];
	
	return ADS1115_Success;
}

eADS1115Return_t ADS1115WriteRegister(sADS1115Dev_t *pDev, eADS1115Reg_t eRegAddr, uint16_t nRegValue) {
	uint8_t anData[3];
	eI2CReturns_t eResult;
	
	anData[0] = (uint8_t)eRegAddr;
	anData[1] = nRegValue >> 8;
	anData[2] = nRegValue & 0xFF;
	
	eResult = pDev->pI2C->pfI2CWriteData(pDev->pI2C, pDev->nI2CAddr, 3, anData);
	if (eResult != I2C_Success) {
		return ADS1115Fail_BusError;
	}
	
	return ADS1115_Success;
}