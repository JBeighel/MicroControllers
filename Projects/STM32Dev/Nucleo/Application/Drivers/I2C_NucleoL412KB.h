/**	@defgroup	i2ciface_nucleo
	@brief		I2C General Interface Implementation for ST Nucleo boards
	@details	v0.3
	# Intent #


	# Design #

	#File Information
		File:	I2C_NucleoL412KB.h
		Author:	J. Beighel
		Date:	2021-05-07
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
	#include "i2c.h"

	#include "I2CGeneralInterface.h"

/*****	Constants	*****/
	#define		I2C_1_HWINFO		(&gSTI2C1)
	#define		I2C_3_HWINFO		(&gSTI2C3)
	
	#define		I2C_INIT			NucleoI2CInitialize

	#define		I2C_TIMEOUT			100
	
	#define		I2C_1_CAPS			(I2CCap_ReadUint8Reg | I2CCap_ReadData | I2CCap_WriteUint8Reg | I2CCap_WriteData | I2CCap_GeneralCall)
	#define		I2C_3_CAPS			I2C_1_CAPS

	#define		STMI2C_SLAVEBUFFER	64

/*****	Definitions		*****/
	typedef struct sSTI2CInfo_t {
		I2C_HandleTypeDef *pHWInfo;					/**< Pointer to HAL hardware object */
		pfI2CSlaveAddrHandler_t pfAddrHandler;		/**< Pointer to address event handler */
		pfI2CSlaveCompHandler_t pfRecvCompHandler;	/**< Pointer to receive complete handler */
		pfI2CSlaveCompHandler_t pfSendCompHandler;	/**< Pointer to send complete handler */
		pfI2CSlaveCompHandler_t pfTransCompHandler;	/**< Pointer to transaction complete handler */

		void (*pfInitFunc)(void);					/**< Pointer to HAL initialization function */

		sI2CIface_t *pIFace;						/**< Pointer to parent interface object */
	} sSTI2CInfo_t;

/*****	Globals			*****/
	extern sSTI2CInfo_t gSTI2C1;
	extern sSTI2CInfo_t gSTI2C3;

/*****	Prototypes		*****/
	eI2CReturn_t NucleoI2CInitialize(sI2CIface_t *pI2CIface, bool bActAsMaster, uint32_t nClockFreq, void *pHWInfo);
	
	eI2CReturn_t NucleoI2CReadUint8Reg (sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nRegAddr, uint8_t *pnValue);

	eI2CReturn_t NucleoI2CReadData (sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nNumBytes, void *pDataBuff, uint8_t *pnBytesRead);

	eI2CReturn_t NucleoI2CWriteUint8Reg (sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nRegAddr, uint8_t nValue);

	eI2CReturn_t NucleoI2CWriteData (sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nNumBytes, void *pDataBuff);

	eI2CReturn_t NucleoI2CGeneralCall (sI2CIface_t *pI2CIface, uint8_t nValue);
	
	eI2CReturn_t NucleoI2CSlaveListenEnable(sI2CIface_t *pI2CIface, uint8_t nAddr, bool bEnable);

	eI2CReturn_t NucleoI2CSlaveSetAddrHandler(sI2CIface_t *pI2CIface, pfI2CSlaveAddrHandler_t pfHandler);

	eI2CReturn_t NucleoI2CSlaveSetRecvCompHandler(sI2CIface_t *pI2CIface, pfI2CSlaveCompHandler_t pfHandler);

	eI2CReturn_t NucleoI2CSlaveSetTransCompHandler(sI2CIface_t *pI2CIface, pfI2CSlaveCompHandler_t pfHandler);

	eI2CReturn_t NucleoI2CSlaveSetSendCompHandler(sI2CIface_t *pI2CIface, pfI2CSlaveCompHandler_t pfHandler);

	eI2CReturn_t NucleoI2CSlaveSendData(sI2CIface_t *pI2CIface, uint16_t nDataLen, uint8_t *pData);

	eI2CReturn_t NucleoI2CSlaveRecvData(sI2CIface_t *pI2CIface, uint16_t nBuffLen, uint8_t *pData);

/*****	Function Code	*****/

	
#endif
