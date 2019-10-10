/**	@defgroup	mcp23017driver
	@brief		Driver for MCP23017 Digital I/O Expander
	@details	
		
*/

#ifndef __MCP23017DRIVER
	#define __MCP23017DRIVER

/***** Includes		*****/
	#include "CommonUtils.h"
	#include "I2CDevice.h"

/***** Definitions	*****/
	/**	@brief		Number of GPIO provided on the MCP23017 devices across all ports
		@ingroup	mcp23017driver
	*/
	#define MCP23017_TOTALIOCNT		16
	
	/**	@brief		Number of GPIO provided on the MCP23017 devices on a single port
		@ingroup	mcp23017driver
	*/
	#define MCP23017_PORTIOCNT		8

	/**	@brief		Values for the I2C address of the MCP23017
		@details	The MCP23017 has a base address value that all of the chips use.  In addition there are 
			3 pins that can add additional bits to that base address to allow up to 8 of these devices to be
			connected to the same I2C bus.
		@ingroup mcp23017driver
	*/
	enum eMCP23017Address_t {
		MCP23017_Base			= 0x20,	/**< Base address */
		MCP23017_A0				= 0x01,	/**< Bit added by address pin 0 */
		MCP23017_A1				= 0x02,	/**< Bit added by address pin 1 */
		MCP23017_A2				= 0x04,	/**< Bit added by address pin 2 */
	};
	
	/**	@brief		Register addresses when IOCON register Bank (bit 7) is 1
		@ingroup	mcp23017driver
	*/
	enum eMCP23017RegsBank1_t {
		MCP23017_IODIRA			= 0x00,	/**< Input Direction for GPIO 0-7: 1 input pin, 0 output pin */
		MCP23017_IPOLA			= 0x01,	/**< Input Poliarity for GPIO 0-7: 1 register bit is opposite logic from input pin, 0 register bit is same logic as input pin */
		MCP23017_GPINTENA		= 0x02,	/**< Interrupt on change for GPIO 0-7: 1 enables interupt A for changes on this pin, 0 disables interrupt A for changes on this pin */
		MCP23017_DEFVALA		= 0x03,	/**< Compare register for interrupt for GPIO 0-7, will raise interrupt is the pin value does not match this value.  Only used based on INTCON register setting. */
		MCP23017_INTCONA		= 0x04,	/**< Interrupt control for GPIO 0-7: 1 pin value compared to DEFVAL register, 0 pin value compared against previous value */
		MCP23017_IOCON			= 0x05,	/**< Configuration register */
		MCP23017_GPPUA			= 0x06,	/**< Pull up resistor for GPIO 0-7: 1 pull-up enabled, 0 pull-up disabled */
		MCP23017_INTFA			= 0x07,	/**< Interrupt flag register for GPIO 0-7: 1 pin caused interrupt, 0 no interrupt pending */
		MCP23017_INTCAPA		= 0x08,	/**< Interrupt captured value for GPIO 0-7: 1 pin was logic high, 0 pin was logic low */
		MCP23017_GPIOA			= 0x09,	/**< Logic level on the pin for GPIO 0-7: 1 pin is logic high, 0 pin is logic low */
		MCP23017_OLATA			= 0x0A,	/**< Output latch for GPIO 0-7: 1 set output to logic high, 0 set output to logic low */
		MCP23017_IODIRB			= 0x10,	/**< Input Direction for GPIO 8-16: 1 input pin, 0 output pin */
		MCP23017_IPOLB			= 0x11,	/**< Input Poliarity for GPIO 8-15: 1 register bit is opposite logic from input pin, 0 register bit is same logic as input pin */
		MCP23017_GPINTENB		= 0x12,	/**< Interrupt on change for GPIO 8-15: 1 enables interupt B for changes on this pin, 0 disables interrupt B for changes on this pin */
		MCP23017_DEFVALB		= 0x13,	/**< Compare register for interrupt for GPIO 8-15, will raise interrupt is the pin value does not match this value.  Only used based on INTCON register setting. */
		MCP23017_INTCONB		= 0x14,	/**< Interrupt control for GPIO 8-15: 1 pin value compared to DEFVAL register, 0 pin value compared against previous value */
		MCP23017_IOCONB			= 0x15,	/**< Dubplicate copy  of configuration register */
		MCP23017_GPPUB			= 0x16,	/**< Pull up resistor for GPIO 8-15: 1 pull-up enabled, 0 pull-up disabled */
		MCP23017_INTFB			= 0x17,	/**< Interrupt flag register for GPIO 8-15: 1 pin caused interrupt, 0 no interrupt pending */
		MCP23017_INTCAPB		= 0x18,	/**< Interrupt captured value for GPIO 8-15: 1 pin was logic high, 0 pin was logic low */
		MCP23017_GPIOB			= 0x19,	/**< Logic level on the pin for GPIO 8-15: 1 pin is logic high, 0 pin is logic low */
		MCP23017_OLATB			= 0x1A,	/**< Output latch for GPIO 8-15: 1 set output to logic high, 0 set output to logic low */
		
		MCP23017_PortOffset		= 0x10, /**< Offset between port A and port B settings */
	};
	
	/**	@brief		Register addresses when IOCON register Bank (bit 7) is 0
		@ingroup	mcp23017driver
	*/
	enum eMCP23017RegsBank0_t {
		MCP23017_B0_IODIRA		= 0x00,	/**< Input Direction for GPIO 0-7: 1 input pin, 0 output pin */
		MCP23017_B0_IODIRB		= 0x01,	/**< Input Direction for GPIO 8-16: 1 input pin, 0 output pin */
		MCP23017_B0_IPOLA		= 0x02,	/**< Input Poliarity for GPIO 0-7: 1 register bit is opposite logic from input pin, 0 register bit is same logic as input pin */
		MCP23017_B0_IPOLB		= 0x03,	/**< Input Poliarity for GPIO 8-15: 1 register bit is opposite logic from input pin, 0 register bit is same logic as input pin */
		MCP23017_B0_GPINTENA	= 0x04,	/**< Interrupt on change for GPIO 0-7: 1 enables interupt A for changes on this pin, 0 disables interrupt A for changes on this pin */
		MCP23017_B0_GPINTENB	= 0x05,	/**< Interrupt on change for GPIO 8-15: 1 enables interupt B for changes on this pin, 0 disables interrupt B for changes on this pin */
		MCP23017_B0_DEFVALA		= 0x06,	/**< Compare register for interrupt for GPIO 0-7, will raise interrupt is the pin value does not match this value.  Only used based on INTCON register setting. */
		MCP23017_B0_DEFVALB		= 0x07,	/**< Compare register for interrupt for GPIO 8-15, will raise interrupt is the pin value does not match this value.  Only used based on INTCON register setting. */
		MCP23017_B0_INTCONA		= 0x08,	/**< Interrupt control for GPIO 0-7: 1 pin value compared to DEFVAL register, 0 pin value compared against previous value */
		MCP23017_B0_INTCONB		= 0x09,	/**< Interrupt control for GPIO 8-15: 1 pin value compared to DEFVAL register, 0 pin value compared against previous value */
		MCP23017_B0_IOCON		= 0x0A,	/**< Configuration register */
		MCP23017_B0_IOCONB		= 0x0B,	/**< Dubplicate copy  of configuration register */
		MCP23017_B0_GPPUA		= 0x0C,	/**< Pull up resistor for GPIO 0-7: 1 pull-up enabled, 0 pull-up disabled */
		MCP23017_B0_GPPUB		= 0x0D,	/**< Pull up resistor for GPIO 8-15: 1 pull-up enabled, 0 pull-up disabled */
		MCP23017_B0_INTFA		= 0x0E,	/**< Interrupt flag register for GPIO 0-7: 1 pin caused interrupt, 0 no interrupt pending */
		MCP23017_B0_INTFB		= 0x0F,	/**< Interrupt flag register for GPIO 8-15: 1 pin caused interrupt, 0 no interrupt pending */
		MCP23017_B0_INTCAPA		= 0x10,	/**< Interrupt captured value for GPIO 0-7: 1 pin was logic high, 0 pin was logic low */
		MCP23017_B0_INTCAPB		= 0x11,	/**< Interrupt captured value for GPIO 7-15: 1 pin was logic high, 0 pin was logic low */
		MCP23017_B0_GPIOA		= 0x12,	/**< Logic level on the pin for GPIO 0-7: 1 pin is logic high, 0 pin is logic low */
		MCP23017_B0_GPIOB		= 0x13,	/**< Logic level on the pin for GPIO 7-15: 1 pin is logic high, 0 pin is logic low */
		MCP23017_B0_OLATA		= 0x14,	/**< Output latch for GPIO 0-7: 1 set output to logic high, 0 set output to logic low */
		MCP23017_B0_OLATB		= 0x15,	/**< Output latch for GPIO 8-15: 1 set output to logic high, 0 set output to logic low */
	};
	
	/**	@brief		Masks for each pin in the MCP23017 registers
		@details	All registers of the MCP23017 except IOCON have bits for each of the pins in an IO port.
			These values are the individual bits for each IO, they can be used as a mask to read a specific 
			setting or bit ORed together to set the values.
		@ingroup	mcp23017driver
	*/
	enum eMCP23017_Pin_t {
		MCP23017_Pin_0			= 0x01,	/**< Pin 0 of the port (GPIO 0 or 8) */
		MCP23017_Pin_1			= 0x02,	/**< Pin 1 of the port (GPIO 1 or 9) */
		MCP23017_Pin_2			= 0x04,	/**< Pin 2 of the port (GPIO 2 or 10) */
		MCP23017_Pin_3			= 0x08,	/**< Pin 3 of the port (GPIO 3 or 11) */
		MCP23017_Pin_4			= 0x10,	/**< Pin 4 of the port (GPIO 4 or 12) */
		MCP23017_Pin_5			= 0x20,	/**< Pin 5 of the port (GPIO 5 or 13) */
		MCP23017_Pin_6			= 0x40,	/**< Pin 6 of the port (GPIO 6 or 14) */
		MCP23017_Pin_7			= 0x80,	/**< Pin 7 of the port (GPIO 7 or 15) */
	};
	
	/**	@brief		Values to specify the two ports of GPIO available on the MCP23017
		@ingroup	mcp23017driver
	*/
	enum eMCP23017_Port_t {
		MCP23017_PortA			= 0x00,
		MCP23017_PortB			= 0x08,
	};
	
	/**	@brief		Flags to specify the modes that a GPIO should use
		@ingroup	mcp23017driver
	*/
	enum eMCP23017_PinMode_t {
		MCP23017_Input			= 0x01,
		MCP23017_Output			= 0x02,
		MCP23017_PullUp			= 0x04,
		MCP23017_ReverseLogic	= 0x08,
		MCP23017_IntOnPinChange	= 0x10,
		MCP23017_IntOnCompare	= 0x20,
		MCP23017_IntCompareVal	= 0x40,
		
	};
	
	enum eMCP23017_RegIOCON_t {
		MCP23017_BANK			= 0x80,	/**< Controls register addressing: 1 pins separated into banks, 0 registers are sequential */
		MCP23017_MIRROR			= 0x40,	/**< Controls interrupt pin connection: 1 pins are internally connected, 0 INTA is associated with Port A and INTB is associated with Port B */
		MCP23017_SEQOP			= 0x20,	/**< Sequential operations mode: 1 disabled so address pointer does not increment, 0 enabled so address pointer increments */
		MCP23017_DISSLW			= 0x10,	/**< Slew Rate control: 1 slew rate disabled, 0 slew rate enabled */
		MCP23017_HAEN			= 0x08,	/**< Hardware address enable: 1 enables address pins, 0 disables address pins */
		MCP23017_ODR			= 0x04,	/**< Control interrupt outputs: 1 open-drain output, 0 active driver output */
		MCP23017_INTPOL			= 0x02,	/**< Polarity of interrupt pins for active driver ouput: 1 Active high, 0 Active low */
	};
	
	class cMCP23017_t : cI2CDevice_t {
		public:
		/**	@brief		Constructor to prepare the class
		*/
		cMCP23017_t(eMCP23017Address_t eAddr, TwoWire *pI2CWire);
		
		/**	@brief		Destructor to release any allocated resources
		*/
		~cMCP23017_t();
		
		/**	@brief		Prepare all hardware for use
			@details	This must be called after the I2C bus has been initialized with a call to its 
				begin() member function.
			@return		True if the operation was completed correctly, false on any error
		*/
		bool Begin();
		
		/**	@brief		Set the mode of operation for one of the GPIO pins
			@param		nPinNum		Number of the pin to read; 0-7 for port A, 8-15 for port B
			@param		eMode		Bitwise OR of all mode flahs to set for this pin
			@return		True if the flags are valid and were correctly applied, false on any error
		*/
		bool PinMode(uint8_t nPinNum, eMCP23017_PinMode_t eMode);
		
		/**	@brief		Set the mode of operation for one of the GPIO pins
			@param		ePort		Which port the pin is on, Port A is 0-7 and Port B is 8-15
			@param		ePin		Which pin in the port is to be set
			@param		eMode		Bitwise OR of all mode flahs to set for this pin
			@return		True if the flags are valid and were correctly applied, false on any error
		*/
		bool PinMode(eMCP23017_Port_t ePort, eMCP23017_Pin_t ePin, eMCP23017_PinMode_t eMode);
		
		/**	@brief		Read the current logic level of the pin
			@param		nPinNum		Number of the pin to read; 0-7 for port A, 8-15 for port B
			@return		True if the operation was completed correctly, false on any error
		*/
		bool Read(uint8_t nPinNum);
		
		/**	@brief		Read the current logic level of the pin
			@param		ePort		Which port the pin is on, Port A is 0-7 and Port B is 8-15
			@param		ePin		Which pin in the port is to be set
			@return		True if the operation was completed correctly, false on any error
		*/
		bool Read(eMCP23017_Port_t ePort, eMCP23017_Pin_t ePin);
		
		/**	@brief		Write a logic level to a pin configured for output
			@param		nPinNum		Number of the pin to read; 0-7 for port A, 8-15 for port B
			@param		bLogicLevel	The logic level to apply to the output pin
			@return		True if the operation was completed correctly, false on any error
		*/
		bool Write(uint8_t nPinNum, bool bLogicLevel);
		
		/**	@brief		Write a logic level to a pin configured for output
			@param		ePort		Which port the pin is on, Port A is 0-7 and Port B is 8-15
			@param		ePin		Which pin in the port is to be set
			@param		bLogicLevel	The logic level to apply to the output pin
			@return		True if the operation was completed correctly, false on any error
		*/
		bool Write(eMCP23017_Port_t ePort, eMCP23017_Pin_t ePin, bool bLogicLevel);
		
		/**	@brief		Check if an interrupt is pending for a specified pin
			@param		nPinNum		Number of the pin to read; 0-7 for port A, 8-15 for port B
			@param		bClearInterrupt	True to clear the interrupt flag, false to preserve it
			@return		True if an interrupt is pending for this pin, false if it does not
		*/
		bool IsPendInterrupt(uint8_t nPinNum, bool bClearInterrupt);
		
		/**	@brief		Check if an interrupt is pending for a specified pin
			@param		ePort			Which port the pin is on, Port A is 0-7 and Port B is 8-15
			@param		ePin			Which pin in the port is to be tested
			@param		bClearInterrupt	True to clear the interrupt flag, false to preserve it
			@return		True if an interrupt is pending for this pin, false if it does not
		*/
		bool IsPendInterrupt(eMCP23017_Port_t ePort, eMCP23017_Pin_t ePin, bool bClearInterrupt);
		
		/**	@brief		Retrieve the status of the pin when the interupt was recorded
			@param		nPinNum		Number of the pin to read; 0-7 for port A, 8-15 for port B
			@return		True if the operation was completed correctly, false on any error
		*/
		bool InterruptValue(uint8_t nPinNum);
		
		/**	@brief		Retrieve the status of the pin when the interupt was recorded
			@param		ePort		Which port the pin is on, Port A is 0-7 and Port B is 8-15
			@param		ePin		Which pin in the port is to be tested
			@return		True if the operation was completed correctly, false on any error
		*/
		bool InterruptValue(eMCP23017_Port_t ePort, eMCP23017_Pin_t ePin);
		
		protected:
		bool DecodePinNum(uint8_t nPinNum, eMCP23017_Port_t *ePort, eMCP23017_Pin_t *ePin);
		
		private:
		
	};

/***** Constants	*****/


/***** Globals		*****/


/***** Prototypes 	*****/


/***** Functions	*****/

cMCP23017_t::cMCP23017_t(eMCP23017Address_t eAddr, TwoWire *pI2CWire) : cI2CDevice_t((uint8_t)(eAddr | MCP23017_Base), pI2CWire) {
	//Can't prepeare the hardware yet, wait for the begin call to do that 
	return;
}
 
cMCP23017_t::~cMCP23017_t() {
	//No resources allocated, no cleanup necessary
	 
	return;
}
 
bool cMCP23017_t::Begin() {
	uint8_t nIOConVal, nRegVal, nCtr;
	
	//Set the initial configuration for the IO Expander.  Listing a bit sets it to 1, excluing it sets it to zero.
	nIOConVal = MCP23017_BANK | MCP23017_SEQOP | MCP23017_HAEN;
	I2CWriteUint8Reg(cnI2CAddr, MCP23017_B0_IOCON, nIOConVal); //Write to both possible IOCON addresses just to be sure
	I2CWriteUint8Reg(cnI2CAddr, MCP23017_IOCON, nIOConVal);
	
	nRegVal = I2CReadUint8Reg(cnI2CAddr, MCP23017_IOCON);
	if (nRegVal != nIOConVal) {
		return false;
	}
	
	//Set all pins to Inputs with pull ups
	I2CWriteUint8Reg(cnI2CAddr, MCP23017_IODIRA, 0xFF);
	I2CWriteUint8Reg(cnI2CAddr, MCP23017_IODIRB, 0xFF);
	I2CWriteUint8Reg(cnI2CAddr, MCP23017_IPOLA, 0x00);
	I2CWriteUint8Reg(cnI2CAddr, MCP23017_IPOLA, 0x00);
	I2CWriteUint8Reg(cnI2CAddr, MCP23017_GPPUA, 0xFF);
	I2CWriteUint8Reg(cnI2CAddr, MCP23017_GPPUB, 0xFF);
	I2CWriteUint8Reg(cnI2CAddr, MCP23017_OLATA, 0x00); //We aren't outputs by default, but have a predictable level when set
	I2CWriteUint8Reg(cnI2CAddr, MCP23017_OLATA, 0x00);
	
	return true;
}

bool cMCP23017_t::PinMode(uint8_t nPinNum, eMCP23017_PinMode_t eMode) {
	eMCP23017_Port_t ePort;
	eMCP23017_Pin_t ePin;
	
	if (DecodePinNum(nPinNum, &ePort, &ePin) == false) {
		return false;
	}
	
	return PinMode(ePort, ePin, eMode);
}

bool cMCP23017_t::PinMode(eMCP23017_Port_t ePort, eMCP23017_Pin_t ePin, eMCP23017_PinMode_t eMode) {
	uint8_t nRegAddrOffset, nRegVal;
	 
	//Default mode is IOCON.Bank=0 Port Grouping
	if (ePort == MCP23017_PortA) {
		nRegAddrOffset = 0;
	} else {
		nRegAddrOffset = MCP23017_PortOffset;
	}
	
	if ((CheckAllBitsInMask(eMode, MCP23017_Input) == true) && (CheckAllBitsInMask(eMode, MCP23017_Output) == true)) { //Both input and output is invalid
		return false;
	} else if (CheckAllBitsInMask(eMode, MCP23017_Input) == true) { //Configured for input
		nRegVal = I2CReadUint8Reg(cnI2CAddr, MCP23017_IODIRA + nRegAddrOffset);

		SetAllBitsInMask(nRegVal, ePin); //Set the direction bit to set input
		
		I2CWriteUint8Reg(cnI2CAddr, MCP23017_IODIRA + nRegAddrOffset, nRegVal);

		//Check and apply pull up
		nRegVal = I2CReadUint8Reg(cnI2CAddr, MCP23017_GPPUA + nRegAddrOffset);
		
		if (CheckAllBitsInMask(eMode, MCP23017_PullUp) == true) {
			SetAllBitsInMask(nRegVal, ePin); //Set the pull up bit to include a pull-up
		} else {
			ZeroAllBitsInMask(nRegVal, ePin); //Clear the pull up bit to remove the pull-up
		}
		
		I2CWriteUint8Reg(cnI2CAddr, MCP23017_GPPUA + nRegAddrOffset, nRegVal);
		
		//Check and apply reverse logic
		nRegVal = I2CReadUint8Reg(cnI2CAddr, MCP23017_IPOLA + nRegAddrOffset);
		
		if (CheckAllBitsInMask(eMode, MCP23017_ReverseLogic) == true) {
			SetAllBitsInMask(nRegVal, ePin); //Set the polarity bit to have reverse logic
		} else {
			ZeroAllBitsInMask(nRegVal, ePin); //Clear the polarity bit to have normal logic
		}
		
		I2CWriteUint8Reg(cnI2CAddr, MCP23017_IPOLA + nRegAddrOffset, nRegVal);
		
		//Check and apply interrupt
		nRegVal = I2CReadUint8Reg(cnI2CAddr, MCP23017_GPINTENA + nRegAddrOffset);
		
		if (CheckAllBitsInMask(eMode, MCP23017_IntOnPinChange | MCP23017_IntOnCompare) == true) {
			SetAllBitsInMask(nRegVal, ePin); //Set the polarity bit to have reverse logic
		} else {
			ZeroAllBitsInMask(nRegVal, ePin); //Clear the polarity bit to have normal logic
		}
		
		I2CWriteUint8Reg(cnI2CAddr, MCP23017_GPINTENA + nRegAddrOffset, nRegVal);
		
		nRegVal = I2CReadUint8Reg(cnI2CAddr, MCP23017_INTCONA + nRegAddrOffset);
		
		if (CheckAllBitsInMask(eMode, MCP23017_IntOnPinChange) == true) {
			ZeroAllBitsInMask(nRegVal, ePin); //Clear the interrupt control bit for compare to last pin value
		} else {
			SetAllBitsInMask(nRegVal, ePin); //Set the interrupt control bit for compare to default value
		}
		
		I2CWriteUint8Reg(cnI2CAddr, MCP23017_INTCONA + nRegAddrOffset, nRegVal);
		
		nRegVal = I2CReadUint8Reg(cnI2CAddr, MCP23017_DEFVALA + nRegAddrOffset);
		
		if (CheckAllBitsInMask(eMode, MCP23017_IntCompareVal) == true) {
			SetAllBitsInMask(nRegVal, ePin); //Set the default value bit to compare to true value
		} else {
			ZeroAllBitsInMask(nRegVal, ePin); //Clear the default value bit to compare to false value
		}
		
		I2CWriteUint8Reg(cnI2CAddr, MCP23017_DEFVALA + nRegAddrOffset, nRegVal);
	} else { //Configured for output
		nRegVal = I2CReadUint8Reg(cnI2CAddr, MCP23017_IODIRA + nRegAddrOffset);

		ZeroAllBitsInMask(nRegVal, ePin); //Clear the direction bit to set output

		I2CWriteUint8Reg(cnI2CAddr, MCP23017_IODIRA + nRegAddrOffset, nRegVal);
	}
	 
	return true;
}
 
bool cMCP23017_t::Read(uint8_t nPinNum) {
	eMCP23017_Port_t ePort;
	eMCP23017_Pin_t ePin;
	
	if (DecodePinNum(nPinNum, &ePort, &ePin) == false) {
		return false;
	}
	
	return Read(ePort, ePin);
}
 
bool cMCP23017_t::Read(eMCP23017_Port_t ePort, eMCP23017_Pin_t ePin) {
	uint8_t nRegAddrOffset, nRegVal;
	 
	//Default mode is IOCON.Bank=0 Port Grouping
	if (ePort == MCP23017_PortA) {
		nRegAddrOffset = 0;
	} else {
		nRegAddrOffset = MCP23017_PortOffset;
	}
	 
	nRegVal = I2CReadUint8Reg(cnI2CAddr, MCP23017_GPIOA + nRegAddrOffset);

	if (CheckAllBitsInMask(nRegVal, ePin) == true) {
		return true;
	} else {
		return false;
	}
}

bool cMCP23017_t::Write(eMCP23017_Port_t ePort, eMCP23017_Pin_t ePin, bool bLogicLevel) {
	uint8_t nRegAddrOffset, nRegVal;
	 
	//Default mode is IOCON.Bank=0 Port Grouping
	if (ePort == MCP23017_PortA) {
		nRegAddrOffset = 0;
	} else {
		nRegAddrOffset = MCP23017_PortOffset;
	}
	
	//See if the pin is an output pin
	nRegVal = I2CReadUint8Reg(cnI2CAddr, MCP23017_IODIRA + nRegAddrOffset);
	
	if (CheckAllBitsInMask(nRegVal, ePin) == true) {
		return false; //This pin is configured for inputs
	}
	
	//Its an output lets request the status
	nRegVal = I2CReadUint8Reg(cnI2CAddr, MCP23017_OLATB + nRegAddrOffset);
		
	if (bLogicLevel == true) {
		SetAllBitsInMask(nRegVal, ePin); //Set the latch bit to request logic true
	} else {
		ZeroAllBitsInMask(nRegVal, ePin); //Clear the latch bit to request logic false
	}
	
	I2CWriteUint8Reg(cnI2CAddr, MCP23017_OLATB + nRegAddrOffset, nRegVal);
	
	return true;
}

bool cMCP23017_t::IsPendInterrupt(uint8_t nPinNum, bool bClearInterrupt = true) {
	eMCP23017_Port_t ePort;
	eMCP23017_Pin_t ePin;
	
	if (DecodePinNum(nPinNum, &ePort, &ePin) == false) {
		return false;
	}
	
	return IsPendInterrupt(ePort, ePin, bClearInterrupt);
}

bool cMCP23017_t::IsPendInterrupt(eMCP23017_Port_t ePort, eMCP23017_Pin_t ePin, bool bClearInterrupt = true) {
	uint8_t nRegAddrOffset, nRegVal;
	bool bRetVal;
	 
	//Default mode is IOCON.Bank=0 Port Grouping
	if (ePort == MCP23017_PortA) {
		nRegAddrOffset = 0;
	} else {
		nRegAddrOffset = MCP23017_PortOffset;
	}
	
	nRegVal = I2CReadUint8Reg(cnI2CAddr, MCP23017_INTFA + nRegAddrOffset);
	
	if (CheckAllBitsInMask(nRegVal, ePin) == true) {
		bRetVal = true;
	} else {
		bRetVal = false;
	}
	
	if (bClearInterrupt == true) {
		ZeroAllBitsInMask(nRegVal, ePin);
		
		I2CWriteUint8Reg(cnI2CAddr, MCP23017_INTFA + nRegAddrOffset, nRegVal);
	}
	
	return bRetVal;
}

bool cMCP23017_t::InterruptValue(uint8_t nPinNum) {
	eMCP23017_Port_t ePort;
	eMCP23017_Pin_t ePin;
	
	if (DecodePinNum(nPinNum, &ePort, &ePin) == false) {
		return false;
	}
	
	return InterruptValue(ePort, ePin);
}

bool cMCP23017_t::InterruptValue(eMCP23017_Port_t ePort, eMCP23017_Pin_t ePin) {
	uint8_t nRegAddrOffset, nRegVal;
	 
	//Default mode is IOCON.Bank=0 Port Grouping
	if (ePort == MCP23017_PortA) {
		nRegAddrOffset = 0;
	} else {
		nRegAddrOffset = MCP23017_PortOffset;
	}
	
	nRegVal = I2CReadUint8Reg(cnI2CAddr, MCP23017_INTCAPA + nRegAddrOffset);
	
	if (CheckAllBitsInMask(nRegVal, ePin) == true) {
		return true;
	} else {
		return false;
	}
}

bool cMCP23017_t::DecodePinNum(uint8_t nPinNum, eMCP23017_Port_t *ePort, eMCP23017_Pin_t *ePin) {
	uint8_t nPinBit;
	
	if (nPinNum >= MCP23017_TOTALIOCNT) { //This is not a valid pin number
		(*ePort) = (eMCP23017_Port_t)0;
		(*ePin) = (eMCP23017_Pin_t)0;
		return false;
	}
	
	if (nPinNum > MCP23017_PORTIOCNT) {
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

#endif

