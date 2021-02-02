/**	@defgroup	i2craspberrypi
	@brief		I2C General Interface implementation for Raspberry Pi
	@details	v0.4
	#Description
		At boot pins 2 and 3 will be configured for I2C.  However, once they 
		are assigned to be GPIO pins that will disable the I2C bus causing
		reads and writes to fail with errno 5 "Input/output error".  The only
		way to correct the pin conffiguration is with a reboot.
		
	#File Information
		File:	I2C_RaspberryPi.h
		Author:	J. Beighel
		Date:	2021-01-22
*/

#ifndef __I2CRASPBERRYPI
	#define __I2CRASPBERRYPI

/*****	Includes	*****/
	#include <unistd.h>
	#include <fcntl.h>
	#include <errno.h>
	#include <sys/ioctl.h>
	#include <linux/i2c-dev.h>
	
	#include "CommonUtils.h"
	#include "I2CGeneralInterface.h"

/*****	Constants	*****/
	/**	@brief		Capabilities provided by this implementation
		@ingroup	i2craspberrypi
	*/
	#define I2C_1_CAPS			(I2CCap_Shutdown | I2CCap_ReadUint8Reg | I2CCap_WriteUint8Reg | I2CCap_ReadData | I2CCap_WriteData)

	/**	@brief		Function to call to initialize the first I2C bus
		@ingroup	i2craspberrypi
	*/
	#define I2C_INIT			RasPiInitializeI2CBus
	
	/**	@brief		Hardware information for the first I2C bus
		@ingroup	i2craspberrypi
	*/
	#define I2C_1_HWINFO		((void *)&(gI2CHWInfo[0]))

/*****	Definitions	*****/
	/**	@brief		Structure holding information on the I2C Hardware
		@ingroup	i2craspberrypi
	*/
	typedef struct sRasPiI2CHWInfo_t {
		const char *pcFilePath;	/**< Path to the filesystem object for this bus */
		int32_t I2CFile;		/**< File handle to use when interacting with this bus */
		int32_t nLastErr;
	} sRasPiI2CHWInfo_t;

/*****	Constants	*****/
	extern sRasPiI2CHWInfo_t gI2CHWInfo[];

/*****	Globals		*****/


/*****	Prototypes 	*****/
	eI2CReturn_t RasPiInitializeI2CBus(sI2CIface_t *pI2CIface, bool bActAsMaster, uint32_t nClockFreq, void *pHWInfo);

/*****	Functions	*****/


#endif

