/**	@defgroup lsm9ds1
	@brief
	@details
	# Part Info #
	Gyroscope gives Instantaneous Velocity of the chip.  This must be integrated int 
	order to determine change in position.  This velocity is an angular velocity, so if 
	the chip is not rotating it will not be registered.
	
	# Intent #
	
	# File Information #
		File:	LSM9DS1Driver.h
		Author:	J. Beighel
		Date:	2021-09-05
*/

/**	@defgroup lsm9ds1_priv : Private Objects
	@brief
	@ingroup lsm9ds1
*/

#ifndef __LSM9DS1_DRIVER
	#define __LSM9DS1_DRIVER

/*****	Includes	*****/
	#include <string.h>

	#include "I2CGeneralInterface.h"

/*****	Constants	*****/
	#define LSM9DS1_AG_ADDRESS			0x6B //Data sheet showed 6A unless SAD[0] = SA0
	#define LSM9DS1_M_ADDRESS			0x1E //Data sheet showed 1C unless SAD[1] = SDO/SA1
	
	#define LSM9DS1_SELFID				0x68

/*****	Definitions	*****/
	/**	@brief		Registers for the LSM9DS1 gyrometer and accelerometer
		@ingroup	lsm9ds1
	*/
	typedef enum eLSM9DS1Reg_AG_t {
		LSM9DS1_ACT_THS				= 0x04,
		LSM9DS1_ACT_DUR				= 0x05,
		LSM9DS1_INT_GEN_CFG_XL		= 0x06,
		LSM9DS1_INT_GEN_THS_X_XL	= 0x07,
		LSM9DS1_INT_GEN_THS_Y_XL	= 0x08,
		LSM9DS1_INT_GEN_THS_Z_XL	= 0x09,
		LSM9DS1_INT_GEN_DUR_XL		= 0x0A,
		LSM9DS1_REFERENCE_G			= 0x0B,
		LSM9DS1_INT1_CTRL			= 0x0C,
		LSM9DS1_INT2_CTRL			= 0x0D,
		LSM9DS1_WHO_AM_I			= 0x0F,
		LSM9DS1_CTRL_REG1_G			= 0x10,
		LSM9DS1_CTRL_REG2_G			= 0x11,
		LSM9DS1_CTRL_REG3_G			= 0x12,
		LSM9DS1_ORIENT_CFG_G		= 0x13,
		LSM9DS1_INT_GEN_SRC_G		= 0x14,
		LSM9DS1_OUT_TEMP_L			= 0x15,
		LSM9DS1_OUT_TEMP_H			= 0x16,
		LSM9DS1_STATUS_REG			= 0x17,
		LSM9DS1_OUT_X_L_G			= 0x18,	/**< Angular rate pitch axis (X) output register, low byte of signed 16 bit */
		LSM9DS1_OUT_X_H_G			= 0x19,	/**< Angular rate pitch axis (X) output register, high byte of signed 16 bit */
		LSM9DS1_OUT_Y_L_G			= 0x1A,	/**< Angular rate pitch axis (Y) output register, low byte of signed 16 bit */
		LSM9DS1_OUT_Y_H_G			= 0x1B,	/**< Angular rate pitch axis (Y) output register, high byte of signed 16 bit */
		LSM9DS1_OUT_Z_L_G			= 0x1C,	/**< Angular rate pitch axis (Z) output register, low byte of signed 16 bit */
		LSM9DS1_OUT_Z_H_G			= 0x1D,	/**< Angular rate pitch axis (Z) output register, high byte of signed 16 bit */
		LSM9DS1_CTRL_REG4			= 0x1E,
		LSM9DS1_CTRL_REG5_XL		= 0x1F,
		LSM9DS1_CTRL_REG6_XL		= 0x20,
		LSM9DS1_CTRL_REG7_XL		= 0x21,
		LSM9DS1_CTRL_REG8			= 0x22,
		LSM9DS1_CTRL_REG9			= 0x23,
		LSM9DS1_CTRL_REG10			= 0x24,
		LSM9DS1_INT_GEN_SRC_XL		= 0x26,
		LSM9DS1_STATUS_REG2			= 0x27,	/**< Status register for the Gyroscope and Accelerometer */
		LSM9DS1_OUT_X_L_XL			= 0x28,
		LSM9DS1_OUT_X_H_XL			= 0x29,
		LSM9DS1_OUT_Y_L_XL			= 0x2A,
		LSM9DS1_OUT_Y_H_XL			= 0x2B,
		LSM9DS1_OUT_Z_L_XL			= 0x2C,
		LSM9DS1_OUT_Z_H_XL			= 0x2D,
		LSM9DS1_FIFO_CTRL			= 0x2E,
		LSM9DS1_FIFO_SRC			= 0x2F,
		LSM9DS1_INT_GEN_CFG_G		= 0x30,
		LSM9DS1_INT_GEN_THS_XH_G	= 0x31,
		LSM9DS1_INT_GEN_THS_XL_G	= 0x32,
		LSM9DS1_INT_GEN_THS_YH_G	= 0x33,
		LSM9DS1_INT_GEN_THS_YL_G	= 0x34,
		LSM9DS1_INT_GEN_THS_ZH_G	= 0x35,
		LSM9DS1_INT_GEN_THS_ZL_G	= 0x36,
		LSM9DS1_INT_GEN_DUR_G		= 0x37,
	} eLSM9DS1Reg_AG_t;
	
	/**	@brief		Registers for the LSM9DS1 magnetometer
		@ingroup	lsm9ds1
	*/
	typedef enum eLSM9DS1Reg_M_t {
		LSM9DS1_OFFSET_X_REG_L_M	= 0x05,
		LSM9DS1_OFFSET_X_REG_H_M	= 0x06,
		LSM9DS1_OFFSET_Y_REG_L_M	= 0x07,
		LSM9DS1_OFFSET_Y_REG_H_M	= 0x08,
		LSM9DS1_OFFSET_Z_REG_L_M	= 0x09,
		LSM9DS1_OFFSET_Z_REG_H_M	= 0x0A,
		LSM9DS1_WHO_AM_I_M			= 0x0F,
		LSM9DS1_CTRL_REG1_M			= 0x20,
		LSM9DS1_CTRL_REG2_M			= 0x21,
		LSM9DS1_CTRL_REG3_M			= 0x22,
		LSM9DS1_CTRL_REG4_M			= 0x23,
		LSM9DS1_CTRL_REG5_M			= 0x24,
		LSM9DS1_STATUS_REG_M		= 0x27,
		LSM9DS1_OUT_X_L_M			= 0x28,
		LSM9DS1_OUT_X_H_M			= 0x29,
		LSM9DS1_OUT_Y_L_M			= 0x2A,
		LSM9DS1_OUT_Y_H_M			= 0x2B,
		LSM9DS1_OUT_Z_L_M			= 0x2C,
		LSM9DS1_OUT_Z_H_M			= 0x2D,
		LSM9DS1_INT_CFG_M			= 0x30,
		LSM9DS1_INT_SRC_M			= 0x31,
		LSM9DS1_INT_THS_L_M			= 0x32,
		LSM9DS1_INT_THS_H_M			= 0x33,
	} eLSM9DS1Reg_M_t;
	
	typedef enum eLSM9DS1RegCtrl1G_t {
		LSM9DS1_CTRL1G_ODR_G_MASK	= 0xE0,	/**< Gyroscope output data rate selection, bits mask */
		LSM9DS1_CTRL1G_ODR_G2		= 0x80,	/**< Gyroscope output data rate selection, high bit */
		LSM9DS1_CTRL1G_ODR_G1		= 0x40,	/**< Gyroscope output data rate selection, middle bit */
		LSM9DS1_CTRL1G_ODR_G0		= 0x20,	/**< Gyroscope output data rate selection, low bit */
		LSM9DS1_CTRL1G_ODR_G_POWDWN	= 0x00,	/**< Gyroscope ODR: Power down */
		
		LSM9DS1_CTRL1G_FS_G_MASK	= 0x18,	/**< Gyroscope full-scale selection bit mask.  Values 00: 245 dps, 01: 500 dps, 10: N/A, 11: 2000 dps */
		LSM9DS1_CTRL1G_FS_G1		= 0x10,	/**< Gyroscope full-scale selection, high bit */
		LSM9DS1_CTRL1G_FS_G0		= 0x08,	/**< Gyroscope full-scale selection, low bit */
		
		LSM9DS1_CTRL1G_BW_G_MASK	= 0x03,	/**< Gyroscope bandwidth selection, bits mask */
		LSM9DS1_CTRL1G_BW_G1		= 0x02,	/**< Gyroscope bandwidth selection, high bit */
		LSM9DS1_CTRL1G_BW_G0		= 0x01,	/**< Gyroscope bandwidth selection, low bit */
		
		LSM9DS1_CTRL1G_G_14		= (LSM9DS1_CTRL1G_ODR_G0),	/**< Gyroscope ODR: 14.9 Hz for Low Pass Filter (LPF) 1 used for output cutoff freq is 5 Hz */
		LSM9DS1_CTRL1G_G_59		= (LSM9DS1_CTRL1G_ODR_G1),	/**< Gyroscope ODR: 59.5 Hz for Low Pass Filter (LPF) 1 used for output cutoff freq is 19 Hz */
		LSM9DS1_CTRL1G_G_119	= (LSM9DS1_CTRL1G_ODR_G1 | LSM9DS1_CTRL1G_ODR_G0),	/**< Gyroscope ODR: 119 Hz for Low Pass Filter (LPF) 1 used for output cutoff freq is 38 Hz */
		LSM9DS1_CTRL1G_G_238	= (LSM9DS1_CTRL1G_ODR_G2),	/**< Gyroscope ODR: 238 Hz for Low Pass Filter (LPF) 1 used for output cutoff freq is 76 Hz */
		LSM9DS1_CTRL1G_G_476	= (LSM9DS1_CTRL1G_ODR_G2 | LSM9DS1_CTRL1G_ODR_G0),	/**< Gyroscope ODR: 476 Hz (If Low Pass Filter (LPF) 1 used for output cutoff freq is 100 Hz */
		LSM9DS1_CTRL1G_G_952	= (LSM9DS1_CTRL1G_ODR_G2 | LSM9DS1_CTRL1G_ODR_G1),	/**< Gyroscope ODR: 952 Hz for Low Pass Filter (LPF) 1 used for output cutoff freq is 100 Hz */
		
		LSM9DS1_CTRL1G_G_14_NA	= (LSM9DS1_CTRL1G_ODR_G0),	/**< Gyroscope ODC & BW settings for data rate 14.9 Hz and no LPF2 Ctoff */
		LSM9DS1_CTRL1G_G_59_16	= (LSM9DS1_CTRL1G_ODR_G1),	/**< Gyroscope ODC & BW settings for data rate 59.5 Hz and LPF2 Ctoff 16 Hz */
		LSM9DS1_CTRL1G_G_119_14	= (LSM9DS1_CTRL1G_ODR_G1 | LSM9DS1_CTRL1G_ODR_G0),	/**< Gyroscope ODC & BW settings for data rate 119 Hz and LPF2 Ctoff 14 Hz */
	} eLSM9DS1RegCtrl1G_t;
	
	typedef enum eLSM9DS1RegStatus_t {
		LSM9DS1_STATUS_IG_XL		= 0x40,	/**< Accelerometer interrupt signal 0: no interrupt, 1: one or more interrupt events */
		LSM9DS1_STATUS_IG_G			= 0x20,	/**< Gyroscope interrupt signal 0: no interrupt, 1: one or more interrupt events */
		LSM9DS1_STATUS_INACT		= 0x10,	/**< Inactivity interrupt signal 0: no interrupt, 1: one or more interrupt events */
		LSM9DS1_STATUS_BOOT_STATUS	= 0x08,	/**< Boot running signal 0: no boot running, 1: boot running */
		LSM9DS1_STATUS_TDA			= 0x04,	/**< Temperature sensor data available 0: no data, 1: data waiting */
		LSM9DS1_STATUS_GDA			= 0x02,	/**< Gyroscope sensor data available 0: no data, 1: data waiting */
		LSM9DS1_STATUS_XLDA			= 0x01,	/**< Accelerometer sensor data available 0: no data, 1: data waiting */
	} eLSM9DS1RegStatus_t;

	typedef struct sLSM9DS1_t {
		sI2CIface_t *pI2CBus;
		uint16_t nAGDevAddr;
		uint16_t nMDevAddr;
	} sLSM9DS1_t;

/*****	Globals		*****/


/*****	Prototypes	*****/
	eReturn_t LSM9DS1Initialize(sI2CIface_t *pI2CIface, sLSM9DS1_t *pDev);
	
	eReturn_t LSM9DS1DetectDevice(sLSM9DS1_t *pDev);
	
	eReturn_t LSM9DS1Configure(sLSM9DS1_t *pDev);
	
	eReturn_t LSM9DS1ReadGyroscope(sLSM9DS1_t *pDev, int16_t *pnXVal, int16_t *pnYVal, int16_t *pnZVal);
	
	eReturn_t LSM9DS1ReadAccelerometer(sLSM9DS1_t *pDev, int16_t *pnXVal, int16_t *pnYVal, int16_t *pnZVal);

#endif