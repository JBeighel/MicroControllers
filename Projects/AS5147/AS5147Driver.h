/**	@defgroup	as5147driver
	@brief		Peripheral driver for the AS5147 magnetic position sensor
	@details	v0.01
	# Description #
	
	# Usage #
	
	# File Information #
		File:	AS5147Driverh
		Author:	J. Beighel
		Created:09-28-2020
*/

#ifndef __AS5147DRIVER_H
	#define __AS5147DRIVER_H

/*****	Includes	*****/
	#include "GPIOGeneralInterface.h"
	#include "SPIGeneralInterface.h"

/*****	Definitions	*****/
	/**	@brief		Return codes for the AS5147 peripheral
		@ingroup	as5147driver
	*/
	typedef enum eAS5147Return_t {
		AS5147Warn_Parity		= 2,	/**< Received data did not have correct parity bit */
		AS5147Warn_Unknown		= 1,	/**< An undescribed but recoverable error occurred */
		AS5147_Success			= 0,	/**< The operation completed successfully */
		AS5147Fail_Unknown		= -1,	/**< An undescribed and unrecoverable error occurred */
	} eAS5147Return_t;
	
	/**	@brief		Register addresses for the AS5147
		@details	The addresses go in the lowest 14 bits of the 16 bit command
			frame of the device.  Read/Write and patrity bits are the highest 2 bits.
			The device uses Even parity,  so the parity bit is set to make the count
			of set bit even.
		@ingroup	as5147driver
	*/
	typedef enum eAS5147Registers_t {
		AS5147Reg_DataMask		= 0x3FFF,	/**< Mask for all data bits in a frame */
		AS5147Reg_FramingMask	= 0xC000,	/**< Mask for all framing bits in a frame */
		
		//Volatile registers
		AS5147Reg_NOP			= 0x0000,	/**< No operation */
		AS5147Reg_ERRFL			= 0x0001,	/**< Error register */
		AS5147Reg_PROG			= 0x0003,	/**< Programming register */
		AS5147Reg_DIAAGC		= 0x3FFC,	/**< Diagnostic and AGC */
		AS5147Reg_MAG			= 0x3FFD,	/**< CORDIC Magnitude */
		AS5147Reg_ANGLEUNC		= 0x3FFE,	/**< Measured angle without dynamic error compensation */
		AS5147Reg_ANGLECOM		= 0x3FFF,	/**< Measured angle with dynamic error compensation */
		
		//Non-volatile registers
		AS5147Reg_ZPOSM			= 0x0016,	/**< Zero position MSB */
		AS5147Reg_ZPOSL			= 0x0017,	/**< Zero position LSB */
		AS5147Reg_SETTINGS1		= 0x0018,	/**< Custom settings register 1 */
		AS5147Reg_SETTINGS2		= 0x0019,	/**< Custom settings register 2 */
		AS5147Reg_RED			= 0x001A,	/**< Redundancy register */
		
		//Framing bits
		AS5147Reg_ReadBit		= 0x4000,	/**< Bit to set in order to read from a register */
		AS5147Reg_WriteBit		= 0x0000,	/**< Bit to set in order to write to a register */
		AS5147Reg_DataBit		= 0x0000,	/**< Bit to set in to denote data */
		
		AS5147Reg_ParitySet		= 0x8000,	/**< Parity bit set to make the frame even */
		AS5147Reg_ParityClear	= 0x0000,	/**< Parity bit cleared to make the frame even */
	} eAS5147Registers_t;
	
	/**	@brief		Enumeration of all bits used in the error register
		@details	This register is cleared automatically after reading it.
		@ingroup	as5147driver
	*/
	typedef enum eAS5147RegErr {
		AS5147RegErr_ParErr		= 0x0004,	/**< Parity error */
		AS5147RegErr_InvComm	= 0x0002,	/**< Invalid command, read/write to invalid address */
		AS5147RegErr_FrErr		= 0x0001,	/**< Framing error, non-compliant SPI frame detected */
	} eAS5147RegErr;
	
	/**	@brief		Enumeration of all bits used in the error register
		@ingroup	as5147driver
	*/
	typedef enum eAS5147RegDiag {
		AS5147RegDiag_MagLow	= 0x0800,	/**< Set when the magnetic field is too low, AGC = 0xFF */
		AS5147RegDiag_MagHigh	= 0x0400,	/**< Set when the magnetic field is too high, AGC = 0x00 */
		AS5147RegDiag_COF		= 0x0200,	/**< CORDIC overflow */
		AS5147RegDiag_LF		= 0x0100,	/**< Loops finished; 0: offset loops not finished, 1:offset loop finished */
		AS5147RegDiag_AGCMask	= 0x00FF,	/**< Automatic gain fain; 0x00 no gain, 0xFF full gain */
	} eAS5147RegDiag;
	
	/**	@brief		Structure containing all information for the AS5147 device
		@ingroup	as5147driver
	*/
	typedef struct sAS5147Dev_t {
		sGPIOIface_t *pGpio;			/**< Pointer to the GPIO interface */
		sSPIIface_t *pSpi;				/**< Pointer to the SPI interface */
		uint16_t nCSPin;				/**< Chip select pin number */
	} sAS5147Dev_t;

/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	/**	@brief		Initializes the device driver object and readies the device
		@details	Will attempt to set the chip select pin mode as well as set it to the
			devices unselected value.  In addtion it will communicate with the peripheral
			in order to confirm it is using the correct configuration.
		@ingroup	as5147driver
	*/
	eAS5147Return_t AS5147Init(sAS5147Dev_t *pDev, sGPIOIface_t *pGpio, sSPIIface_t *pSpi, uint16_t nCSPin);
	
	/**< @brief		Reads all information relevant to the sensors reading
		@details	Reads multiple values from the device and returns them.  This is meant 
			for initial testing of the device.  Specific functions will be added that get
			targetted data from it for application use.
		@ingroup	as5147driver
	*/
	eAS5147Return_t AS5147TakeReading(sAS5147Dev_t *pDev, uint16_t *pnAngleUnc, uint16_t *pnAngleCom, uint16_t *pnMag, uint16_t *pnAGC);

/*****	Functions	*****/


#endif

