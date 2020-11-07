/**	@defgroup	as5050driver
	@brief		Peripheral driver for the MLX90393 triaxis magnetometer sensor
	@details	v0.01
	# Description #
	
	# Usage #
	
	# File Information #
		File:	MLX90393Driverh
		Author:	J. Beighel
		Created:10-14-2020
*/

#ifndef __MLX90393DRIVER_H
	#define __MLX90393DRIVER_H

/*****	Includes	*****/
	#include "GPIOGeneralInterface.h"
	#include "SPIGeneralInterface.h"
	#include "I2CGeneralInterface.h"

/*****	Definitions	*****/
	/**	@brief		Return codes for the MLX90393 peripheral
		@ingroup	mlx90393driver
	*/
	typedef enum eMLX90393Return_t {
		MLX90393Warn_PartialData= 3,
		MLX90393Warn_SingleErr	= 2,	/**< The device reported a single error and recovered */
		MLX90393Warn_Unknown	= 1,	/**< An undescribed but recoverable error occurred */
		MLX90393_Success		= 0,	/**< The operation completed successfully */
		MLX90393Fail_Unknown	= -1,	/**< An undescribed and unrecoverable error occurred */
		MLX90393Fail_DeviceErr	= -2,	/**< The device reported an error that requires it be resarted */
		MLX90393Fail_DataVerify	= -3,	/**< A verify read of a write operation did not return the epxpected value */
		MLX90393Fail_MeasTimeout= -4,
		MLX90393Fail_BusError	= -5,
	} eMLX90393Return_t;
	
	typedef enum eMLX90393I2CAddr_t {
		MLX90393_None			= 0x00,
		
		MLX90393_LW_ABA_011Base	= 0x0C,
		MLX90393_LW_ABA_012Base	= 0x10,
		MLX90393_LW_ABA_013Base	= 0x14,
		MLX90393_LW_ABA_014Base	= 0x18,
		MLX90393_LQ_ABA_011Base	= 0x0C,
		
		MLX90393_A0				= 0x01,
		MLX90393_A1				= 0x02,
	} eMLX90393I2CAddr_t;
	
	typedef enum eMLX90393Command_t {
		MLX90393_NOP			= 0x00,	/**< No operation, used to accept data */
		
		//Lower nibble is zyxt bits
		MLX90393_StartBurst		= 0x10,	/**< Start burst mode */
		MLX90393_StartWakeOnChng= 0x20,	/**< Start wake-up on change mode */
		MLX90393_SingleMeasure	= 0x30, /**< Start single measurement mode */
		MLX90393_ReadMeasure	= 0x40, /**< Read a single measurement */
		
		MLX90393_XAxis			= 0x02,	/**< Include sensor data for the X axis */
		MLX90393_YAxis			= 0x04,	/**< Include sensor data for the Y axis */
		MLX90393_ZAxis			= 0x08,	/**< Include sensor data for the Z axis */
		MLX90393_Temp			= 0x01,	/**< Include sensor data for temperature */

		//Lower nibble is 0abc bits
		MLX90393_ReadReg		= 0x50,	/**< Read register, followed by 1 address byte, reads from volatile memory */
		MLX90393_WriteReg		= 0x60, /**< Read register, followed by 2 data bytes and 1 address byte, writes to volatile memory */
		
		MLX90393_ABC			= 0x00,	/**< For the memory access commands ABC should be oxoo to get normal read/write from the memory */
		
		//Commads define both nibbles
		MLX90393_ExitMode		= 0x80,	/**< Exit Mode */
		MLX90393_MemoryRecall	= 0xD0,	/**< Memory recall, loads contents of NV space to volatile memory */
		MLX90393_MemoryStore	= 0xE0, /**< Memory store, writes volatile memory to NV space */
		MLX90393_Reset			= 0xF0,	/**< Device reset */
	} eMLX90393Command_t;
	
	/** @brief		The status byte is first byte transmitted in response to a commands
		@ingroup	mlx90393driver
	*/
	typedef enum eMLX90393Status_t {
		MLX90393Stat_BurstMode	= 0x80,	/**< Set if device is in burst data mode */
		MLX90393Stat_WOCMode	= 0x40,	/**< Set if the device is in wake on change mode */
		MLX90393Stat_SMMode		= 0x20,	/**< Set if the device is in single measurement mode */
		MLX90393Stat_Error		= 0x10,	/**< Set if a command has been rejected or uncorrectable error is detected */
		MLX90393Stat_SED		= 0x08,	/**< Single error detected, one error is corrected two errors and the device must be reset */
		MLX90393Stat_RS			= 0x04,	/**< Set if the device has  just reset, set in the first status byte is read then cleared */
		MLX90393Stat_D1			= 0x02,	/**< Only used after reading a register or measurement.  The number of bytes being sent is 2 * ((D1 << 1) | D0) + 2 */
		MLX90393Stat_D2			= 0x01,	/**< Only used after reading a register or measurement.  The number of bytes being sent is 2 * ((D1 << 1) | D0) + 2 */
	} eMLX90393Status_t;
	
	/**	@brief		Enumeration of all significant addresses available in the device
		@details	There are a number of registers that are defined for use by the device as
			configuration values.  There is also a block of free space that is available for
			use by the application.
			
			These values are kept in volatile memory.  These values can be written to and loaded from non-volatile memory through specific commands that will copy the 
			entire memory contents.
			
			When transferring the address to the peripheral it requires the address be bit 
			shifted two places.  This is included in the definitions.
		@ingroup	mlx90393driver
	*/
	typedef enum eMLX90393MemMap_t {
		MLX90393Mem_Config1		= 0x00 << 2,	/**< First configuration register */
		MLX90393Mem_Config2		= 0x01 << 2,	/**< Second configuration register */
		MLX90393Mem_Config3		= 0x02 << 2,	/**< Third configuration register */
		MLX90393Mem_SenseComp	= 0x03 << 2,	/**< Sensitivity drift compensation factors */
		MLX90393Mem_OffsetX		= 0x04 << 2,	/**< Constant offset correction for X direction */
		MLX90393Mem_OffsetY		= 0x05 << 2,	/**< Constant offset correction for Y direction */
		MLX90393Mem_OffsetZ		= 0x06 << 2,	/**< Constant offset correction for Z direction */
		MLX90393Mem_WOCThreshXY	= 0x07 << 2,	/**< Wake-up on change threshold for X and Y directions */
		MLX90393Mem_WOCThreshZ	= 0x08 << 2,	/**< Wake-up on change threshold for Z directions */
		MLX90393Mem_WOCThreshT	= 0x09 << 2,	/**< Wake-up on change threshold for temperature */
		MLX90393Mem_BeginFree	= 0x0A << 2,	/**< First address of freely available space */
		MLX90393Mem_EndFree		= 0x1F << 2,	/**< Last address of freely available space */
	} eMLX90393MemMap_t;
	
	typedef enum eMLX90393RegCfg1_t {
		MLX90393Cfg1_AnaResLowMask	= 0xFE00,	/**< Reserved bits for analog trimming, set at factory, do not modify */
		MLX90393Cfg1_BIST			= 0x0100,	/**< Enables (1) or disables (0) built in self-test coil */
		MLX90393Cfg1_ZSeries		= 0x0080,	/**< Enables series connection of hall plates for Z axis, normal operation set to 0 */
		
		MLX90393Cfg1_GainSelMask	= 0x0070,	/**< Mask for bts that set the analog gain.  Sensitivity is dependent on this and the ResX, ResY, and ResZ settings in COnfig 3 */
		
		MLX90393Cfg1_HallConfMask	= 0x000F,	/**< Modifies hall plate spinning which effects minimum sampling rate.  Default is 0x000C */
		MLX90393Cfg1_HallConfDefault= 0x000C,
		
	} eMLX90393RegCfg1_t;
	
	typedef enum eMLX90393RegCfg2_t {
		MLX90393Cfg2_BstSelMask	= 0x03C0,	/**< Specifies the axes to report if BurstMode command is sent with 0 for XYZT */
		MLX90393Cfg2_BstSelX	= 0x0200,	/**< Burst Selection for X axis */
		MLX90393Cfg2_BstSelY	= 0x0040,	/**< Burst Selection for Y axis */
		MLX90393Cfg2_BstSelZ	= 0x0080,	/**< Burst Selection for Z axis */
		MLX90393Cfg2_BstSelT	= 0x0100,	/**< Burst Selection for temperature */
		
		MLX90393Cfg2_BstRateMask= 0x003F,	/**< Defines Tinterval as BstRaate * 20ms */
		
		MLX90393Cfg2_CommModeMask=0x6000,	/**< Specifies the communications interrupt in use */
		MLX90393Cfg2_CommSPIOnly= 0x4000,	/**< Only SPI communications in use */
		MLX90393Cfg2_CommI2COnly= 0x6000,	/**< Only I2C communications in use */
		MLX90393Cfg2_CommI2CSPI	= 0x0000,	/**< Both I2C and SPI communications allowed, selection is through CS pin */
		
		MLX90393Cfg2_TrigInt	= 0x8000,	/**< When 0 the TrigInt pin is in trigger mode, if 1 TrigInt pin acts as an interrupt pin */
		MLX90393Cfg2_WOCDiff	= 0x1000,	/**< If 0 First initial measurement is reference for change, if 1 previous sample is reference for change */
		MLX90393Cfg2_ExtTrig	= 0x0800,	/**< If 0 desables external trigger, 1 enables external trigger */
		MLX90393Cfg2_TCmpEn		= 0x0400,	/**< Enable 1, Disables 0 the sensitivity drift compensation */
	} eMLX90393RegCfg2_t;
	
	typedef enum eMLX90393RegCfg3_t {
		MLX90393Cfg3_OSR2Mask	= 0x1800,
		MLX90393Cfg3_ResZMask	= 0x0600,
		MLX90393Cfg3_ResYMask	= 0x0180,
		MLX90393Cfg3_ResXMask	= 0x0060,
		MLX90393Cfg3_DifFiltMask= 0x001C,
		MLX90393Cfg3_OSRMask	= 0x0003,
	} eMLX90393RegCfg3_t;
	
	/**	@brief		Structure containing all information for the MLX90393 device
		@ingroup	mlx90393driver
	*/
	typedef struct sMLX90393Dev_t {
		sGPIOIface_t *pGpio;			/**< Pointer to the GPIO interface */
		sSPIIface_t *pSpi;				/**< Pointer to the SPI interface */
		sI2CIface_t *pI2C;
		uint16_t nCSPin;				/**< Chip select pin number */
		uint16_t nTriggerPin;
		uint16_t nInterruptPin;
		eMLX90393I2CAddr_t nI2CAddr;
	} sMLX90393Dev_t;

/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	/**	@brief		Initializes the device driver object and readies the device
		@details	Will attempt to set the chip select pin mode as well as set it to the
			devices unselected value.  In addtion it will communicate with the peripheral
			in order to confirm it is using the correct configuration.
		@ingroup	mlx90393driver
	*/
	eMLX90393Return_t MLX90393SpiInit(sMLX90393Dev_t *pDev, sGPIOIface_t *pGpio, sSPIIface_t *pSpi, uint16_t nCSPin);
	
	eMLX90393Return_t MLX90393I2CInit(sMLX90393Dev_t *pDev, sGPIOIface_t *pGpio, sI2CIface_t *pI2C, eMLX90393I2CAddr_t eAddr, uint8_t nTrigPin, uint8_t nIntPin);
	
	/**< @brief		Reads all information relevant to the sensors reading
		@details	Reads multiple values from the device and returns them.  This is meant 
			for initial testing of the device.  Specific functions will be added that get
			targetted data from it for application use.
		@ingroup	mlx90393driver
	*/
	eMLX90393Return_t MLX90393TakeReading(sMLX90393Dev_t *pDev, uint16_t *pXDir, uint16_t *pnYDir, uint16_t *pnZDir, uint16_t *pnTemp);
	
	eMLX90393Return_t MLX90393Reset(sMLX90393Dev_t *pDev);

/*****	Functions	*****/


#endif

