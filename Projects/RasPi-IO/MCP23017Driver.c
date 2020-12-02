/**	File:	MCP23017Driver.c
	Author:	J. Beighel
	Date:	11-30-2020
*/

/*****	Includes	*****/
	#include "MCP23017Driver.h"

/*****	Constants	*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	/**	@brief		Convert from 0-15 pin notation to Port/Pin notation
		@param		nPinNum		The pin number, from 0 to 15
		@param		ePort		Returns the port for the specified pin
		@param		ePin		Returns the enum for the pin in the peripheral
		@return		True if the pin could be translated, false on any error
		@ingroup	mcp23017driver
	*/
	bool MCP23017DecodePinNum(uint8_t nPinNum, eMCP23017_Port_t *ePort, eMCP23017_Pin_t *ePin);

/*****	Functions	*****/
eMCP23017Returns_t MCP23017Initialize(sMCP23017Info_t *pDev, eMCP23017Address_t eAddr, sI2CIface_t *pI2CIface) {
	uint8_t nIOConVal, nRegVal, nCtr;
	eI2CReturns_t nResult;
	
	//Setup the device object
	pDev->nAddr = (uint8_t)eAddr;
	pDev->pI2C = pI2CIface;
	
	//Set the initial configuration for the IO Expander.  Listing a bit sets it to 1, excluing it sets it to zero.
	nIOConVal = MCP23017_BANK | MCP23017_SEQOP | MCP23017_HAEN;
	pDev->pI2C->pfI2CWriteUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_B0_IOCON, nIOConVal);
	//Write to both possible IOCON addresses just to be sure
	pDev->pI2C->pfI2CWriteUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_IOCON, nIOConVal);
	
	pDev->pI2C->pfI2CReadUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_IOCON, &nRegVal);
	if (nRegVal != nIOConVal) { //Device did not store the values set
		return MCP23017Fail_Unknown;
	}
	
	//Set all pins to Inputs with pull ups
	pDev->pI2C->pfI2CWriteUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_IODIRA, MCP23017_Pin_All);
	pDev->pI2C->pfI2CWriteUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_IODIRB, MCP23017_Pin_All);
	pDev->pI2C->pfI2CWriteUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_IPOLA, MCP23017_Pin_None);
	pDev->pI2C->pfI2CWriteUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_IPOLB, MCP23017_Pin_None);
	pDev->pI2C->pfI2CWriteUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_GPPUA, MCP23017_Pin_All);
	pDev->pI2C->pfI2CWriteUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_GPPUB, MCP23017_Pin_All);
	pDev->pI2C->pfI2CWriteUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_OLATA, MCP23017_Pin_None); //Not outputs by default, but have a predictable level when set
	pDev->pI2C->pfI2CWriteUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_OLATB, MCP23017_Pin_None);
	
	return MCP23017_Success;
}

eMCP23017Returns_t MCP23017PinModeByPin(sMCP23017Info_t *pDev, uint8_t nPinNum, eMCP23017_PinMode_t eMode) {
	eMCP23017_Port_t ePort;
	eMCP23017_Pin_t ePin;
	
	if (MCP23017DecodePinNum(nPinNum, &ePort, &ePin) == false) {
		return MCP23017Fail_InvalidPin;
	}
	
	return MCP23017PinModeByPort(pDev, ePort, ePin, eMode);
}

eMCP23017Returns_t MCP23017PinModeByPort(sMCP23017Info_t *pDev, eMCP23017_Port_t ePort, eMCP23017_Pin_t ePin, eMCP23017_PinMode_t eMode) {
	uint8_t nRegAddrOffset, nRegVal;
	 
	//Default mode is IOCON.Bank=0 Port Grouping
	if (ePort == MCP23017_PortA) {
		nRegAddrOffset = 0;
	} else {
		nRegAddrOffset = MCP23017_PortOffset;
	}
	
	if ((CheckAllBitsInMask(eMode, MCP23017_Input) == true) && (CheckAllBitsInMask(eMode, MCP23017_Output) == true)) { //Both input and output is invalid
		return MCP23017Fail_InvalidMode;
	} else if (CheckAllBitsInMask(eMode, MCP23017_Input) == true) { //Configured for input
		pDev->pI2C->pfI2CReadUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_IODIRA + nRegAddrOffset, &nRegVal);

		SetAllBitsInMask(nRegVal, ePin); //Set the direction bit to set input
		
		pDev->pI2C->pfI2CWriteUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_IODIRA + nRegAddrOffset, nRegVal);

		//Check and apply pull up
		pDev->pI2C->pfI2CReadUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_GPPUA + nRegAddrOffset, &nRegVal);
		
		if (CheckAllBitsInMask(eMode, MCP23017_PullUp) == true) {
			SetAllBitsInMask(nRegVal, ePin); //Set the pull up bit to include a pull-up
		} else {
			ZeroAllBitsInMask(nRegVal, ePin); //Clear the pull up bit to remove the pull-up
		}
		
		pDev->pI2C->pfI2CWriteUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_GPPUA + nRegAddrOffset, nRegVal);
		
		//Check and apply reverse logic
		pDev->pI2C->pfI2CReadUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_IPOLA + nRegAddrOffset, &nRegVal);
		
		if (CheckAllBitsInMask(eMode, MCP23017_ReverseLogic) == true) {
			SetAllBitsInMask(nRegVal, ePin); //Set the polarity bit to have reverse logic
		} else {
			ZeroAllBitsInMask(nRegVal, ePin); //Clear the polarity bit to have normal logic
		}
		
		pDev->pI2C->pfI2CWriteUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_IPOLA + nRegAddrOffset, nRegVal);
		
		//Check and apply interrupt
		pDev->pI2C->pfI2CReadUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_GPINTENA + nRegAddrOffset, &nRegVal);
		
		if (CheckAllBitsInMask(eMode, MCP23017_IntOnPinChange | MCP23017_IntOnCompare) == true) {
			SetAllBitsInMask(nRegVal, ePin); //Set the polarity bit to have reverse logic
		} else {
			ZeroAllBitsInMask(nRegVal, ePin); //Clear the polarity bit to have normal logic
		}
		
		pDev->pI2C->pfI2CWriteUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_GPINTENA + nRegAddrOffset, nRegVal);
		
		pDev->pI2C->pfI2CReadUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_INTCONA + nRegAddrOffset, &nRegVal);
		
		if (CheckAllBitsInMask(eMode, MCP23017_IntOnPinChange) == true) {
			ZeroAllBitsInMask(nRegVal, ePin); //Clear the interrupt control bit for compare to last pin value
		} else {
			SetAllBitsInMask(nRegVal, ePin); //Set the interrupt control bit for compare to default value
		}
		
		pDev->pI2C->pfI2CWriteUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_INTCONA + nRegAddrOffset, nRegVal);
		
		pDev->pI2C->pfI2CReadUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_DEFVALA + nRegAddrOffset, &nRegVal);
		
		if (CheckAllBitsInMask(eMode, MCP23017_IntCompareVal) == true) {
			SetAllBitsInMask(nRegVal, ePin); //Set the default value bit to compare to true value
		} else {
			ZeroAllBitsInMask(nRegVal, ePin); //Clear the default value bit to compare to false value
		}
		
		pDev->pI2C->pfI2CWriteUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_DEFVALA + nRegAddrOffset, nRegVal);
	} else { //Configured for output
		pDev->pI2C->pfI2CReadUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_IODIRA + nRegAddrOffset, &nRegVal);

		ZeroAllBitsInMask(nRegVal, ePin); //Clear the direction bit to set output

		pDev->pI2C->pfI2CWriteUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_IODIRA + nRegAddrOffset, nRegVal);
	}
	 
	return MCP23017_Success;
}

eMCP23017Returns_t MCP23017ReadInputByPin(sMCP23017Info_t *pDev, uint8_t nPinNum, bool *pbValue) {
	eMCP23017_Port_t ePort;
	eMCP23017_Pin_t ePin;
	
	if (MCP23017DecodePinNum(nPinNum, &ePort, &ePin) == false) {
		return MCP23017Fail_InvalidPin;
	}
	
	return MCP23017ReadInputByPort(pDev, ePort, ePin, pbValue);
}
 
eMCP23017Returns_t MCP23017ReadInputByPort(sMCP23017Info_t *pDev, eMCP23017_Port_t ePort, eMCP23017_Pin_t ePin, bool *pbValue) {
	uint8_t nRegAddrOffset, nRegVal;
	eI2CReturns_t eResult;
	 
	//Default mode is IOCON.Bank=0 Port Grouping
	if (ePort == MCP23017_PortA) {
		nRegAddrOffset = 0;
	} else {
		nRegAddrOffset = MCP23017_PortOffset;
	}
	 
	eResult = pDev->pI2C->pfI2CReadUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_GPIOA + nRegAddrOffset, &nRegVal);
	
	if (eResult != I2C_Success) {
		return MCP23017Fail_BusError;
	}

	if (CheckAllBitsInMask(nRegVal, ePin) == true) {
		*pbValue = true;
	} else {
		*pbValue = false;
	}
	
	return MCP23017_Success;
}

eMCP23017Returns_t MCP23017WriteOutputByPin(sMCP23017Info_t *pDev, uint8_t nPinNum, bool bLogicLevel) {
	eMCP23017_Port_t ePort;
	eMCP23017_Pin_t ePin;
	
	if (MCP23017DecodePinNum(nPinNum, &ePort, &ePin) == false) {
		return MCP23017Fail_InvalidPin;
	}
	
	return MCP23017WriteOutputByPort(pDev, ePort, ePin, bLogicLevel);
}

eMCP23017Returns_t MCP23017WriteOutputByPort(sMCP23017Info_t *pDev, eMCP23017_Port_t ePort, eMCP23017_Pin_t ePin, bool bLogicLevel) {
	uint8_t nRegAddrOffset, nRegVal;
	 
	//Default mode is IOCON.Bank=0 Port Grouping
	if (ePort == MCP23017_PortA) {
		nRegAddrOffset = 0;
	} else {
		nRegAddrOffset = MCP23017_PortOffset;
	}
	
	//See if the pin is an output pin
	pDev->pI2C->pfI2CReadUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_IODIRA + nRegAddrOffset, &nRegVal);
	
	if (CheckAllBitsInMask(nRegVal, ePin) == true) {
		return MCP23017Fail_WrongMode; //This pin is configured for inputs
	}
	
	//Its an output lets request the status
	pDev->pI2C->pfI2CReadUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_OLATA + nRegAddrOffset, &nRegVal);
		
	if (bLogicLevel == true) {
		SetAllBitsInMask(nRegVal, ePin); //Set the latch bit to request logic true
	} else {
		ZeroAllBitsInMask(nRegVal, ePin); //Clear the latch bit to request logic false
	}
	
	pDev->pI2C->pfI2CWriteUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_OLATA + nRegAddrOffset, nRegVal);
	
	return MCP23017_Success;
}

eMCP23017Returns_t MCP23017IsPendInterruptByPin(sMCP23017Info_t *pDev, uint8_t nPinNum, bool bClearInterrupt, bool *pbIsPendInt) {
	eMCP23017_Port_t ePort;
	eMCP23017_Pin_t ePin;
	
	if (MCP23017DecodePinNum(nPinNum, &ePort, &ePin) == false) {
		return MCP23017Fail_InvalidPin;
	}
	
	return MCP23017IsPendInterruptByPort(pDev, ePort, ePin, bClearInterrupt, pbIsPendInt);
}

eMCP23017Returns_t MCP23017IsPendInterruptByPort(sMCP23017Info_t *pDev, eMCP23017_Port_t ePort, eMCP23017_Pin_t ePin, bool bClearInterrupt, bool *pbIsPendInt) {
	uint8_t nRegAddrOffset, nRegVal;
	bool bRetVal;
	 
	//Default mode is IOCON.Bank=0 Port Grouping
	if (ePort == MCP23017_PortA) {
		nRegAddrOffset = 0;
	} else {
		nRegAddrOffset = MCP23017_PortOffset;
	}
	
	pDev->pI2C->pfI2CReadUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_INTFA + nRegAddrOffset, &nRegVal);
	
	if (CheckAllBitsInMask(nRegVal, ePin) == true) {
		bRetVal = true;
	} else {
		bRetVal = false;
	}
	
	if (bClearInterrupt == true) {
		ZeroAllBitsInMask(nRegVal, ePin);
		
		pDev->pI2C->pfI2CWriteUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_INTFA + nRegAddrOffset, nRegVal);
	}
	
	*pbIsPendInt = bRetVal;
	return MCP23017_Success;
}

eMCP23017Returns_t MCP23017InterruptValueByPin(sMCP23017Info_t *pDev, uint8_t nPinNum, bool *pbValue) {
	eMCP23017_Port_t ePort;
	eMCP23017_Pin_t ePin;
	
	if (MCP23017DecodePinNum(nPinNum, &ePort, &ePin) == false) {
		return MCP23017Fail_InvalidPin;
	}
	
	return MCP23017InterruptValueByPort(pDev, ePort, ePin, pbValue);
}

eMCP23017Returns_t MCP23017InterruptValueByPort(sMCP23017Info_t *pDev, eMCP23017_Port_t ePort, eMCP23017_Pin_t ePin, bool *pbValue) {
	uint8_t nRegAddrOffset, nRegVal;
	 
	//Default mode is IOCON.Bank=0 Port Grouping
	if (ePort == MCP23017_PortA) {
		nRegAddrOffset = 0;
	} else {
		nRegAddrOffset = MCP23017_PortOffset;
	}
	
	pDev->pI2C->pfI2CReadUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_INTCAPA + nRegAddrOffset, &nRegVal);
	
	if (CheckAllBitsInMask(nRegVal, ePin) == true) {
		*pbValue = true;
	} else {
		*pbValue = false;
	}
	
	return MCP23017_Success;
}


bool MCP23017DecodePinNum(uint8_t nPinNum, eMCP23017_Port_t *ePort, eMCP23017_Pin_t *ePin) {
	uint8_t nPinBit;
	
	if (nPinNum >= MCP23017_TOTALIOCNT) { //This is not a valid pin number
		(*ePort) = (eMCP23017_Port_t)0;
		(*ePin) = (eMCP23017_Pin_t)0;
		return false;
	}
	
	if (nPinNum >= MCP23017_PORTIOCNT) {//nPinNum is zero based indexing, PORTIOCNT is 1 based indexing
		nPinNum -= MCP23017_PORTIOCNT;
		
		(*ePort) = MCP23017_PortB;
	} else {
		(*ePort) = MCP23017_PortA;
	}
	
	nPinBit = 0x01;
	nPinBit <<= nPinNum;
	
	(*ePin) = (eMCP23017_Pin_t)nPinBit;
	
	return true;
}

