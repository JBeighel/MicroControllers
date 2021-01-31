/**	File:	MAX7219Driver.c
	Author:	J. Beighel
	Date:	2021-01-26
*/

/*****	Includes	*****/
	#include "MAX7219Driver.h"

/*****	Defines		*****/
	

/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
eMAX7219Return_t MAX7219WriteValue(sMAX7219Info_t *pDev, eMAX7219Reg_t nReg, uint8_t nValue);

/*****	Functions	*****/

eMAX7219Return_t MAX7219Initialize(sMAX7219Info_t *pDev, uint8_t nNumDigits, GPIOID_t nChipSel, sGPIOIface_t *pGpioDev, sSPIIface_t *pSpiDev) {
	//Setup device object
	pDev->pGpio = pGpioDev;
	pDev->pSpi = pSpiDev;
	pDev->nChipSelect = nChipSel;
	
	//Sanitize this input
	nNumDigits &= MAX7219SCAN_Mask;
	
	//Setup the hardware
	pDev->pGpio->pfSetModeByPin(pDev->pGpio, pDev->nChipSelect, GPIO_DigitalOutput);
	pDev->pGpio->pfDigitalWriteByPin(pDev->pGpio, pDev->nChipSelect, true);
	
	//Assume the device is just powering up, and set all registers
	MAX7219WriteValue(pDev, MAX7219_SCANLIMIT, nNumDigits - 1);
	MAX7219WriteValue(pDev, MAX7219_DECODEMODE, MAX7219_DecodeNone);
	MAX7219WriteValue(pDev, MAX7219_SHUTDOWN, MAX7219SHUTDWN_Norm);
	MAX7219WriteValue(pDev, MAX7219_INTENSITY, MAX7219INTEN_Min);
	MAX7219WriteValue(pDev, MAX7219_DISPLAYTEST, MAX7219DISPTST_Off);
	
	return Success;
}

eMAX7219Return_t MAX7219SetDigitLEDs(sMAX7219Info_t *pDev, uint8_t nDigit, eMAX7219Led_t eLights) {
	if (nDigit >= MAX7219_MAXDIGITS) {
		return Fail_Invalid;
	} else {
		nDigit += 1; //Switch form zero based to one based indexing
	}
	
	MAX7219WriteValue(pDev, (eMAX7219Reg_t)nDigit, (uint8_t)eLights);
	
	return Success;
}

eMAX7219Return_t MAX7219SetIntensity(sMAX7219Info_t *pDev, uint8_t nLevel) {
	if (nLevel < MAX7219INTEN_Min) {
		nLevel = MAX7219INTEN_Min;
	} else if (nLevel > MAX7219INTEN_Max) {
		nLevel = MAX7219INTEN_Max;
	}
	
	return MAX7219WriteValue(pDev, MAX7219_INTENSITY, nLevel);
}


eMAX7219Return_t MAX7219WriteValue(sMAX7219Info_t *pDev, eMAX7219Reg_t nReg, uint8_t nValue) {
	uint8_t nByte;
	
	//Begin the transfer
	pDev->pGpio->pfDigitalWriteByPin(pDev->pGpio, pDev->nChipSelect, false);
	pDev->pSpi->pfBeginTransfer(pDev->pSpi);
	
	//Send register address, then value
	pDev->pSpi->pfTransferByte(pDev->pSpi, nReg, &nByte);
	pDev->pSpi->pfTransferByte(pDev->pSpi, nValue, &nByte);
	
	//End the transfer
	pDev->pSpi->pfEndTransfer(pDev->pSpi);
	pDev->pGpio->pfDigitalWriteByPin(pDev->pGpio, pDev->nChipSelect, true);
	
	return Success;
}
