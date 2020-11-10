/**	@defgroup	tlv493ddriver
	@brief		Peripheral driver for the ADS1115 analog to digital converter
	@details	v0.1
	#Description
		
	
	#File Information
		File:	TLV493DDriver.h
		Author:	J. Beighel
		Date:	10-21-2020
*/

#ifndef __TLV493DDriver_h
	#define __TLV493DDriver_h

/*****	Includes	*****/
	#include "CommonUtils.h"
	#include "GPIOGeneralInterface.h"
	#include "I2CGeneralInterface.h"

/*****	Constants	*****/
	#define TLV493D_I2CCaps		(I2CCap_ReadData | I2CCap_WriteData | I2CCap_GeneralCall)
	
	#define TLV493D_12BITFACTOR	0.098
	
	#define TLV493D_8BITFACTOR	145.1
	
	#define TLV493D_TEMPFACTOR	1.1
	
	#define TLV493D_TEMPOFFSET	25

/*****	Definitions	*****/
	typedef enum eTLV493DReturn_t {
		TLV493DWarn_ConvInPg	= 2,	/**< A conversion was in progress, measurements unreliable */
		TLV493DWarn_Unknown		= 1,	/**< An unknown but recoverable error happened during the operation */
		TLV493D_Success			= 0,	/**< The operation completed successfully */
		TLV493DFail_Unknown		= -1,	/**< An unknown and unrecoverable error happened during the operation */
		TLV493DFail_BusError	= -2,	/**< Communication bus failure */
		TLV493DFail_NoConfig	= -3,	/**< An attempt to change the config register failed */
		TLV493DFail_Timeout		= -4,	/**< Timed out waiting for results from device */
		TLV493DFail_DeviceErr	= -5,	/**< The device had a problem flag set */
	} eTLV493DReturn_t;
	
	typedef enum eTLV493DI2CAddr_t {
		TLV493DAddr_Base		= 0x5E,	/**< Base I2C address of the peripheral */
		TLV493DAddr_SDAHigh		= 0x1F,	/**< Address if SDA is pulled high on power up */
		
		TLV493DAddr_Bus0SDAHigh	= 0x4B,
		TLV493DAddr_Bus1SDAHigh	= 0x4E,
		TLV493DAddr_Bus2SDAHigh	= 0x5B,
		TLV493DAddr_Bus3SDAHigh	= 0x5E,
		TLV493DAddr_Bus4SDALow	= 0x0C,
		TLV493DAddr_Bus5SDALow	= 0x0F,
		TLV493DAddr_Bus6SDALow	= 0x1C,
		TLV493DAddr_Bus7SDALow	= 0x1F,
	} eTLV493DI2CAddr_t;
	
	/**	@brief		Enumeration of the registers in the peripheral
		@details	The registers are not addressed.  Instead the data is written/read 
			sequentially.  Therefore to read all of the temperature you must read through 
			all of the other registers first.
			
			When reading and writing the registers available will change.
		@ingroup	tlv493ddriver
	*/
	typedef enum eTLV493DReg_t {
		TLV493DReg_RdCount		= 7,
		TLV493DReg_RdXHigh		= 0,
		TLV493DReg_RdYHigh		= 1,
		TLV493DReg_RdZHigh		= 2,
		TLV493DReg_RdTempHigh	= 3,
		TLV493DReg_RdXYLow		= 4,
		TLV493DReg_RdZLow		= 5,
		TLV493DReg_RdTempLow	= 6,
		
		TLV493DReg_WrCount		= 4,
		TLV493DReg_WrRes1		= 0,
		TLV493DReg_WrMode1		= 1,
		TLV493DReg_WrRes2		= 2,
		TLV493DReg_WrMode2		= 3,
		
		TLV493DReg_RdInitCount	= 10,
		TLV493DReg_RdReg8		= 8,
		TLV493DReg_RdReg9		= 9,
	} eTLV493DReg_t;
	
	typedef enum eTLV493DHigh_t {
		TLV493DHigh_XYZMask			= 0xFF,	/**< Mask of all bits used for the X, Y and Z high bits */
		TLV493DHigh_XYZLShift		= 4,	/**< Number of bits to left shift the high bits when combining to form X Y and Z measurements */
		
		TLV493DHigh_TempTempMask	= 0xF0,	/**< Mask of all bits used for the Temperature high bits */
		TLV493DHigh_TempLShift		= 4,	/**< Number of bits to left shift the high bits when combining to form temperature measurement */	
		TLV493DHigh_TempFrmMask		= 0x0C,	/**< Mask of the bits used to hold the frame counter */
		
		TLV493DHigh_TempChMask		= 0x03,	/**< Mask of all bits used to indicate which channel is being converted */
		TLV493DHigh_TempChConvX		= 0x00,	/**< Indicates the X channel is being converted, this also means it is safe to read previous conversion if PD (power down) is set */
		TLV493DHigh_TempChConvY		= 0x01,	/**< Indicates the Y channel is being converted */
		TLV493DHigh_TempChConvZ		= 0x02,	/**< Indicates the Z channel is being converted */
		TLV493DHigh_TempChConvT		= 0x03,	/**< Indicates the temperature channel is being converted */
	} eTLV493DHigh_t;
	
	typedef enum eTLV493DLow_t {
		TLV493DLow_XMask			= 0xF0,	/**< Mask of all bits used for the X reading */
		TLV493DLow_XRShift			= 4,	/**< Number of bits to right shift the X bits when combining to form a measurement */
		TLV493DLow_YMask			= 0x0F,	/**< Mask of all bits used for the Y reading */
		TLV493DLow_YRShift			= 0,	/**< Number of bits to right shift the Y bits when combining to form a measurement */
		TLV493DLow_ZMask			= 0x0F,	/**< Mask of all bits used for the Z reading */
		TLV493DLow_ZRShift			= 0,	/**< Number of bits to right shift the Z bits when combining to form a measurement */
		TLV493DLow_TempMask			= 0xFF,	/**< Mask of all bits used for the Temperature reading */
		TLV493DLow_TempRShift		= 0,	/**< Number of bits to right shift the Temperature bits when combining to form a measurement */
		
		TLV493DLow_ZTestMode		= 0x40,	/**< Set to 1 if sensor is not in a normal mode */
		TLV493DLow_ZFuseFlag		= 0x20,	/**< Flag for internal fuse parity check, 1 if ok 0 if not (try sensor reset) */
		TLV493DLow_ZPowerDownFlag	= 0x10,	/**< 1 if all conversions are completed, 0 if any conversion is underway */
	} eTLV493DLow_t;
	
	typedef enum eTLV493DMode1_t {
		TLV493DM1_ParitySet		= 0x80,	/**< Sum of all 32 bits in write registers must be odd */
		TLV493DM1_ParityClear	= 0x00,
		
		TLV493DM1_I2CAddrMask	= 0x60,	/**< Bits used to set the I2C address when in Bus configuration */
		
		TLV493DM1_InterruptEn	= 0x04,	/**< Enable an interrupt pulse on the SCL pin */
		TLV493DM1_InterruptDis	= 0x00,	/**< Disable the interrupt pulse */
		
		TLV493DM1_FastEnable	= 0x02,	/**< Put the device in Fast mode (Disable fast mode before entering low power mode) */
		TLV493DM1_FastDisable	= 0x00,	/**< Disable fast mode */
		
		TLV493DM1_LowPowerEn	= 0x01,	/**< Put the device in low power mode  (Disable fast mode before entering low power mode) */
		TLV493DM1_LowPowerDis	= 0x00,	/**< Normal operation */
	} eTLV493DMode1_t;
	
	typedef enum eTLV493DMode2_t {
		TLV493DM2_TempEnable	= 0x80,	/**< Enable temperature conversion */
		TLV493DM2_TempDisable	= 0x00,	/**< Disable temperature conversion */
		
		TLV493DM2_LowPow100ms	= 0x00,	/**< Low power period is 100 milliseconds */
		TLV493DM2_LowPow12ms	= 0x40,	/**< Low power period is 12 milliseconds */
		
		TLV493DM2_ParityTestEn	= 0x20,	/**< Enable the parity test */
		TLV493DM2_ParityTestDis	= 0x00,	/**< Disable the parity test */
		
		TLV493DM2_Rd9Mask		= 0x1F,
	} eTLV493DMode2_t;
	
	typedef struct sTLV493DDev_t {
		uint8_t nI2CAddr;
		uint8_t nRdReg8;
		uint8_t nRdReg9;
		sI2CIface_t *pI2C;
		eI2CReturns_t eLastI2CErr;
	} sTLV493DDev_t;

/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	/**	@brief		Prepares the program object as well as the peripheral configuration
		@ingroup	tlv493ddriver
	*/
	eTLV493DReturn_t TLV493DInit(sTLV493DDev_t *pDev, sI2CIface_t *pI2C, eTLV493DI2CAddr_t eI2CAddr);
	
	eTLV493DReturn_t TLV493DReadSingleMeasurement(sTLV493DDev_t *pDev, float *pnXmT, float *pnYmT, float *pnZmT, float *pnTempC);

/*****	Functions	*****/


#endif

