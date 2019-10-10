/**	@defgroup	pca9865driver
	@brief		Driver for PCA9865 PWMController
	@details	v0.1
		The pulse width frequency can span from 25 Hz (prescaler of 1 / 0x01) to 6100 Hz (prescaler of 255 / 
		0xFF).  The default frequency is 200 Hz (prescaler of 30 / 0x1E)
*/

#ifndef __PCA9865DRIVER_H
	#define __PCA9865DRIVER_H

/***** Includes		*****/
	#include "CommonUtils.h"
	#include "I2CDevice.h"

/***** Definitions	*****/
	/**	@brief		The PCA9865 has an internal clock with a frequency of 25MHz
		@ingroup	pca9865driver
	*/
	#define PCA9865_INTCLKFREQ	25000000
	
	/**	@brief		Used to specify that an optional pin of the device is not connected
		@ingroup	pca9865driver
	*/
	#define PCA9865_NOPIN		0xFF
	
	/**	@brief		Specifies the number of PWM outputs the PCA9865 controls
		@ingroup	pca9865driver
	*/
	#define PCA9865_NUMPWM		16
	
	/**	@brief		The number of counts available in the pulse width settings
		@ingroup	pca9865driver
	*/
	#define PCA9865_PULSECOUNTS	4096

	/**	@brief		Enum of I2C address components for the PCA9865 PWM Driver
		@details	The device has a fixed address bit of 0x80, then 6 pins that can 
			set additional bits in this address.  By bitwise ORing these values 
			together the full address of the device can be created.
			
			In addition there are a number of optional I2C addresses the device can
			respond to.  These are also identified within this enum.
			
			The data sheet has them 1 bit higher.  The Arduino I2C functions will 
			apply a bit shift to make space for the R/W bit.
		@ingroup	pca9865driver
	*/
	typedef enum ePCA9865Address_t {
		PCA9865_None	= 0x00,		/**< @brief None of the external bits are set */
		PCA9865_Fixed	= 0x40,		/**< @brief Address bit that is always used */
		PCA9865_A5		= 0x20,		/**< @brief Address bit added throug pin A5 */
		PCA9865_A4		= 0x10,		/**< @brief Address bit added throug pin A4 */
		PCA9865_A3		= 0x08,		/**< @brief Address bit added throug pin A3 */
		PCA9865_A2		= 0x04,		/**< @brief Address bit added throug pin A2 */
		PCA9865_A1		= 0x02,		/**< @brief Address bit added throug pin A1 */
		PCA9865_A0		= 0x01,		/**< @brief Address bit added throug pin A0 */
		
		PCA9865_AllCall	= 0xE0,		/**< @brief Default value for the All Call address */
		PCA9865_SubAdr1	= 0xE2,		/**< @brief Default value for the sub-address 1 */
		PCA9865_SubAdr2	= 0xE4,		/**< @brief Default value for the sub-address 2 */
		PCA9865_SubAdr3	= 0xE8,		/**< @brief Default value for the sub-address 3 */
		
		PCA9865_Reset	= 0x06,		/**< @brief Reserved address that must be used for software resets */
	} ePCA9865Address_t;
	
	/**	@brief		Enumeration of all registers accessible via I2C bus for the PCA9865 PWM controller	
		@ingroup	pca9865driver
	*/
	typedef enum ePCA9865Regs_t {
		PWMREG_Mode1		= 0x00,		/**< @brief Mode register 1 */
		PWMREG_Mode2		= 0x01,		/**< @brief Mode register 2 */
		PWMREG_SubAdr1		= 0x02,		/**< @brief I2C bus sub-address 1 */
		PWMREG_SubAdr2		= 0x03,		/**< @brief I2C bus sub-address 2 */
		PWMREG_SubAdr3		= 0x04,		/**< @brief I2C bus sub-address 3 */
		PWMREG_AllCall		= 0x05,		/**< @brief I2C bus all call address */
		
		PWMREG_PWM00_OnL	= 0x06,		/**< @brief PWM 00 low byte for turn on count */
		PWMREG_PWM00_OnH	= 0x07,		/**< @brief PWM 00 high byte for turn on count */
		PWMREG_PWM00_OffL	= 0x08,		/**< @brief PWM 00 low byte for turn off count */
		PWMREG_PWM00_OffH	= 0x09,		/**< @brief PWM 00 high byte for turn off count */
		PWMREG_PWM01_OnL	= 0x0A,		/**< @brief PWM 01 low byte for turn on count */
		PWMREG_PWM01_OnH	= 0x0B,		/**< @brief PWM 01 high byte for turn on count */
		PWMREG_PWM01_OffL	= 0x0C,		/**< @brief PWM 01 low byte for turn off count */
		PWMREG_PWM01_OffH	= 0x0D,		/**< @brief PWM 01 high byte for turn off count */
		PWMREG_PWM02_OnL	= 0x0E,		/**< @brief PWM 02 low byte for turn on count */
		PWMREG_PWM02_OnH	= 0x0F,		/**< @brief PWM 02 high byte for turn on count */
		PWMREG_PWM02_OffL	= 0x10,		/**< @brief PWM 02 low byte for turn off count */
		PWMREG_PWM02_OffH	= 0x11,		/**< @brief PWM 02 high byte for turn off count */
		PWMREG_PWM03_OnL	= 0x12,		/**< @brief PWM 03 low byte for turn on count */
		PWMREG_PWM03_OnH	= 0x13,		/**< @brief PWM 03 high byte for turn on count */
		PWMREG_PWM03_OffL	= 0x14,		/**< @brief PWM 03 low byte for turn off count */
		PWMREG_PWM03_OffH	= 0x15,		/**< @brief PWM 03 high byte for turn off count */
		PWMREG_PWM04_OnL	= 0x16,		/**< @brief PWM 04 low byte for turn on count */
		PWMREG_PWM04_OnH	= 0x17,		/**< @brief PWM 04 high byte for turn on count */
		PWMREG_PWM04_OffL	= 0x18,		/**< @brief PWM 04 low byte for turn off count */
		PWMREG_PWM04_OffH	= 0x19,		/**< @brief PWM 04 high byte for turn off count */
		PWMREG_PWM05_OnL	= 0x1A,		/**< @brief PWM 05 low byte for turn on count */
		PWMREG_PWM05_OnH	= 0x1B,		/**< @brief PWM 05 high byte for turn on count */
		PWMREG_PWM05_OffL	= 0x1C,		/**< @brief PWM 05 low byte for turn off count */
		PWMREG_PWM05_OffH	= 0x1D,		/**< @brief PWM 05 high byte for turn off count */
		PWMREG_PWM06_OnL	= 0x1E,		/**< @brief PWM 06 low byte for turn on count */
		PWMREG_PWM06_OnH	= 0x1F,		/**< @brief PWM 06 high byte for turn on count */
		PWMREG_PWM06_OffL	= 0x20,		/**< @brief PWM 06 low byte for turn off count */
		PWMREG_PWM06_OffH	= 0x21,		/**< @brief PWM 06 high byte for turn off count */
		PWMREG_PWM07_OnL	= 0x22,		/**< @brief PWM 07 low byte for turn on count */
		PWMREG_PWM07_OnH	= 0x23,		/**< @brief PWM 07 high byte for turn on count */
		PWMREG_PWM07_OffL	= 0x24,		/**< @brief PWM 07 low byte for turn off count */
		PWMREG_PWM07_OffH	= 0x25,		/**< @brief PWM 07 high byte for turn off count */
		PWMREG_PWM08_OnL	= 0x26,		/**< @brief PWM 08 low byte for turn on count */
		PWMREG_PWM08_OnH	= 0x27,		/**< @brief PWM 08 high byte for turn on count */
		PWMREG_PWM08_OffL	= 0x28,		/**< @brief PWM 08 low byte for turn off count */
		PWMREG_PWM08_OffH	= 0x29,		/**< @brief PWM 08 high byte for turn off count */
		PWMREG_PWM09_OnL	= 0x2A,		/**< @brief PWM 09 low byte for turn on count */
		PWMREG_PWM09_OnH	= 0x2B,		/**< @brief PWM 09 high byte for turn on count */
		PWMREG_PWM09_OffL	= 0x2C,		/**< @brief PWM 09 low byte for turn off count */
		PWMREG_PWM09_OffH	= 0x2D,		/**< @brief PWM 09 high byte for turn off count */
		PWMREG_PWM10_OnL	= 0x2E,		/**< @brief PWM 10 low byte for turn on count */
		PWMREG_PWM10_OnH	= 0x2F,		/**< @brief PWM 10 high byte for turn on count */
		PWMREG_PWM10_OffL	= 0x30,		/**< @brief PWM 10 low byte for turn off count */
		PWMREG_PWM10_OffH	= 0x31,		/**< @brief PWM 10 high byte for turn off count */
		PWMREG_PWM11_OnL	= 0x32,		/**< @brief PWM 11 low byte for turn on count */
		PWMREG_PWM11_OnH	= 0x33,		/**< @brief PWM 11 high byte for turn on count */
		PWMREG_PWM11_OffL	= 0x34,		/**< @brief PWM 11 low byte for turn off count */
		PWMREG_PWM11_OffH	= 0x35,		/**< @brief PWM 11 high byte for turn off count */
		PWMREG_PWM12_OnL	= 0x36,		/**< @brief PWM 12 low byte for turn on count */
		PWMREG_PWM12_OnH	= 0x37,		/**< @brief PWM 12 high byte for turn on count */
		PWMREG_PWM12_OffL	= 0x38,		/**< @brief PWM 12 low byte for turn off count */
		PWMREG_PWM12_OffH	= 0x39,		/**< @brief PWM 12 high byte for turn off count */
		PWMREG_PWM13_OnL	= 0x3A,		/**< @brief PWM 13 low byte for turn on count */
		PWMREG_PWM13_OnH	= 0x3B,		/**< @brief PWM 13 high byte for turn on count */
		PWMREG_PWM13_OffL	= 0x3C,		/**< @brief PWM 13 low byte for turn off count */
		PWMREG_PWM13_OffH	= 0x3D,		/**< @brief PWM 13 high byte for turn off count */
		PWMREG_PWM14_OnL	= 0x3E,		/**< @brief PWM 14 low byte for turn on count */
		PWMREG_PWM14_OnH	= 0x3F,		/**< @brief PWM 14 high byte for turn on count */
		PWMREG_PWM14_OffL	= 0x40,		/**< @brief PWM 14 low byte for turn off count */
		PWMREG_PWM14_OffH	= 0x41,		/**< @brief PWM 14 high byte for turn off count */
		PWMREG_PWM15_OnL	= 0x42,		/**< @brief PWM 15 low byte for turn on count */
		PWMREG_PWM15_OnH	= 0x43,		/**< @brief PWM 15 high byte for turn on count */
		PWMREG_PWM15_OffL	= 0x44,		/**< @brief PWM 15 low byte for turn off count */
		PWMREG_PWM15_OffH	= 0x45,		/**< @brief PWM 15 high byte for turn off count */
		
		PWMREG_AllLED_OnL	= 0xFA,		/**< @brief Load all LED On low byte registers */
		PWMREG_AllLED_OnH	= 0xFB,		/**< @brief Load all LED On high byte registers */
		PWMREG_AllLED_OffL	= 0xFC,		/**< @brief Load all LED Off low byte registers */
		PWMREG_AllLED_OffH	= 0xFD,		/**< @brief Load all LED Off high byte registers */
		
		PWMREG_Low_CntMask	= 0xFF,		/**< @brief Mask of all the bits in the low byte registers used to set the count value */
		PWMREG_High_CntMask	= 0x0F,		/**< @brief Mask of all the bits in the high byte registers used to set teh count value */
		PWMREG_High_FullOn	= 0x10,		/**< @brief This bit in the high byte register sets the output to full on */
		PWMREG_PWMnn_Offset	= 4,		/**< @brief Offset between matching register addresses of PWM output settings */
		
		PWMREG_FreqPreScale	= 0xFE,		/**< @brief Prescaler for PWM output frequency */
		PWMREG_TestMode		= 0xFF,		/**< @brief Defines the test mode to be entered */
	} ePCA9865Regs_t;
	
	/**	@brief		Bits and values controlled in the Mode 1 register of the PCA9865 PWM controller
		@ingroup	pca9865driver
	*/
	typedef enum ePCA9865_Mode1Reg_t {
		PWM_Mode1Reg_Restart	= 0x80,	/**< @brief State of restart logic: 1 Enabled, 0 Disabled */
		PWM_Mode1Reg_ExtClock	= 0x40,	/**< @brief Control use of external clock: 1 Use external clock, 0 use internal clock */
		PWM_Mode1Reg_AuotInc	= 0x20,	/**< @brief Register Auto-Increment status: 1 Enabled, 0 Disabled */
		PWM_Mode1Reg_Sleep		= 0x10,	/**< @brief Power mode: 1 Low power mode, 0 Normal mode */
		PWM_Mode1Reg_Sub1		= 0x08,	/**< @brief Usage of sub-address 1: 1 Enabled, 0 Disabled */
		PWM_Mode1Reg_Sub2		= 0x04,	/**< @brief Usage of sub-address 2: 1 Enabled, 0 Disabled */
		PWM_Mode1Reg_Sub3		= 0x02,	/**< @brief Usage of sub-address 3: 1 Enabled, 0 Disabled */
		PWM_Mode1Reg_AllCall	= 0x01,	/**< @brief Usage of all call address: 1 Enabled, 0 Disabled */
	} ePCA9865_Mode1Reg_t;
	
	/**	@brief		Bits and values controlled in the Mode 1 register of the PCA9865 PWM controller
		@ingroup	pca9865driver
	*/
	typedef enum ePCA9865_Mode2Reg_t {
		PWM_Mode2Reg_Invert		= 0x10,	/**< @brief Output state logic: 1 Inverted, 0 Not Inverted */
		PWM_Mode2Reg_OutChange	= 0x08,	/**< @brief Output change: 1 Change on ACK, 0 Change on STOP command */
		PWM_Mode2Reg_OutDrive	= 0x04,	/**< @brief Output structure: 1 open-drain, 0 totem pole */
		
		PWM_Mode2Reg_OutEnMask	= 0x02,	/**< @brief These bits control the behavior of the output enable pin */
		PWM_Mode2Reg_OutEn0		= 0x00, /**< @brief Causes the output enable to set all outputs to logic zero */
		PWM_Mode2Reg_OutEnOutDrv= 0x01,	/**< @brief If OutDrive is 1 then output enable makes all outputs logic one, if OutDrive is 0 then all outputs go to high impedence */
		PWM_Mode2Reg_OutEnImped	= 0x10,	/**< @brief Causes the output enable to set all outputs to high impedance */
	} ePCA9865_Mode2Reg_t;
	
	/**	@brief		Structure to hold the limits of the output the pulsed device supports
		@details	A basic structure defining the set of pulse signals that the device being pulsed can 
			respond to. 
		@ingroup	pca9865driver
	*/
	struct sPCA9865_PWMLimits_t {
		int16_t nMinOnValue;		/**< Minimum amount of On Counts the pulsed device supports */
		int16_t nMaxOnValue;		/**< Maximum amount of On Counts the pulsed device supports */
		bool bValidOnly;			/**< Allow only the On Counts listed in the valid values array */
		uint8_t	nNumValid;			/**< The number of values included in the valid values arrao */
		uint16_t anValidValues[10];	/**< List of valid values that the pulsed device supports */
	};
	
	struct sPCA9865_PWMOut_t {
		sPCA9865_PWMLimits_t Limits;
		
		uint16_t nOnCurrCount;
		uint16_t nOnStartCount;
		uint16_t nOnEndCount;
		
		uint16_t nOffCurrCount;
		uint16_t nOffStartCount;
		uint16_t nOffEndCount;
		
		uint32_t tSchedStart;
		uint32_t tSchedEnd;
	};
	
	class cPCA9865_t : cI2CDevice_t {
	public:
		/**	@brief		Contrusctor to prepare the class instance for use
			@details	Sets up all class variables, does not touch any hardware
			@param		nOutEnablePin	The pin number that the output enable is connected to
			@param		eI2CAddrBits	Bit ORed flags for which bits are set in the device address
			@param		pI2CWire		Pointer to the I2C bus interface this class will use
		*/
		cPCA9865_t(uint8_t nOutEnablePin, ePCA9865Address_t eI2CAddrBits, TwoWire *pI2CWire);
		
		/**	@brief		Destructor to release all resources allocated by the class
		*/
		~cPCA9865_t();
		
		/**	@brief		Initialize all hardware including connected device
			@details	Sets the pin modes for all GPIO pins used, as well as supplies configuration to the
				PCA9865 over I2C.
			@return		True on success, false on any error
		*/
		bool Initialize();
		
		/**	@brief		Sets the PWM values for a specified output
			@details	The output runs on a 12 bit counter.  You can specify the count when the pulse
				should begin and end.
				
				nOnCnt is the count value when the pulse should have its leading edge, going from low
				to high voltage.
				
				nOffCount is the count value when the pulse will have its trailing edge, going from high
				to low.
				
				These counts should not exceed 2^12 = 4096.  They also can be in any order.
				
				These settings are applied immediately.  The function will block until the commands are
				sent over the bus to the PCA9865.
			@param		nPWMOutNum		The PWM output to operate
			@param		nPhaseShiftCnt	The starting offset of the pulse
			@param		nOnCnt			The number of counts for hte pulse to sstay high
			@return		False if any requested value is invalid, True if the settings were applied
		*/
		bool SetPWMOutput(uint8_t nPWMOutNum, uint16_t nOnCount, uint16_t nOffCount);
		
		/**	@brief		Specify the limits of a PWM output
			@details	The limits are intended to serve as drivers for the pulsed device.  When set they will
				attempt to ensure that only valid signals are sent over the PWM output.
				
				By default no limits will be applied, the gsPWM_Default settings will be in place for all 
				outputs.
			@param		nPWMOutNum		Output number to apply new limits to
			@param		psLimits		Pointer to the structure containing the new limits
			@return		True if the limts are applied, false on any error.
		*/
		bool SetPWMOutputLimits(uint8_t nPWMOutNum, const sPCA9865_PWMLimits_t *psLimits);
		
		/**	@brief		Retrieve the output frequency for all PWM outputs 
			@return		The pulse output frequency in Hertz
		*/
		uint32_t GetPWMFrequency();
		
		/**	@brief		Specify the output frequency for all PWM outputs
			@param		The frequency to set in Hertz
			@return		True if the operation succeeds, false on any error
		*/
		bool SetPWMFrequency(uint32_t nFreqHz);
		
		/**	@brief		If the PWM is set to change over time, this function performs those updates
			@details	If a change over time is scheduled, this function will carry out the desired
				change.  It will check the current time then instruct the output to change to the 
				duty cycle expected at that moment.
			@return		True if the update is successful, false on any error
		*/
		bool Update();
		
		/**	@brief		Schedules the output to sweep to the specified duty cycle over time
			@details	The PWM will start from its current On/Off counts then over the specified time
				adjust the On and Off counts to reach the values requested.  The steps will be divided 
				equally over the specified time duration.
				
				The Update() function must be called regularly to carry out this change.  If it is not
				called then the PWM output will not be changed.
			@param		nPWMOutNum		The PWM output to schedule a change for
			@param		nOnCount		The count the On value should end on
			@param		nOffCount		The count the Off value should end on
			@param		tMsecDuration	The duration in milliseconds the change should span
			@return		True if the paramaters are valid, false if the change can not be done
		*/
		bool ScheduleOutput(uint8_t nPWMOutNum, uint16_t nOnCount, uint16_t nOffCount, uint32_t tMsecDuration);
		
	protected:
		
	private:
		uint8_t cnOutEnablePin;	/**< @brief Pin number connected to the output enable of the PCA9865 */
		sPCA9865_PWMOut_t caPWMInfo[PCA9865_NUMPWM];	/**< Limits and Settings for all PWM outputs */
	};

/***** Constants	*****/
	/**	@brief		Default PWM limits, allows full range
		@ingroup	pca9865driver
	*/
	const sPCA9865_PWMLimits_t gsPWM_Default = {
		.nMinOnValue = 0,
		.nMaxOnValue = PCA9865_PULSECOUNTS,
		.bValidOnly = false,
		.nNumValid = 0,
		.anValidValues = {
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		}
	};
	
	/**	@brief		PWM Limits for the MG90S Servo motors
		@details	These servo motors can turn through 90 degrees of rotation.  They expect a pulse 
			width that has a frequency of 50 Hz, a period of 20 mSec.  In that time the pulse must be 
			between 1 and 2 mSec long.  The 1 mSec pulse will be at 0 degrees, 1.5 is 90 degrees, and 
			2 mSec is 180 degrees.
			
			By calculation this should be pulse widths from 205 to 409 counts.  Experimentally the 
			counts of 150 to 450 achieved the full rotation.
		@ingroup	pca9865driver
	*/
	const sPCA9865_PWMLimits_t gsPWM_MG90S = {
		.nMinOnValue = 150,
		.nMaxOnValue = 450,
		.bValidOnly = false,
		.nNumValid = 0,
		.anValidValues = {
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		}
	};

/***** Globals		*****/


/***** Prototypes 	*****/
	/**	@brief		Calculates the prescale value needed for different PWM frquencies
		@details	There is a register in the PCA9865 that allows you to set  prescale value that will set
			the frequency of the pulses sent out the outputs.  This macro will calculate the value needed in
			that register based on the desired output frequence.
		@param		nClockHz	Frequency of the oscillator by the PCA9865 in Hertz
		@param		nFreq		Desired output frequence in Hertz
		@return		The value to supply in the frequency prescale register of the PCA9865
		@ingroup	pca9865driver
	*/
	#define PCA9865CalcPreScale(nClockHz, nFreqHz)	(nClockHz / (PCA9865_PULSECOUNTS * (uint32_t)nFreqHz))
	
	/**	@brief		Calculates the frequency of the PWM output based on the prescale value
		@details	There is a register in the PCA9865 that allows you to set  prescale value that will set
			the frequency of the pulses sent out the outputs.  This macro will calculate the effective 
			frequency of the output based on a specified prescale value.
		@param		nClockHz	Frequency of the oscillator by the PCA9865 in Hertz
		@param		nPrescale	Prescale value set in the PCA9865
		@return		The effective frequency of the PWM based on the prescale value specified for the PCA9865
		@ingroup	pca9865driver
	*/
	#define PCA9865CalcPWMFreq(nClockHz, nPrescale)	(nClockHz / (PCA9865_PULSECOUNTS * (uint32_t)nPrescale))
	
	/**	@brief		Calculate the Count value from the High/Low settings registers
		@details	The PCA9865 uses a 12 bit counter to control the PWM outputs.  You specify the count value 
			when the pulse should start and end.  Since the PCA9865 uses 8-bit registers the count value must
			be split into two seperate registers.  When these high and low bit registers are read this function 
			will calculate the value in a 16 bit unsigned integer.
			
			This same calculation will work for the on and off count values.
		@param		nHighReg	Value in the high bit register as an unsigned integer
		@param		nLowReg		Value in the low bit register as an unsigend integer
		@ingroup	pca9865driver
	*/
	#define PCA9865CalcCount(nHighReg, nLowReg)		(((uint16_t)(nHighReg & PWMREG_High_CntMask) << 8) | ((uint16_t)nLowReg))

/***** Functions	*****/
cPCA9865_t::cPCA9865_t(uint8_t nOutEnablePin, ePCA9865Address_t eI2CAddrBits, TwoWire *pI2CWire) : cI2CDevice_t((uint8_t)(eI2CAddrBits | PCA9865_Fixed), pI2CWire) {
	uint8_t nCtr;
	
	//Setup all class variables
	cnOutEnablePin = nOutEnablePin;
	
	//Set the default limits to all outputs
	for (nCtr = 0; nCtr < PCA9865_NUMPWM; nCtr++) {
		caPWMInfo[nCtr].nOnCurrCount = 0;
		caPWMInfo[nCtr].nOnStartCount = 0;
		caPWMInfo[nCtr].nOnEndCount = 0;
		
		caPWMInfo[nCtr].nOffCurrCount = 0;
		caPWMInfo[nCtr].nOffStartCount = 0;
		caPWMInfo[nCtr].nOffEndCount = 0;
		
		caPWMInfo[nCtr].tSchedStart = 0;
		caPWMInfo[nCtr].tSchedEnd = 0;
		
		SetPWMOutputLimits(nCtr, &gsPWM_Default);
	}
	
	return;
}

cPCA9865_t::~cPCA9865_t() {
	//No resources are allocated, no clanup work is needed
	return;
}

bool cPCA9865_t::Initialize() {
	uint8_t nRegVal;
	
	if (cnOutEnablePin != PCA9865_NOPIN) {
		pinMode(cnOutEnablePin, OUTPUT);
	}
	
	//Start by resetting the chip
	I2CGeneralCall(PCA9865_Reset);
	
	//Now specify the modes to work in
	nRegVal = I2CReadUint8Reg(cnI2CAddr, PWMREG_Mode1);
	
	ZeroAllBitsInMask(nRegVal, PWM_Mode1Reg_ExtClock | PWM_Mode1Reg_AuotInc | PWM_Mode1Reg_Sleep | PWM_Mode1Reg_Sub1 | PWM_Mode1Reg_Sub2 | PWM_Mode1Reg_Sub3);
	
	I2CWriteUint8Reg(cnI2CAddr, PWMREG_Mode1, nRegVal);
	
	nRegVal = I2CReadUint8Reg(cnI2CAddr, PWMREG_Mode2);
	
	ZeroAllBitsInMask(nRegVal, PWM_Mode2Reg_Invert | PWM_Mode2Reg_OutChange);
	SetAllBitsInMask(nRegVal, PWM_Mode2Reg_OutDrive);
	
	I2CWriteUint8Reg(cnI2CAddr, PWMREG_Mode2, nRegVal);
	
	//Make sure all outputs are off
	I2CWriteUint8Reg(cnI2CAddr, PWMREG_AllLED_OnL, 0);
	I2CWriteUint8Reg(cnI2CAddr, PWMREG_AllLED_OnH, 0);
	I2CWriteUint8Reg(cnI2CAddr, PWMREG_AllLED_OffL, 0);
	I2CWriteUint8Reg(cnI2CAddr, PWMREG_AllLED_OffH, 0);
	
	return;
}

bool cPCA9865_t::SetPWMOutput(uint8_t nPWMOutNum, uint16_t nOnCount, uint16_t nOffCount) {
	uint8_t nRegAddr, nRegVal;
	uint16_t nOnDur;
	
	if (nPWMOutNum >= PCA9865_NUMPWM) { //requested output does not exist
		return false;
	}
	
	if ((nOnCount >= PCA9865_PULSECOUNTS) || (nOffCount >= PCA9865_PULSECOUNTS)) { //The duty cycle requested is invalid
		return false;
	}
	
	if (nOnCount > nOffCount) { //Request to turn off first during each count cycle
		nOnDur = nOffCount; //On time at start of cycle
		nOnDur += 4096 - nOnCount; //On time at end of cycle
	} else { //Request to turn on first during each count cycle
		nOnDur = nOffCount - nOnCount;
	}
	
	if ((nOnDur > caPWMInfo[nPWMOutNum].Limits.nMaxOnValue) || (nOnDur < caPWMInfo[nPWMOutNum].Limits.nMinOnValue)) {
		return false; //Requested counts violate the specified limits
	}
	
	//Set the phase shift (high count)
	nRegAddr = PWMREG_PWM00_OnL + (nPWMOutNum * PWMREG_PWMnn_Offset);
	nRegVal = nOnCount & PWMREG_Low_CntMask;
	
	I2CWriteUint8Reg(cnI2CAddr, nRegAddr, nRegVal);
	
	nRegAddr = PWMREG_PWM00_OnH + (nPWMOutNum * PWMREG_PWMnn_Offset);
	nRegVal = (nOnCount >> 8) & PWMREG_High_CntMask;

	I2CWriteUint8Reg(cnI2CAddr, nRegAddr, nRegVal);
	
	//Set the on duration (low count)
	nRegAddr = PWMREG_PWM00_OffL + (nPWMOutNum * PWMREG_PWMnn_Offset);
	nRegVal = nOffCount & PWMREG_Low_CntMask;
	
	I2CWriteUint8Reg(cnI2CAddr, nRegAddr, nRegVal);

	nRegAddr = PWMREG_PWM00_OffH + (nPWMOutNum * PWMREG_PWMnn_Offset);
	nRegVal = (nOffCount >> 8) & PWMREG_High_CntMask;

	I2CWriteUint8Reg(cnI2CAddr, nRegAddr, nRegVal);
	
	//Update the PWM output information so update will make no changes
	caPWMInfo[nPWMOutNum].nOnCurrCount = nOnCount;
	caPWMInfo[nPWMOutNum].nOnEndCount = nOnCount;
	caPWMInfo[nPWMOutNum].nOffCurrCount = nOffCount;
	caPWMInfo[nPWMOutNum].nOffEndCount = nOffCount;

	return true;
}

bool cPCA9865_t::SetPWMOutputLimits(uint8_t nPWMOutNum, const sPCA9865_PWMLimits_t *psLimits) {
	if (nPWMOutNum >= PCA9865_NUMPWM) { //requested output does not exist
		return false;
	}
	
	memcpy(&(caPWMInfo[nPWMOutNum].Limits), psLimits, sizeof(sPCA9865_PWMLimits_t));
	
	return true;
}

uint32_t cPCA9865_t::GetPWMFrequency() {
	uint8_t nRegVal;
	
	//Read the prescale value applied
	nRegVal = I2CReadUint8Reg(cnI2CAddr, PWMREG_FreqPreScale);

	//Return the frequency
	return PCA9865CalcPWMFreq(PCA9865_INTCLKFREQ, nRegVal);
}
		
bool cPCA9865_t::SetPWMFrequency(uint32_t nFreqHz) {
	uint8_t nRegVal, nOrgMode1;
	bool bResult;
	
	//PWM controller must be in sleep mode to change frequency
	nOrgMode1 = I2CReadUint8Reg(cnI2CAddr, PWMREG_Mode1);
	
	if (CheckAllBitsInMask(nOrgMode1, PWM_Mode1Reg_Sleep) == 0) {
		nRegVal = nOrgMode1;
		SetAllBitsInMask(nRegVal, PWM_Mode1Reg_Sleep);
		
		bResult = I2CWriteUint8Reg(cnI2CAddr, PWMREG_Mode1, nRegVal);
		
		if (bResult == false) {
			return false;
		}
	}
	
	//Update the frequency value
	nRegVal = PCA9865CalcPreScale(PCA9865_INTCLKFREQ, nFreqHz);
	
	bResult = I2CWriteUint8Reg(cnI2CAddr, PWMREG_FreqPreScale, nRegVal);

	if (bResult == false) {
		return false;
	}

	//REstore the previous mode 1 settigns
	bResult = I2CWriteUint8Reg(cnI2CAddr, PWMREG_Mode1, nOrgMode1);
		
	if (bResult == false) {
		return false;
	}
	
	return true;
}

bool cPCA9865_t::Update() {
	uint32_t nCtr, tCurrTime, tTimePassed, tSchedDur;
	float nChgPcnt;
	uint8_t nRegAddr, nRegVal;
	
	tCurrTime = millis();
	
	for (nCtr = 0; nCtr < PCA9865_NUMPWM; nCtr++) {
		tTimePassed = tCurrTime - caPWMInfo[nCtr].tSchedStart;
		tSchedDur = caPWMInfo[nCtr].tSchedEnd - caPWMInfo[nCtr].tSchedStart;
		
		if (caPWMInfo[nCtr].nOnEndCount != caPWMInfo[nCtr].nOnCurrCount) {
			//A change is needed for the On Count value
			if (tTimePassed >= tSchedDur) { //Schedule duration fully ellapsed
				nRegAddr = PWMREG_PWM00_OnL + (nCtr * PWMREG_PWMnn_Offset);
				nRegVal = caPWMInfo[nCtr].nOnEndCount & PWMREG_Low_CntMask;
				
				I2CWriteUint8Reg(cnI2CAddr, nRegAddr, nRegVal);

				nRegAddr = PWMREG_PWM00_OnH + (nCtr * PWMREG_PWMnn_Offset);
				nRegVal = (caPWMInfo[nCtr].nOnEndCount >> 8) & PWMREG_High_CntMask;
				
				I2CWriteUint8Reg(cnI2CAddr, nRegAddr, nRegVal);
				
				caPWMInfo[nCtr].nOnCurrCount = caPWMInfo[nCtr].nOnEndCount;
			} else {
				nChgPcnt = (float)tTimePassed / (float)tSchedDur;  //Get the percentage of time ellapsed
				
				nChgPcnt = nChgPcnt * (float)((int32_t)caPWMInfo[nCtr].nOnEndCount - (int32_t)caPWMInfo[nCtr].nOnStartCount);
				
				caPWMInfo[nCtr].nOnCurrCount = caPWMInfo[nCtr].nOnStartCount + (uint16_t)nChgPcnt;
				
				nRegAddr = PWMREG_PWM00_OnL + (nCtr * PWMREG_PWMnn_Offset);
				nRegVal = caPWMInfo[nCtr].nOnCurrCount & PWMREG_Low_CntMask;
				
				I2CWriteUint8Reg(cnI2CAddr, nRegAddr, nRegVal);

				nRegAddr = PWMREG_PWM00_OnH + (nCtr * PWMREG_PWMnn_Offset);
				nRegVal = (caPWMInfo[nCtr].nOnCurrCount >> 8) & PWMREG_High_CntMask;
				
				I2CWriteUint8Reg(cnI2CAddr, nRegAddr, nRegVal);
			}
		}
		
		if (caPWMInfo[nCtr].nOffEndCount != caPWMInfo[nCtr].nOffCurrCount) {
			//A change is needed for the Off Count value
			if (tTimePassed >= tSchedDur) { //Schedule duration fully ellapsed
				nRegAddr = PWMREG_PWM00_OffL + (nCtr * PWMREG_PWMnn_Offset);
				nRegVal = caPWMInfo[nCtr].nOffEndCount & PWMREG_Low_CntMask;
				
				I2CWriteUint8Reg(cnI2CAddr, nRegAddr, nRegVal);

				nRegAddr = PWMREG_PWM00_OffH + (nCtr * PWMREG_PWMnn_Offset);
				nRegVal = (caPWMInfo[nCtr].nOffEndCount >> 8) & PWMREG_High_CntMask;
				
				I2CWriteUint8Reg(cnI2CAddr, nRegAddr, nRegVal);
				
				caPWMInfo[nCtr].nOffCurrCount = caPWMInfo[nCtr].nOffEndCount;
			} else {
				nChgPcnt = (float)tTimePassed / (float)tSchedDur;  //Get the percentage of time ellapsed

				nChgPcnt = nChgPcnt * (float)((int32_t)caPWMInfo[nCtr].nOffEndCount - (int32_t)caPWMInfo[nCtr].nOffStartCount);

				caPWMInfo[nCtr].nOffCurrCount = caPWMInfo[nCtr].nOffStartCount + (uint16_t)nChgPcnt;

				nRegAddr = PWMREG_PWM00_OffL + (nCtr * PWMREG_PWMnn_Offset);
				nRegVal = caPWMInfo[nCtr].nOffCurrCount & PWMREG_Low_CntMask;
				
				I2CWriteUint8Reg(cnI2CAddr, nRegAddr, nRegVal);

				nRegAddr = PWMREG_PWM00_OffH + (nCtr * PWMREG_PWMnn_Offset);
				nRegVal = (caPWMInfo[nCtr].nOffCurrCount >> 8) & PWMREG_High_CntMask;
				
				I2CWriteUint8Reg(cnI2CAddr, nRegAddr, nRegVal);
			}
		}		
	}
	
	return true;
}

bool cPCA9865_t::ScheduleOutput(uint8_t nPWMOutNum, uint16_t nOnCount, uint16_t nOffCount, uint32_t tMsecDuration) {
	uint16_t nOnDur;
	uint32_t tCurrTime;
	
	if (nPWMOutNum >= PCA9865_NUMPWM) { //Requested output does not exist
		return false;
	}
	
	if ((nOnCount >= PCA9865_PULSECOUNTS) || (nOffCount >= PCA9865_PULSECOUNTS)) { //The duty cycle requested is invalid
		return false;
	}
	
	if (nOnCount > nOffCount) { //Request to turn off first during each count cycle
		nOnDur = nOffCount; //On time at start of cycle
		nOnDur += 4096 - nOnCount; //On time at end of cycle
	} else { //Request to turn on first during each count cycle
		nOnDur = nOffCount - nOnCount;
	}
	
	if ((nOnDur > caPWMInfo[nPWMOutNum].Limits.nMaxOnValue) || (nOnDur < caPWMInfo[nPWMOutNum].Limits.nMinOnValue)) {
		return false; //Requested counts violate the specified limits
	}
	
	//Scheduled change is valid, record it for the update
	tCurrTime = millis();
	
	caPWMInfo[nPWMOutNum].nOnStartCount = caPWMInfo[nPWMOutNum].nOnCurrCount;
	caPWMInfo[nPWMOutNum].nOnEndCount = nOnCount;
	
	caPWMInfo[nPWMOutNum].nOffStartCount = caPWMInfo[nPWMOutNum].nOffCurrCount;
	caPWMInfo[nPWMOutNum].nOffEndCount = nOffCount;
	
	caPWMInfo[nPWMOutNum].tSchedStart = tCurrTime;
	caPWMInfo[nPWMOutNum].tSchedEnd = tCurrTime + tMsecDuration;
	
	return true;
}

#endif

