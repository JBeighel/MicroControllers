/**	@defgroup	spiraspberrypi
	@brief		SPI General Interface implementation for Raspberry Pi
	@details	v0.1
	#Description
	
	#File Information
		File:	SPI_RaspberryPi.h
		Author:	J. Beighel
		Date:	12-01-2020
*/

#ifndef __SPIRASPBERRYPI_H
	#define __SPIRASPBERRYPI_H

/*****	Includes	*****/
	#include <unistd.h>
	#include <fcntl.h>
	#include <errno.h>
	#include <sys/ioctl.h>
	#include <linux/spi/spidev.h>
	
	#include "CommonUtils.h"
	#include "SPIGeneralInterface.h"

/*****	Defines		*****/
	/**	@brief		Function to call to initialize the first SPI bus
		@ingroup	spiraspberrypi
	*/
	#define SPI_1_PORTINIT		RasPiSPIPortInitialize
	
	/**	@brief		Hardware information for the first SPI bus
		@ingroup	spiraspberrypi
	*/
	#define SPI_1_HWINFO		((void *)&(gSPIHWInfo[0]))
	
	#define RASPISPI_BITSPERWORD	8

/*****	Definitions	*****/
	/**	@brief		Structure holding information on the SPI Hardware
		@ingroup	i2craspberrypi
	*/
	typedef struct sRasPiSPIHWInfo_t {
		const char *pcFilePath;	/**< Path to the filesystem object for this bus */
		int32_t SPIFile;		/**< File handle to use when interacting with this bus */
		int32_t nLastErr;
	} sRasPiSPIHWInfo_t;

/*****	Constants	*****/
	extern sRasPiSPIHWInfo_t gSPIHWInfo[];

/*****	Globals		*****/


/*****	Prototypes 	*****/
	eSPIReturn_t RasPiSPIPortInitialize(sSPIIface_t *pIface, void *pHWInfo, uint32_t nBusClockFreq, eSPIDataOrder_t eDataOrder, eSPIMode_t eMode);

/*****	Functions	*****/


#endif

