/**	@defgroup	i2ciface
	@brief		Abstracted interface for general purpose I2C port communications
	@details	v0.4
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
		
		The driver must also define values for each hardware object that it covers.  This should
		take the form of I2C_#_HWINFO

		In addition the driver must define a value to reach the I2CPortInitialize() function.
		This should take the form of I2C_INIT

		Having these defined gives a very consistent and generic means of establishing the
		interface object in the application that looks like this:

		sI2CIface_t I2CObj;

		I2C_INIT(&I2CObj, true, 100000, I2C_1_HWINFO);

		The last thing the driver must do is create a define of the capabilities that it allows.
		This define should be options from the eI2CCapabilities_t enumeration ORed together. 
		The peripheral drivers will similarly provide a define a value listing the capabilities
		that it requires.  Using these defines the application should be able to determine at
		compilation whether or not a peripheral will work on a particular bus.  This define
		should take the form of I2C_#_CAPS
		
	# File Information #
		File:	I2CGeneralInterface.c
		Author:	J. Beighel
		Date:	2021-05-10
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
	typedef enum eI2CReturn_t {
		I2C_Warn_PartialRead	= 2,
		I2C_Warn_Unknown		= 1,	/**< An unknown warning occured communicating with the I2C port */
		I2C_Success				= 0,	/**< I2C communication completed successfully */
		I2C_Fail_Unknown		= -1,	/**< An unknown failure occured communicating with the I2C port */
		I2C_Fail_Unsupported	= -2,	/**< The requested I2C operation is not supported by this specified port */
		I2C_Fail_WriteBuffOver	= -3,
		I2C_Fail_NackAddr		= -4,
		I2C_Fail_NackData		= -5,
	} eI2CReturn_t;
	
	/**	@brief		Enumeration of all capabilities this interface provides
		@ingroup	i2ciface
	*/
	typedef enum eI2CCapabilities_t {
		I2CCap_None				= 0x0000,	/**< No supported capabilities */
		I2CCap_Shutdown			= 0x0001,	/**< Port can be shut down */
		I2CCap_ReadUint8Reg		= 0x0002,	/**< Can send an 8 bit address and read an 8 bit value */
		I2CCap_ReadData			= 0x0004,	/**< Can read an arbitrary number of bytes */
		I2CCap_WriteUint8Reg	= 0x0008,	/**< Can send an 8 bit address and write an 8 bit value */
		I2CCap_WriteData		= 0x0010,	/**< Can write an arbitrary number of bytes */
		I2CCap_GeneralCall		= 0x0020,	/**< Can send a single byte value to the general call address */

		I2CCap_Slave			= 0x0040,	/**< Can operate in slave mode, includes send and receive data */
		I2CCap_SlaveAddrEvent	= 0x0080,	/**< Can raise an event when the bus address is heard */
		I2CCap_SlaveSendEvent	= 0x0100,	/**< Can raise an event when all data is sent and master wants more */
		I2CCap_SlaveRecvEvent	= 0x0200,	/**< Can raise an event when all data is received and master sends more */
		I2CCap_SlaveTransEvent	= 0x0400,	/**< Can raise an event when the master ends all transfers */
	} eI2CCapabilities_t;
	
	/**	@brief		Enumeration of master requests from the slave
	 *	@ingroup	i2ciface
	 */
	typedef enum eI2CSlaveDirection_t {
		I2CSlave_MasterReceive,		/**< Master has requested to read data from slave */
		I2CSlave_MasterTransmit,	/**< Master has requested to send data to slave */
	} eI2CSlaveDirection_t;

	/**	@brief		Prototype for function that will initialize the hardware for an I2C interface
	 *	@param		pI2CIface		Pointer to the I2C interface object
	 *	@param		bActAsMaster	True to be a master, false for slave, on the bus
	 *	@param		nClockFreq		I2C bus clock frequency
	 *	@param		pHWInfo			Pointer to hardware information
	 *	@return		I2C_Success upon success, or a code indicating the
	 *		error encountered.
	 *	@ingroup	i2ciface
	*/
	typedef eI2CReturn_t (*pfInitializeI2CBus_t)(sI2CIface_t *pI2CIface, bool bActAsMaster, uint32_t nClockFreq, void *pHWInfo);
	
	typedef eI2CReturn_t (*pfI2CSlaveAddrHandler_t)(sI2CIface_t *pI2CIface, eI2CSlaveDirection_t eDirect, uint16_t nAddrMatch);
	typedef eI2CReturn_t (*pfI2CSlaveCompHandler_t)(sI2CIface_t *pI2CIface);

	/**	@brief		Structure defining all functions an I2C bus driver must provide
		@ingroup	i2ciface
	*/
	typedef struct sI2CIface_t {
		pfInitializeI2CBus_t pfInitialize;
		eI2CReturn_t	(*pfShutdown)			(sI2CIface_t *pI2CIface);
		
		//Master functions

		eI2CReturn_t	(*pfI2CReadUint8Reg)	(sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nRegAddr, uint8_t *pnValue);
		eI2CReturn_t	(*pfI2CReadData)		(sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nNumBytes, void *pDataBuff, uint8_t *pnBytesRead);
		eI2CReturn_t	(*pfI2CWriteUint8Reg)	(sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nRegAddr, uint8_t nValue);
		eI2CReturn_t	(*pfI2CWriteData)		(sI2CIface_t *pI2CIface, uint8_t nDevAddr, uint8_t nNumBytes, void *pDataBuff);
		eI2CReturn_t	(*pfI2CGeneralCall)		(sI2CIface_t *pI2CIface, uint8_t nValue);
		
		//Slave functions

		/**	@brief		Sets the Slave configured I2C to begin listening
		 *	@details	For slave I2C Interface only.
		 *		The I2C interface will listen on the data line for its address.
		 *		When the address is matched it will raise the Address Event.
		 *	@param		pI2CIface		Pointer to the I2C interface object
		 *	@param		nAddr			Slave address to listen for
		 *	@param		bEnable			True to begin listening, false to stop
		 *	@return		I2C_Success upon success, or a code indicating the
		 *		error encountered.
		 */
		eI2CReturn_t	(*pfSlaveListenEnable)			(sI2CIface_t *pI2CIface, uint8_t nAddr, bool bEnable);

		/**	@brief		Sets the handler for the Address Event
		 *	@details	For slave I2C Interface only.
		 *		When the I2C interface hears its address on the data bus it
		 *		will raise this event to allow the application to prepare for
		 *		the next frame of data.
		 *	@param		pI2CIface		Pointer to the I2C interface object
		 *	@param		pfHandler		Pointer to function that will handle this event
		 *	@return		I2C_Success upon success, or a code indicating the
		 *		error encountered.
		 */
		eI2CReturn_t	(*pfSlaveSetAddrHandler)		(sI2CIface_t *pI2CIface, pfI2CSlaveAddrHandler_t pfHandler);

		/**	@brief		Sets the handler for the Receive Complete Event
		 *	@details	For slave I2C Interface only.
		 *		When the I2C interface has received the expected amount of data
		 *		this event will be called to allow the application to prepare
		 *		for the next frame.
		 *	@param		pI2CIface		Pointer to the I2C interface object
		 *	@param		pfHandler		Pointer to function that will handle this event
		 *	@return		I2C_Success upon success, or a code indicating the
		 *		error encountered.
		 */
		eI2CReturn_t	(*pfSlaveSetRecvCompHandler)	(sI2CIface_t *pI2CIface, pfI2CSlaveCompHandler_t pfHandler);

		/**	@brief		Sets the handler for the Send Complete Event
		 *	@details	For slave I2C Interface only.
		 *		When the I2C interface has sent the expected amount of data
		 *		this event will be called to allow the application to prepare
		 *		for the next frame.
		 *	@param		pI2CIface		Pointer to the I2C interface object
		 *	@param		pfHandler		Pointer to function that will handle this event
		 *	@return		I2C_Success upon success, or a code indicating the
		 *		error encountered.
		 */
		eI2CReturn_t	(*pfSlaveSetSendCompHandler)	(sI2CIface_t *pI2CIface, pfI2CSlaveCompHandler_t pfHandler);

		/**	@brief		Sets the handler for the Receive Complete Event
		 *	@details	For slave I2C Interface only.
		 *		When the master on the connected I2C buss has ended the data
		 *		transfer the Transaction Complete event will be raised, called
		 *		the function specified here.
		 *		This may be called prior to the Send/Receive Complete events it
		 *		the master ends the transfer prematurely.
		 *	@param		pI2CIface		Pointer to the I2C interface object
		 *	@param		pfHandler		Pointer to function that will handle this event
		 *	@return		I2C_Success upon success, or a code indicating the
		 *		error encountered.
		 */
		eI2CReturn_t	(*pfSlaveSetTransCompHandler)	(sI2CIface_t *pI2CIface, pfI2CSlaveCompHandler_t pfHandler);

		/**	@brief		Queues data to be sent after the master requests data
		 *	@details	The data buffer pointed to will not be copied.
		 *		Modifying its contents will modify the data that will be sent.
		 *		The memory can be reused once the Send Complete event is
		 *		raised and handled.
		 *	@param		pI2CIface		Pointer to the I2C interface object
		 *	@param		nDataLen		Length of the data to send
		 *	@param		pData			Pointer to data buffer
		 *	@return		I2C_Success upon success, or a code indicating the
		 *		error encountered.
		 */
		eI2CReturn_t	(*pfSlaveSendData)				(sI2CIface_t *pI2CIface, uint16_t nDataLen, uint8_t *pData);

		/**	@brief		Queues data to be received after the master sends data
		 *	@details	The data buffer will be stored to write data to, it
		 *		will not be entirely updated until the Receive Complete event
		 *		is raised and handled.
		 *	@param		pI2CIface		Pointer to the I2C interface object
		 *	@param		nDataLen		Amount of data to expect
		 *	@param		pData			Pointer to data buffer
		 *	@return		I2C_Success upon success, or a code indicating the
		 *		error encountered.
		 */
		eI2CReturn_t	(*pfSlaveRecvData)				(sI2CIface_t *pI2CIface, uint16_t nDataLen, uint8_t *pData);
		
		eI2CCapabilities_t	eCapabilities;	/**< Capabilities flags this interface provides */
		
		bool			bMaster;			/**< True if interface is master on the bus */
		uint16_t		nSlaveAddress;		/**< Address slave is listening for */
		uint32_t		nClockFreq;			/**< Bus clock frequency */
		void			*pHWInfo;			/**< Pointer to hardware information */
	} sI2CIface_t;

/*****	Globals		*****/


/*****	Prototypes	*****/
	/**	@brief		Initializes an I2C Interface object and prepare it for use
		@ingroup	i2ciface
	*/
	eI2CReturn_t I2CInterfaceInitialize(sI2CIface_t *pI2CIface);
#endif
