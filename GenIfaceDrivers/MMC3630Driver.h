/**	@defgroup	mmc3630driver
	@brief		Peripheral driver for the ADS1115 analog to digital converter
	@details	v0.1
	#Description
		
	
	#File Information
		File:	MMC3630Driver.h
		Author:	J. Beighel
		Date:	10-22-2020
*/

#ifndef __MMC3630Driver_h
	#define __MMC3630Driver_h

/*****	Includes	*****/
	#include "CommonUtils.h"
	#include "GPIOGeneralInterface.h"
	#include "I2CGeneralInterface.h"

/*****	Constants	*****/
	#define MMC3630_I2CCaps		(I2CCap_ReadData | I2CCap_WriteData | I2CCap_ReadUint8Reg | I2CCap_WriteUint8Reg)
	
	/**	@brief		Factor to convert the integer temp value into degrees Chip
		@details	The reported value ranges from -75 to 125 degrees C.  This makes the 
			conversion factor (125 - -125)/256, or 200/256 = 0.78125
		@ingroup	mmc3630driver
	*/
	#define MMC3630_TEMPFACTOR	0.78125
	
	#define MMC3630_TEMPOFFSET	-75

/*****	Definitions	*****/
	typedef enum eMMC3630Return_t {
		MMC3630Warn_ConvInPg	= 2,	/**< A conversion was in progress, measurements unreliable */
		MMC3630Warn_Unknown		= 1,	/**< An unknown but recoverable error happened during the operation */
		MMC3630_Success			= 0,	/**< The operation completed successfully */
		MMC3630Fail_Unknown		= -1,	/**< An unknown and unrecoverable error happened during the operation */
		MMC3630Fail_BusError	= -2,	/**< Communication bus failure */
		MMC3630Fail_NotFound	= -3,	/**< Self identification request failed */
		MMC3630Fail_Timeout		= -4,	/**< Timed out waiting for results from device */
		MMC3630Fail_DeviceErr	= -5,	/**< The device had a problem flag set */
	} eMMC3630Return_t;
	
	typedef enum eMMC3630I2CAddr_t {
		MMC3630Addr_Base		= 0x30,
	} eMMC3630I2CAddr_t;
	
	/**	@brief		Enumeration of the registers in the peripheral
		@ingroup	mmc3630driver
	*/
	typedef enum eMMC3630Reg_t {
		MMC3630Reg_XoutLow		= 0x00,
		MMC3630Reg_XoutHigh		= 0x01,
		MMC3630Reg_YoutLow		= 0x02,
		MMC3630Reg_YoutHigh		= 0x03,
		MMC3630Reg_ZoutLow		= 0x04,
		MMC3630Reg_ZoutHigh		= 0x05,
		MMC3630Reg_Temperature	= 0x06,
		MMC3630Reg_Status		= 0x07,
		MMC3630Reg_Control0		= 0x08,
		MMC3630Reg_Control1		= 0x09,
		MMC3630Reg_Control2		= 0x0A,
		MMC3630Reg_XThreshold	= 0x0B,
		MMC3630Reg_YThreshold	= 0x0C,
		MMC3630Reg_ZThreshold	= 0x0D,
		MMC3630Reg_ProductID	= 0x2F,
		
		MMC3630Reg_ProdIDVal	= 0x0A,
	} eMMC3630Reg_t;
	
	typedef enum eMMC3630RegStat_t {
		MMC3630Stat_OTPReadDone	= 0x10,	/**< Chip successfully read its memory */
		MMC3630Stat_PumpOn		= 0x08,	/**< Indicates charge pump status, 1 while charging 0 when at target voltage */
		MMC3630Stat_MotionDet	= 0x04,	/**< Indicates motion detected, writing 1 clears interrupt */
		MMC3630Stat_MeasTDone	= 0x02,	/**< Temperature measurement completed, set to zero when measurement command is recevied */
		MMC3630Stat_MeasMDone	= 0x01,	/**< Magnetic measurement is completed, set to zero when measurement command is received */
	} eMMC3630RegStat_t;
	
	typedef enum eMMC3630Cont0_t {
		MMC3630Cont0_OTPRead	= 0x40,	/**< Write 1 to re-read OTP data */
		MMC3630Cont0_RefillCap	= 0x20,	/**< Write 1 to recharge the capacitor at VCAP pin */
		MMC3630Cont0_Reset		= 0x10,	/**< Write 1 to RESET the sensor */
		MMC3630Cont0_Set		= 0x08,	/**< Write 1 to SET the sensor */
		MMC3630Cont0_StartMDT	= 0x04,	/**< Write 1 to start the motion detector, on detect this is set to 0 */
		MMC3630Cont0_MeasTemp	= 0x02,	/**< Write 1 to begin a temperature measurement, resets to 0 when measurement is taken */
		MMC3630Cont0_MeasMagnet	= 0x01,	/**< Write 1 to begin a magnetic measurement, resets to 0 when measurement is taken */
		
		MMC3630Cont0_Default	= 0x00,	/**< Default value for this register */
	} eMMC3630Cont0_t;
	
	typedef enum eMMC3630Cont1_t {
		MMC3630Cont1_BitWidthMsk= 0x03,
		MMC3630Cont1_BW1mSec	= 0x00,	/**< 16 bit resolution, 1.6 mSec measure time */
		MMC3630Cont1_BW2mSec	= 0x01,	/**< 16 bit resolution, 2.5 mSec measure time */
		MMC3630Cont1_BW5mSec	= 0x02,	/**< 16 bit resolution, 5 mSec measure time */
		MMC3630Cont1_BW10mSec	= 0x03,	/**< 16 bit resolution, 10 mSec measure time */
		
		MMC3630Cont1_SWReset	= 0x80,	/**< Write 1 to reset the part */
		MMC3630Cont1_ZDisable	= 0x10,	/**< Write 1 to disable Z channel during measurement */
		MMC3630Cont1_YDisable	= 0x08,	/**< Write 1 to disable Y channel during measurement */
		MMC3630Cont1_XDisable	= 0x04,	/**< Write 1 to disable X channel during measurement */
		
		MMC3630Cont1_Default	= 0x00,	/**< Default value for this register */
	} eMMC3630Cont1_t;
	
	typedef enum eMMC3630Cont2_t {
		MMC3630Cont2_CMFreqMask	= 0x0F,	/**< Bits used by continuous measurement frequency */
		MMC3630Cont2_CMOff		= 0x00,
		MMC3630Cont2_CM14Hz		= 0x01,
		MMC3630Cont2_CM5Hz		= 0x02,
		MMC3630Cont2_CM2_2Hz	= 0x03,
		MMC3630Cont2_CM1Hz		= 0x04,
		MMC3630Cont2_CM0_5Hz	= 0x05,
		MMC3630Cont2_CM0_25Hz	= 0x06,
		MMC3630Cont2_CM0_125Hz	= 0x07,
		MMC3630Cont2_CM0_0625Hz	= 0x08,
		MMC3630Cont2_CM0_0313Hz	= 0x09,
		MMC3630Cont2_CM0_0156Hz	= 0x0A,
		MMC3630Cont2_IntMotioDet= 0x10,	/**< 1 Enable/0 Disable motion detection interrupt */
		MMC3630Cont2_IntMeasDone= 0x20,	/**< 1 Enable/0 Disable measurement done interrupt */
		
		MMC3630Cont2_Default	= 0x00,	/**< Default value for this register */
	} eMMC3630Cont2_t;
	
	typedef struct sMMC3630Dev_t {
		uint8_t nI2CAddr;
		uint16_t nXOffset;
		uint16_t nYOffset;
		uint16_t nZOffset;
		sI2CIface_t *pI2C;
	} sMMC3630Dev_t;

/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	/**	@brief		Prepares the program object as well as the peripheral configuration
		@ingroup	mmc3630driver
	*/
	eMMC3630Return_t MMC3630Init(sMMC3630Dev_t *pDev, sI2CIface_t *pI2C, eMMC3630I2CAddr_t eI2CAddr);
	
	eMMC3630Return_t MMC3630ReadSingleMeasurement(sMMC3630Dev_t *pDev, int16_t *pnXVal, int16_t *pnYVal, int16_t *pnZVal, float *pnTempC);

/*****	Functions	*****/


#endif

