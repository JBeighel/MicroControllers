/**	@defgroup	spiiface
	@brief		General interface for using the SPI bus
	@details	v0.2
	# Intent #
		This module is to create a common interface for interacting with a SPI bus.  Drivers 
		for devices that operate over this bus should use this interface to operate the 
		peripheral bus.  This is to ensure that the driver will not be bound to a specific 
		processor or platform since this interface can be implemented anywhere.
		
		The interface itself will provide no functionality, it only offers a standardized set
		of functions to use to communicate with the hardware.  Device drivers can rely on these
		to get consistent capabilities and behavior from the bus.  Processor bus drivers are
		expected to implement and provide these functions.
		
	# Usage #
		Each processor or device bus that will support this interface must create define and
		create its own copies of the interface functions.

		The SPIPortInitialize() function in the driver must call the SPIInterfaceInitialize()
		function to prepare the instance of the sSPIIface_t object.  This will ensure that all
		function pointers will have a valid value, applications can not inadvertently branch to
		an nonexistent function.  It will then overwrite the contents of the object with its
		information.
		
		The driver must also define values for each hardware object that it covers.  This should
		take the form of SPI_#_HWINFO

		In addition the driver must define a value to reach the SPIPortInitialize() function.
		This should take the form of SPI_#_INIT

		Having these defined gives a very consistent and generic means of establishing the
		interface object in the application that looks like this:

		sSPIIface_t SpiObj;

		SPI_1_INIT(&SpiObj, SPI_1_HWINFO, 5000000, SPI_MSBFirst, SPI_Mode0);

		The last thing the driver must do is create a define of the capabilities that it allows.
		This define should be options from the eSPICapabilities_t enumeration ORed together.  The
		peripheral drivers will similarly provide a define a value listing the capabilities that
		it requires.  Using these defines the application should be able to determine at
		compilation whether or not a peripheral will work on a particular bus.  This define
		should take the form of SPI_#_CAPS

	# File Information #
		File:	SPIGeneralInterface.c
		Author:	J. Beighel
		Date:	09-04-2020
*/

#ifndef __SPIGENIFACE
	#define __SPIGENIFACE

/***** Includes		*****/
	#include <stddef.h>
	#include <stdint.h>

/***** Definitions	*****/
	typedef struct sSPIIface_t sSPIIface_t;  //Declaring this early, will define it later

	/**	@brief		Enumeration of all SPI bus capabilities
		@ingroup	spiiface
	*/
	typedef enum eSPICapabilities_t {
		SPI_NoCapabilities	= 0x00000000,	/**< Default, SPI bus driver has no capabilities */
		SPI_Configure		= 0x00000001,	/**< SPI bus driver allows configuration of the port */
		SPI_BeginTransfer	= 0x00000002,	/**< SPI bus driver allows user to begin a data transfer */
		SPI_EndTransfer		= 0x00000004,	/**< SPI bus driver allows user to end a data transfer */
		SPI_BiDirTransfer	= 0x00000008,	/**< SPI bus driver allows user to bi-directionally transfer data */
	} eSPICapabilities_t;

	/**	@brief		Enumeration of markers to indicate the order the bus sends data out 
		@ingroup	spiiface
	*/
	typedef enum eSPIDataOrder_t {
		SPI_MSBFirst,
		SPI_LSBFirst,
	} eSPIDataOrder_t;
	
	/**	@brief		Enumeration of clock and data sampling methods the bus will use
		@ingroup	spiiface
	*/
	typedef enum eSPIMode_t {
		SPI_Mode0,			/**< CPOL 0, CPHA 0, Output Edge Falling, Data Capture Edge Rising */
		SPI_Mode1,			/**< CPOL 0, CPHA 1, Output Edge Rising, Data Capture Edge Falling */
		SPI_Mode2,			/**< CPOL 1, CPHA 0, Output Edge Rising, Data Capture Edge Falling */
		SPI_Mode3,			/**< CPOL 1, CPHA 1, Output Edge Falling, Data Capture Edge Rising */
	} eSPIMode_t;

	/**	@brief		Enumeration of all SPI interface return values
		@ingroup	spiiface
	*/
	typedef enum eSPIReturn_t {
		SPIWarn_Unknown		= 1,	/**< An unknown but recoverable error happened during the operation */
		SPI_Success			= 0,	/**< The operation completed successfully */
		SPIFail_Unknown		= -1,	/**< An unknown and unrecoverable error happened during the operation */
		SPIFail_Unsupported	= -2,	/**< The requested operation is not supported by this device */
	} eSPIReturn_t;
	
	typedef eSPIReturn_t (*pfInitializeSPIBus_t)(sSPIIface_t *pIface, void *pHWInfo, uint32_t nBusClockFreq, eSPIDataOrder_t eDataOrder, eSPIMode_t eMode);
	
	typedef struct sSPIIface_t {
		pfInitializeSPIBus_t pfInitialise;
		eSPIReturn_t (*pfBeginTransfer)(sSPIIface_t *pIface);
		eSPIReturn_t (*pfEndTransfer)(sSPIIface_t *pIface);
		
		eSPIReturn_t (*pfTransferByte)(sSPIIface_t *pIface, uint8_t nSendByte, uint8_t *pnReadByte);
		
		eSPICapabilities_t (*pfGetCapabilities)(sSPIIface_t *pIface);

		uint32_t		nBusClockFreq;
		eSPIDataOrder_t	eDataOrder;
		eSPIMode_t		eMode;
		void			*pHWInfo;
	} sSPIIface_t;
	

/***** Constants	*****/


/***** Globals		*****/


/***** Prototypes 	*****/
	eSPIReturn_t SPIInterfaceInitialize(sSPIIface_t *pIface);
	
	eSPIReturn_t SPIPortInitialize(sSPIIface_t *pIface, void *pHWInfo, uint32_t nBusClockFreq, eSPIDataOrder_t eDataOrder, eSPIMode_t eMode);

	eSPIReturn_t SPIBeginTransfer(sSPIIface_t *pIface);
	
	eSPIReturn_t SPIEndTransfer(sSPIIface_t *pIface);
		
	eSPIReturn_t SPITransferByte(sSPIIface_t *pIface, uint8_t nSendByte, uint8_t *pnReadByte);

	eSPICapabilities_t SPIGetCapabilities(sSPIIface_t *pIface);

/***** Functions	*****/

#endif

