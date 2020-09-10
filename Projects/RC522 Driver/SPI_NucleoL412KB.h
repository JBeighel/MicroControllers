/**	@defgroup	spiiface_nucleoL412KB
	@brief
	@details	v0.1
	# Description #

	# Usage #

	# File Information #
		File:	SPI_NucleoL412KB.h
		Author:	J. Beighel
		Created:09-04-2020
*/

#ifndef SPI_NUCLEOL412KB_H_
	#define SPI_NUCLEOL412KB_H_

/*****	Includes	*****/
	//#include <stdint.h>
	//#include <stdbool.h>

	#include "main.h"
	#include "stm32l4xx.h"

	#include "SPIGeneralInterface.h"

/*****	Definitions	*****/
	#define SPI_1_HWINFO	(&hspi1)

	#ifdef SPI_1_PORTINIT
		#undef SPI_1_PORTINIT
	#endif
	#define SPI_1_PORTINIT	NucleoSPIPortInitialize

	#define SPI_1_CAPS		(SPI_BeginTransfer | SPI_EndTransfer | SPI_BiDir1Byte)

	#ifndef mSecDelay
		#define mSecDelay	osDelay
	#endif

	#define SPI_TIMEOUT		100

/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	eSPIReturn_t NucleoSPIPortInitialize(sSPIIface_t *pIface, void *pHWInfo, uint32_t nBusClockFreq, eSPIDataOrder_t eDataOrder, eSPIMode_t eMode);

	eSPIReturn_t NucleoSPIBeginTransfer(sSPIIface_t *pIface);

	eSPIReturn_t NucleoSPIEndTransfer(sSPIIface_t *pIface);

	eSPIReturn_t NucleoSPITransferByte(sSPIIface_t *pIface, uint8_t nSendByte, uint8_t *pnReadByte);

	eSPIReturn_t NucleoSPITransfer2Bytes(sSPIIface_t *pIface, const uint8_t *anSendBytes, uint8_t *anReadBytes);

	eSPICapabilities_t NucleoSPIGetCapabilities(sSPIIface_t *pIface);

/*****	Functions	*****/


#endif
