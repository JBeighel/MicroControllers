/**	File:	SPI_Nucleo412KB.c
	Author:	J. Beighel
	Date:	01-03-2021
*/

/*****	Includes	*****/
	#include "SPI_NucleoL412KB.h"

/*****	Defines		*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/


/*****	Functions	*****/

eSPIReturn_t NucleoInitializeSPIBus(sSPIIface_t *pIface, void *pHWInfo, uint32_t nBusClockFreq, eSPIDataOrder_t eDataOrder, eSPIMode_t eMode) {
	SPIInterfaceInitialize(pIface);

	pIface->pfInitialise = &NucleoInitializeSPIBus;
	pIface->pfBeginTransfer = &NucleoBeginTransfer;
	pIface->pfEndTransfer = &NucleoEndTransfer;
	pIface->pfTransferByte = NucleoTransferByte;
	pIface->pfGetCapabilities = &NucleoGetCapabilities;

	pIface->eMode = eMode;
	pIface->eDataOrder = eDataOrder;
	pIface->nBusClockFreq = nBusClockFreq;
	pIface->pHWInfo = pHWInfo;

	return SPI_Success;
}

eSPICapabilities_t NucleoGetCapabilities(sSPIIface_t *pIface) {
	return SPI_CAPS;
}

eSPIReturn_t NucleoBeginTransfer(sSPIIface_t *pIface) {
	//No work is needed to begin a transfer
	return SPI_Success;
}

eSPIReturn_t NucleoEndTransfer(sSPIIface_t *pIface) {
	//No work is needed to end a transfer
	return SPI_Success;
}

eSPIReturn_t NucleoTransferByte(sSPIIface_t *pIface, uint8_t nSendByte, uint8_t *pnReadByte) {
	HAL_StatusTypeDef eResult;

	eResult = HAL_SPI_TransmitReceive(pIface->pHWInfo, &nSendByte, pnReadByte, 1, I2C_TIMEOUT);

	if (eResult == HAL_OK) {
		return SPI_Success;
	} else {
		return SPIFail_Unknown;
	}
}
