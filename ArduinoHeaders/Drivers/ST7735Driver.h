/**	@defgroup	st7735driver
	@brief		Driver for the ST7735 LCD display
	@details	v0.1
	
	This display driver uses a SPI interface, except a single data pin is used to both read and write data.  This
	can be accomplished with the Arduino SPI interface proved that no data reads are made.  
	
	This is not working.  Replaced by driver in the TFT.h driver for the LCD.
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
	};

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
			bool SPISetSettings(uint32_t nClockFreq, eSPIDataOrder_t eDataOrder, eSPIDataMode_t nDataMode);
			
			/**	@brief		Send a single byte of data over the SPI bus
				@param		nByte			The value to send 
				@return		True if the opration is successful, false on any error
			*/
			bool SPISendDataUint8(uint8_t nByte);
			
			/**	@brief		Send a register address then a value to be stored in that register
				@param		nRegister		The address of the register to write to
				@param		nValue			The value to be stored in that register
				@return		True if the opration is successful, false on any error
			*/
			bool SPISendRegisterValueUint8(uint8_t nRegister, uint8_t nValue);
			
			bool SPISendCommandDataBlockUint8(uint8_t nCommand, uint8_t *pDataBlock, uint16_t nNumBytes);
			
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
			bool SPISendDataBlock(void *pDataBlock, uint32_t nNumBytes);
			
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
	
	/**	@brief		SPI Bus clock used by the display chip
		@ingroup	st7735driver
	*/
	#define ST7735_SPICLOCK		6000000
	
	/**	@brief		SPI clock polatiry of the ST7735 display driver chip
		@ingroup	st7735driver
	*/
	#define ST7735_SPIMODE		SPI_Mode0
	
	/**	@brief		SPI data order used by the ST7735 display chip
		@ingroup	st7735driver
	*/
	#define ST7735_SPIDATAORDER	SPI_MSBFirst
	
	/**	@brief		The number of milliseconds the device needs to complete a reset
		@ingroup	st7735driver
	*/
	#define ST7735_RESETTIME		120
	
	/**	@brief		Enumeration of all registers and commands of the ST7735 display driver chip
		@ingroup	st7735driver
	*/
	typedef enum eST7735RCommand_t {
		ST7735_NOP			= 0x00,		/** No Operation */
		ST7735_SWRESET		= 0x01,		/** Software reset */
		ST7735_RDDID		= 0x04,		/** Read display ID */
		ST7735_RDDST		= 0x09,		/** Read display status */
		ST7735_RDDPM		= 0x0A,		/** Read display power */
		ST7735_RDDMADCTL	= 0x0B,		/** Read display */
		ST7735_RDDCOLMOD	= 0x0C,		/** Read display pixel */
		ST7735_RDDIM		= 0x0D,		/** Read display image */
		ST7735_RDDSM		= 0x0E,		/** Read display signal */
		ST7735_SLPIN		= 0x10,		/** Sleep in & booster off */
		ST7735_SLPOUT		= 0x11,		/** Sleep ount & booster on */
		ST7735_PTLON		= 0x12,		/** Partial mode on */
		ST7735_NORON		= 0x13,		/** Partial off (normal) */
		ST7735_INVOFF		= 0x20,		/** Display inversion off */
		ST7735_INVON		= 0x21,		/** Display inversion on */
		ST7735_GAMSET		= 0x26,		/** Gamma curve select */
		ST7735_DISPOFF		= 0x28,		/** Display off */
		ST7735_DISPON		= 0x29,		/** Display on */
		ST7735_CASET		= 0x2A,		/** Column address set */
		ST7735_RASET		= 0x2B,		/** Row address set */
		ST7735_RAMWR		= 0x2C,		/** Memory write */
		ST7735_RGBSET		= 0x2D,		/** LUT for 4k, 65k, or 262k color */
		ST7735_RAMRD		= 0x2E,		/** Memory read */
		ST7735_PTLAR		= 0x30,		/** Partial start/end address set */
		ST7735_TEOFF		= 0x34,		/** Tearing effect line off */
		ST7735_TEON			= 0x35,		/** Tearing effect mode set & on */
		ST7735_MADCTL		= 0x36,		/** Memory data access control */
		ST7735_IDMOFF		= 0x38,		/** Idle mode off */
		ST7735_IDMON		= 0x39,		/** Idle mode on */
		ST7735_COLMOD		= 0x3A,		/** Interface pixel format */
		ST7735_RDID1		= 0xDA,		/** Read ID 1 */
		ST7735_RDID2		= 0xDB,		/** Read ID 2 */
		ST7735_RDID3		= 0xDC,		/** Read ID 3 */
	};
	
	enum eST7735Gamma_t {
		LCD_Gamma0			= 0x01,		/**< Gamma curve 1 (G1.0) */
		LCD_Gamma1			= 0x02,		/**< Gamma curve 2 (G2.5) */
		LCD_Gamma2			= 0x04,		/**< Gamma curve 3 (G2.2) */
		LCD_Gamma3			= 0x08,		/**< Gamma curve 4 (G1.8) */
	};
	
	enum eST7735MADCtl_t {
		LCD_RowAddrOrder	= 0x80,		/**< Controls MCU to momeory write/read direction */
		LCD_ColAddrOrder	= 0x40,		/**< Controls MCU to momeory write/read direction */
		LCD_RowColExchange	= 0x20,		/**< Controls MCU to momeory write/read direction */
		
		LCD_VertRfrshOrder	= 0x10,		/**< LCD vertical refresh direction: 0 top to bottom, 1 bottom to top */
		LCD_ColorOrder		= 0x08,		/**< Collor order: 0 RGB, 1 BGR */
		LCD_HorzRfrshOrder	= 0x04,		/**< LCD horizontal refresh direction: 0 left to right, 1 right to left */
	};
	
	enum eST7735ColorMode_t {
		LCD_12Bit			= 0x03,		/**< 12 bits per pixel */
		LCD_16Bit			= 0x05,		/**< 16 bits per pixel */
		LCD_18Bit			= 0x06,		/**< 18 bits per pixel */
	};
	
	class cST7735_t : cSPIDevice_t {
		public:
			/**	@brief		Constructor to setup and prepare an instance of the class
				@param		pSPIObj		Pointer to the SPI bus object
				@param		nCSPin		Pin number for the chip select pin
				@param		nDCPin		Pin number for the data/command pin
				@param		nResetPin	Pin number for the reset pin
				@param		nXPixels	Number of pixels along the X axis
				@param		nYPixels	NUmber of pixels along the Y axis
			*/
			cST7735_t(SPIClass *pSPIObj, uint8_t nCSPin, uint8_t nDCPin, uint8_t nResetPin, uint16_t nXPixels, uint16_t nYPixels);
			
			/**	@brief		Deconstructor to clean up any resources allocated by the class
			*/
			~cST7735_t();
			
			/**	@brief		Sets up all the hardware and prepares the device for operation
				@details	Must be called after the SPI bus object is prepared for tuse via its begin() 
					function.
				@return		True on successful completion, false on any error
			*/
			bool Begin();
			
			bool DrawPixel(uint16_t nXCoord, uint16_t nYCoord, uint8_t nRed, uint8_t nGreen, uint8_t nBlue);
			
			bool SetInvertDisplay(bool bEnable);
		
		protected:
			bool SetDrawRegion(uint16_t nLeftXStart, uint16_t nRightXEnd, uint16_t nTopYStart, uint16_t nBottomYEnd);
			
			bool SetRegisterValue(uint8_t nRegister, uint8_t nValue);
		
		private:
			/**	Local storage of the data/command pin number */
			uint8_t cnDCPin;
			
			/** Local storage of the reset pin number */
			uint8_t cnResetPin;
			
			/** Number of pixels along the screen's X dimension */
			uint16_t cnXDimension;
			
			/** Number of pixels along the screen's Y dimension */
			uint16_t cnYDimension;
	};

/***** Constants	*****/


/***** Globals		*****/


/***** Prototypes 	*****/


/***** Functions	*****/
cSPIDevice_t::cSPIDevice_t(SPIClass *pSPIObj, uint8_t nCSPin) {
	//Initialize class variables
	cpSPIBus = pSPIObj;
	cnCSPin = nCSPin;
	
	//Make sure chip select pin is an output_iterator
	if (cnCSPin != SPI_NOPIN) {
		pinMode(nCSPin, OUTPUT);
		digitalWrite(cnCSPin, HIGH); //Deselect the chip
	}
	
	//Set some default settings value
	cSPISettings = SPISettings(SPI_DEFAULTCLOCK, (BitOrder)SPI_MSBFirst, (uint8_t)SPI_Mode0);
	
	return;
}

cSPIDevice_t::~cSPIDevice_t() {
	//No resources allocated by this class, no cleanup necessary
	return;
}

bool cSPIDevice_t::SPISetSettings(uint32_t nClockFreq, eSPIDataOrder_t eDataOrder, eSPIDataMode_t nDataMode) {
	cSPISettings = SPISettings(nClockFreq, (BitOrder)eDataOrder, (uint8_t)nDataMode);
	
	return true;
}

bool cSPIDevice_t::SPISendDataUint8(uint8_t nByte) {
	BeginTransfer();
	
	cpSPIBus->transfer(nByte);
	
	EndTransfer();
	
	return true;
}

bool cSPIDevice_t::SPISendRegisterValueUint8(uint8_t nRegister, uint8_t nValue) {
	BeginTransfer();
	
	cpSPIBus->transfer(nRegister);
	cpSPIBus->transfer(nValue);
	
	EndTransfer();
	
	return true;
}

bool cSPIDevice_t::SPISendCommandDataBlockUint8(uint8_t nCommand, uint8_t *pDataBlock, uint16_t nNumBytes) {
	uint32_t nCtr;
	uint8_t *pNextByte = (uint8_t *)pDataBlock;
	
	BeginTransfer();
	
	cpSPIBus->transfer(nCommand);  //Send the command first
	
	for (nCtr = 0; nCtr < nNumBytes; nCtr++) {
		cpSPIBus->transfer(*pNextByte); //Send the current data byte
		
		pNextByte += 1; //Advance to the next data byte
	}
	
	EndTransfer();
}

uint8_t cSPIDevice_t::SPIReadRegisterValueUint8(uint8_t nRegister) {
	uint8_t nReadValue;
	
	BeginTransfer();
	
	cpSPIBus->transfer(nRegister);
	
	nReadValue = cpSPIBus->transfer(0x00); //Transfer does both read and write, send any value just to get the read data
	
	EndTransfer();
	
	return nReadValue;
}

uint8_t cSPIDevice_t::SPIReadDataUint8() {
	uint8_t nReadValue;
	
	BeginTransfer();
	
	nReadValue = cpSPIBus->transfer(0x00); //Transfer does both read and write, send any value just to get the read data
	
	EndTransfer();
	
	return nReadValue;
}

bool  cSPIDevice_t::SPISendDataBlock(void *pDataBlock, uint32_t nNumBytes) {
	uint32_t nCtr;
	uint8_t *pNextByte = (uint8_t *)pDataBlock;
	
	BeginTransfer();
	
	for (nCtr = 0; nCtr < nNumBytes; nCtr++) {
		cpSPIBus->transfer(*pNextByte); //Send the current data byte
		
		pNextByte += 1; //Advance to the next data byte
	}
	
	EndTransfer();
}

bool cSPIDevice_t::BeginTransfer() {
	if (cnCSPin != SPI_NOPIN) {
		digitalWrite(cnCSPin, LOW); //Select the chip
	}
	
	SPI.beginTransaction(cSPISettings);

	return true;
}

bool cSPIDevice_t::EndTransfer() {
	if (cnCSPin != SPI_NOPIN) {
		digitalWrite(cnCSPin, HIGH); //Deselect the chip
	}
	
	SPI.endTransaction();

	return true;
}

cST7735_t::cST7735_t(SPIClass *pSPIObj, uint8_t nCSPin, uint8_t nDCPin, uint8_t nResetPin, uint16_t nXPixels, uint16_t nYPixels) : cSPIDevice_t(pSPIObj, nCSPin) {
	//Setup up all class variables
	cnDCPin = nDCPin;
	cnResetPin = nResetPin;
	cnXDimension = nXPixels;
	cnYDimension = nYPixels;
	
	if (cnResetPin != SPI_NOPIN) {
		pinMode(cnResetPin, OUTPUT);
		digitalWrite(cnResetPin, HIGH);
	}
	
	pinMode(cnDCPin, OUTPUT);
	
	return;
}

cST7735_t::~cST7735_t() {
	//No resources allocated for this class, nothing to release
	return;
}

bool cST7735_t::Begin() {
	uint8_t anSPIData[4];
	
	//Configure SPI bus
	SPISetSettings(ST7735_SPICLOCK, ST7735_SPIDATAORDER, ST7735_SPIMODE);
	
	//Reset the device to get default settings
	if (cnResetPin != SPI_NOPIN) { //Hardware reset
		digitalWrite(cnResetPin, LOW);
		delay(1);
		digitalWrite(cnResetPin, HIGH);
	} else { //Software reset
		digitalWrite(cnDCPin, LOW);
		SPISendDataUint8(ST7735_SWRESET);
	}
	
	delay(ST7735_RESETTIME);
	
	//Do all configuration with the display off
	SPISendDataUint8(ST7735_DISPOFF);
	
	digitalWrite(cnDCPin, LOW);  //These are commands only
	//Set all device modes
	SPISendDataUint8(ST7735_SLPOUT);
	SPISendDataUint8(ST7735_NORON);
	SPISendDataUint8(ST7735_TEOFF);
	SPISendDataUint8(ST7735_IDMOFF);
	SPISendDataUint8(ST7735_INVOFF);
	
	//Set configuration registers
	SPISendRegisterValueUint8(ST7735_GAMSET, LCD_Gamma0);
	SPISendRegisterValueUint8(ST7735_MADCTL, 0x00); //Refresh top to bottom, left to right, and RGB color order
	SPISendRegisterValueUint8(ST7735_COLMOD, LCD_18Bit);
	
	//Configuration done, turn display back on
	SPISendDataUint8(ST7735_DISPON);
	
	return true;
}

bool cST7735_t::DrawPixel(uint16_t nXCoord, uint16_t nYCoord, uint8_t nRed, uint8_t nGreen, uint8_t nBlue) {
	uint8_t anSPIData[3];
	
	//Convert from 8 bit color ro 6 bit color
	nRed = (uint8_t)((float)nRed * (79.0f / 255.0f)); // 4f / ff = 79 / 255
	nGreen = (uint8_t)((float)nGreen * (79.0f / 255.0f));
	nBlue = (uint8_t)((float)nBlue * (79.0f / 255.0f));
	
	//Position the bits where they are expected by the driver chip
	anSPIData[0] = nRed << 2;
	anSPIData[1] = nGreen << 2;
	anSPIData[2] = nBlue << 2;
	
	//Drawing a single pixel
	SetDrawRegion(nXCoord, nXCoord, nYCoord, nYCoord);
	
	//Finally the data write
	digitalWrite(cnDCPin, LOW);  //Flag thsi byte as a command
	SPISendDataUint8(ST7735_RAMWR);
	
	digitalWrite(cnDCPin, HIGH);  //Flag thsi byte as data	
	SPISendDataBlock(anSPIData, 3);
	
	return true;
}

bool cST7735_t::SetInvertDisplay(bool bEnable) {
	digitalWrite(cnDCPin, LOW);  //Flag thsi byte as a command
	
	if (bEnable == true) {
		SPISendDataUint8(ST7735_INVON);
	} else {
		SPISendDataUint8(ST7735_INVOFF);
	}
	
	return true;
}

bool cST7735_t::SetDrawRegion(uint16_t nLeftXStart, uint16_t nRightXEnd, uint16_t nTopYStart, uint16_t nBottomYEnd) {
	uint8_t anSPIData[4];
	
	anSPIData[0] = (nLeftXStart >> 8) & 0xFF; //High byte
	anSPIData[1] = nLeftXStart & 0x0FF; //Low byte
	anSPIData[2] = (nRightXEnd >> 8) & 0xFF;
	anSPIData[3] = nRightXEnd & 0x0FF;
	
	digitalWrite(cnDCPin, LOW);  //Flag thsi byte as a command
	SPISendDataUint8(ST7735_CASET);
	
	digitalWrite(cnDCPin, HIGH);  //Flag thsi byte as data
	SPISendDataBlock(anSPIData, 4);
	
	anSPIData[0] = (nTopYStart >> 8) & 0xFF; //High byte
	anSPIData[1] = nTopYStart & 0x0FF; //Low byte
	anSPIData[2] = (nBottomYEnd >> 8) & 0xFF;
	anSPIData[3] = nBottomYEnd & 0x0FF;
	
	digitalWrite(cnDCPin, LOW);  //Flag thsi byte as a command
	SPISendDataUint8(ST7735_RASET);
	
	digitalWrite(cnDCPin, HIGH);  //Flag thsi byte as data	
	SPISendDataBlock(anSPIData, 4);
	
	return true;
}

bool cST7735_t::SetRegisterValue(uint8_t nRegister, uint8_t nValue) {
	digitalWrite(cnDCPin, LOW);  //Flag thsi byte as a command
	SPISendDataUint8(nRegister);
	
	digitalWrite(cnDCPin, HIGH);  //Flag thsi byte as data
	SPISendDataUint8(nValue);
	
	return true;
}

#endif

