/**	@defgroup	ads1115driver
	@brief		Peripheral driver for the ADS1115 analog to digital converter
	@details	v0.2
	#Description
		This device can measure inputs from Gnd to Vdd.  However, each cound from the ADC is 
		a fixed value of 76.3 microvolts, such that a reading of 32767 equals 2.5 volts (1/2 Vdd).  This means that its effective measurement range is from -2.5 volts to +2.5 volts.  Setting a 2.5 volt reference on an input to compare to allows the full
		range to be used.
		
		The default for the chip is to measure A0 compared to A1.  Positive values for A0 is
		greater than A1 with 32767 meaning A0 >= A1 + 2.5 volts; negative values for A0 is 
		less than A1 with -32767 meaning A0 <= A1 - 2.5 volts.
		
		The default for the device also sets it into single measurement mode.  Where it will
		need to be instructred to take a measurement which can then be read out of the 
		conversion register.
	
	#File Information
		File:	ADS1115Driver.h
		Author:	J. Beighel
		Date:	28-28-2021
*/

#ifndef __ADS1115Driver_h
	#define __ADS1115Driver_h

/*****	Includes	*****/
	#include "CommonUtils.h"

	#include "GPIOGeneralInterface.h"
	#include "I2CGeneralInterface.h"

/*****	Constants	*****/
	#define ADS1115_I2CCaps		(I2CCap_ReadData | I2CCap_WriteData)

/*****	Definitions	*****/
	typedef enum eADS1115Return_t {
		ADS1115Warn_Unknown		= 1,	/**< An unknown but recoverable error happened during the operation */
		ADS1115_Success			= 0,	/**< The operation completed successfully */
		ADS1115Fail_Unknown		= -1,	/**< An unknown and unrecoverable error happened during the operation */
		ADS1115Fail_BusError	= -2,	/**< Communication bus failure */
		ADS1115Fail_NoConfig	= -3,	/**< An attempt to change the config register failed */
		ADS1115Fail_Timeout		= -4,	/**< Timed out waiting for results from device */
		ADS1115Fail_Invalid		= -5,	/**< An invalid setting was requested */
	} eADS1115Return_t;
	
	typedef enum eADS1115I2CAddr_t {
		ADS1115Addr_Base		= 0x48,	/**< Base I2C address of the peripheral */
		ADS1115Addr_AddrToGnd	= 0x00,	/**< Additional address bits if the address pin is connected to ground */
		ADS1115Addr_AddrToVdd	= 0x01,	/**< Additional address bits if the address pin is connected to Vdd */
		ADS1115Addr_AddrToSDA	= 0x02,	/**< Additional address bits if the address pin is connected to SDA */
		ADS1115Addr_AddrToSCL	= 0x03,	/**< Additional address bits if the address pin is connected to SCL */
	} eADS1115I2CAddr_t;
	
	typedef enum eADS1115Reg_t {
		ADS1115_Conversion		= 0x00,	/**< Result of last conversion in binary two's compliment */
		ADS1115_Config			= 0x01,	/**< Configuration register, sets mode, input, data rate, range and comparator */
		ADS1115_LowThresh		= 0x02,	/**< Lower threshold value used by the comparator */
		ADS1115_HighThresh		= 0x03,	/**< Upper threshold value used by the comparator */
	} eADS1115Reg_t;
	
	typedef enum eADS1115Config_t {
		ADS1115Cfg_OpStatMask	= 0x8000,	/**< Maske to find the bits used by the operational status */
		ADS1115Cfg_OpStatStConv	= 0x8000,	/**< Write this value to begin a conversion */
		ADS1115Cfg_OpStatConv	= 0x0000,	/**< When read this means a conversion is underway */
		ADS1115Cfg_OpStatNoConv	= 0x8000,	/**< When read this means no conversion is happening */
		
		ADS1115Cfg_MUXMask		= 0x7000,	/**< Mask of all bits used by the MUX setting */
		ADS1115Cfg_MUXA0toA1	= 0x0000,	/**< Multiplexer setting to compare input A0 to input A1 */
		ADS1115Cfg_MUXA0toA3	= 0x1000,	/**< Multiplexer setting to compare input A0 to input A3 */
		ADS1115Cfg_MUXA1toA3	= 0x2000,	/**< Multiplexer setting to compare input A1 to input A3 */
		ADS1115Cfg_MUXA2toA3	= 0x3000,	/**< Multiplexer setting to compare input A2 to input A3 */
		ADS1115Cfg_MUXA0toGnd	= 0x4000,	/**< Multiplexer setting to compare input A0 to ground */
		ADS1115Cfg_MUXA1toGnd	= 0x5000,	/**< Multiplexer setting to compare input A1 to ground */
		ADS1115Cfg_MUXA2toGnd	= 0x6000,	/**< Multiplexer setting to compare input A2 to ground */
		ADS1115Cfg_MUXA3toGnd	= 0x7000,	/**< Multiplexer setting to compare input A3 to ground */
		
		ADS1115Cfg_PGAMask		= 0x0E00,	/**< Mask of all bits used by the PGA setting */
		ADS1115Cfg_PGA6144		= 0x0000,	/**< FSR = +/- 6.144 V */
		ADS1115Cfg_PGA4096		= 0x0200,	/**< FSR = +/- 4.096 V */
		ADS1115Cfg_PGA2048		= 0x0400,	/**< FSR = +/- 2.048 V */
		ADS1115Cfg_PGA1024		= 0x0600,	/**< FSR = +/- 1.024 V */
		ADS1115Cfg_PGA0512		= 0x0800,	/**< FSR = +/- 0.512 V */
		ADS1115Cfg_PGA0256		= 0x0A00,	/**< FSR = +/- 0.256 V */
		ADS1115Cfg_PGA0256_1	= 0x0C00,	/**< FSR = +/- 0.256 V */
		ADS1115Cfg_PGA0256_2	= 0x0E00,	/**< FSR = +/- 0.256 V */
		
		ADS1115Cfg_ModeMask		= 0x0100,	/**< Mask of all bits used by the mode setting */
		ADS1115Cfg_ModeContin	= 0x0000,	/**< COntinuous conversion mode */
		ADS1115Cfg_ModeSingle	= 0x0100,	/**< Single shot mode or power down state */
		
		ADS1115Cfg_DRMask		= 0x00E0,	/**< Mask of all bits used by the data rate setting */
		ADS1115Cfg_DR8SPS		= 0x0000,	/**< 8 samples per second */
		ADS1115Cfg_DR16SPS		= 0x0020,	/**< 16 samples per second */
		ADS1115Cfg_DR32SPS		= 0x0040,	/**< 32 samples per second */
		ADS1115Cfg_DR64SPS		= 0x0060,	/**< 64 samples per second */
		ADS1115Cfg_DR128SPS		= 0x0080,	/**< 128 samples per second */
		ADS1115Cfg_DR250SPS		= 0x00A0,	/**< 250 samples per second */
		ADS1115Cfg_DR475SPS		= 0x00C0,	/**< 475 samples per second */
		ADS1115Cfg_DR800SPS		= 0x00E0,	/**< 800 samples per second */
		
		ADS1115Cfg_CompMdMask	= 0x0010,	/**< Mask of all buts used by the comparator mode setting */
		ADS1115Cfg_CompMdTrad	= 0x0000,
		ADS1115Cfg_CompMdWindow	= 0x0010,
		
		ADS1115Cfg_CompPolMask	= 0x0008,	/**< Mask of all buts used by the comparator polarity setting */
		ADS1115Cfg_CompPolLow	= 0x0000,	/**< Active low */
		ADS1115Cfg_CompPolHigh	= 0x0008,	/**< Active high */
		
		ADS1115Cfg_CompLatMask	= 0x0004,	/**< Mask of all buts used by the comparator latching setting */
		ADS1115Cfg_CompLatNonL	= 0x0000,	/**< Non-latching comparator, Assert/Ready pins are not set */	
		ADS1115Cfg_CompLatLatch	= 0x0004,	/**< Latching comparator, Assert/Ready pin is set until conversion data is read */
		
		ADS1115Cfg_CompQMask	= 0x0003,	/**< Mask of all buts used by the comparatorr
		queue setting */
		ADS1115Cfg_CompQ1Conv	= 0x0000,	/**< Assert Alert/Ready after 1 conversion */
		ADS1115Cfg_CompQ2Conv	= 0x0001,	/**< Assert Alert/Ready after 2 conversions */
		ADS1115Cfg_CompQ4Conv	= 0x0002,	/**< Assert Alert/Ready after 4 conversions */
		ADS1115Cfg_CompQDis		= 0x0003,	/**< Disable comparator and set Alert/Ready to high impedance */
		
		ADS1115Cfg_Default		= 0x8583,	/**< Default value of the Config register */
	} eADS1115Config_t;
	
	typedef enum eADS1115Measurement_t {
		ADS1115_MeasA0ToA1,
		ADS1115_MeasA0ToA3,
		ADS1115_MeasA1ToA3,
		ADS1115_MeasA2ToA3,
		ADS1115_MeasA0ToGnd,
		ADS1115_MeasA1ToGnd,
		ADS1115_MeasA2ToGnd,
		ADS1115_MeasA3ToGnd,
	} eADS1115Measurement_t;
	
	typedef struct sADS1115Dev_t {
		uint8_t nI2CAddr;
		uint16_t nAlertPin;
		uint16_t nReadyPin;
		uint16_t nConfig;
		sI2CIface_t *pI2C;
		sGPIOIface_t *pGpio;
	} sADS1115Dev_t;

/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	/**	@brief		Prepares the program object as well as the peripheral configuration
		@ingroup	ads1115driver
	*/
	eADS1115Return_t ADS1115Init(sADS1115Dev_t *pDev, sGPIOIface_t *pGpio, sI2CIface_t *pI2C, eADS1115I2CAddr_t eI2CAddr, uint16_t nAlertPin);
	
	/**	@brief		Sets the input tho measure and the point to compare it to
		@ingroup	ads1115driver
	*/
	eADS1115Return_t ADS1115SetMeasurementInput(sADS1115Dev_t *pDev, eADS1115Measurement_t eMeas);
	
	/**	@brief		Singals the device to perform a single conversion
		@details	This function will request the device to take a measurement then will 
			block until the device signals that the conversion is complete.  It will return
			once the results have been read from the peripheral.
		@ingroup	ads1115driver
	*/
	eADS1115Return_t ADS1115TakeSingleReading(sADS1115Dev_t *pDev, int16_t *pnA0Val);

/*****	Functions	*****/


#endif

