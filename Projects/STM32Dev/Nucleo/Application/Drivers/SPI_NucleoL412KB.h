/**	@defgroup	spinucleo
	@brief		SPI General Interface Implementation for ST Nucleo Boards
	@details	v0.1
	#Description
		In the CubeMX it has Clock Polarity (CPOL) where true has the clock remain
		high and start with a falling edge.  Clock Phase (CPHA) has 1 Edge and 2
		Edge options, meaning the first or second edge is used for data capture.
		CPOL	CPHA	Mode
		False	2 Edge	0
		False	1 Edge	1
		True	2 Edge	3
		True	1 Edge	4
	#File Information
		File:	SPI_NucleoL412KB.h
		Author:	J. Beighel
		Date:	01-03-2021
*/

#ifndef __SPI_NUCLEOL412KB_H
	#define __SPI_NUCLEOL412KB_H

/*****	Includes	*****/
	#include "spi.h"

	#include "SPIGeneralInterface.h"

/*****	Defines		*****/
	#define SPI_1_HWINFO	((void *)&hspi1)

	#define SPI_INIT		NucleoInitializeSPIBus

	#define SPI_CAPS		((eSPICapabilities_t)SPI_BeginTransfer | SPI_EndTransfer | SPI_BiDir1Byte)

	#define I2C_TIMEOUT		100

/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	eSPIReturn_t NucleoInitializeSPIBus(sSPIIface_t *pIface, void *pHWInfo, uint32_t nBusClockFreq, eSPIDataOrder_t eDataOrder, eSPIMode_t eMode);

	eSPICapabilities_t NucleoGetCapabilities(sSPIIface_t *pIface);

	eSPIReturn_t NucleoBeginTransfer(sSPIIface_t *pIface);
	eSPIReturn_t NucleoEndTransfer(sSPIIface_t *pIface);

	eSPIReturn_t NucleoTransferByte(sSPIIface_t *pIface, uint8_t nSendByte, uint8_t *pnReadByte);

/*****	Functions	*****/


#endif
