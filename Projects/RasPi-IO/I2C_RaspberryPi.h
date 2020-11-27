/**	@defgroup	i2craspberrypi
	@brief		I2C General Interface implementation for Raspberry Pi
	@details	v0.1
	#Description
	
	#File Information
		File:	I2C_RaspberryPi.h
		Author:	J. Beighel
		Date:	11-25-2020
*/

#ifndef __I2CRASPBERRYPI
	#define __I2CRASPBERRYPI

/*****	Includes	*****/
	#include <unistd.h>
	#include <fcntl.h>
	#include <sys/ioctl.h>
	#include <linux/i2c-dev.h>
	
	#include "CommonUtils.h"
	#include "I2CGeneralInterface.h"

/*****	Constants	*****/
	#define I2C_1_PORTINIT		RasPiInitializeI2CBus
	
	#define I2C_1_HWINFO		((void *)&(gI2CHWInfo[0]))

/*****	Definitions	*****/
	typedef struct sRasPiI2CHWInfo_t {
		const char *pcFilePath;
		int32_t I2CFile;
	} sRasPiI2CHWInfo_t;

/*****	Constants	*****/
	const char cgI2C1File[] = "/dev/i2c-1";

	sRasPiI2CHWInfo_t gI2CHWInfo [] = {
		{ .pcFilePath = cgI2C1File, .I2CFile = 0, },
	};

/*****	Globals		*****/
	int32_t gI2CFile;

/*****	Prototypes 	*****/
	eI2CReturns_t RasPiInitializeI2CBus(sI2CIface_t *pI2CIface, bool bActAsMaster, uint32_t nClockFreq, void *pHWInfo);

/*****	Functions	*****/


#endif

