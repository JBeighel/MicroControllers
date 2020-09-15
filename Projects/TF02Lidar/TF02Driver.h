/**	@defgroup	tf02driver
	@brief		
	@details	v0.1
	#Description
	
	#File Information
		File:	TF02Driver.c
		Author:	J. Beighel
		Date:	09-15-2020
*/

#ifndef __TF02DRIVER_H
	#define __TF02DRIVER_H

/*****	Includes	*****/
	#include "UARTGeneralInterface.h"

/*****	Constants	*****/
	#define TF02_UARTCAPS		(UART_ReadData)
	
	#define TF02_HEADERBYTE		0x59
	
	#define TF02_FRAMESIZE		9
	

/*****	Definitions	*****/
	typedef enum eTF02Returns_t {
		TFO2Warn_Unknown		= -1,	/**< An unknown but recoverable error occurred */
		TF02_Success			= 0,	/**< Operation completed successfully */
		TF02Fail_Unknown		= 1,	/**< An unknown and unrecoverable error occurred */
		TF02Fail_UART			= 2,	/**< The UART port reported a failure */
		TF02Fail_NoData			= 3,	/**< Incomplete or no data received from device */
		TF02Fail_Checksum		= 4,	/**< Data received had an invalidated checksum */
	} eTF02Returns_t;
	
	typedef union uTF02Data_t {
		uint8_t aRaw[9];
		
		struct {
			uint8_t nHeader[2];
			uint8_t nDistLow;
			uint8_t nDistHigh;
			uint8_t nSigStrLow;
			uint8_t nSigStrHigh;
			uint8_t nReliability;
			uint8_t nReserved;
			uint8_t nChecksum;
		} sParse;
	} uTF02Data_t;
	
	typedef struct sTF02Device_t {
		sUARTIface_t *pUART;
		uint8_t nDataIdx;
		uTF02Data_t uData;
	} sTF02Device_t;

/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	eTF02Returns_t TF02Initialize(sTF02Device_t *pTF02Obj, sUARTIface_t *pUART);
	eTF02Returns_t TF02GetReading(sTF02Device_t *pTF02Obj);

/*****	Functions	*****/


#endif

