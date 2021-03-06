/**	@defgroup	i2ciface_nucleo
	@brief		I2C General Interface Implementation for ST Nucleo boards
	@details	v0.1
	# Intent #


	# Design #

	#File Information
		File:	I2C_NucleoL412KB.h
		Author:	J. Beighel
		Date:	2021-03-01
*/

/**	@defgroup i2ciface_nucleo_priv : Private Objects
	@brief
	@ingroup i2ciface_nucleo
*/

#ifndef __I2CIFACE_NUCLEO
	#define __I2CIFACE_NUCLEO

/*****	Includes	*****/
	#include <stdint.h>
	#include <stdbool.h>

	#include "main.h"
	#include "stm32l4xx.h"

	#include "I2CGeneralInterface.h"

/*****	Constants	*****/
	#define		I2C_1_HWINFO	(&hi2c1)
	#define		I2C_3_HWINFO	(&hi2c3)
	
	#define		I2C_INIT		NucleoI2CInitialize

	#define		I2C_TIMEOUT		100

/*****	Definitions		*****/


/*****	Globals			*****/


/*****	Prototypes		*****/
	eI2CReturn_t NucleoI2CInitialize(sI2CIface_t *pI2CIface, bool bActAsMaster, uint32_t nClockFreq, void *pHWInfo);
	
	eI2CReturn_t NucleoI2CReadUint8Reg (sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nRegAddr, uint8_t *pnValue);

	eI2CReturn_t NucleoI2CReadData (sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nNumBytes, void *pDataBuff, uint8_t *pnBytesRead);

	eI2CReturn_t NucleoI2CWriteUint8Reg (sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nRegAddr, uint8_t nValue);

	eI2CReturn_t NucleoI2CWriteData (sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nNumBytes, void *pDataBuff);

	eI2CReturn_t NucleoI2CGeneralCall (sI2CIface_t *pI2CIface, uint8_t nValue);
	
/*****	Function Code	*****/

	
#endif
