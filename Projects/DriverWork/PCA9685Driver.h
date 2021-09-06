/**	@defgroup	pca9685driver
	@brief		16 channel PWM outputs
	@details	v0.1
	#Description
		The chip has an output enable pin, which will shut off the outpus when it is set high.
		If it is low or an open connection, the outputs will be enabled.
		
	#File Information
		File:	PCA9685Driver.h
		Author:	J. Beighel
		Date:	12-17-2020
*/

#ifndef __PCA9685DRIVER_H
	#define __PCA9685DRIVER_H

/*****	Includes	*****/
	#include "CommonUtils.h"
	#include "GPIOGeneralInterface.h"
	#include "I2CGeneralInterface.h"

/*****	Defines		*****/
	/**	@brief		The PCA9685 has an internal clock with a frequency of 25MHz
		@ingroup	pca9685driver
	*/
	#define PCA9685_INTCLKFREQ	25000000
	
	/**	@brief		Used to specify that an optional pin of the device is not connected
		@ingroup	pca9685driver
	*/
	#define PCA9685_NOPIN		0xFF
	
	/**	@brief		Specifies the number of PWM outputs the PCA9685 controls
		@ingroup	pca9685driver
	*/
	#define PCA9685_NUMPWM		16
	
	/**	@brief		The number of counts available in the pulse width settings
		@ingroup	pca9685driver
	*/
	#define PCA9685_PULSECOUNTS	4096
	
	#define PCA9685_GPIOCAPS	(GPIOCap_DigitalWrite)
	
	#define PCA9685_I2CCAPS		(I2CCap_ReadUint8Reg | I2CCap_WriteUint8Reg | I2CCap_GeneralCall)

/*****	Definitions	*****/
	typedef enum ePCA9685Return_t {
		PCA9685Warn_Unknown		= 1,	/**< An unknown but recoverable error happened during the operation */
		PCA9685_Success			= 0,	/**< The operation completed successfully */
		PCA9685Fail_Unknown		= -1,	/**< An unknown and unrecoverable error happened during the operation */
		PCA9685Fail_BusError	= -2,	/**< An error occurred in the communications bus */
	} ePCA9685Return_t;

	/**	@brief		Enum of I2C address components for the PCA9685 PWM Driver
		@details	The device has a fixed address bit of 0x80, then 6 pins that can 
			set additional bits in this address.  By bitwise ORing these values 
			together the full address of the device can be created.
			
			In addition there are a number of optional I2C addresses the device can
			respond to.  These are also identified within this enum.
			
			The data sheet has them 1 bit higher.  The Arduino I2C functions will 
			apply a bit shift to make space for the R/W bit.
		@ingroup	pca9685driver
	*/
	typedef enum ePCA9685Address_t {
		PCA9685_None	= 0x00,		/**< @brief None of the external bits are set */
		PCA9685_Fixed	= 0x40,		/**< @brief Address bit that is always used */
		PCA9685_A5		= 0x20,		/**< @brief Address bit added throug pin A5 */
		PCA9685_A4		= 0x10,		/**< @brief Address bit added throug pin A4 */
		PCA9685_A3		= 0x08,		/**< @brief Address bit added throug pin A3 */
		PCA9685_A2		= 0x04,		/**< @brief Address bit added throug pin A2 */
		PCA9685_A1		= 0x02,		/**< @brief Address bit added throug pin A1 */
		PCA9685_A0		= 0x01,		/**< @brief Address bit added throug pin A0 */
		
		PCA9685_AllCall	= 0xE0,		/**< @brief Default value for the All Call address */
		PCA9685_SubAdr1	= 0xE2,		/**< @brief Default value for the sub-address 1 */
		PCA9685_SubAdr2	= 0xE4,		/**< @brief Default value for the sub-address 2 */
		PCA9685_SubAdr3	= 0xE8,		/**< @brief Default value for the sub-address 3 */
		
		PCA9685_Reset	= 0x06,		/**< @brief Reserved address that must be used for software resets */
	} ePCA9685Address_t;
	
	/**	@brief		Enumeration of all registers accessible via I2C bus for the PCA9685 PWM controller	
		@ingroup	pca9685driver
	*/
	typedef enum ePCA9685Regs_t {
		PCA9685REG_Mode1		= 0x00,		/**< @brief Mode register 1 */
		PCA9685REG_Mode2		= 0x01,		/**< @brief Mode register 2 */
		PCA9685REG_SubAdr1		= 0x02,		/**< @brief I2C bus sub-address 1 */
		PCA9685REG_SubAdr2		= 0x03,		/**< @brief I2C bus sub-address 2 */
		PCA9685REG_SubAdr3		= 0x04,		/**< @brief I2C bus sub-address 3 */
		PCA9685REG_AllCall		= 0x05,		/**< @brief I2C bus all call address */
		
		PCA9685REG_PWM00_OnL	= 0x06,		/**< @brief PWM 00 low byte for turn on count */
		PCA9685REG_PWM00_OnH	= 0x07,		/**< @brief PWM 00 high byte for turn on count */
		PCA9685REG_PWM00_OffL	= 0x08,		/**< @brief PWM 00 low byte for turn off count */
		PCA9685REG_PWM00_OffH	= 0x09,		/**< @brief PWM 00 high byte for turn off count */
		PCA9685REG_PWM01_OnL	= 0x0A,		/**< @brief PWM 01 low byte for turn on count */
		PCA9685REG_PWM01_OnH	= 0x0B,		/**< @brief PWM 01 high byte for turn on count */
		PCA9685REG_PWM01_OffL	= 0x0C,		/**< @brief PWM 01 low byte for turn off count */
		PCA9685REG_PWM01_OffH	= 0x0D,		/**< @brief PWM 01 high byte for turn off count */
		PCA9685REG_PWM02_OnL	= 0x0E,		/**< @brief PWM 02 low byte for turn on count */
		PCA9685REG_PWM02_OnH	= 0x0F,		/**< @brief PWM 02 high byte for turn on count */
		PCA9685REG_PWM02_OffL	= 0x10,		/**< @brief PWM 02 low byte for turn off count */
		PCA9685REG_PWM02_OffH	= 0x11,		/**< @brief PWM 02 high byte for turn off count */
		PCA9685REG_PWM03_OnL	= 0x12,		/**< @brief PWM 03 low byte for turn on count */
		PCA9685REG_PWM03_OnH	= 0x13,		/**< @brief PWM 03 high byte for turn on count */
		PCA9685REG_PWM03_OffL	= 0x14,		/**< @brief PWM 03 low byte for turn off count */
		PCA9685REG_PWM03_OffH	= 0x15,		/**< @brief PWM 03 high byte for turn off count */
		PCA9685REG_PWM04_OnL	= 0x16,		/**< @brief PWM 04 low byte for turn on count */
		PCA9685REG_PWM04_OnH	= 0x17,		/**< @brief PWM 04 high byte for turn on count */
		PCA9685REG_PWM04_OffL	= 0x18,		/**< @brief PWM 04 low byte for turn off count */
		PCA9685REG_PWM04_OffH	= 0x19,		/**< @brief PWM 04 high byte for turn off count */
		PCA9685REG_PWM05_OnL	= 0x1A,		/**< @brief PWM 05 low byte for turn on count */
		PCA9685REG_PWM05_OnH	= 0x1B,		/**< @brief PWM 05 high byte for turn on count */
		PCA9685REG_PWM05_OffL	= 0x1C,		/**< @brief PWM 05 low byte for turn off count */
		PCA9685REG_PWM05_OffH	= 0x1D,		/**< @brief PWM 05 high byte for turn off count */
		PCA9685REG_PWM06_OnL	= 0x1E,		/**< @brief PWM 06 low byte for turn on count */
		PCA9685REG_PWM06_OnH	= 0x1F,		/**< @brief PWM 06 high byte for turn on count */
		PCA9685REG_PWM06_OffL	= 0x20,		/**< @brief PWM 06 low byte for turn off count */
		PCA9685REG_PWM06_OffH	= 0x21,		/**< @brief PWM 06 high byte for turn off count */
		PCA9685REG_PWM07_OnL	= 0x22,		/**< @brief PWM 07 low byte for turn on count */
		PCA9685REG_PWM07_OnH	= 0x23,		/**< @brief PWM 07 high byte for turn on count */
		PCA9685REG_PWM07_OffL	= 0x24,		/**< @brief PWM 07 low byte for turn off count */
		PCA9685REG_PWM07_OffH	= 0x25,		/**< @brief PWM 07 high byte for turn off count */
		PCA9685REG_PWM08_OnL	= 0x26,		/**< @brief PWM 08 low byte for turn on count */
		PCA9685REG_PWM08_OnH	= 0x27,		/**< @brief PWM 08 high byte for turn on count */
		PCA9685REG_PWM08_OffL	= 0x28,		/**< @brief PWM 08 low byte for turn off count */
		PCA9685REG_PWM08_OffH	= 0x29,		/**< @brief PWM 08 high byte for turn off count */
		PCA9685REG_PWM09_OnL	= 0x2A,		/**< @brief PWM 09 low byte for turn on count */
		PCA9685REG_PWM09_OnH	= 0x2B,		/**< @brief PWM 09 high byte for turn on count */
		PCA9685REG_PWM09_OffL	= 0x2C,		/**< @brief PWM 09 low byte for turn off count */
		PCA9685REG_PWM09_OffH	= 0x2D,		/**< @brief PWM 09 high byte for turn off count */
		PCA9685REG_PWM10_OnL	= 0x2E,		/**< @brief PWM 10 low byte for turn on count */
		PCA9685REG_PWM10_OnH	= 0x2F,		/**< @brief PWM 10 high byte for turn on count */
		PCA9685REG_PWM10_OffL	= 0x30,		/**< @brief PWM 10 low byte for turn off count */
		PCA9685REG_PWM10_OffH	= 0x31,		/**< @brief PWM 10 high byte for turn off count */
		PCA9685REG_PWM11_OnL	= 0x32,		/**< @brief PWM 11 low byte for turn on count */
		PCA9685REG_PWM11_OnH	= 0x33,		/**< @brief PWM 11 high byte for turn on count */
		PCA9685REG_PWM11_OffL	= 0x34,		/**< @brief PWM 11 low byte for turn off count */
		PCA9685REG_PWM11_OffH	= 0x35,		/**< @brief PWM 11 high byte for turn off count */
		PCA9685REG_PWM12_OnL	= 0x36,		/**< @brief PWM 12 low byte for turn on count */
		PCA9685REG_PWM12_OnH	= 0x37,		/**< @brief PWM 12 high byte for turn on count */
		PCA9685REG_PWM12_OffL	= 0x38,		/**< @brief PWM 12 low byte for turn off count */
		PCA9685REG_PWM12_OffH	= 0x39,		/**< @brief PWM 12 high byte for turn off count */
		PCA9685REG_PWM13_OnL	= 0x3A,		/**< @brief PWM 13 low byte for turn on count */
		PCA9685REG_PWM13_OnH	= 0x3B,		/**< @brief PWM 13 high byte for turn on count */
		PCA9685REG_PWM13_OffL	= 0x3C,		/**< @brief PWM 13 low byte for turn off count */
		PCA9685REG_PWM13_OffH	= 0x3D,		/**< @brief PWM 13 high byte for turn off count */
		PCA9685REG_PWM14_OnL	= 0x3E,		/**< @brief PWM 14 low byte for turn on count */
		PCA9685REG_PWM14_OnH	= 0x3F,		/**< @brief PWM 14 high byte for turn on count */
		PCA9685REG_PWM14_OffL	= 0x40,		/**< @brief PWM 14 low byte for turn off count */
		PCA9685REG_PWM14_OffH	= 0x41,		/**< @brief PWM 14 high byte for turn off count */
		PCA9685REG_PWM15_OnL	= 0x42,		/**< @brief PWM 15 low byte for turn on count */
		PCA9685REG_PWM15_OnH	= 0x43,		/**< @brief PWM 15 high byte for turn on count */
		PCA9685REG_PWM15_OffL	= 0x44,		/**< @brief PWM 15 low byte for turn off count */
		PCA9685REG_PWM15_OffH	= 0x45,		/**< @brief PWM 15 high byte for turn off count */
		
		PCA9685REG_AllLED_OnL	= 0xFA,		/**< @brief Load all LED On low byte registers */
		PCA9685REG_AllLED_OnH	= 0xFB,		/**< @brief Load all LED On high byte registers */
		PCA9685REG_AllLED_OffL	= 0xFC,		/**< @brief Load all LED Off low byte registers */
		PCA9685REG_AllLED_OffH	= 0xFD,		/**< @brief Load all LED Off high byte registers */
		
		PCA9685REG_Low_CntMask	= 0xFF,		/**< @brief Mask of all the bits in the low byte registers used to set the count value */
		PCA9685REG_High_CntMask	= 0x0F,		/**< @brief Mask of all the bits in the high byte registers used to set teh count value */
		PCA9685REG_High_FullOn	= 0x10,		/**< @brief This bit in the high byte register sets the output to full on */
		PCA9685REG_PWMnn_Offset	= 4,		/**< @brief Offset between matching register addresses of PWM output settings */
		
		PCA9685REG_FreqPreScale	= 0xFE,		/**< @brief Prescaler for PWM output frequency */
		PCA9685REG_TestMode		= 0xFF,		/**< @brief Defines the test mode to be entered */
	} ePCA9685Regs_t;
	
	/**	@brief		Bits and values controlled in the Mode 1 register of the PCA9685 PWM controller
		@ingroup	pca9685driver
	*/
	typedef enum ePCA9685_Mode1Reg_t {
		PCA9685_Mode1Reg_Restart	= 0x80,	/**< @brief State of restart logic: 1 Enabled, 0 Disabled */
		PCA9685_Mode1Reg_ExtClock	= 0x40,	/**< @brief Control use of external clock: 1 Use external clock, 0 use internal clock */
		PCA9685_Mode1Reg_AuotInc	= 0x20,	/**< @brief Register Auto-Increment status: 1 Enabled, 0 Disabled */
		PCA9685_Mode1Reg_Sleep		= 0x10,	/**< @brief Power mode: 1 Low power mode, 0 Normal mode */
		PCA9685_Mode1Reg_Sub1		= 0x08,	/**< @brief Usage of sub-address 1: 1 Enabled, 0 Disabled */
		PCA9685_Mode1Reg_Sub2		= 0x04,	/**< @brief Usage of sub-address 2: 1 Enabled, 0 Disabled */
		PCA9685_Mode1Reg_Sub3		= 0x02,	/**< @brief Usage of sub-address 3: 1 Enabled, 0 Disabled */
		PCA9685_Mode1Reg_AllCall	= 0x01,	/**< @brief Usage of all call address: 1 Enabled, 0 Disabled */
	} ePCA9685_Mode1Reg_t;
	
	/**	@brief		Bits and values controlled in the Mode 1 register of the PCA9685 PWM controller
		@ingroup	pca9685driver
	*/
	typedef enum ePCA9685_Mode2Reg_t {
		PCA9685_Mode2Reg_Invert		= 0x10,	/**< @brief Output state logic: 1 Inverted, 0 Not Inverted */
		PCA9685_Mode2Reg_OutChange	= 0x08,	/**< @brief Output change: 1 Change on ACK, 0 Change on STOP command */
		PCA9685_Mode2Reg_OutDrive	= 0x04,	/**< @brief Output structure: 1 open-drain, 0 totem pole */
		
		PCA9685_Mode2Reg_OutEnMask	= 0x02,	/**< @brief These bits control the behavior of the output enable pin */
		PCA9685_Mode2Reg_OutEn0		= 0x00, /**< @brief Causes the output enable to set all outputs to logic zero */
		PCA9685_Mode2Reg_OutEnOutDrv= 0x01,	/**< @brief If OutDrive is 1 then output enable makes all outputs logic one, if OutDrive is 0 then all outputs go to high impedence */
		PCA9685_Mode2Reg_OutEnImped	= 0x10,	/**< @brief Causes the output enable to set all outputs to high impedance */
	} ePCA9685_Mode2Reg_t;
	
	typedef struct sPCA9685Info_t {
		sGPIOIface_t *pGpio;
		sI2CIface_t *pI2C;
		ePCA9685Address_t eAddr;
		GPIOID_t nOutEnablePin;
	} sPCA9685Info_t;

/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	/**	@brief		Contrusctor to prepare the class instance for use
		@details	Sets up the software object and prepares the hardware for use
		@param		pObj			Pointer to the peripheral software object
		@param		pGpio			Pointer to the GPIO interface this driver will use
		@param		pI2C			Pointer to the I2C bus interface this driver will use
		@param		eAddr			Bit ORed flags for which bits are set in the device address
		@param		nOutEnPin		The pin number that the output enable is connected to
		@return		PCA9685_Success on success, or a code indicating the type of problem 
			encounterd
		@ingroup	pca9685driver
	*/
	ePCA9685Return_t PCA9685Initialize(sPCA9685Info_t *pObj, sGPIOIface_t *pGpio, sI2CIface_t *pI2C, ePCA9685Address_t eAddr, GPIOID_t nOutEnPin);

	/**	@brief		Sets the PWM values for a specified output
		@details	The output runs on a 12 bit counter.  You can specify the count when the pulse
			should begin and end.
			
			nOnCnt is the count value when the pulse should have its leading edge, going from low
			to high voltage.
			
			nOffCnt is the count value when the pulse will have its trailing edge, going from high
			to low.
			
			These counts should not exceed 2^12 = 4096.  They also can be in any order.
			
			These settings are applied immediately.  The function will block until the commands are
			sent over the bus to the PCA9865.
		@param		nPWMOutNum		The PWM output to operate
		@param		nOnCnt			The starting count of the pulse
		@param		nOffCnt			The ending count of the pulse
		@return		PCA9685_Success on success, or a code indicating the type of problem 
			encounterd
		@ingroup	pca9685driver
	*/
	ePCA9685Return_t PCA9685SetOutput(sPCA9685Info_t *pObj, uint8_t nPWMOutNum, uint16_t nOnCnt, uint16_t nOffCnt);
	
	/**	@brief		Retrieve the output frequency for all PWM outputs 
		@return		PCA9685_Success on success, or a code indicating the type of problem 
			encounterd
		@ingroup	pca9685driver
	*/
	ePCA9685Return_t PCA9685GetPWMFrequency(sPCA9685Info_t *pObj, uint32_t *pnFreqHz);
	
	/**	@brief		If the PWM is set to change over time, this function performs those updates
		@details	If a change over time is scheduled, this function will carry out the desired
			change.  It will check the current time then instruct the output to change to the 
			duty cycle expected at that moment.
		@return		PCA9685_Success on success, or a code indicating the type of problem 
			encounterd
		@ingroup	pca9685driver
	*/
	ePCA9685Return_t PCA9685SetPWMFrequency(sPCA9685Info_t *pObj, uint32_t nFreqHz);
	
	/**	@brief		Enables or disables all PWM outputs
		@return		PCA9685_Success on success, or a code indicating the type of problem 
			encounterd
		@ingroup	pca9685driver
	*/
	ePCA9685Return_t PCA9685EnableOutputs(sPCA9685Info_t *pObj, bool bEnable);

/*****	Functions	*****/


#endif
