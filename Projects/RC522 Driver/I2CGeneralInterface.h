/**	@defgroup	i2ciface
	@brief		Abstracted interface for general purpose I2C port communications
	@details	v0.1
	# Intent #
		The intent of this module is to ensure that device drivers are not coupled with I2C hardware
		implementations.  By using this interface to operate the hardware it should allow the device
		driver to be easily ported to new hardware.
		
		The interface will not provide any direct functionality, it will only provide a standardized
		set of functions and objects to use in order to make use of the hardware.  Each hardware 
		driver is expected to provide this functionality, and in turn the decive peripheral drivers
		will only expect and make use of this.

	# Usage #
		Hardware I2C bus drivers are to provide some initialization function following the 
		fInitializeI2CBus_t prototype definition.  This function should expect the sI2CIface_t 
		parameter to have already been initialized and made ready for use through a call to 
		I2CInterfaceInitialize().  This initialization function is then expected to populate the
		function pointers in the I2C interface object.
		
		The pHWInfo member of the I2C Interface strcutre is solely for use but the hardware driver.
		Any information needed by that driver should be accessed through that pointer.  All other
		members of that structre have specific purposes.
		
		Device drivers using this interface should only include this header file, they should not
		include headers of the hardware I2C bus driver.  They should receive an instance of the 
		I2C Interface object and be able to communicate over the pus to the peripheral entirely 
		through the function pointers provided.

		Bus drivers must also create a define listing all of the capabilities that it allows.
		This define should be options from the eI2CCapabilities_t enumeration ORed together.  The
		peripheral drivers will similarly provide a define a value listing the capabilities that
		it requires.  Using these defines the application should be able to determine at
		compilation whether or not a peripheral will work on a particular bus.  This define
		should take the form of I2C_#_CAPS

	# File Information #
		File:	I2CGeneralInterface.c
		Author:	J. Beighel
		Created:09-10-2020
*/

/**	@defgroup i2ciface_priv : Private Objects
	@brief
	@ingroup i2ciface
*/

#ifndef __I2CINTERFACE
	#define __I2CINTERFACE

/*****	Includes	*****/
	#include <stdbool.h>
	#include <stdint.h>
	#include <string.h>

/*****	Constants	*****/
	/**	@brief		Sending this address on the I2C bus attempts to reach all connected devices
		@ingroup	i2ciface
	*/
	#define I2C_GENERALCALLADDR	0x00

/*****	Definitions	*****/
	typedef struct sI2CIface_t sI2CIface_t; //Declaring the type, full definition will appear later
	
	/**	@brief		Enumeration of all I2C bus capabilities
		@ingroup	i2ciface
	*/
	typedef enum eI2CCapabilities_t {
		I2C_NoCapabilities	= 0x00000000,	/**< Default, SPI bus driver has no capabilities */
		I2C_Configure		= 0x00000001,	/**< I2C bus driver allows configuration of the port */
		I2C_Shutdown		= 0x00000002,	/**< I2C bus driver provides a means of shutting down the bus */
		I2C_ReadU8Reg		= 0x00000004,	/**< I2C bus driver allows reading of 8 bit registers */
		I2C_ReadData		= 0x00000008,	/**< I2C bus driver allows reading arbitrary sized data */
		I2C_WriteU8Reg		= 0x00000010,	/**< I2C bus driver allows writing to 8 bit registers */
		I2C_WriteData		= 0x00000020,	/**< I2C bus driver allows writing arbitrary sized data */
		I2C_GeneralCall		= 0x00000040,	/**< I2C bus driver allows sending a 1 byte general call value */
	} eI2CCapabilities_t;

	/**	@brief		Enumeration of all return codes functions from the I2C Interface can give
		@details	All functions fulfilling functionality for this interface must only return these 
			values.  These are grouped to categorize the responses.
			- Zero means complete success
			- Positive means warning, the operate succeeded imperfectly
			- Negative means failure, the operation could not complete
			
			The interface will reserve values from -25 to +25 for standard messages shared by all 
			hardware.  +/-26 through +/-100 are available for specific hardware imlementations.
		@ingroup	i2ciface
	*/		
	typedef enum eI2CReturns_t {
		I2C_Warn_PartialRead	= -2,
		I2C_Warn_Unknown		= -1,	/**< An unknown warning occured communicating with the I2C port */
		I2C_Success				= 0,	/**< I2C communication completed successfully */
		I2C_Fail_Unknown		= 1,	/**< An unknown failure occured communicating with the I2C port */
		I2C_Fail_Unsupported	= 2,	/**< The requested I2C operation is not supported by this specified port */
		I2C_Fail_WriteBuffOver	= 3,
		I2C_Fail_NackAddr		= 4,
		I2C_Fail_NackData		= 5,
	} eI2CReturns_t;
	
	/**	@brief		Prototype for function that will initizlize the hardware for an I2C interface
		@ingroup	i2ciface
	*/
	typedef eI2CReturns_t (*pfInitializeI2CBus_t)(sI2CIface_t *pI2CIface, bool bActAsMaster, uint32_t nClockFreq, void *pHWInfo);
	
	/**	@brief		Structure defining all functions an I2C bus driver must provide
		@ingroup	i2ciface
	*/
	typedef struct sI2CIface_t {
		pfInitializeI2CBus_t pfInitialize;
		eI2CReturns_t	(*pfShutdown)			(sI2CIface_t *pI2CIface);
		
		eI2CReturns_t	(*pfI2CReadUint8Reg)	(sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nRegAddr, uint8_t *pnValue);
		eI2CReturns_t	(*pfI2CReadData)		(sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nNumBytes, void *pDataBuff, uint8_t *pnBytesRead);
		
		eI2CReturns_t	(*pfI2CWriteUint8Reg)	(sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nRegAddr, uint8_t nValue);
		eI2CReturns_t	(*pfI2CWriteData)		(sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nNumBytes, void *pDataBuff);
		
		eI2CReturns_t	(*pfI2CGeneralCall)		(sI2CIface_t *pI2CIface, uint8_t nValue);
		
		bool			bMaster;
		uint32_t		nClockFreq;
		void			*pHWInfo;
	} sI2CIface_t;

/*****	Globals		*****/


/*****	Prototypes	*****/
	/**	@brief		Initializes an I2C Interface object and prepare it for use
		@ingroup	i2ciface
	*/
	eI2CReturns_t I2CInterfaceInitialize(sI2CIface_t *pI2CIface);
#endif
