/**	@defgroup	uartiface_nucleoL412KB
	@brief		UART General Interface Implementation for ST Nucleo boards
	@details	v0.2
		
*/

#ifndef UART_NUCLEO_L412KB
	#define UART_NUCLEO_L412KB

/***** Includes		*****/
	#include <stdint.h>
	#include <stdbool.h>

	#include "main.h"
	#include "stm32l4xx.h"
	
	#include "UARTGeneralInterface.h"

/***** Definitions	*****/
	#define UART_1_HWINFO (huart1)
	
	#define UART_2_HWINFO (huart2)
	
	#ifdef UART_PORTINITIALIZE
		#undef UART_PORTINITIALIZE
	#endif
	#define UART_PORTINITIALIZE	NucleoUARTPortInitialize
	
	#ifndef mSecDelay
		#define mSecDelay	osDelay
	#endif

	#define UART_TIMEOUT	100
	
/***** Constants	*****/


/***** Globals		*****/


/***** Prototypes 	*****/
	eUARTReturn_t NucleoUARTPortInitialize(sUARTIface_t *pUARTIface, uint32_t nBaudRate, eUARTModes_t eReqMode, void *pHWInfo);
	
	eUARTReturn_t NucleoUARTShutdown(sUARTIface_t *pUARTIface);
		
	eUARTReturn_t NucleoUARTReadData(sUARTIface_t *pUARTIface, uint16_t nDataSize, void *pDataBuff, uint16_t *pnBytesRead);
	eUARTReturn_t NucleoUARTWriteData(sUARTIface_t *pUARTIface, uint16_t nDataSize, const void *pDataBuff);
	eUARTReturn_t NucleoUARTDataAvailable(sUARTIface_t *pUARTIface, uint16_t *pnBytesAvailable);
	eUARTReturn_t NucleoUARTWaitDataSend(sUARTIface_t *pUARTIface);

/***** Functions	*****/

#endif

