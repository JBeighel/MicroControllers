/**	@defgroup	st7735driver
	@brief		Driver for the ST7735 LCD display
	@details	v0.1
	
	This display driver uses a SPI interface, except a single data pin is used to both read and write data.  This
	can be accomplished with the Arduino SPI interface proved that no data reads are made.  
*/

/**	@defgroup	spidevice
	@brief		Base class to provide core SPI bus functionality
	@details	v0.1
	
*/

#ifndef __NAME_H
	#define __NAME_H

/***** Includes		*****/
	#include <SPI.h>

/***** Definitions	*****/
	/** @brief		If a defined connection pin is optional, set it to this value to mark it as not in used
		@ingroup	spidevice
	*/
	#define SPI_NOPIN			0xFF
	
	/**	@brief		A default SPI clock frequency in hertz
		@ingroup	spidevice
	*/
	#define SPI_DEFAULTCLOCK	6000000

	/** @brief		Enumeration of SPI data byte order values
		@ingroup	spidevice
	*/
	typedef enum eSPIDataOrder_t {
		SPI_MSBFirst	= MSBFIRST,		/** Higher order byte is transmitted first */
		SPI_LSBFirst	= LSBFIRST,		/** Lower order byte is transmitted first */
	};
	
	/** @brief		Enumeration of SPI data mode values
		@ingroup	spidevice
	*/
	typedef enum eSPIDataMode_t {
		SPI_Mode0		= SPI_MODE0,	/** Clock polarity: Idles at 0; Clock phase: Read data on clock leading edge, change on trailing edge */
		SPI_Mode1		= SPI_MODE1,	/** Clock polarity: Idles at 0; Clock phase: Read data on clock trailing edge, change on leading edge */
		SPI_Mode2		= SPI_MODE2,	/** Clock polarity: Idles at 1; Clock phase: Read data on clock leading edge, change on trailing edge */
		SPI_Mode3		= SPI_MODE3,	/** Clock polarity: Idles at 1; Clock phase: Read data on clock trailing edge, change on leading edge */
	}

	class cSPIDevice_t {
		public:
			/**	@brief		Constructor to setup class and extra signal pins
				@param		pSPIObj		Pointer to the SPI bus control object
				@param		nCSPin		Pin number to use to select the chip on the SPI bus to communicate with
			*/
			cSPIDevice_t(SPIClass *pSPIObj, uint8_t nCSPin);
			
			/**	@brief		Deconstructor to clean up any resources allocated by the class
			*/
			~cSPIDevice_t();
			
		protected:
			/**	@brief		Setup data rate and exchange method for the SPI bus to used
				@param		nClockFreq		Frequency to use as the SPI clock in Hertz
				@param		eDataOrder		The order to send the data bytes over the bus
				@param		nDataMode		The mode to use when changing levels for the data bitset
				@return		True if the opration is successful, false on any error
			*/
			bool SetSPISettings(uint32_t nClockFreq, eSPIDataOrder_t eDataOrder, eSPIDataMode_t nDataMode);
			
			/**	@brief		Send a single byte of data over the SPI bus
				@param		nByte			The value to send 
				@return		True if the opration is successful, false on any error
			*/
			bool SPISendDataUint8(nByte);
			
			/**	@brief		Send a register address then a value to be stored in that register
				@param		nRegister		The address of the register to write to
				@param		nValue			The value to be stored in that register
				@return		True if the opration is successful, false on any error
			*/
			bool SPISendRegisterValueUint8(uint8_t nRegister, uint8_t nValue);
			
			/**	@brief		Send a byte value as a register request, then read a bytre of data from the SPI bus
				@param		nRegister		The address of the register to read
				@return		The value that was read from the SPI bus after transmitted the register address
			*/
			uint8_t SPIReadRegisterValueUint8(uint8_t nRegister);
			
			/**	@brief		Does not transmit any data, just reads a byte from the SPI bus
				@return		The value that was read from the SPI bus after transmitted the register address
			*/
			uint8_t SPIReadDataUint8();
			
			/**	@brief		Send a block of data over the SPI bus
				@param		nNumBytes		The number of bytes to send
				@param		pDataBlock		Pointer to the buffer holding the data to transmitted
				@return		True if the opration is successful, false on any error
			*/
			bool SPISendDataBlock(uint32_t nNumBytes, void *pDataBlock);
			
		private:
			/**	@brief		The chip select pin assigned to this device */
			uint8_t cnCSPin;
			
			/**	@brief		Pointer to the SPI control object to use */
			SPIClass *cpSPIBus;
			
			/**	@brief		Settings to use when transmitting data to this device */
			SPISettings cSPISettings;
			
			/**	@brief		Sets up the device to begin communications over the SPI bus
				@return		True if the opration is successful, false on any error
			*/
			bool BeginTransfer();
			
			/**	@brief		Ends communications over the SPI bus
				@return		True if the opration is successful, false on any error
			*/
			bool EndTransfer();
	};
	
	typedef enum eST7735RCommand_t {
		ST7735_NOP			= 0x00,		/** No Operation */
		ST7735_SWRESET		= 0x01,		/** Software reset */
		ST7735_RDDID		= 0x04,		/** Read display ID */
		ST7735_RDDST		= 0x09,		/** Read display status */
		ST7735_RDDST		= 0x0A,		/** Read display power */
	};

/***** Constants	*****/


/***** Globals		*****/


/***** Prototypes 	*****/


/***** Functions	*****/
cSPIDevice_t(SPIClass *pSPIObj, uint8_t nCSPin) {
	//Initialize class variables
	cpSPIBus = pSPIObj;
	cnCSPin = nCSPin;
	
	//Make sure chip select pin is an output_iterator
	if (cnCSPin != SPI_NOPIN) {
		pinMode(nCSPin, OUTPUT);
		digitalWrite(cnCSPin, HIGH); //Deselect the chip
	}
	
	//Set some default settings value
	cSPISettings = SPISettings(SPI_DEFAULTCLOCK, SPI_MSBFirst, SPI_Mode0);
	
	return;
}

~cSPIDevice_t() {
	//No resources allocated by this class, no cleanup necessary
	return;
}

bool SetSPISettings(uint32_t nClockFreq, eSPIDataOrder_t eDataOrder, eSPIDataMode_t nDataMode) {
	cSPISettings = SPISettings(nClockFreq, eDataOrder, nDataMode);
	
	return true;
}

bool SPISendDataUint8(nByte) {
	BeginTransfer();
	
	cpSPIBus->transfer(nByte);
	
	EndTransfer();
	
	return true;
}

bool SPISendRegisterValueUint8(uint8_t nRegister, uint8_t nValue) {
	BeginTransfer();
	
	cpSPIBus->transfer(nByte);
	cpSPIBus->transfer(nValue);
	
	EndTransfer();
	
	return true;
}

uint8_t SPIReadRegisterValueUint8(uint8_t nRegister) {
	uint8_t nReadValue;
	
	BeginTransfer();
	
	cpSPIBus->transfer(nByte);
	
	nReadValue = cpSPIBus->transfer(0x00); //Transfer does both read and write, send any value just to get the read data
	
	EndTransfer();
	
	return nReadValue;
}

uint8_t SPIReadDataUint8() {
	uint8_t nReadValue;
	
	BeginTransfer();
	
	nReadValue = cpSPIBus->transfer(0x00); //Transfer does both read and write, send any value just to get the read data
	
	EndTransfer();
	
	return nReadValue;
}

bool  SPISendDataBlock(uint32_t nNumBytes, void *pDataBlock) {
	uint32_t nCtr;
	uint8_t *pNextByte = (uint8_t *)pDataBlock;
	
	BeginTransfer();
	
	for (nCtr = 0; nCtr < nNumBytes; nCtr++) {
		cpSPIBus->transfer(*pNextByte); //Send the current data byte
		
		pNextByte += 1; //Advance to the next data byte
	}
	
	EndTransfer();
}

bool BeginTransfer() {
	if (cnChipSelectPin != SPI_NOPIN) {
		digitalWrite(cnCSPin, LOW); //Select the chip
	}
	
	SPI.beginTransaction(cSPISettings);

	return true;
}

bool EndTransfer() {
	if (cnChipSelectPin != SPI_NOPIN) {
		digitalWrite(cnCSPin, HIGH); //Deselect the chip
	}
	
	SPI.endTransaction();

	return true;
}

#endif

