/**	@defgroup	as5050driver
	@brief		Peripheral driver for the AS5050 magnetic position sensor
	@details	v0.01
	# Description #
	
	# Usage #
	
	# File Information #
		File:	AS5050Driverh
		Author:	J. Beighel
		Created:09-28-2020
*/

#ifndef __AS5050DRIVER_H
	#define __AS5050DRIVER_H

/*****	Includes	*****/
	#include "GPIOGeneralInterface.h"
	#include "SPIGeneralInterface.h"

/*****	Definitions	*****/
	/**	@brief		Return codes for the AS5050 peripheral
		@ingroup	as5050driver
	*/
	typedef enum eAS5050Return_t {
		AS5050Warn_DevError		= 3,	/**< The device is reporting an error when receiving data */
		AS5050Warn_Parity		= 2,	/**< Received data did not have correct parity bit */
		AS5050Warn_Unknown		= 1,	/**< An undescribed but recoverable error occurred */
		AS5050_Success			= 0,	/**< The operation completed successfully */
		AS5050Fail_Unknown		= -1,	/**< An undescribed and unrecoverable error occurred */
	} eAS5050Return_t;
	
	/**	@brief		Register addresses for the AS5050
		@details	The addresses go in the lowest 14 bits of the 16 bit command
			frame of the device.  Read/Write and patrity bits are the highest 2 bits.
			The device uses Even parity,  so the parity bit is set to make the count
			of set bit even.
		@ingroup	as5050driver
	*/
	typedef enum eAS5050Registers_t {
		AS5050Reg_CmdAddrMask	= 0x7FFE,	/**< Mask for all data bits in a frame */
		AS5050Reg_DataMask		= 0xFFFC,	/**< Mask for all data bits in a frame */
		AS5050Reg_CmdAddrLShift	= 1,		/**< Bits the LShift commands and addresses */
		AS5050Reg_DataShift		= 2,		/**< Bits to LShift data for write, and RShift for reads */
		
		//Commands
		AS5050Reg_POROff		= 0x3F22,	/**< Writing 0x005A to this register deactivates POR cell to reduce power use */
		AS5050Reg_SoftwareReset	= 0x3C00,	/**< Software reset */
		AS5050Reg_MasterReset	= 0x33A5,	/**< Master reset, no data frame needed */
		AS5050Reg_ClearEF		= 0x3380,	/**< Clear error flag, read this to clear should return 0x0000 */
		AS5050Reg_NOP			= 0x0000,	/**< No operation */
		
		
		//Volatile registers
		AS5050Reg_ErrStatus		= 0x335A,	/**< Error status register */
		AS5050Reg_AGC			= 0x3FF8,	/**< AGC, automatic gain control, register */
		AS5050Reg_Angle			= 0x3FFF,	/**< Measured angle */
		
		//Non-volatile registers
		AS5050Reg_SysConfig		= 0x3F20,	/**< System configuration register */
		
		//Framing bits
		AS5050Reg_ReadBit		= 0x8000,	/**< Bit to set in order to read from a register */
		AS5050Reg_WriteBit		= 0x0000,	/**< Bit to set in order to write to a register */
		AS5050Reg_DataBit		= 0x0000,	/**< Bit to set in to denote data */
		
		AS5050Reg_ParitySet		= 0x0001,	/**< Parity bit set to make the frame even */
		AS5050Reg_ParityClear	= 0x0000,	/**< Parity bit cleared to make the frame even */
		
		AS5050Reg_ErrFlag		= 0x0002,	/**< Data frames set this bit to indicate an error on the last message received */
	} eAS5050Registers_t;
	
	/**	@brief		Command to initiate a software reset and begin an angle conversion
		@ingroup	as5050driver
	*/
	typedef enum eAS5050RegSoftRes_t {
		AS5050RegSoftRes_SPIRes	= 0x0001,	/**< Bit to set to reset all SPI registers */
	} eAS5050RegSoftRes_t;
	
	/**	#brief		Reads the gain level from the peripheral
		@ingroup	as5050driver
	*/
	typedef enum eAS5050RegAGC_t {
		AS5050RegAGC_Mask		= 0x001F,	/**< Bit containing the AGC level */
	} eAS5050RegAGC_t;
	
	/**	@brief		Reads the angular measurement from the peripheral
		@ingroup	as5050driver
	*/
	typedef enum eAS5050RegAngle_t {
		eAS5050RegAngle_AlarmLo	= 0x1000,	/**< Alarm for too weak magnetic field */
		eAS5050RegAngle_AlarmHi	= 0x2000,	/**< Alarm for too strong magnetic field */
		eAS5050RegAngle_AngMask	= 0x03FF,	/**< Mask of all bits in measurement value */
	} eAS5050RegAngle_t;
	
	/**	@brief		Reports any errors the device encountered during operation
		@ingroup	as5050driver
	*/
	typedef enum eAS5050RegErr_t {
		//SPI errors
		eAS5050RegErr_Parity	= 0x0001,	/**< Transmitted parity does not match calculation */
		eAS5050RegErr_ClkMon	= 0x0002, 	/**< Amount of clock cycles is not correct */
		eAS5050RegErr_AddMon	= 0x0004,	/**< Non existing address was used */
		
		//System errors
		eAS5050RegErr_WOW		= 0x0010,	/**< Read during angle conversion */
		
		//DPS errors
		eAS5050RegErr_AdConv	= 0x0100,	/**< Magnetic field strength is too large for at least 1 Hall element */
		eAS5050RegErr_CORDIC	= 0x0200,	/**< Input signals to the CORDIC are too large and the algorithm fails */
		eAS5050RegErr_Range		= 0x0400,	/**< The Hall bias circuit has reached head room limit */
		eAS5050RegErr_FieldHI	= 0x0800,	/**< AGC is at or below mimimum, magnetci field too strong */
		eAS5050RegErr_FieldLO	= 0x1000,	/**< AGC is at or below maximum, magnetci field too weak */
	} eAS5050RegErr_t;
	
	/**	@brief		Register to set/get configruation and read chip identifiers
		@ingroup	as5050driver
	*/
	typedef enum eAS5050RegConf_t {
		eAS5050RegConf_GainFlags= 0x0018,	/**< Bits used to override disable AGC gain and force a gain in the sensor */
		eAS5050RegConf_IDMask	= 0x0E00,	/**< Reports silicon version */
		eAS5050RegConf_IDCurr	= 0x0400,	/**< Current silicon revision */
		eAS5050RegConf_ResMask	= 0x3000,	/**< Mask of bits reporting resolution of measurements */
		eAS5050RegConf_Res10Bit	= 0x1000,	/**< 10 bit resolution */
	} eAS5050RegConf_t;
	
	/**	@brief		Structure containing all information for the AS5050 device
		@ingroup	as5050driver
	*/
	typedef struct sAS5050Dev_t {
		sGPIOIface_t *pGpio;			/**< Pointer to the GPIO interface */
		sSPIIface_t *pSpi;				/**< Pointer to the SPI interface */
		uint16_t nCSPin;				/**< Chip select pin number */
		uint16_t nLastErr;
	} sAS5050Dev_t;

/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	/**	@brief		Initializes the device driver object and readies the device
		@details	Will attempt to set the chip select pin mode as well as set it to the
			devices unselected value.  In addtion it will communicate with the peripheral
			in order to confirm it is using the correct configuration.
		@ingroup	as5050driver
	*/
	eAS5050Return_t AS5050Init(sAS5050Dev_t *pDev, sGPIOIface_t *pGpio, sSPIIface_t *pSpi, uint16_t nCSPin);
	
	/**< @brief		Reads all information relevant to the sensors reading
		@details	Reads multiple values from the device and returns them.  This is meant 
			for initial testing of the device.  Specific functions will be added that get
			targetted data from it for application use.
		@ingroup	as5050driver
	*/
	eAS5050Return_t AS5050TakeReading(sAS5050Dev_t *pDev, uint16_t *pnAngle, uint16_t *pnAGC);

/*****	Functions	*****/


#endif

