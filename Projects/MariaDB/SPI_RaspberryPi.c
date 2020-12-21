/**	File:	SPI_RaspberryPi.c
	Author:	J. Beighel
	Date:	12-01-2020
*/

/*****	Includes	*****/
	#include "SPI_RaspberryPi.h"

/*****	Defines		*****/


/*****	Definitions	*****/


/*****	Constants	*****/
	/**	@brief		Path and file name of the filesystem object for the SPI 1 bus
		@ingroup	spiraspberrypi
	*/
	const char cgSPI1File[] = "/dev/spidev0.0";
	
	/**	@brief		Path and file name of the filesystem object for the SPI 2 bus
		@details	This SPI bus is only available on the 40 pin header
		@ingroup	spiraspberrypi
	*/
	const char cgSPI2File[] = "/dev/spidev0.1";

	/**	@brief		Array of all SPI buses for the Raspberry Pi
		@ingroup	spiraspberrypi
	*/
	sRasPiSPIHWInfo_t gSPIHWInfo [] = {
		{ .pcFilePath = cgSPI1File, .SPIFile = -1, },
		{ .pcFilePath = cgSPI2File, .SPIFile = -1, },
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
	uint32_t nSpiMode = 0;
	uint8_t nBits = RASPISPI_BITSPERWORD;
	eSPIReturn_t eRetVal = SPI_Success;
	
	//Setup the interface structure
	SPIInterfaceInitialize(pIface);
	
	pIface->nBusClockFreq = nBusClockFreq;
	pIface->eDataOrder = eDataOrder;
	pIface->eMode = eMode;
	pIface->pHWInfo = pHWInfo;
	
	//Se all the interface functions
	pIface->pfInitialise = &RasPiSPIPortInitialize;
	pIface->pfBeginTransfer = &RasPiSPIBeginTransfer;
	pIface->pfEndTransfer = &RasPiSPIEndTransfer;
	pIface->pfTransferByte = &RasPiSPITransferByte;
	
	//Set up the hardware
	pSPI->SPIFile = open(pSPI->pcFilePath, O_RDWR);
	
	if (pSPI->SPIFile < 0) {
		pSPI->nLastErr = errno;
		return SPIFail_Unknown;
	}
	
	//Determine the SPI mode settigns
	//Unused flags: SPI_LOOP, SPI_CS_HIGH, SPI_TX_OCTAL, SPI_TX_QUAD, SPI_TX_DUAL
	//              SPI_RX_OCTAL, SPI_RX_QUAD, SPI_RX_DUAL, SPI_3WIRE
	nSpiMode = SPI_NO_CS;
	
	switch (eMode) {
		case SPI_Mode0:
			break;
		case SPI_Mode1:
			nSpiMode |= SPI_CPHA;
			break;
		case SPI_Mode2:
			nSpiMode |= SPI_CPOL;
			break;
		case SPI_Mode3:
			nSpiMode |= SPI_CPHA | SPI_CPOL;
			break;
		default :
			return SPIFail_Unsupported;
	}
	
	if (eDataOrder == SPI_LSBFirst) {
		nSpiMode |= SPI_LSB_FIRST;
	}
	
	//Apply the settints to read and write
	nResult = ioctl(pSPI->SPIFile, SPI_IOC_WR_MODE32, &nSpiMode);
	if (nResult == -1) {
		pSPI->nLastErr = errno;
		eRetVal = SPIFail_Unknown;
	}
	
	nResult = ioctl(pSPI->SPIFile, SPI_IOC_RD_MODE32, &nSpiMode);
	if (nResult == -1) {
		pSPI->nLastErr = errno;
		eRetVal = SPIFail_Unknown;
	}
	
	//Read and write have the same word size
	nResult = ioctl(pSPI->SPIFile, SPI_IOC_WR_BITS_PER_WORD, &nBits);
	if (nResult == -1) {
		pSPI->nLastErr = errno;
		eRetVal = SPIFail_Unknown;
	}

	nResult = ioctl(pSPI->SPIFile, SPI_IOC_RD_BITS_PER_WORD, &nBits);
	if (nResult == -1) {
		pSPI->nLastErr = errno;
		eRetVal = SPIFail_Unknown;
	}
	
	//Set the clock frequency
	nResult = ioctl(pSPI->SPIFile, SPI_IOC_WR_MAX_SPEED_HZ, &nBusClockFreq);
	if (nResult == -1) {
		pSPI->nLastErr = errno;
		eRetVal = SPIFail_Unknown;
	}

	nResult = ioctl(pSPI->SPIFile, SPI_IOC_RD_MAX_SPEED_HZ, &nBusClockFreq);
	if (nResult == -1) {
		pSPI->nLastErr = errno;
		eRetVal = SPIFail_Unknown;
	}
	
	//If anything failed close the file
	if (eRetVal != SPI_Success) {
		close(pSPI->SPIFile);
		pSPI->SPIFile = -1;
	}
	
	return eRetVal;
}

eSPIReturn_t RasPiSPIBeginTransfer(sSPIIface_t *pIface) {
	// No work to do here, just need to include the function
	return SPI_Success;
}
	
eSPIReturn_t RasPiSPIEndTransfer(sSPIIface_t *pIface) {
	// No work to do here, just need to include the function
	return SPI_Success;
}

eSPIReturn_t RasPiSPITransferByte(sSPIIface_t *pIface, uint8_t nSendByte, uint8_t *pnReadByte) {
	sRasPiSPIHWInfo_t *pSPI = (sRasPiSPIHWInfo_t *)(pIface->pHWInfo);
	struct spi_ioc_transfer TranInfo;
	int32_t nResult;
	
	memset(&TranInfo, 0, sizeof(struct spi_ioc_transfer));
	
	TranInfo.tx_buf = (uint32_t)(&nSendByte);
	TranInfo.rx_buf = (uint32_t)pnReadByte;
	TranInfo.len = 1; //This function sends 1 byte
	TranInfo.delay_usecs = 0; //Delay between CS and data transfer
	//TranInfo.speed_hz = pIface->nBusClockFreq;
	TranInfo.bits_per_word = RASPISPI_BITSPERWORD;

	TranInfo.tx_nbits = 1; //1 data out line
	TranInfo.rx_nbits = 1; //1 data in line
	
	//This is really only valid if multiple transfers are queued
	//TranInfo.cs_change = true; //true deselcts the device before next transfer
	//This call performs the data transfer using the provided buffers
	nResult = ioctl(pSPI->SPIFile, SPI_IOC_MESSAGE(1), &TranInfo);
	if (nResult <= 0) {
		pSPI->nLastErr = errno;
		return SPIFail_Unknown;
	}
	
	return SPI_Success;
}