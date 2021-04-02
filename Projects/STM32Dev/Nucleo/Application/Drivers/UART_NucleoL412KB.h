/**	@defgroup	uartiface_nucleoL412KB
	@brief		UART General Interface Implementation for ST Nucleo boards
	@details	v0.3
	#Description
		This implementation of it requires that the UART Global Interrupt be
		enabled.  The data is received by this interrupt and stored in the
		buffer until read by the application.
		
		Without this mechanism in place the data must be read as is is received
		otherwise the data will be lost.

		Define the value UART_STINTERRUPT to use the UART global interrupt to
		accept and buffer incoming data.  This will accept UART input one byte
		at a time and store it in a buffer until read by the application.

		Interrupt support is applied across all UART buses.  The software will
		not handle some ports with the interrupt and some without.

	#File Information
		File:	UART_NucleoL412KB.h
		Author:	J. Beighel
		Date:	2021-03-31
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

#ifdef UART_STINTERRUPT
	#define UART_CAPS		(UART_ReadData | UART_WriteData | UART_BufferedInput | UART_DataAvailable)
#else
	#define UART_CAPS		(UART_ReadData | UART_WriteData)
#endif
	
	#ifdef UART_INIT
		#undef UART_INIT
	#endif
	#define UART_INIT		NucleoUARTPortInitialize
	
	#ifndef mSecDelay
		#define mSecDelay	osDelay
	#endif

	#define UART_TIMEOUT	100

	#ifdef UART_STINTERRUPT
		/**	@brief		Number of bytes to have in the received data buffer
		 *	@ingroup	uartiface_nucleoL412KB
		 */
		#define UART_RXBUFFSIZE	64
	#endif

	typedef struct sNucleoUART_t {
		#ifdef UART_STINTERRUPT
			uint8_t aRXBuff[UART_RXBUFFSIZE]; /**< Ring buffer to accumulate received data */
			uint32_t nIdxStart;	/**< Index in the ring buffer of the first data byte */
			uint32_t nIdxStop;	/**< Index in the ring buffer to store the next received byte */
		#endif

		UART_HandleTypeDef *pHWInfo;	/**< Pointer to the ST hardware object */
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

