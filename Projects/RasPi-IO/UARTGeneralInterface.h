/**	@defgroup	uartiface
	@brief		Abstracted interface for general purpose UART port
	@details	v0.1
	# Description #
	
	# Usage #
		Each processor or device port that will support this interface must create define and
		create its own copies of the interface functions.

		The UARTPortInitialize() function in the driver must call the UARTInterfaceInitialize()
		function to prepare the instance of the sUARTIface_t object.  This will ensure that all
		function pointers will have a valid value, applications can not inadvertently branch to
		an nonexistent function.  It will then overwrite the contents of the object with its
		information.
		
		The driver must also define values for each hardware object that it covers.  This should
		take the form of UART_#_HWINFO

		In addition the driver must define a value to reach the UARTPortInitialize() function.
		This should take the form of UART_#_PORTINIT

		Having these defined gives a very consistent and generic means of establishing the
		interface object in the application that looks like this:

		sUARTIface_t UartObj;

		UART_PORTINITIALIZE(&UartObj, 9600, UART_8None1, UART_1_HWINFO);

		The last thing the driver must do is create a define of the capabilities that it allows.
		This define should be options from the eUARTCapabilities_t enumeration ORed together. 
		The peripheral drivers will similarly provide a define a value listing the capabilities
		that it requires.  Using these defines the application should be able to determine at
		compilation whether or not a peripheral will work on a particular bus.  This define
		should take the form of UART_#_CAPS
	
	# File Information #
		File:	UARTGeneralInterface.h
		Author:	J. Beighel
		Created:09-15-2020
*/

#ifndef __UARTINTERFACE
	#define __UARTINTERFACE

/***** Includes		*****/
	#include <stdint.h>

/***** Definitions	*****/
	#define UART_PORTINITIALIZE	UARTPortInitialize

	typedef struct sUARTIface_t sUARTIface_t;

	typedef enum eUARTReturns_t {
		UART_Warn_Timeout		= -2,	/**< An operation timed out, data transfer was ncomplete */
		UART_Warn_Unknown		= -1,	/**< An unknown warning occured communicating with the UART port */
		UART_Success			= 0,	/**< UART communication completed successfully */
		UART_Fail_Unknown		= 1,
		UART_Fail_Unsupported	= 2,	/**< The requested UART operation is not supported by this specified port */
	} eUARTReturns_t;
	
	typedef enum eUARTCapabilities_t {
		UART_NoCapabilities	= 0x00000000,	/**< Default, UART driver has no capabilities */
		UART_Configure		= 0x00000001,	/**< UART driver allows configuration of the port */
		UART_Shutdown		= 0x00000002,	/**< UART driver allows the port to be shutdown */
		UART_ReadData		= 0x00000004,	/**< UART driver allows reading of data from the port */
		UART_WriteData		= 0x00000008,	/**< UART driver allows writing of data to the port */
		UART_DataAvailable	= 0x00000010,	/**< UART driver provides a means of checking if data is waiting to be read */
		UART_DataWaitSend	= 0x00000020,	/**< UART driver has a method for waiting for all data to be sent */
	} eUARTCapabilities_t;
	
	typedef enum eUARTModes_t {
		UART_5None1,
		UART_6None1,
		UART_7None1,
		UART_8None1,
		UART_5None2,
		UART_6None2,
		UART_7None2,
		UART_8None2,
		UART_5Even1,
		UART_6Even1,
		UART_7Even1,
		UART_8Even1,
		UART_5Even2,
		UART_6Even2,
		UART_7Even2,
		UART_8Even2,
		UART_5Odd1,
		UART_6Odd1,
		UART_7Odd1,
		UART_8Odd1,
		UART_5Odd2,
		UART_6Odd2,
		UART_7Odd2,
		UART_8Odd2,
	} eUARTModes_t;
	
	typedef eUARTReturns_t (*pfInitializeUART_t)(sUARTIface_t *pUARTIface, uint32_t nBaudRate, eUARTModes_t eMode, void *pHWInfo);
	
	typedef struct sUARTIface_t {
		pfInitializeUART_t pfPortInitialize;
		eUARTReturns_t	(*pfShutdown)			(sUARTIface_t *pUARTIface);
		
		eUARTReturns_t	(*pfUARTReadData)		(sUARTIface_t *pUARTIface, uint16_t nBuffSize, void *pDataBuff, uint16_t *pnBytesRead);
		eUARTReturns_t	(*pfUARTWriteData)		(sUARTIface_t *pUARTIface, uint16_t nBuffSize, const void *pDataBuff);
		eUARTReturns_t	(*pfUARTDataAvailable)	(sUARTIface_t *pUARTIface, uint16_t *pnBytesAvailable);
		eUARTReturns_t	(*pfUARTWaitDataSend)	(sUARTIface_t *pUARTIface);
		
		uint32_t		nBaudRate;
		eUARTModes_t	eMode;
		void			*pHWInfo;
	} sUARTIface_t;

/***** Constants	*****/


/***** Globals		*****/


/***** Prototypes 	*****/
	eUARTReturns_t UARTInterfaceInitialize(sUARTIface_t *pUARTIface);
	
	eUARTReturns_t UARTPortInitialize(sUARTIface_t *pUARTIface, uint32_t nBaudRate, eUARTModes_t eMode, void *pHWInfo);
	
	eUARTReturns_t UARTShutdown(sUARTIface_t *pUARTIface);
		
	eUARTReturns_t UARTReadData(sUARTIface_t *pUARTIface, uint16_t nBuffSize, void *pDataBuff, uint16_t *pnBytesRead);
	eUARTReturns_t UARTWriteData(sUARTIface_t *pUARTIface, uint16_t nBuffSize, const void *pDataBuff);
	eUARTReturns_t UARTDataAvailable(sUARTIface_t *pUARTIface, uint16_t *pnBytesAvailable);
	eUARTReturns_t UARTWaitDataSend(sUARTIface_t *pUARTIface);

/***** Functions	*****/
	

#endif

