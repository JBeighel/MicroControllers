/**	@defgroup	uartiface
	@brief		Abstracted interface for general purpose UART port
	@details	
		
*/

#ifndef __UARTINTERFACE
	#define __UARTINTERFACE

/***** Includes		*****/


/***** Definitions	*****/
	#define UART_PORTINITIALIZE	UARTPortInitialize

	typedef struct sUARTIface_t;

	typedef enum eUARTReturns_t {
		UART_Warn_Unknown		= -1,	/**< An unknown warning occured communicating with the UART port */
		UART_Success			= 0,	/**< UART communication completed successfully */
		UART_Fail_Unknown		= 1,
		UART_Fail_Unsupported	= 2,	/**< The requested UART operation is not supported by this specified port */
	} eUARTReturns_t;
	
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
		eUARTReturns_t	(*pfUARTWriteData)		(sUARTIface_t *pUARTIface, uint16_t nBuffSize, void *pDataBuff);
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
	eUARTReturns_t UARTWriteData(sUARTIface_t *pUARTIface, uint16_t nBuffSize, void *pDataBuff);
	eUARTReturns_t UARTDataAvailable(sUARTIface_t *pUARTIface, uint16_t *pnBytesAvailable);
	eUARTReturns_t UARTWitDataSend(sUARTIface_t *pUARTIface);

/***** Functions	*****/
	eUARTReturns_t UARTInterfaceInitialize(sUARTIface_t *pUARTIface) {
		pUARTIface->pfShutdown = &UARTShutdown;
		pUARTIface->pfPortInitialize = &UARTPortInitialize;
		pUARTIface->pfUARTReadData = &UARTReadData;
		pUARTIface->pfUARTWriteData = &UARTWriteData;
		pUARTIface->pfUARTDataAvailable = &UARTDataAvailable;
		pUARTIface->pfUARTWaitDataSend = &UARTWitDataSend;
		
		return UART_Success;
	}
	
	eUARTReturns_t UARTPortInitialize(sUARTIface_t *pUARTIface, uint32_t nBaudRate, eUARTModes_t eMode, void *pHWInfo) {
		return UART_Fail_Unsupported;
	}
	
	eUARTReturns_t UARTShutdown(sUARTIface_t *pUARTIface) {
		return UART_Fail_Unsupported;
	}
		
	eUARTReturns_t UARTReadData(sUARTIface_t *pUARTIface, uint16_t nBuffSize, void *pDataBuff, uint16_t *pnBytesRead) {
		return UART_Fail_Unsupported;
	}
	
	eUARTReturns_t UARTWriteData(sUARTIface_t *pUARTIface, uint16_t nBuffSize, void *pDataBuff) {
		return UART_Fail_Unsupported;
	}
	
	eUARTReturns_t UARTDataAvailable(sUARTIface_t *pUARTIface, uint16_t *pnBytesAvailable) {
		return UART_Fail_Unsupported;
	}
	
	eUARTReturns_t UARTWitDataSend(sUARTIface_t *pUARTIface) {
		return UART_Fail_Unsupported;
	}

#endif

