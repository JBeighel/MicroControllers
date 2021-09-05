/**	@defgroup	mpu6050
	@brief		
	@details	v
	#Description
		Gyrometer and Accelerometer
		max 400 khz clock
	
	#File Information
		File:	MPU6050Driver.h
		Author:	J. Beighel
		Date:	2021-09-05
*/

#ifndef __MPU6050DRIVER_H
	#define __MPU6050DRIVER_H

/*****	Includes	*****/
	#include "CommonUtils.h"
	#include "I2CGeneralInterface.h"

/*****	Defines		*****/
	/**	@brief		Maximum I2C clock the device supports
		@ingroup	mpu6050
	*/
	#define MPU6050_MAXCLOCK	400000
	
	/**	@brief		I2C bus capabilities this driver requires
		@ingroup	mpu6050
	*/
	#define MPU6050_I2CCAPS		(I2CCap_ReadUint8Reg | I2CCap_WriteUint8Reg | I2CCap_ReadData)

/*****	Definitions	*****/
	typedef eReturn_t eMPU6050Return_t;
	
	/** @breif		I2C Address values for the MPU-6050
		@ingroup	mpu6050
	*/
	typedef enum eMPU6050Addr_t {
		MPU6050Addr_Base		= 0x68,
		MPU6050Addr_AD0			= 0x01,
	} eMPU6050Addr_t;
	
	/**	@brief		Enumeration of all MPU-6050 register addresses
		@ingroup	mpu6050
	*/
	typedef enum eMPU6050Reg_t {
		MPU6050Reg_SelfTestX	= 0x0D,
		MPU6050Reg_SelfTestY	= 0x0E,
		MPU6050Reg_SelfTestZ	= 0x0F,
		MPU6050Reg_SelfTestA	= 0x10,
		MPU6050Reg_SmplRateDiv	= 0x19,	/**< Sample rate divider for Gyroscope output */
		MPU6050Reg_Config		= 0x1A,	/**< Frame sync and low pass filter settings */
		MPU6050Reg_GyroConfig	= 0x1B,	/**< Gyroscope configuration */
		MPU6050Reg_AccelConfig	= 0x1C,	/**< Accelerometer configuration */
		MPU6050Reg_FIFOEnable	= 0x23,	/**< Specifies which measurements go to FIFO */
		MPU6050Reg_I2CMstrCtrl	= 0x24,	/**< I2C Master controls */
		MPU6050Reg_I2CSlv0Addr	= 0x25,
		MPU6050Reg_I2CSlv0Reg	= 0x26,
		MPU6050Reg_I2CSlv0Ctrl	= 0x27,
		MPU6050Reg_I2CSlv1Addr	= 0x28,
		MPU6050Reg_I2CSlv1Reg	= 0x29,
		MPU6050Reg_I2CSlv1Ctrl	= 0x2A,
		MPU6050Reg_I2CSlv2Addr	= 0x2B,
		MPU6050Reg_I2CSlv2Reg	= 0x2C,
		MPU6050Reg_I2CSlv2Ctrl	= 0x2D,
		MPU6050Reg_I2CSlv3Addr	= 0x2E,
		MPU6050Reg_I2CSlv3Reg	= 0x2F,
		MPU6050Reg_I2CSlv3Ctrl	= 0x30,
		MPU6050Reg_I2CSlv4Addr	= 0x31,
		MPU6050Reg_I2CSlv4Reg	= 0x32,
		MPU6050Reg_I2CSlv4DO	= 0x33,
		MPU6050Reg_I2CSlv4Ctrl	= 0x34,
		MPU6050Reg_I2CSlv4DI	= 0x35,
		MPU6050Reg_I2CMstrStat	= 0x36,
		MPU6050Reg_IntPinCfg	= 0x37,	/**< Controls BEHAVIOR of interrupt signals */
		MPU6050Reg_IntEnable	= 0x38,	/**< Controls interrupt generation sources */
		MPU6050Reg_IntStatus	= 0x3A,	/**< Shows status of each interrupt source */
		MPU6050Reg_AccelXOutH	= 0x3B,	/**< Most recent accelerometer X axis reading high byte */
		MPU6050Reg_AccelXOutL	= 0x3C,	/**< Most recent accelerometer X axis reading low byte */
		MPU6050Reg_AccelYOutH	= 0x3D,	/**< Most recent accelerometer Y axis reading high byte */
		MPU6050Reg_AccelYOutL	= 0x3E,	/**< Most recent accelerometer Y axis reading low byte */
		MPU6050Reg_AccelZOutH	= 0x3F,	/**< Most recent accelerometer Z axis reading high byte */
		MPU6050Reg_AccelZOutL	= 0x40,	/**< Most recent accelerometer Z axis reading low byte */
		MPU6050Reg_TempOutH		= 0x41,	/**< Most recent temperature reading high byte */
		MPU6050Reg_TempOutL		= 0x42,	/**< Most recent temperature reading low byte */
		MPU6050Reg_GyroXOutH	= 0x43,	/**< Most recent gyrometer X axis reading high byte */
		MPU6050Reg_GyroXOutL	= 0x44,	/**< Most recent gyrometer X axis reading low byte */
		MPU6050Reg_GyroYOutH	= 0x45,	/**< Most recent gyrometer Y axis reading high byte */
		MPU6050Reg_GyroYOutL	= 0x46,	/**< Most recent gyrometer Y axis reading low byte */
		MPU6050Reg_GyroZOutH	= 0x47,	/**< Most recent gyrometer Z axis reading high byte */
		MPU6050Reg_GyroZOutL	= 0x48,	/**< Most recent gyrometer Z axis reading low byte */
		MPU6050Reg_ExtSens00	= 0x49,
		MPU6050Reg_ExtSens01	= 0x4A,
		MPU6050Reg_ExtSens02	= 0x4B,
		MPU6050Reg_ExtSens03	= 0x4C,
		MPU6050Reg_ExtSens04	= 0x4D,
		MPU6050Reg_ExtSens05	= 0x4E,
		MPU6050Reg_ExtSens06	= 0x4F,
		MPU6050Reg_ExtSens07	= 0x50,
		MPU6050Reg_ExtSens08	= 0x51,
		MPU6050Reg_ExtSens09	= 0x52,
		MPU6050Reg_ExtSens10	= 0x53,
		MPU6050Reg_ExtSens11	= 0x54,
		MPU6050Reg_ExtSens12	= 0x55,
		MPU6050Reg_ExtSens13	= 0x56,
		MPU6050Reg_ExtSens14	= 0x57,
		MPU6050Reg_ExtSens15	= 0x58,
		MPU6050Reg_ExtSens16	= 0x59,
		MPU6050Reg_ExtSens17	= 0x5A,
		MPU6050Reg_ExtSens18	= 0x5B,
		MPU6050Reg_ExtSens19	= 0x5C,
		MPU6050Reg_ExtSens20	= 0x5D,
		MPU6050Reg_ExtSens21	= 0x5E,
		MPU6050Reg_ExtSens22	= 0x5F,
		MPU6050Reg_ExtSens23	= 0x60,
		MPU6050Reg_I2CSlv0DO	= 0x63,
		MPU6050Reg_I2CSlv1DO	= 0x64,
		MPU6050Reg_I2CSlv2DO	= 0x65,
		MPU6050Reg_I2CSlv3DO	= 0x66,
		MPU6050Reg_I2CMstDelay	= 0x67,
		MPU6050Reg_SigPathRest	= 0x68,	/**< Reset analog signal path of gyro and accel sensors */
		MPU6050Reg_UserControl	= 0x6A,	/**< Enable/Reset for FIFO, I2C master, and I2C slave */
		MPU6050Reg_PowerMgmt1	= 0x6B,	/**< Configure power mode adn clock source */
		MPU6050Reg_PowerMgmt2	= 0x6C,	/**< Configre wake-ups in accelerometer only low power */
		MPU6050Reg_FIFOCntH		= 0x72,
		MPU6050Reg_FIFOCntL		= 0x73,
		MPU6050Reg_FIFIRdWr		= 0x74,
		MPU6050Reg_WhoAmI		= 0x75,	/**< Responds with devices base I2C address */
	} eMPU6050Reg_t;
	
	typedef enum eMPU6050RegCfg_t {
		MPU6050RegCfg_SyncMask		= 0x38,
		MPU6050RegCfg_SyncDis		= 0x00,
		MPU6050RegCfg_SyncTemp		= 0x08,
		MPU6050RegCfg_SyncGyroX		= 0x10,
		MPU6050RegCfg_SyncGyroY		= 0x18,
		MPU6050RegCfg_SyncGyroZ		= 0x20,
		MPU6050RegCfg_SyncAccelX	= 0x28,
		MPU6050RegCfg_SyncAccelY	= 0x30,
		MPU6050RegCfg_SyncAccelZ	= 0x38,
		
		MPU6050RegCfg_LPFMask		= 0x07,
		MPU6050RegCfg_LPF260Hz		= 0x00,
		MPU6050RegCfg_LPF180Hz		= 0x01,
		MPU6050RegCfg_LPF90Hz		= 0x02,
		MPU6050RegCfg_LPF40Hz		= 0x03,
		MPU6050RegCfg_LPF20Hz		= 0x04,
		MPU6050RegCfg_LPF10Hz		= 0x05,
		MPU6050RegCfg_LPF5Hz		= 0x06,
	} eMPU6050RegCfg_t;
	
	typedef enum eMPU6050RegGCfg_t {
		MPU6050RegGCfg_XSelfTest	= 0x80,
		MPU6050RegGCfg_YSelfTest	= 0x40,
		MPU6050RegGCfg_ZSelfTest	= 0x20,
		MPU6050RegGCfg_FullScaleMsk	= 0x18,
		MPU6050RegGCfg_FS250		= 0x00,
		MPU6050RegGCfg_FS500		= 0x08,
		MPU6050RegGCfg_FS1000		= 0x10,
		MPU6050RegGCfg_FS2000		= 0x18,
	} eMPU6050RegGCfg_t;
		
	typedef enum eMPU6050RegACfg_t {
		MPU6050RegACfg_XSelfTest	= 0x80,
		MPU6050RegACfg_YSelfTest	= 0x40,
		MPU6050RegACfg_ZSelfTest	= 0x20,
		MPU6050RegACfg_FullScaleMsk	= 0x18,
		MPU6050RegACfg_FS2g			= 0x00,
		MPU6050RegACfg_FS4g			= 0x08,
		MPU6050RegACfg_FS8g			= 0x10,
		MPU6050RegACfg_FS16g		= 0x18,
	} eMPU6050RegACfg_t;
	
	typedef enum eMPU6050RegFIFOEn_t {
		MPU6050RegFIFOen_None		= 0x00,
		MPU6050RegFIFOEn_Temp		= 0x80,
		MPU6050RegFIFOen_GyroX		= 0x40,
		MPU6050RegFIFOen_GyroY		= 0x20,
		MPU6050RegFIFOen_GyroZ		= 0x10,
		MPU6050RegFIFOen_AccelX		= 0x08,
		MPU6050RegFIFOen_Slave2		= 0x04,
		MPU6050RegFIFOen_Slave1		= 0x02,
		MPU6050RegFIFOen_Slave0		= 0x01,
	} eMPU6050RegFIFOEn_t;
	
	typedef enum eMPU6050RegI2CMst_t {
		MPU6050RegI2CMst_MultiMstr	= 0x80,
		MPU6050RegI2CMst_WaitForExt	= 0x40,
		MPU6050RegI2CMst_Slave3FIFO	= 0x20,
		MPU6050RegI2CMst_PNSR		= 0x10,
		MPU6050RegI2CMst_MstClcMask	= 0x0F,
		MPU6050RegI2CMst_Clk348kHz	= 0x00,
		MPU6050RegI2CMst_Clk333kHz	= 0x01,
		MPU6050RegI2CMst_Clk320kHz	= 0x02,
		MPU6050RegI2CMst_Clk308kHz	= 0x03,
		MPU6050RegI2CMst_Clk296kHz	= 0x04,
		MPU6050RegI2CMst_Clk286kHz	= 0x05,
		MPU6050RegI2CMst_Clk276kHz	= 0x06,
		MPU6050RegI2CMst_Clk267kHz	= 0x07,
		MPU6050RegI2CMst_Clk258kHz	= 0x08,
		MPU6050RegI2CMst_Clk500kHz	= 0x09,
		MPU6050RegI2CMst_Clk471kHz	= 0x0A,
		MPU6050RegI2CMst_Clk444kHz	= 0x0B,
		MPU6050RegI2CMst_Clk421kHz	= 0x0C,
		MPU6050RegI2CMst_Clk400kHz	= 0x0D,
		MPU6050RegI2CMst_Clk381kHz	= 0x0E,
		MPU6050RegI2CMst_Clk364kHz	= 0x0F,
	} eMPU6050RegI2CMst_t;
	
	typedef enum eMPU6050RegIntCfg_t {
		MPU6050RegIntCfg_Level		= 0x80,
		MPU6050RegIntCfg_Open		= 0x40,
		MPU6050RegIntCfg_LatchEn	= 0x20,
		MPU6050RegIntCfg_ReadClear	= 0x10,
		MPU6050RegIntCfg_FsyncLevel	= 0x08,
		MPU6050RegIntCfg_FsyncEn	= 0x04,
		MPU6050RegIntCfg_BypassEn	= 0x02,
		MPU6050RegIntCfg_None		= 0x00,
	} eMPU6050RegIntCfg_t;
	
	typedef enum eMPU6050RegIntEn_t {
		MPU6050RegIntEn_None		= 0x00,
		MPU6050RegIntEn_FIFOOFlow	= 0x10,
		MPU6050RegIntEn_I2CMaster	= 0x40,
		MPU6050RegIntEn_DataReady	= 0x01,
	} eMPU6050RegIntEn_t;
	
	typedef enum eMPU6050RegSigRst_t {
		MPU6050RegSigRst_All		= 0x07,
		MPU6050RegSigRst_Gyro		= 0x04,
		MPU6050RegSigRst_Accel		= 0x02,
		MPU6050RegSigRst_Temp		= 0x01,
	} eMPU6050RegSigRst_t;
	
	typedef enum eMPU6050RegUsrCtrl_t {
		MPU6050RegUsrCtrl_6050Def	= 0x00,
		MPU6050RegUsrCtrl_FIFOEn	= 0x40,
		MPU6050RegUsrCtrl_I2CMstEn	= 0x20,
		MPU6050RegUsrCtrl_SPIEn		= 0x10,	/**< Enables the SPI interface and disables I2C */
		MPU6050RegUsrCtrl_FIFORst	= 0x40,
		MPU6050RegUsrCtrl_I2CMstRst	= 0x20,
		MPU6050RegUsrCtrl_SigCndRst	= 0x01,
	} eMPU6050RegUsrCtrl_t;
	
	typedef enum eMPU6050RegPwr1_t {
		MPU6050RegPwr1_Reset		= 0x80,	/**< Set to reset all internal registers to default */
		MPU6050RegPwr1_Sleep		= 0x40,
		MPU6050RegPwr1_Cycle		= 0x20,
		MPU6050RegPwr1_TempDis		= 0x08,	/**< Set to disable temperature sensor */
		MPU6050RegPwr1_ClkSelMask	= 0x07,
		MPU6050RegPwr1_ClkIntOsc	= 0x00,
		MPU6050RegPwr1_ClkGyroX		= 0x01,
		MPU6050RegPwr1_ClkGyroY		= 0x02,
		MPU6050RegPwr1_ClkGyroZ		= 0x03,
		MPU6050RegPwr1_ClkPLL32kHz	= 0x04,
		MPU6050RegPwr1_ClkPLL19MKz	= 0x05,
		MPU6050RegPwr1_ClkStop		= 0x07,
	} eMPU6050RegPwr1_t;
	
	typedef enum eMPU6050RegPwr2_t {
		MPU6050RegPwr2_LPWake1Hz	= 0x00,
		MPU6050RegPwr2_LPWake5Hz	= 0x40,
		MPU6050RegPwr2_LPWake20Hz	= 0x80,
		MPU6050RegPwr2_LPWake40Hz	= 0xC0,
		
		MPU6050RegPwr2_StdByAccelX	= 0x20,
		MPU6050RegPwr2_StdByAccelY	= 0x10,
		MPU6050RegPwr2_StdByAccelZ	= 0x08,
		
		MPU6050RegPwr2_StdByGyroX	= 0x04,
		MPU6050RegPwr2_StdByGyroY	= 0x02,
		MPU6050RegPwr2_StdByGyroZ	= 0x01,
	} eMPU6050RegPwr2_t;
	
	typedef struct sMPU6050Obj_t {
		sI2CIface_t *pI2C;
		
		eMPU6050Addr_t eAddr;
	} sMPU6050Obj_t;
	
/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/


/*****	Functions	*****/
	eMPU6050Return_t MPU6050Initialize(sMPU6050Obj_t *pObj, sI2CIface_t *pI2C, eMPU6050Addr_t eI2CAddr);

	eMPU6050Return_t MPU6050ReadGyro(sMPU6050Obj_t *pObj, int16_t *pnXMeas, int16_t *pnYMeas, int16_t *pnZMeas);
	
	eMPU6050Return_t MPU6050ReadAccel(sMPU6050Obj_t *pObj, int16_t *pnXMeas, int16_t *pnYMeas, int16_t *pnZMeas);

#endif