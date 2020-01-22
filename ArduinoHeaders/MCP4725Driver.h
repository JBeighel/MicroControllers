/**	@defgroup	mcp4725driver
	@brief		Driver for the MCP4725 digital to analog converter
	@details	v0.1
		This converter can run fine on the high speed 1MB/s I2C bus.  It still shows a 
		clear step when the value changes.  All of the delay was from the time needed to
		transfer data over the bus, not for the device to change its output.
*/

#ifndef __MCP4725DRIVER
	#define __MCP4725DRIVER

/***** Includes		*****/
	#include "I2CDevice.h"

/***** Definitions	*****/
	/**	@brief		Base I2C address of the DAC chip
		@details	The chip has two internal bits that can be changed at the factory that
			arenot accounted for in this.
		@ingroup	mcp4725driver
	*/
	#define MCP4725_AddrBase		0x62
	
	/**	@brief		Address bit used by the external address pin
		@details	If the address pin is pulled high this bit will need set.  If it is
			pulled low it does not need included.
		@ingroup	mcp4725driver
	*/
	#define MCP4725_AddrA0			0x01
	
	/**	@brief		Number of bytes the DAC transmits in response to a read request
		@ingroup	mcp4725driver
	*/
	#define MCP4725_ReadBytes		5
	
	/**	@brief		All commands the DAC will respond to
		@details	Depending on the message type (write versus fast mode) the data may
			be positioned differently in the message.  The shifts needed are also included
			in this enumeration
		@ingroup	mcp4725driver
	*/
	typedef enum eMCP4725Commands_t {
		MCP4725_FastMode		= 0x00,	/**< Change DAC register only, address followed by 2 data bytes */
		MCP4725_WriteDAC		= 0x40,	/**< Load configuration bits and data code into DAC register, address followed by 3 data bytes */
		MCP4725_WriteEEPROM		= 0x60,	/**< Load configuration bits and data code into DAC register and DAC EEPROM, address followed by 3 data bytes */
		
		MCP4725_FastDataLShift	= 0x00,	/**< Number of bits to left shift the commands to include them in a Fast Mode command */
		MCP4725_WriteDataLShift	= 0x04,	/**< Number of bits to left shift the commands to include them in a Write command */
		
		MCP4725_ReadDataRShift	= 0x04,	/**< Number of bits to right shift the commands to parse them form a read command */
	} eMCP4725Commands_t;
	
	/**	@brief		Bits used to set the power state of the DAC
		@details	The commands include two bits to control the power state of the device
			This enum lists all available options as well as the information to position
			them in each type of command.
		@ingroup	mcp4725driver
	*/
	typedef enum eMCP4725PowerDownBits_t {
		MCP4725_PDNormal		= 0x00,	/**< Normal operation */
		MCP4725_PD1k			= 0x10,	/**< Vout is off and 1k Ohm resistor to ground */
		MCP4725_PD100k			= 0x20,	/**< Vout is off and 100k Ohm resistor to ground */
		MCP4725_PD500k			= 0x30,	/**< Vout is off and 500k Ohm resistor to ground */
		
		MCP4725_PDMask			= 0x30,	/**< Mask to filteer out just the correctly positioned power down bits */
		
		MCP4725_PDWriteRShift	= 0x03,	/**< For fast mode no shift is needed, to insert into other writes right shift the value this many bits */
		
		MCP4725_PDReadLShift	= 0x03,
	} eMCP4725PowerDownBits_t;
	
	/**	@brief		Class that will control the DAC device
		@ingroup	mcp4725driver
	*/
	class cMCP4725_t : cI2CDevice_t {
		public:
			/**	@brief		Contructor to ready the class, device is not setup by this
				@param		bA0PulledHigh	True if the address pin is pulled high, false otherwise
				@param		pI2CWire		Pointer to the I2C device to use to communicate with the DAC
			*/
			cMCP4725_t(bool bA0PulledHigh, TwoWire *pI2CWire);
			
			/**	@brief		Destructor to release all resources
			*/
			~cMCP4725_t();
			
			/**	@brief		Begin commmuncations with the device and prepare it for use
				@return		True on success, false on any failure
			*/
			bool Begin();
			
			/**	@brief		Read the current value of the DAC output
				@details	This is the number of counts the DAC is outputting.  The voltage
					is the ration of this over max counts (2^12) with the actual voltage
					over the power supply value.
				@param		pValue		Pointer to the buffer that will receive the value read from the DAC
				@return		True on success, false on any failure
			*/
			bool ReadDACValue(uint16_t *pValue);
			
			/**	@brief		Read the output value stored in the DAC EEPROM
				@details	This is the counts that the device will output upon power up.
				@param		pValue		Pointer to the buffer that will receive the value read from the DAC
				@return		True on success, false on any failure
			*/
			bool ReadEEPROMValue(uint16_t *pValue);
			
			/**	@brief		Set the output value of the DAC
				@param		nValue		The value in counts for the DAC to output
				@return		True on success, false on any failure
			*/
			bool WriteDACValue(uint16_t nValue);
			
			/**	@brief		Set the value in the EEPROM of the DAC
				@details	This will set the startup value of the DAC.  In addition this 
					will also set the output value of the DAC.
				@param		nValue		The value in counts to save in EEPROM and to output
				@return		True on success, false on any failure
			*/
			bool WriteEEPROMValue(uint16_t nValue);
		
		protected:
		
		private:
			/**	@brief		Reads all values from the device and parses out all values
				@details	The read message returns all available values in one response,
					All the relevant information is pulled out and returned indiviudally.
					It is up to the calling function to determine which information is 
					requried and discard the rest.
				@param		pDACPDMode		Power down mode in use on the DAC currently
				@param		pEEPROMPDMode	Power down mode stored in DAC EEPROM
				@param		pDACValue		Current output of the DAC in counts
				@param		pEEPROMPDValue	Output in counts stored in the DAC EEPROM
				@return		True on success, false on any failure
			*/
			bool ReadValues(eMCP4725PowerDownBits_t *pDACPDMode, eMCP4725PowerDownBits_t *pEEPROMPDMode, uint16_t *pDACValue, uint16_t *pEEPROMValue);
			
			/**	@brief		Sends a fast mode message setting the DAC ouput
				@param		ePDMode		Power down mode to set in the DAC
				@param		nValue		Output in counts for hte DAC to use
				@return		True on success, false on any failure
			*/
			bool WriteDAC(eMCP4725PowerDownBits_t ePDMode, uint16_t nValue);
			
			/**	@brief		Sends a write message setting the DAC ouput in EEPROM
				@details	This message will update the EEPROM value, but will also set the
					current DAC output.
				@param		ePDMode		Power down mode to set 
				@param		nValue		Output in counts to save in EEPROM and use
				@return		True on success, false on any failure
			*/
			bool WriteEEPROM(eMCP4725PowerDownBits_t ePDMode, uint16_t nValue);
	};

/***** Constants	*****/


/***** Globals		*****/


/***** Prototypes 	*****/


/***** Functions	*****/

#define MCP4725PositionWriteSample(bFastMode, nSample)	(((bFastMode) == true) ? ((nSample) << MCP4725_FastDataLShift) : ((nSample) << MCP4725_WriteDataLShift))

#define MCP4725ParseReadPDMode(bIsDACValue, nFirstDataByte) ((eMCP4725PowerDownBits_t)(((nFirstDataByte) << (MCP4725_PDReadLShift * bIsDACValue)) & MCP4725_PDMask))

#define MCP4725ParseDACReadValue(nFirstValueByte, nSecondValueByte) (((uint16_t)(nFirstValueByte) << MCP4725_ReadDataRShift) + ((uint16_t)(nSecondValueByte) >> MCP4725_ReadDataRShift))

#define MCP4725ParseEEPROMReadValue(nFirstValueByte, nSecondValueByte) (((uint16_t)((nFirstValueByte) & 0x0F) << 8) + (uint16_t)(nSecondValueByte))

cMCP4725_t::cMCP4725_t(bool bA0PulledHigh, TwoWire *pI2CWire) : cI2CDevice_t((uint8_t)(MCP4725_AddrBase | (MCP4725_AddrA0 * bA0PulledHigh)), pI2CWire) {
	//cI2CDevice_t does all work, no additional setup needed
	return;
}

cMCP4725_t::~cMCP4725_t() {
	//No allocated resources, nothing to clean up
	return;
}

bool cMCP4725_t::Begin() {
	bool bResult;
	uint16_t nDACVal, nEEPROMVal;
	eMCP4725PowerDownBits_t eDACPDMode, eEEPROMPDMode;
	
	//Read from the device as  a test that it exists and is responding
	bResult = ReadValues(&eDACPDMode, &eEEPROMPDMode, &nDACVal, &nEEPROMVal);
	
	return bResult;
}

bool cMCP4725_t::ReadDACValue(uint16_t *pValue) {
	uint8_t aReadVals[MCP4725_ReadBytes];
	uint8_t nReadBytes;
	
	nReadBytes = I2CReadData(cnI2CAddr, MCP4725_ReadBytes, aReadVals);
	
	if (nReadBytes != MCP4725_ReadBytes) {
		return false;
	}
	
	(*pValue) = MCP4725ParseDACReadValue(aReadVals[3], aReadVals[4]);
	
	return true;
}

bool cMCP4725_t::ReadEEPROMValue(uint16_t *pValue) {
	uint8_t aReadVals[MCP4725_ReadBytes];
	uint8_t nReadBytes;
	
	nReadBytes = I2CReadData(cnI2CAddr, MCP4725_ReadBytes, aReadVals);
	
	if (nReadBytes != MCP4725_ReadBytes) {
		return false;
	}
	
	(*pValue) = MCP4725ParseEEPROMReadValue(aReadVals[3], aReadVals[4]);
	
	return true;
}

bool cMCP4725_t::WriteDACValue(uint16_t nValue) {
	return WriteDAC(MCP4725_PDNormal, nValue);
}

bool cMCP4725_t::WriteEEPROMValue(uint16_t nValue) {
	return WriteEEPROM(MCP4725_PDNormal, nValue);
}

bool cMCP4725_t::ReadValues(eMCP4725PowerDownBits_t *pDACPDMode, eMCP4725PowerDownBits_t *pEEPROMPDMode, uint16_t *pDACValue, uint16_t *pEEPROMValue) {
	uint8_t nReadBytes;
	uint8_t aReadVals[MCP4725_ReadBytes]; //The DAC sends 6 bytes when reading its values
	
	nReadBytes = I2CReadData(cnI2CAddr, MCP4725_ReadBytes, aReadVals);
	
	if (nReadBytes != MCP4725_ReadBytes) {
		return false;
	}
	
	(*pDACPDMode) = MCP4725ParseReadPDMode(true, aReadVals[0]);
	(*pEEPROMPDMode) = MCP4725ParseReadPDMode(false, aReadVals[3]);
	
	(*pDACValue) = MCP4725ParseDACReadValue(aReadVals[1], aReadVals[2]);
	(*pEEPROMValue) = MCP4725ParseEEPROMReadValue(aReadVals[3], aReadVals[4]);
	
	return true;
}

bool cMCP4725_t::WriteDAC(eMCP4725PowerDownBits_t ePDMode, uint16_t nValue) {
	uint16_t nWriteValue; //Fast mode writes 2 bytes
	uint8_t nWriteBytes[2];
	
	nWriteValue = MCP4725PositionWriteSample(true, nValue);
	
	nWriteBytes[0] = nWriteValue >> 8; //Copy high byte
	nWriteBytes[1] = nWriteValue & 0xFF; //Copy low byte
	
	nWriteBytes[0] |= ePDMode; //No shift in power down bits for fast mode, just put in in the high byte
	
	return I2CWriteData(cnI2CAddr, 2, nWriteBytes);
}

bool cMCP4725_t::WriteEEPROM(eMCP4725PowerDownBits_t ePDMode, uint16_t nValue) {
	uint16_t nWriteValue; //EEPROM writes mode writes 3 bytes
	uint8_t nWriteBytes[3];
	
	nWriteValue = MCP4725PositionWriteSample(false, nValue);
	
	nWriteBytes[2] = nWriteValue >> 8; //Copy high byte
	nWriteBytes[3] = nWriteValue; //Copy low byte
	
	nWriteBytes[0] = MCP4725_WriteEEPROM;
	nWriteBytes[0] |= ePDMode >> MCP4725_PDWriteRShift;
	
	return I2CWriteData(cnI2CAddr, 3, nWriteBytes);
}

#endif

