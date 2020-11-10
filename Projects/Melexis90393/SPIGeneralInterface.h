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
	
	# File Info #
		File:	SPIGeneralInterface.h
		Author:	J. Beighel
		Date:	08-27-2020
*/

#ifndef __SPIGENIFACE
	#define __SPIGENIFACE

/***** Includes		*****/
	#include <stddef.h>
	#include <stdint.h>
	#include <stdbool.h>

/***** Definitions	*****/
	typedef struct sSPIIface_t sSPIIface_t;  //Declaring this early, will define it later

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
		
		uint32_t nBusClockFreq;
		eSPIDataOrder_t eDataOrder;
		eSPIMode_t eMode;
		void		*pHWInfo;
	} sSPIIface_t;
	
	

/***** Constants	*****/



/***** Globals		*****/


/***** Prototypes 	*****/
	eSPIReturn_t SPIInterfaceInitialize(sSPIIface_t *pIface);
	
	eSPIReturn_t SPIPortInitialize(sSPIIface_t *pIface, void *pHWInfo, uint32_t nBusClockFreq, eSPIDataOrder_t eDataOrder, eSPIMode_t eMode);

	eSPIReturn_t SPIBeginTransfer(sSPIIface_t *pIface);
	
	eSPIReturn_t SPIEndTransfer(sSPIIface_t *pIface);
		
	eSPIReturn_t SPITransferByte(sSPIIface_t *pIface, uint8_t nSendByte, uint8_t *pnReadByte);

/***** Functions	*****/
	
#endif

