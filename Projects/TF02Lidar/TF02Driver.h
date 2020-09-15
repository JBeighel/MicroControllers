/**	@defgroup	tf02driver
	@brief		
	@details	v0.1
	#Description
		Device connects of 3.3v UART port.  It runs at 115200 baud with 8 data bits, no parity 
		bit, and 1 stop bit.
	
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
	/**	@brief		UART capabilities needed by the TF02 driver
		@ingroup	tf02driver
	*/
	#define TF02_UARTCAPS		(UART_ReadData)
	
	/**	@brief		Bytes sent as a start of frame by the TF02 device
		@ingroup	tf02driver
	*/
	#define TF02_HEADERBYTE		0x59

	/**	@brief		Number of bytes in a data frame of the TF02
		@ingroup	tf02driver
	*/
	#define TF02_FRAMESIZE		9
	

/*****	Definitions	*****/
	/**	@brief		Enumeration of all error codes for the TF02 driver
		@ingroup	tf02driver
	*/
	typedef enum eTF02Returns_t {
		TF02Warn_Unreliable		= -2,	/**< A readign was read that was reported unreliable */
		TFO2Warn_Unknown		= -1,	/**< An unknown but recoverable error occurred */
		TF02_Success			= 0,	/**< Operation completed successfully */
		TF02Fail_Unknown		= 1,	/**< An unknown and unrecoverable error occurred */
		TF02Fail_UART			= 2,	/**< The UART port reported a failure */
		TF02Fail_NoData			= 3,	/**< Incomplete or no data received from device */
		TF02Fail_Checksum		= 4,	/**< Data received had an invalidated checksum */
	} eTF02Returns_t;
	
	/**	@brief		Structure for storage and parsing of data from TF02
		@ingroup	tf02driver
	*/
	typedef union uTF02Data_t {
		/**	Raw bytes recieved over the UART */
		uint8_t aRaw[TF02_FRAMESIZE];
		
		/**	Structure that parses out bytes of data from the received frame */
		struct {
			uint8_t nHeader[2];		/**< Two header bytes that start the message */
			uint8_t nDistLow;		/**< Low byte of distance reading */
			uint8_t nDistHigh;		/**< High byte of distance reading */
			uint8_t nSigStrLow;		/**< Low byte of signal strength reading */
			uint8_t nSigStrHigh;	/**< High byte of signal strength reading */
			uint8_t nReliability;	/**< Reliability of measurement, discard if not 7 or 8 */
			uint8_t nReserved;
			uint8_t nChecksum;		/**< Checksum is lowest byte of sum of all previous bytes */
		} sParse;
	} uTF02Data_t;
	
	/**	@brief		Structure containing data needed by TF02 driver
		@ingroup	tf02driver
	*/
	typedef struct sTF02Device_t {
		sUARTIface_t *pUART;		/**< Pointer to UART object */
		uint8_t nDataIdx;			/**< Index in data array for next received byte */
		uTF02Data_t uData;			/**< Storage space for received data */
	} sTF02Device_t;

/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	/**	@brief		Prepares the TF02 LiDAR object for use
		@param		pTF02Obj		Pointer to obect to prepare
		@param		pUART			UART interface the TF02 is connected to
		@return		TF02_Success on successful completion, or a warning or fail code on error
		@ingroup	tf02driver
	*/
	eTF02Returns_t TF02Initialize(sTF02Device_t *pTF02Obj, sUARTIface_t *pUART);
	
	/**	@brief		Blocks until a reading is receved from the TF02
		@details	Will wait until a frame of data is received from the TF02 over the UART.  If
			the port has no data waiting it will stop waiting.
		@param		pTF02Obj		Pointer to the TF02 device object
		@param		pnDistCM		Distance reading fom the sensor, in centimeters
		@param		pnSigStr		Signal strength reported by the sensor
		@ingorup	tf02driver
	*/
	eTF02Returns_t TF02GetReading(sTF02Device_t *pTF02Obj, uint16_t *pnDistCM, uint16_t *pnSigStr);

/*****	Functions	*****/


#endif

