/*	v0.2
	File:	SPIGeneralInterface.c
	Author:	J. Beighel
	Date:	08-27-2020
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
