/*
		File:	SPIGeneralInterface.c
		Author:	J. Beighel
		Created:09-04-2020
*/

/***** Includes		*****/
	#include "SPIGeneralInterface.h"

/***** Definitions	*****/
	

/***** Constants	*****/


/***** Globals		*****/


/***** Prototypes 	*****/


/***** Functions	*****/
eSPIReturn_t SPIInterfaceInitialize(sSPIIface_t *pIface) {
	pIface->pfInitialise = &SPIPortInitialize;
	pIface->pfBeginTransfer = &SPIBeginTransfer;
	pIface->pfEndTransfer = &SPIEndTransfer;
	pIface->pfTransferByte = &SPITransferByte;
	pIface->pfTransfer2Bytes = &SPITransfer2Bytes;
	pIface->pfGetCapabilities = &SPIGetCapabilities;

	pIface->nBusClockFreq = 5000000;
	pIface->pHWInfo = NULL;
	pIface->eMode = SPI_Mode0;
	pIface->eDataOrder = SPI_MSBFirst;
	
	return SPI_Success;
}

eSPIReturn_t SPIPortInitialize(sSPIIface_t *pIface, void *pHWInfo, uint32_t nBusClockFreq, eSPIDataOrder_t eDataOrder, eSPIMode_t eMode) {
	return SPIFail_Unsupported;
}

eSPIReturn_t SPIBeginTransfer(sSPIIface_t *pIface) {
	return SPIFail_Unsupported;
}

eSPIReturn_t SPIEndTransfer(sSPIIface_t *pIface) {
	return SPIFail_Unsupported;
}
	
eSPIReturn_t SPITransferByte(sSPIIface_t *pIface, uint8_t nSendByte, uint8_t *pnReadByte) {
	return SPIFail_Unsupported;
}

eSPIReturn_t SPITransfer2Bytes(sSPIIface_t *pIface, const uint8_t *anSendBytes, uint8_t *anReadBytes) {
	return SPIFail_Unsupported;
}

eSPICapabilities_t SPIGetCapabilities(sSPIIface_t *pIface) {
	return SPI_NoCapabilities;
}


