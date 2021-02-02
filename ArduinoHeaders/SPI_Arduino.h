/**	@defgroup	spiarduino
	@brief		Arduino implementation of the SPI General Interface
	@details	v0.2
		
*/

#ifndef __SPIARDUINO
	#define __SPIARDUINO

/***** Includes		*****/
	#include <SPI.h>
	
	#include "SPIGeneralInterface.h"
	

/*****	Constants	*****/
	#define		SPI_1_HWINFO	(&gSPI1)
	
	#define		SPI_INIT		SPIArduinoInit

	#define		BUILD_DEBUG		1
	#ifdef BUILD_DEBUG
		#define	DEBUG_PRINT	Serial.print
	#endif


/***** Definitions	*****/
	typedef struct sArduinoSPI_t {
		SPISettings Settings;
		SPIClass *pSPI;
	} sArduinoSPI_t;

/***** Constants	*****/
	sArduinoSPI_t gSPI1 = { .Settings = SPISettings(), .pSPI = &SPI };

/***** Globals		*****/


/***** Prototypes 	*****/
	eSPIReturn_t SPIArduinoInit(sSPIIface_t *pIface, void *pHWInfo, uint32_t nBusClockFreq, eSPIDataOrder_t eDataOrder, eSPIMode_t eMode);
	
	eSPIReturn_t SPIArduinoBeginTrans(sSPIIface_t *pIface);
	eSPIReturn_t SPIArduinoEndTrans(sSPIIface_t *pIface);
		
	eSPIReturn_t SPIArduinoTransByte(sSPIIface_t *pIface, uint8_t nSendByte, uint8_t *pnReadByte);

/***** Functions	*****/

eSPIReturn_t SPIArduinoInit(sSPIIface_t *pIface, void *pHWInfo, uint32_t nBusClockFreq, eSPIDataOrder_t eDataOrder, eSPIMode_t eMode) {
	BitOrder nBitOrder;
	uint8_t nDataMode;
	sArduinoSPI_t *pSPIObj = (sArduinoSPI_t *)pHWInfo;
	
	SPIInterfaceInitialize(pIface); //Make sure the object has sane values

	//Setup supported functions
	pIface->pfInitialise = &SPIArduinoInit;
	pIface->pfBeginTransfer = &SPIArduinoBeginTrans;
	pIface->pfEndTransfer = &SPIArduinoEndTrans;
	pIface->pfTransferByte = &SPIArduinoTransByte;
	
	//Copy settigns into the object
	pIface->nBusClockFreq = nBusClockFreq;
	pIface->pHWInfo = pHWInfo;
	pIface->eMode = eMode;
	pIface->eDataOrder = eDataOrder;
	
	//Configure the port for use
	switch (pIface->eDataOrder) {
		case SPI_MSBFirst:
			nBitOrder = MSBFIRST;
			break;
		
		case SPI_LSBFirst:
			nBitOrder = LSBFIRST;
			break;
			
		default: //Unsupported bit order
			return SPIFail_Unsupported;
	}
	
	switch (pIface->eMode) {
		case SPI_Mode0:
			nDataMode = SPI_MODE0;
			break;
			
		case SPI_Mode1:
			nDataMode = SPI_MODE1;
			break;
			
		case SPI_Mode2:
			nDataMode = SPI_MODE2;
			break;
			
		case SPI_Mode3:
			nDataMode = SPI_MODE3;
			break;
			
		default: //Unsupported data mode
			return SPIFail_Unsupported;
	}
	
	pSPIObj->Settings = SPISettings(nBusClockFreq, nBitOrder, nDataMode);
	pSPIObj->pSPI->begin();
	
	return SPI_Success;
}
	
eSPIReturn_t SPIArduinoBeginTrans(sSPIIface_t *pIface) {
	sArduinoSPI_t *pSPIObj = (sArduinoSPI_t *)pIface->pHWInfo;
	
	pSPIObj->pSPI->beginTransaction(pSPIObj->Settings);
	
	return SPI_Success;
}

eSPIReturn_t SPIArduinoEndTrans(sSPIIface_t *pIface) {
	sArduinoSPI_t *pSPIObj = (sArduinoSPI_t *)pIface->pHWInfo;
	
	pSPIObj->pSPI->endTransaction();
	
	return SPI_Success;
}
	
eSPIReturn_t SPIArduinoTransByte(sSPIIface_t *pIface, uint8_t nSendByte, uint8_t *pnReadByte) {
	sArduinoSPI_t *pSPIObj = (sArduinoSPI_t *)pIface->pHWInfo;
	
	(*pnReadByte) = pSPIObj->pSPI->transfer(nSendByte);
	
	return SPI_Success;
}

#endif

