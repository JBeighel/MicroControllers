/**	File:	PCA9685Driver.c
	Author:	J. Beighel
	Date:	12-17-2020
*/

/*****	Includes	*****/
	#include "PCA9685Driver.h"

/*****	Defines		*****/
	/**	@brief		Calculates the prescale value needed for different PWM frquencies
		@details	There is a register in the PCA9685 that allows you to set  prescale value that will set
			the frequency of the pulses sent out the outputs.  This macro will calculate the value needed in
			that register based on the desired output frequence.
		@param		nClockHz	Frequency of the oscillator by the PCA9685 in Hertz
		@param		nFreq		Desired output frequence in Hertz
		@return		The value to supply in the frequency prescale register of the PCA9685
		@ingroup	pca9685driver
	*/
	#define PCA9685CalcPreScale(nClockHz, nFreqHz)	(nClockHz / (PCA9685_PULSECOUNTS * (uint32_t)nFreqHz))
	
	/**	@brief		Calculates the frequency of the PWM output based on the prescale value
		@details	There is a register in the PCA9685 that allows you to set  prescale value that will set
			the frequency of the pulses sent out the outputs.  This macro will calculate the effective 
			frequency of the output based on a specified prescale value.
		@param		nClockHz	Frequency of the oscillator by the PCA9685 in Hertz
		@param		nPrescale	Prescale value set in the PCA9685
		@return		The effective frequency of the PWM based on the prescale value specified for the PCA9685
		@ingroup	pca9685driver
	*/
	#define PCA9685CalcPWMFreq(nClockHz, nPrescale)	(nClockHz / (PCA9685_PULSECOUNTS * (uint32_t)nPrescale))
	
	/**	@brief		Calculate the Count value from the High/Low settings registers
		@details	The PCA9685 uses a 12 bit counter to control the PWM outputs.  You specify the count value 
			when the pulse should start and end.  Since the PCA9685 uses 8-bit registers the count value must
			be split into two seperate registers.  When these high and low bit registers are read this function 
			will calculate the value in a 16 bit unsigned integer.
			
			This same calculation will work for the on and off count values.
		@param		nHighReg	Value in the high bit register as an unsigned integer
		@param		nLowReg		Value in the low bit register as an unsigend integer
		@ingroup	pca9685driver
	*/
	#define PCA9685CalcCount(nHighReg, nLowReg)		(((uint16_t)(nHighReg & PCA9685REG_High_CntMask) << 8) | ((uint16_t)nLowReg))

/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/


/*****	Functions	*****/

ePCA9685Return_t PCA9685Initialize(sPCA9685Info_t *pObj, sGPIOIface_t *pGpio, sI2CIface_t *pI2C, ePCA9685Address_t eAddr, GPIOID_t nOutEnPin) {
	uint8_t nRegVal;
	
	//Put the details in the object
	pObj->pGpio = pGpio;
	pObj->pI2C = pI2C;
	pObj->eAddr = eAddr | PCA9685_Fixed; //Ensure the fixed portion of the address is there
	pObj->nOutEnablePin = nOutEnPin;
	
	//Setup the pin	
	if (nOutEnPin != GPIO_NOPIN) {
		pObj->pGpio->pfSetModeByPin(pObj->pGpio, pObj->nOutEnablePin, GPIO_DigitalOutput);
		pObj->pGpio->pfDigitalWriteByPin(pObj->pGpio, pObj->nOutEnablePin, true);
	}
	
	//Start by resetting the chip
	pObj->pI2C->pfI2CGeneralCall(pObj->pI2C, PCA9685_Reset);
	
	//Now specify the modes to work in
	pObj->pI2C->pfI2CReadUint8Reg(pObj->pI2C, pObj->eAddr, PCA9685REG_Mode1, &nRegVal);
	
	ZeroAllBitsInMask(nRegVal, PCA9685_Mode1Reg_ExtClock | PCA9685_Mode1Reg_AuotInc | PCA9685_Mode1Reg_Sleep | PCA9685_Mode1Reg_Sub1 | PCA9685_Mode1Reg_Sub2 | PCA9685_Mode1Reg_Sub3);
	
	pObj->pI2C->pfI2CWriteUint8Reg(pObj->pI2C, pObj->eAddr, PCA9685REG_Mode1, nRegVal);
	
	pObj->pI2C->pfI2CReadUint8Reg(pObj->pI2C, pObj->eAddr, PCA9685REG_Mode2, &nRegVal);
	
	ZeroAllBitsInMask(nRegVal, PCA9685_Mode2Reg_Invert | PCA9685_Mode2Reg_OutChange);
	SetAllBitsInMask(nRegVal, PCA9685_Mode2Reg_OutDrive);
	
	pObj->pI2C->pfI2CWriteUint8Reg(pObj->pI2C, pObj->eAddr, PCA9685REG_Mode2, nRegVal);
	
	//Make sure all outputs are off
	pObj->pI2C->pfI2CWriteUint8Reg(pObj->pI2C, pObj->eAddr, PCA9685REG_AllLED_OnL, 0);
	pObj->pI2C->pfI2CWriteUint8Reg(pObj->pI2C, pObj->eAddr, PCA9685REG_AllLED_OnH, 0);
	pObj->pI2C->pfI2CWriteUint8Reg(pObj->pI2C, pObj->eAddr, PCA9685REG_AllLED_OffL, 0);
	pObj->pI2C->pfI2CWriteUint8Reg(pObj->pI2C, pObj->eAddr, PCA9685REG_AllLED_OffH, 0);
	
	return PCA9685_Success;
}

ePCA9685Return_t PCA9685SetOutput(sPCA9685Info_t *pObj, uint8_t nPWMOutNum, uint16_t nOnCnt, uint16_t nOffCnt) {
	uint8_t nRegAddr, nRegVal;
	
	if (nPWMOutNum >= PCA9685_NUMPWM) { //requested output does not exist
		return false;
	}
	
	if ((nOnCnt >= PCA9685_PULSECOUNTS) || (nOffCnt >= PCA9685_PULSECOUNTS)) { //The duty cycle requested is invalid
		return false;
	}
	
	//Set the phase shift (high count)
	nRegAddr = PCA9685REG_PWM00_OnL + (nPWMOutNum * PCA9685REG_PWMnn_Offset);
	nRegVal = nOnCnt & PCA9685REG_Low_CntMask;
	
	pObj->pI2C->pfI2CWriteUint8Reg(pObj->pI2C, pObj->eAddr, nRegAddr, nRegVal);
	
	nRegAddr = PCA9685REG_PWM00_OnH + (nPWMOutNum * PCA9685REG_PWMnn_Offset);
	nRegVal = (nOnCnt >> 8) & PCA9685REG_High_CntMask;

	pObj->pI2C->pfI2CWriteUint8Reg(pObj->pI2C, pObj->eAddr, nRegAddr, nRegVal);
	
	//Set the on duration (low count)
	nRegAddr = PCA9685REG_PWM00_OffL + (nPWMOutNum * PCA9685REG_PWMnn_Offset);
	nRegVal = nOffCnt & PCA9685REG_Low_CntMask;
	
	pObj->pI2C->pfI2CWriteUint8Reg(pObj->pI2C, pObj->eAddr, nRegAddr, nRegVal);

	nRegAddr = PCA9685REG_PWM00_OffH + (nPWMOutNum * PCA9685REG_PWMnn_Offset);
	nRegVal = (nOffCnt >> 8) & PCA9685REG_High_CntMask;

	pObj->pI2C->pfI2CWriteUint8Reg(pObj->pI2C, pObj->eAddr, nRegAddr, nRegVal);

	return true;
}

ePCA9685Return_t PCA9685GetPWMFrequency(sPCA9685Info_t *pObj, uint32_t *pnFreqHz) {
	eI2CReturn_t eResult;
	uint8_t nRegVal;
	
	//Read the prescale value applied
	eResult = pObj->pI2C->pfI2CReadUint8Reg(pObj->pI2C, pObj->eAddr, PCA9685REG_FreqPreScale, &nRegVal);
	if (eResult != I2C_Success) {
		*pnFreqHz = 0;
		return PCA9685Fail_BusError;
	}

	if (nRegVal == 0) {
		return PCA9685Fail_Unknown;
	}

	//Return the frequency
	*pnFreqHz = PCA9685CalcPWMFreq(PCA9685_INTCLKFREQ, nRegVal);
	
	return PCA9685_Success;
}

ePCA9685Return_t PCA9685SetPWMFrequency(sPCA9685Info_t *pObj, uint32_t nFreqHz) {
	uint8_t nRegVal, nOrgMode1;
	eI2CReturn_t eResult;
	
	//PWM controller must be in sleep mode to change frequency
	pObj->pI2C->pfI2CReadUint8Reg(pObj->pI2C, pObj->eAddr, PCA9685REG_Mode1, &nOrgMode1);
	
	if (CheckAllBitsInMask(nOrgMode1, PCA9685_Mode1Reg_Sleep) == 0) {
		nRegVal = nOrgMode1;
		SetAllBitsInMask(nRegVal, PCA9685_Mode1Reg_Sleep);
		
		eResult = pObj->pI2C->pfI2CWriteUint8Reg(pObj->pI2C, pObj->eAddr, PCA9685REG_Mode1, nRegVal);
		
		if (eResult != I2C_Success) {
			return PCA9685Fail_BusError;
		}
	}
	
	//Update the frequency value
	nRegVal = PCA9685CalcPreScale(PCA9685_INTCLKFREQ, nFreqHz);
	
	eResult = pObj->pI2C->pfI2CWriteUint8Reg(pObj->pI2C, pObj->eAddr, PCA9685REG_FreqPreScale, nRegVal);

	if (eResult != I2C_Success) {
		return PCA9685Fail_BusError;
	}

	//REstore the previous mode 1 settigns
	eResult = pObj->pI2C->pfI2CWriteUint8Reg(pObj->pI2C, pObj->eAddr, PCA9685REG_Mode1, nOrgMode1);
		
	if (eResult != I2C_Success) {
		return PCA9685Fail_BusError;
	}
	
	return PCA9685_Success;
}
