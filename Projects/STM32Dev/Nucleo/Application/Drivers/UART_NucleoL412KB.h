/**	@defgroup	uartiface_nucleoL412KB
	@brief		UART General Interface Implementation for ST Nucleo boards
	@details	v0.3
		This implementation of it requires that the UART Global Interrupt be
		enabled.  The data is received by this interrupt and stored in the
		buffer until read by the application.
		
		Without this mechanism in place the data must be read as is is received
		otherwise the data will be lost.
*/

#ifndef UART_NUCLEO_L412KB
	#define UART_NUCLEO_L412KB

/***** Includes		*****/
	#include <stdint.h>
	#include <stdbool.h>

	#include "main.h"
	#include "stm32l4xx.h"
	#include "usart.h"
	
	#include "UARTGeneralInterface.h"

/***** Definitions	*****/
	#define UART_1_HWINFO	((void *)&gSTUart1)
	
	#define UART_2_HWINFO	((void *)&gSTUart2)

	#define UART_CAPS		(UART_ReadData | UART_WriteData | UART_BufferedInput)
	
	#ifdef UART_INIT
		#undef UART_INIT
	#endif
	#define UART_INIT		NucleoUARTPortInitialize
	
	#ifndef mSecDelay
		#define mSecDelay	osDelay
	#endif

	#define UART_TIMEOUT	100

	/**	@brief		Number of bytes to have in the received data buffer
	 *	@ingroup	uartiface_nucleoL412KB
	 */
	#define UART_RXBUFFSIZE	64

	typedef struct sNucleoUART_t {
		uint8_t aRXBuff[UART_RXBUFFSIZE];
		uint32_t nIdxStart;
		uint32_t nIdxStop;
		UART_HandleTypeDef *pHWInfo;
	} sNucleoUART_t;

/***** Constants	*****/


/***** Globals		*****/
	extern sNucleoUART_t gSTUart1;
	extern sNucleoUART_t gSTUart2;

/***** Prototypes 	*****/
	eUARTReturn_t NucleoUARTPortInitialize(sUARTIface_t *pUARTIface, uint32_t nBaudRate, eUARTModes_t eReqMode, void *pHWInfo);
	
	eUARTReturn_t NucleoUARTShutdown(sUARTIface_t *pUARTIface);
		
	eUARTReturn_t NucleoUARTReadData(sUARTIface_t *pUARTIface, uint16_t nDataSize, void *pDataBuff, uint16_t *pnBytesRead);
	eUARTReturn_t NucleoUARTWriteData(sUARTIface_t *pUARTIface, uint16_t nDataSize, const void *pDataBuff);
	eUARTReturn_t NucleoUARTDataAvailable(sUARTIface_t *pUARTIface, uint16_t *pnBytesAvailable);
	eUARTReturn_t NucleoUARTWaitDataSend(sUARTIface_t *pUARTIface);

/***** Functions	*****/

#endif

