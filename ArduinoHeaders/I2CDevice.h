/**	@defgroup	i2cdevice
	@brief		Base class to provide core I2B bus functionality
	@details	
		
*/

#ifndef __I2CDEVICE
	#define __I2CDEVICE

/***** Includes		*****/
	#include <Wire.h>

/***** Definitions	*****/
	/**	@brief		Sending this address on the I2C bus attempts to reach all connected devices
		@ingroup	i2cdevice
	*/
	#define I2C_GENERALCALLADDR	0x00

	/**	@brief		Base class to inherit when handling a device connected via I2C
		@details	Contains functions and members that provide all the basic functionality needed to interact
			with an I2C connected device.
		@ingroup	i2cdevice
	*/
	class cI2CDevice_t {
	public:
		/**	@brief		Contrusctor to prepare the class instance for use
			@details	Sets up all class variables, does not touch any hardware
			@param		nI2CAddr		Address of the I2C connected device
			@param		pI2CWire		Pointer to the I2C bus interface this class will use
		*/
		cI2CDevice_t(uint8_t nI2CAddr, TwoWire *pI2CWire);
		
		/**	@brief		Destructor to release all resources allocated by the class
		*/
		~cI2CDevice_t();
	
	protected:
		uint8_t cnI2CAddr;	/**< @brief Address of the I2C connected device */
		TwoWire *cpI2CBus;	/**< @brief Pointer to the I2C bus controller to use */
	
		/**	@brief		Read a value from an 8-bit register over I2C
			@param		nDevAddr	Address of the device to read from
			@param		nRegAddr	Register address to read a byte from
			@return		Value read from the address
		*/
		uint8_t I2CReadUint8Reg(uint8_t nDevAddr, uint8_t nRegAddr);
		
		/**	@brief		Write a value to an 8-bit register over I2C
			@param		nDevAddr	Address of the device to read from
			@param		nRegAddr	Register address to read a byte from
			@param		nValue		Value to write into the register
			@return		True if the write was successful, false on any error
		*/
		bool I2CWriteUint8Reg(uint8_t nDevAddr, uint8_t nRegAddr, uint8_t nValue);
		
		/**	@brief		Write  value after an I2C All Call
			@details	If address 0x00 is requested over the I2C bus it is considered an All Cll where all 
				devices on that bus are to listen.  This function will do that, then write a single byte of 
				data which should be accepted by all devices.
			@param		nValue		The byte of data to write following the All Call
			@return		True on success, false on any error
		*/
		bool I2CGeneralCall(uint8_t nValue);
		
	private:
		
	};

/***** Constants	*****/


/***** Globals		*****/


/***** Prototypes 	*****/


/***** Functions	*****/
cI2CDevice_t::cI2CDevice_t(uint8_t nI2CAddr, TwoWire *pI2CWire) {
	cnI2CAddr = nI2CAddr;
	cpI2CBus = pI2CWire;
	
	return;
}

cI2CDevice_t::~cI2CDevice_t() {
	//No resources are allocated, no clanup work is needed
	return;
}

uint8_t cI2CDevice_t::I2CReadUint8Reg(uint8_t nDevAddr, uint8_t nRegAddr) {
	uint8_t nByteRecv;

	cpI2CBus->beginTransmission(nDevAddr);
	cpI2CBus->write(nRegAddr);
	cpI2CBus->endTransmission();

	cpI2CBus->requestFrom(nDevAddr, (uint8_t)1); //This function assumes registers have 1 byte
	nByteRecv = cpI2CBus->read();

	return nByteRecv;
}

bool cI2CDevice_t::I2CWriteUint8Reg(uint8_t nDevAddr, uint8_t nRegAddr, uint8_t nValue) {
	uint8_t nResult;
	
	cpI2CBus->beginTransmission(nDevAddr);
	cpI2CBus->write(nRegAddr);
	cpI2CBus->write(nValue);
	nResult = cpI2CBus->endTransmission();
	
	if (nResult == 0) {
		return true;
	} else {
		//nResult value:
		//1 = Data was too long for transmit buffer
		//2 = Received NACK on transmit of address
		//3 = Received NACK on transmit of data
		//4 = Unknown error
		//Serial.print("I2C Write Result ");
		//Serial.println(nResult);
		return false;
	}
}

bool cI2CDevice_t::I2CGeneralCall(uint8_t nValue) {
	cpI2CBus->beginTransmission(I2C_GENERALCALLADDR);
	cpI2CBus->write(nValue);
	cpI2CBus->endTransmission();
	
	return true;
}

#endif

