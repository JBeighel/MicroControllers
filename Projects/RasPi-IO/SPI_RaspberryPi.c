/**	File:	SPI_RaspberryPi.c
	Author:	J. Beighel
	Date:	12-01-2020
*/

/*****	Includes	*****/
	#include "SPI_RaspberryPi.h"

/*****	Defines		*****/


/*****	Definitions	*****/


/*****	Constants	*****/
	/**	@brief		Path and file name of the filesystem object for the SPI bus
		@ingroup	spiraspberrypi
	*/
	const char cgSPI1File[] = "/dev/spi-1";

	/**	@brief		Array of all SPI buses for the Raspberry Pi
		@ingroup	spiraspberrypi
	*/
	sRasPiI2CHWInfo_t gSPIHWInfo [] = {
		{ .pcFilePath = cgSPI1File, .SPIFile = -1, },
	};

/*****	Globals		*****/


/*****	Prototypes 	*****/
	eSPIReturn_t RasPiSPIBeginTransfer(sSPIIface_t *pIface);
	
	eSPIReturn_t RasPiSPIEndTransfer(sSPIIface_t *pIface);
	
	eSPIReturn_t RasPiSPITransferByte(sSPIIface_t *pIface, uint8_t nSendByte, uint8_t *pnReadByte);

/*****	Functions	*****/

eSPIReturn_t RasPiSPIPortInitialize(sSPIIface_t *pIface, void *pHWInfo, uint32_t nBusClockFreq, eSPIDataOrder_t eDataOrder, eSPIMode_t eMode) {
	sRasPiSPIHWInfo_t *pSPI = (sRasPiSPIHWInfo_t *)pHWInfo;
	int32_t nResult;
	uint32_t nSpiMode;
	uint8_t nBits = 8;
	
	//Setup the interface structure
	SPIInterfaceInitialize(pIface);
	
	pIface->nBusClockFreq = nBusClockFreq;
	pIface->eDataOrder = eDataOrder;
	pIface->eMode = eMode;
	pIface->pHWInfo = pHWInfo;
	
	//Se all the interface functions
	pIface->pfInitialise = &RasPiSPIPortInitialize;
	
	//Set up the hardware
	pSPI->SPIFile = open(pSPI->pcFilePath, O_RDWR);
	
	if (pSPI->SPIFile < 0) {
		pSPI->nLastErr = errno;
		return SPIFail_Unknown;
	}
	
	//Set mode, byte order, etc....
	nResult = ioctl(pSPI->SPIFile, SPI_IOC_WR_MODE32, &mode); mode = (SPI_CPHA | SPI_CPOL | SPI_LSB_FIRST | SPI_CS_HIGH | SPI_3WIRE | SPI_NO_CS | SPI_LOOP)
	if (nResult == -1) {
		pSPI->nLastErr = errno;
		return SPIFail_Unknown;
	}
	
	nResult = ioctl(pSPI->SPIFile, SPI_IOC_WR_BITS_PER_WORD, &bits = 8);
	if (nResult == -1) {
		pSPI->nLastErr = errno;
		return SPIFail_Unknown;
	}

	nResult = ioctl(pSPI->SPIFile, SPI_IOC_RD_BITS_PER_WORD, &bits = 8);
	if (nResult == -1) {
		pSPI->nLastErr = errno;
		return SPIFail_Unknown;
	}
	
	nResult = ioctl(pSPI->SPIFile, SPI_IOC_WR_MAX_SPEED_HZ, nBusClockFreq);
	if (nResult == -1) {
		pSPI->nLastErr = errno;
		return SPIFail_Unknown;
	}

	nResult = ioctl(pSPI->SPIFile, SPI_IOC_RD_MAX_SPEED_HZ, nBusClockFreq);
	if (nResult == -1) {
		pSPI->nLastErr = errno;
		return SPIFail_Unknown;
	}
	
	return SPI_Success;
}

eSPIReturn_t RasPiSPIBeginTransfer(sSPIIface_t *pIface) {
	sRasPiSPIHWInfo_t *pSPI = (sRasPiSPIHWInfo_t *)(pIface->pHWInfo);
	
}
	
eSPIReturn_t RasPiSPIEndTransfer(sSPIIface_t *pIface) {
	sRasPiSPIHWInfo_t *pSPI = (sRasPiSPIHWInfo_t *)(pIface->pHWInfo);
	
}

eSPIReturn_t RasPiSPITransferByte(sSPIIface_t *pIface, uint8_t nSendByte, uint8_t *pnReadByte) {
	sRasPiSPIHWInfo_t *pSPI = (sRasPiSPIHWInfo_t *)(pIface->pHWInfo);
	
}