/**	@defgroup	mcp23017driver
	@brief		
	@details	v0.1
	#Description
	
	#File Information
		File:	MCP23017Driver.h
		Author:	J. Beighel
		Date:	11-30-2020
*/

#ifndef __MCP23017DRIVER_H
	#define __MCP23017DRIVER_H

/*****	Includes	*****/
	#include "CommonUtils.h"
	#include "I2CGeneralInterface.h"

/*****	Constants	*****/
	#define MCP23017_I2CCAPS		(I2CCap_ReadUint8Reg | I2CCap_WriteUint8Reg)

	/**	@brief		Number of GPIO provided on the MCP23017 devices across all ports
		@ingroup	mcp23017driver
	*/
	#define MCP23017_TOTALIOCNT		16
	
	/**	@brief		Number of GPIO provided on the MCP23017 devices on a single port
		@ingroup	mcp23017driver
	*/
	#define MCP23017_PORTIOCNT		8

/*****	Definitions	*****/
	typedef enum eMCP23017Returns_t {
		MCP23017Warn_Unknown	= -1,	/**< An unknown but recoverable error occurred */
		MCP23017_Success		= 0,	/**< Operation completed successfully */
		MCP23017Fail_Unknown	= 1,	/**< An unknown and unrecoverable occured */
		MCP23017Fail_InvalidPin	= 2,	/**< Pin requested is invalid */
		MCP23017Fail_InvalidMode= 3,	/**< Pin mode requested is invalid */
		MCP23017Fail_WrongMode	= 4,	/**< Output requested for Input pin*/
		MCP23017Fail_BusError	= 5,	/**< Error reported from the I2C interface */
	} eMCP23017Returns_t;

	/**	@brief		Values for the I2C address of the MCP23017
		@details	The MCP23017 has a base address value that all of the chips use.  In addition there are 
			3 pins that can add additional bits to that base address to allow up to 8 of these devices to be
			connected to the same I2C bus.
		@ingroup mcp23017driver
	*/
	typedef enum eMCP23017Address_t {
		MCP23017_Base			= 0x20,	/**< Base address */
		MCP23017_A0				= 0x01,	/**< Bit added by address pin 0 */
		MCP23017_A1				= 0x02,	/**< Bit added by address pin 1 */
		MCP23017_A2				= 0x04,	/**< Bit added by address pin 2 */
	} eMCP23017Address_t;
	
	/**	@brief		Register addresses when IOCON register Bank (bit 7) is 1
		@ingroup	mcp23017driver
	*/
	typedef enum eMCP23017RegsBank1_t {
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
	} eMCP23017RegsBank1_t;
	
	/**	@brief		Register addresses when IOCON register Bank (bit 7) is 0
		@ingroup	mcp23017driver
	*/
	typedef enum eMCP23017RegsBank0_t {
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
	} eMCP23017RegsBank0_t;
	
	/**	@brief		Masks for each pin in the MCP23017 registers
		@details	All registers of the MCP23017 except IOCON have bits for each of the pins in an IO port.
			These values are the individual bits for each IO, they can be used as a mask to read a specific 
			setting or bit ORed together to set the values.
		@ingroup	mcp23017driver
	*/
	typedef enum eMCP23017_Pin_t {
		MCP23017_Pin_0			= 0x01,	/**< Pin 0 of the port (GPIO 0 or 8) */
		MCP23017_Pin_1			= 0x02,	/**< Pin 1 of the port (GPIO 1 or 9) */
		MCP23017_Pin_2			= 0x04,	/**< Pin 2 of the port (GPIO 2 or 10) */
		MCP23017_Pin_3			= 0x08,	/**< Pin 3 of the port (GPIO 3 or 11) */
		MCP23017_Pin_4			= 0x10,	/**< Pin 4 of the port (GPIO 4 or 12) */
		MCP23017_Pin_5			= 0x20,	/**< Pin 5 of the port (GPIO 5 or 13) */
		MCP23017_Pin_6			= 0x40,	/**< Pin 6 of the port (GPIO 6 or 14) */
		MCP23017_Pin_7			= 0x80,	/**< Pin 7 of the port (GPIO 7 or 15) */
		
		MCP23017_Pin_None		= 0x00,
		MCP23017_Pin_All		= 0xFF,	/**< All 8 pins of the port */
	} eMCP23017_Pin_t;
	
	/**	@brief		Values to specify the two ports of GPIO available on the MCP23017
		@ingroup	mcp23017driver
	*/
	typedef enum eMCP23017_Port_t {
		MCP23017_PortA			= 0x00,
		MCP23017_PortB			= 0x08,
	} eMCP23017_Port_t;
	
	/**	@brief		Flags to specify the modes that a GPIO should use
		@ingroup	mcp23017driver
	*/
	typedef enum eMCP23017_PinMode_t {
		MCP23017_Input			= 0x01,
		MCP23017_Output			= 0x02,
		MCP23017_PullUp			= 0x04,
		MCP23017_ReverseLogic	= 0x08,
		MCP23017_IntOnPinChange	= 0x10,
		MCP23017_IntOnCompare	= 0x20,
		MCP23017_IntCompareVal	= 0x40,
	} eMCP23017_PinMode_t;
	
	enum eMCP23017_RegIOCON_t {
		MCP23017_BANK			= 0x80,	/**< Controls register addressing: 1 pins separated into banks, 0 registers are sequential */
		MCP23017_MIRROR			= 0x40,	/**< Controls interrupt pin connection: 1 pins are internally connected, 0 INTA is associated with Port A and INTB is associated with Port B */
		MCP23017_SEQOP			= 0x20,	/**< Sequential operations mode: 1 disabled so address pointer does not increment, 0 enabled so address pointer increments */
		MCP23017_DISSLW			= 0x10,	/**< Slew Rate control: 1 slew rate disabled, 0 slew rate enabled */
		MCP23017_HAEN			= 0x08,	/**< Hardware address enable: 1 enables address pins, 0 disables address pins */
		MCP23017_ODR			= 0x04,	/**< Control interrupt outputs: 1 open-drain output, 0 active driver output */
		MCP23017_INTPOL			= 0x02,	/**< Polarity of interrupt pins for active driver ouput: 1 Active high, 0 Active low */
	};

	typedef struct sMCP23017Info_t {
		uint8_t nAddr;
		sI2CIface_t *pI2C;
	} sMCP23017Info_t;

/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	eMCP23017Returns_t MCP23017Initialize(sMCP23017Info_t *pDev, eMCP23017Address_t eAddr, sI2CIface_t *pI2CIface);
	
	/**	@brief		Set the mode of operation for one of the GPIO pins
		@param		pDev		The peripheral device object to use
		@param		nPinNum		Number of the pin to read; 0-7 for port A, 8-15 for port B
		@param		eMode		Bitwise OR of all mode flahs to set for this pin
		@return		MCP23017_Success if the flags are valid and were correctly applied, or
						a code  describing any failure
		@ingroup	mcp23017driver
	*/
	eMCP23017Returns_t MCP23017PinModeByPin(sMCP23017Info_t *pDev, uint8_t nPinNum, eMCP23017_PinMode_t eMode);
	
	/**	@brief		Set the mode of operation for one of the GPIO pins
		@param		pDev		The peripheral device object to use
		@param		ePort		Which port the pin is on, Port A is 0-7 and Port B is 8-15
		@param		ePin		Which pin in the port is to be set
		@param		eMode		Bitwise OR of all mode flahs to set for this pin
		@return		MCP23017_Success if the flags are valid and were correctly applied, or
						a code  describing any failure error
		@ingroup	mcp23017driver
	*/
	eMCP23017Returns_t MCP23017PinModeByPort(sMCP23017Info_t *pDev, eMCP23017_Port_t ePort, eMCP23017_Pin_t ePin, eMCP23017_PinMode_t eMode);
	
	/**	@brief		Read the current logic level of the pin
		@param		pDev		The peripheral device object to use
		@param		nPinNum		Number of the pin to read; 0-7 for port A, 8-15 for port B
		@param		pbValue		Returns the value of hte pin, True for logic high
		@return		MCP23017_Success if the flags are valid and were correctly applied, or
					a code  describing any failure
		@ingroup	mcp23017driver
	*/
	eMCP23017Returns_t MCP23017ReadInputByPin(sMCP23017Info_t *pDev, uint8_t nPinNum, bool *pbValue);
	
	/**	@brief		Read the current logic level of the pin
		@param		pDev		The peripheral device object to use
		@param		ePort		Which port the pin is on, Port A is 0-7 and Port B is 8-15
		@param		ePin		Which pin in the port is to be set
		@param		pbValue		Returns the value of hte pin, True for logic high
		@return		MCP23017_Success if the flags are valid and were correctly applied, or
						a code  describing any failure
		@ingroup	mcp23017driver
	*/
	eMCP23017Returns_t MCP23017ReadInputByPort(sMCP23017Info_t *pDev, eMCP23017_Port_t ePort, eMCP23017_Pin_t ePin, bool *pbValue);
	
	/**	@brief		Write a logic level to a pin configured for output
		@param		pDev		The peripheral device object to use
		@param		nPinNum		Number of the pin to read; 0-7 for port A, 8-15 for port B
		@param		bLogicLevel	The logic level to apply to the output pin
		@return		MCP23017_Success if the flags are valid and were correctly applied, or
						a code  describing any failure
		@ingroup	mcp23017driver
	*/
	eMCP23017Returns_t MCP23017WriteOutputByPin(sMCP23017Info_t *pDev, uint8_t nPinNum, bool bLogicLevel);
	
	/**	@brief		Write a logic level to a pin configured for output
		@param		pDev		The peripheral device object to use
		@param		ePort		Which port the pin is on, Port A is 0-7 and Port B is 8-15
		@param		ePin		Which pin in the port is to be set
		@param		bLogicLevel	The logic level to apply to the output pin
		@return		MCP23017_Success if the flags are valid and were correctly applied, or
						a code  describing any failure
		@ingroup	mcp23017driver
	*/
	eMCP23017Returns_t MCP23017WriteOutputByPort(sMCP23017Info_t *pDev, eMCP23017_Port_t ePort, eMCP23017_Pin_t ePin, bool bLogicLevel);
	
	/**	@brief		Check if an interrupt is pending for a specified pin
		@param		pDev		The peripheral device object to use
		@param		nPinNum		Number of the pin to read; 0-7 for port A, 8-15 for port B
		@param		bClearInterrupt	True to clear the interrupt flag, false to preserve it
		@return		MCP23017_Success if the flags are valid and were correctly applied, or
						a code  describing any failure
		@ingroup	mcp23017driver
	*/
	eMCP23017Returns_t MCP23017IsPendInterruptByPin(sMCP23017Info_t *pDev, uint8_t nPinNum, bool bClearInterrupt, bool *pbIsPendInt);
	
	/**	@brief		Check if an interrupt is pending for a specified pin
		@param		pDev			The peripheral device object to use
		@param		ePort			Which port the pin is on, Port A is 0-7 and Port B is 8-15
		@param		ePin			Which pin in the port is to be tested
		@param		bClearInterrupt	True to clear the interrupt flag, false to preserve it
		@return		MCP23017_Success if the flags are valid and were correctly applied, or
						a code  describing any failure
		@ingroup	mcp23017driver
	*/
	eMCP23017Returns_t MCP23017IsPendInterruptByPort(sMCP23017Info_t *pDev, eMCP23017_Port_t ePort, eMCP23017_Pin_t ePin, bool bClearInterrupt, bool *pbIsPendInt);
	
	/**	@brief		Retrieve the status of the pin when the interupt was recorded
		@param		pDev		The peripheral device object to use
		@param		nPinNum		Number of the pin to read; 0-7 for port A, 8-15 for port B
		@param		pbValue		Returns the value of the pin when causing interrupt
		@return		MCP23017_Success if the flags are valid and were correctly applied, or
						a code  describing any failure
		@ingroup	mcp23017driver
	*/
	eMCP23017Returns_t MCP23017InterruptValueByPin(sMCP23017Info_t *pDev, uint8_t nPinNum, bool *pbValue);
	
	/**	@brief		Retrieve the status of the pin when the interupt was recorded
		@param		pDev		The peripheral device object to use
		@param		ePort		Which port the pin is on, Port A is 0-7 and Port B is 8-15
		@param		ePin		Which pin in the port is to be tested
		@param		pbValue		Returns the value of the pin causing the interrupt
		@return		True if the operation was completed correctly, false on any error
		@ingroup	mcp23017driver
	*/
	eMCP23017Returns_t MCP23017InterruptValueByPort(sMCP23017Info_t *pDev, eMCP23017_Port_t ePort, eMCP23017_Pin_t ePin, bool *pbValue);

/*****	Functions	*****/


#endif

