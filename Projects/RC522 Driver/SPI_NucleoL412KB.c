/*
		File:	SPI_NucleoL412KB.c
		Author:	J. Beighel
		Created:09-04-2020
*/

/*****	Includes	*****/
	#include "SPI_NucleoL412KB.h"

/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/


/*****	Functions	*****/

eSPIReturn_t NucleoSPIPortInitialize(sSPIIface_t *pIface, void *pHWInfo, uint32_t nBusClockFreq, eSPIDataOrder_t eDataOrder, eSPIMode_t eMode) {
	SPIInterfaceInitialize(pIface);  //Sanitize the object before setting it up

	//Copy in config values, the Cube sets them so they aren't adjustable
	pIface->nBusClockFreq = nBusClockFreq;
	pIface->eDataOrder = eDataOrder;
	pIface->eMode = eMode;
	pIface->pHWInfo = pHWInfo;

	//Setup all the interface functions
	pIface->pfGetCapabilities = &NucleoSPIGetCapabilities;
	pIface->pfInitialise = &NucleoSPIPortInitialize;
	pIface->pfBeginTransfer = &NucleoSPIBeginTransfer;
	pIface->pfEndTransfer = &NucleoSPIEndTransfer;
	pIface->pfTransferByte = &NucleoSPITransferByte;
	pIface->pfTransfer2Bytes = &NucleoSPITransfer2Bytes;

	//The cube code sets up the port, no further work to be done
	return SPI_Success;
}

eSPIReturn_t NucleoSPIBeginTransfer(sSPIIface_t *pIface) {
	//SPI Transfer byte handles the start and end work
	return SPI_Success;
}

eSPIReturn_t NucleoSPIEndTransfer(sSPIIface_t *pIface) {
	//SPI Transfer byte handles the start and end work
	return SPI_Success;
}

eSPIReturn_t NucleoSPITransferByte(sSPIIface_t *pIface, uint8_t nSendByte, uint8_t *pnReadByte) {
	HAL_StatusTypeDef eRetVal;

	eRetVal = HAL_SPI_TransmitReceive(pIface->pHWInfo, &nSendByte, pnReadByte, 1, SPI_TIMEOUT);
	if (eRetVal == HAL_OK) {
		return SPI_Success;
	} else if (eRetVal == HAL_TIMEOUT) {
		return SPIFail_Timeout;
	} else {
		return SPIFail_Unknown;
	}
}

eSPIReturn_t NucleoSPITransfer2Bytes(sSPIIface_t *pIface, const uint8_t *anSendBytes, uint8_t *anReadBytes) {
	HAL_StatusTypeDef eRetVal;
	uint8_t anData[2];

	anData[0] = anSendBytes[0];
	anData[1] = anSendBytes[1];

	eRetVal = HAL_SPI_TransmitReceive(pIface->pHWInfo, anData, anReadBytes, 2, SPI_TIMEOUT);
	if (eRetVal == HAL_OK) {
		return SPI_Success;
	} else if (eRetVal == HAL_TIMEOUT) {
		return SPIFail_Timeout;
	} else {
		return SPIFail_Unknown;
	}
}

eSPICapabilities_t NucleoSPIGetCapabilities(sSPIIface_t *pIface) {
	return SPI_1_CAPS;
}
