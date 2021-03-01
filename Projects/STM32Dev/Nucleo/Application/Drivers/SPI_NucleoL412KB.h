/**	@defgroup	spinucleo
	@brief		SPI General Interface Implementation for ST Nucleo Boards
	@details	v0.1
	#Description

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
