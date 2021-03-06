/**	@defgroup	oled1306
	@brief		SPI driver for the SSD1306 0.96" OLED Screen
	@details	Ver 1.0
		Initializes and communicates with the SSD1306 screen via SPI 
		interface to the Arduino Uno.

		On the Uno Digital IO 13 is the Clock and 11 is the MOSI pin.
		Pin 12 is the MISO pin, but this is not required by the screen.
		The screen needs three additional IO pins for Reset, Chip 
		Select and Command/Data control.  Reset is only needed to 
		restart the screen without removing power.  Chip select is
		required for SPI communications.  The Command/Data control
		is needed to instruct the screen how to handle the incoming
		data bytes.

		A memory buffer will be needed to store the image data that
		the screen will display.  This buffer will need to have 1 bit
		per pixel of the screen, so a 128x64 pixel display will have
		8192 pixels and need 1024 bytes for this buffer.

		Available preprocessor options:
		- SSD1306_ALLCAPLETTERS all constant values are placed in RAM 
			in the Arduino.  The fonts use 7 bytes per character and
			as a result can take up a chunk of memory.  This will cause
			all letters to be uppercase and removes the lower case 
			letter constants to free up 182 bytes of RAM.
*/

#ifndef __SSD1306OLEDDRIVER_H
	#define __SSD1306OLEDDRIVER_H

/***** Includes    *****/
	#include <stdlib.h>

	#include <SPI.h>
	#include <Wire.h>

/***** Constants   *****/
	/** @brief		Assumed number of rows of pixesl in the screen
		@ingroup	oled1306
	*/
	#define OLED1306_ROWS		64

	/** @brief		Assumed number of columns of pixesl in the screen
		@ingroup	oled1306
	*/
	#define OLED1306_COLUMNS	128

	/** @brief		Default clock frequency for the 1306 display
		@ingroup	oled1306
	*/
	#define OLED1306_CLKFREQ	8000000
	
	/**	@brief		If a pin is not connected then set its value to this so it is ignored in the logical_and
		@ingroup	oled1306
	*/
	#define OLED1306_NOPIN		UINT8_MAXVALUE
	
	#define OLED1306_I2CADDRBASE	0x3C
	
	#define OLED1306_I2CSA0BIT		0x01
	
	#define OLED1306_I2CCOBIT		0x80
	
	#define OLED1306_I2CDCBIT		0x40
	
	#ifdef BUFFER_LENGTH
		#define OLED1306_I2CBUFFLEN		BUFFER_LENGTH
	#else
		#define OLED1306_I2CBUFFLEN		32
	#endif

	/**	@brief		Constant holding capital letters for the display font
		@details	Each character is 5 bits wide and 7 bits tall.  Making each letter
			use 7 bytes.  The last 3 bits in each byte are not needed.  It is expected
			that when rendered the bits that are off will not be drawn.
		@ingroup	oled1306
	*/
	const uint8_t aBitImg_CapLetters_5x7[] = {
		//Capital A
		B01110000,
		B10001000,
		B10001000,
		B10001000,
		B11111000,
		B10001000,
		B10001000,
		//Capital B
		B11110000,
		B10001000,
		B10001000,
		B11110000,
		B10001000,
		B10001000,
		B11110000,
		//Capital C
		B01110000,
		B10001000,
		B10000000,
		B10000000,
		B10000000,
		B10001000,
		B11110000,
		//Capital D
		B11110000,
		B10001000,
		B10001000,
		B10001000,
		B10001000,
		B10001000,
		B11110000,
		//Capital E
		B11111000,
		B10000000,
		B10000000,
		B11110000,
		B10000000,
		B10000000,
		B11111000,
		//Capital F
		B11111000,
		B10000000,
		B10000000,
		B11110000,
		B10000000,
		B10000000,
		B10000000,
		//Capital G
		B01110000,
		B10001000,
		B10000000,
		B10111000,
		B10001000,
		B10001000,
		B11110000,
		//Capital H
		B10001000,
		B10001000,
		B10001000,
		B11111000,
		B10001000,
		B10001000,
		B10001000,
		//Capital I
		B01110000,
		B00100000,
		B00100000,
		B00100000,
		B00100000,
		B00100000,
		B01110000,
		//Capital J
		B00111000,
		B00010000,
		B00010000,
		B00010000,
		B00010000,
		B10010000,
		B01100000,
		//Capital K
		B10001000,
		B10001000,
		B10010000,
		B11100000,
		B10010000,
		B10001000,
		B10001000,
		//Capital L
		B10000000,
		B10000000,
		B10000000,
		B10000000,
		B10000000,
		B10000000,
		B11111000,
		//Capital M
		B10001000,
		B11011000,
		B10101000,
		B10101000,
		B10001000,
		B10001000,
		B10001000,
		//Capital N
		B10001000,
		B10001000,
		B11001000,
		B10101000,
		B10011000,
		B10001000,
		B10001000,
		//Capital 0
		B01110000,
		B10001000,
		B10001000,
		B10001000,
		B10001000,
		B10001000,
		B11110000,
		//Capital P
		B11110000,
		B10001000,
		B10001000,
		B11110000,
		B10000000,
		B10000000,
		B10000000,
		//Capital Q
		B01110000,
		B10001000,
		B10001000,
		B10001000,
		B10101000,
		B10011000,
		B11111000,
		//Capital R
		B11110000,
		B10001000,
		B10001000,
		B11110000,
		B10010000,
		B10001000,
		B10001000,
		//Capital S
		B01111000,
		B10000000,
		B10000000,
		B01110000,
		B00001000,
		B00001000,
		B11110000,
		//Capital T
		B11111000,
		B00100000,
		B00100000,
		B00100000,
		B00100000,
		B00100000,
		B00100000,
		//Capital U
		B10001000,
		B10001000,
		B10001000,
		B10001000,
		B10001000,
		B10001000,
		B01110000,
		//Capital V
		B10001000,
		B10001000,
		B10001000,
		B10001000,
		B10001000,
		B01010000,
		B00100000,
		//Capital W
		B10001000,
		B10001000,
		B10001000,
		B10101000,
		B10101000,
		B11011000,
		B10001000,
		//Capital X
		B10001000,
		B10001000,
		B01010000,
		B00100000,
		B01010000,
		B10001000,
		B10001000,
		//Capital Y
		B10001000,
		B10001000,
		B01010000,
		B00100000,
		B00100000,
		B00100000,
		B00100000,
		//Capital Z
		B11111000,
		B00001000,
		B00010000,
		B00100000,
		B01000000,
		B10000000,
		B11111000,
	};

	#ifndef SSD1306_ALLCAPLETTERS
		/**	@brief		Constant holding lower case letters for the display font
			@details	Each character is 5 bits wide and 7 bits tall.  Making each letter
				use 7 bytes.  The last 3 bits in each byte are not needed.  It is expected
				that when rendered the bits that are off will not be drawn.
			@ingroup	oled1306
		*/
		const uint8_t aBitImg_LowLetters_5x7[] = {
			//Lower a
			B00000000,
			B00000000,
			B01110000,
			B00001000,
			B01111000,
			B10001000,
			B01111000,
			//Lower b
			B10000000,
			B10000000,
			B10000000,
			B11110000,
			B10001000,
			B10001000,
			B11110000,
			//Lower c
			B00000000,
			B00000000,
			B01110000,
			B10001000,
			B10000000,
			B10001000,
			B11110000,
			//Lower d
			B00001000,
			B00001000,
			B00001000,
			B01111000,
			B10001000,
			B10001000,
			B11111000,
			//Lower e
			B00000000,
			B00000000,
			B01110000,
			B10001000,
			B11111000,
			B10000000,
			B11111000,
			//Lower f
			B00110000,
			B01001000,
			B01000000,
			B11110000,
			B01000000,
			B01000000,
			B01000000,
			//Lower g
			B00000000,
			B00000000,
			B01110000,
			B10001000,
			B01111000,
			B00001000,
			B01110000,
			//Lower h
			B10000000,
			B10000000,
			B10000000,
			B11110000,
			B10001000,
			B10001000,
			B10001000,
			//Lower i
			B00100000,
			B00000000,
			B01100000,
			B00100000,
			B00100000,
			B00100000,
			B01110000,
			//Lower j
			B00010000,
			B00000000,
			B00110000,
			B00010000,
			B00010000,
			B10010000,
			B01100000,
			//Lower k
			B10000000,
			B10000000,
			B10001000,
			B10010000,
			B11100000,
			B10010000,
			B10001000,
			//Lower l
			B01100000,
			B00100000,
			B00100000,
			B00100000,
			B00100000,
			B00100000,
			B01110000,
			//Lower m
			B00000000,
			B00000000,
			B11110000,
			B10101000,
			B10101000,
			B10101000,
			B10101000,
			//Lower n
			B00000000,
			B00000000,
			B11110000,
			B10001000,
			B10001000,
			B10001000,
			B10001000,
			//Lower o
			B00000000,
			B00000000,
			B01110000,
			B10001000,
			B10001000,
			B10001000,
			B01110000,
			//Lower p
			B00000000,
			B00000000,
			B11110000,
			B10001000,
			B11110000,
			B10000000,
			B10000000,
			//Lower q
			B00000000,
			B00000000,
			B01111000,
			B10001000,
			B01111000,
			B00001000,
			B00001000,
			//Lower r
			B00000000,
			B00000000,
			B10110000,
			B11001000,
			B10000000,
			B10000000,
			B10000000,
			//Lower s
			B00000000,
			B00000000,
			B01111000,
			B10000000,
			B01110000,
			B00001000,
			B11110000,
			//Lower t
			B00100000,
			B01110000,
			B00100000,
			B00100000,
			B00100000,
			B00100000,
			B00011000,
			//Lower u
			B00000000,
			B00000000,
			B10001000,
			B10001000,
			B10001000,
			B10011000,
			B01101000,
			//Lower v
			B00000000,
			B00000000,
			B10001000,
			B10001000,
			B10001000,
			B01010000,
			B00100000,
			//Lower w
			B00000000,
			B00000000,
			B10001000,
			B10001000,
			B10101000,
			B10101000,
			B01111000,
			//Lower x
			B00000000,
			B00000000,
			B10001000,
			B01010000,
			B00100000,
			B01010000,
			B10001000,
			//Lower y
			B00000000,
			B00000000,
			B10001000,
			B10001000,
			B01111000,
			B00001000,
			B01110000,
			//Lower z
			B00000000,
			B00000000,
			B11111000,
			B00010000,
			B00100000,
			B01000000,
			B11111000,
		};
	#else
		const uint8_t *aBitImg_LowLetters_5x7 = aBitImg_CapLetters_5x7;
	#endif

	/**	@brief		Constant holding numeric digits for the display font
		@details	Each character is 5 bits wide and 7 bits tall.  Making each letter
			use 7 bytes.  The last 3 bits in each byte are not needed.  It is expected
			that when rendered the bits that are off will not be drawn.
		@ingroup	oled1306
	*/
	const uint8_t aBitImg_Digits_5x7[] = {
		//0
		B01110000,
		B10001000,
		B10011000,
		B10101000,
		B11001000,
		B10001000,
		B01110000,
		//1
		B00100000,
		B01100000,
		B10100000,
		B00100000,
		B00100000,
		B00100000,
		B11111000,
		//2
		B01110000,
		B10001000,
		B00001000,
		B00110000,
		B01000000,
		B10000000,
		B11111000,
		//3
		B01110000,
		B10001000,
		B00001000,
		B00110000,
		B00001000,
		B10001000,
		B01110000,
		//4
		B00010000,
		B00110000,
		B01010000,
		B10010000,
		B11111000,
		B00010000,
		B00010000,
		//5
		B11111000,
		B10000000,
		B01110000,
		B00001000,
		B00001000,
		B10001000,
		B01110000,
		//6
		B01110000,
		B10001000,
		B10000000,
		B11110000,
		B10001000,
		B10001000,
		B01110000,
		//7
		B11111000,
		B00001000,
		B00010000,
		B00100000,
		B00100000,
		B00100000,
		B00100000,
		//8
		B01110000,
		B10001000,
		B10001000,
		B01110000,
		B10001000,
		B10001000,
		B01110000,
		//9
		B01110000,
		B10001000,
		B10001000,
		B01111000,
		B00001000,
		B10001000,
		B01110000,
	};

/***** Definitions *****/
	/** @brief		Enumeration of all commands and default values needed for the SSD1306 Screen
		@ingroup	oled1306
	*/
	enum eSSD1306Commands_t {
		OLED_DisplayOff				= 0xAE,	/**< Display goes to sleep mode, all pixels off */
		OLED_DisplayOn				= 0xAF,	/**< Display is on, contents of GDDRAM are drawn */
		OLED_ComScanIncr			= 0xC0,
		OLED_ComScanDecr			= 0xC8,
		OLED_NormalDisplay			= 0xA6,	/**< Data of 1 indicates On pixel */
		OLED_InverseDisplay			= 0xA7,	/**< Data of 0 indicates On pixel */
		OLED_DisplayAllOn			= 0xA5,	/**< Ignore contents of GDDRAM and turn all pixels on */
		OLED_DisplayResume			= 0xA4,	/**< Resume displaying GDDRAM contents from All On command */
		OLED_SegmentRemapColAddr	= 0xA0,
		OLED_SegmentRemapSegDriv	= 0xA1,
		OLED_DeactivateScroll		= 0x2E,	/**< Stops the motion of the scrolling */
		OLED_ActivateScroll			= 0x2F,	/**< Starts the motion of the scrolling */

		OLED_DisplayStartLine		= 0x40,	/**< Sets the start line from 0 to 63, bit OR this value with the line number to use */

		OLED_SetContrast			= 0x81,	/** Specifies the brightness of the On pixels, takes a parameter that is (no contrast )0x00 to 0xFF (max contrast) */
		OLED_MultiplexRatio			= 0xA8,	/**< Sets the number of rows of pixels, takes a parameter from 16 to 63, default value is the number of rows - 1 */
		OLED_DisplayOffset			= 0xD3,	/**< Specifies the mapping of the display, takes a parameter that specifies the start line from 0 to 63 */

		OLED_DisplayClockDivide		= 0xD5,	/**< Set display clock divide ratio / Oscilator frequency, takes a parameter that is the ratio */
		OLED_ClockDivideDefault		= 0x80,	/**< Suggested clock divide ratio */

		OLED_SetChargePump			= 0x8D,	/**< Enables/Disables the charge pump, it must be on if the 5V VCC is used */
		OLED_ChargePumpOn			= 0x14,	/**< Charge pump parameter to enable */
		OLED_ChargePumpOff			= 0x10,	/**< Charge pump parameter to disable */

		OLED_SetComPins				= 0xDA,	/**< Specifies how the data should be transfered to the screen, the order pixels will be set, accepts a paramter */
		OLED_SetComPinsDefault		= 0x02,

		OLED_SetPreCharge			= 0xD9,	/**< Set the duration of the pre-charge period, takes a parameter of the number of DCLK */

		OLED_SetVComDeselect		= 0xDB, /**< Adjusts the VCom regulator output, takes a parameter that is the new level */

		OLED_SetPageAddress			= 0x22,	/**< Specify the row number that will be the start of the next data stream.  Takes two parameters first is the start row, second is the end row */

		OLED_SetColumnAddress		= 0x21,	/**< Specify the column number that will be the start of the next data stream.  Takes two parameters first is the start column, second is the end column */

		OLED_SetMemoryMode			= 0x20,	/** Specifies how data recieved is written to the GDDRAM */
		OLED_MemoryModeDefault		= 0x00,	/**< Draws to the screen in blocks that are 8 pixels tall and 16 pixels wide, left to right then top to bottom */
	};

	/**	@brief		Structure to hold a block of pixel data for the OLED screen
		@details	The screen is drawn to in 128 pixel blocks.  Each block is 16 columns wide
			by 8 rows tall.  Each byte transmited is 8 pixels in a column, least significant
			bit is the topmost row.  Subsequent bytes are columns moving from left to right.

			This block holds all of the bytes for one of these blocks arranged by the 8 bits 
			in a column.
		@ingroup	oled1306
	*/
	struct sScreenBlock_t {
		uint8_t nColBits[16];
	};

	/**	@brief		Class containing all operations needed for the 1306 screen
		@details	This class will initialize RAM variables, including dynamically allocating
			1024 bytes of RAM, needed by the operations.  It will initialize the hardware when
			the Begin() function is called.

			Then the various drawing functions can be used to render an image in the allocated
			buffer.  Once the SendToScree() function is called the created drawing will be 
			passed to the hardware.
		@ingroup	oled1306
	*/
	class SSD1306 {
		public:
			/**	@brief		Constructor that prepates an instance of the class
				@details	This only prepares the class, it does not aattempt to 
					communicate with the screen.  This allows the class to be 
					instantiated in global space before the Arduino Setup function has
					ran to prepare the I/O pins.
			*/
			SSD1306();
			
			/**	@brief		Deonstructor to clean up any allocated resources
				@details	The class allocates RAM for the drawing buffer, this ensures
					that this memory is freed
			*/
			~SSD1306();
			
			/**	@brief		Setup the pins needed to communicate with the screen via I2C bus
				@param 		pI2C		Pointer to the I2C bus controller to communicate through
				@param		nWidth		The width in pixels of the connected screen
				@param		nHeight		The height in pixes of the connected screen
				@param		nResetPin	The binary I/O pin connected to the screen reset
			*/
			void BeginI2C(TwoWire *pI2C, bool bSA0Bit, uint8_t nWidth, uint8_t nHeight, uint8_t nResetPin = OLED1306_NOPIN);
			
			/**	@brief		Setup the pins needed to communicate with the screen via SPI bus
				@param 		pSpi		Pointer to the SPI bus controller to communicate through
				@param		nWidth		The width in pixels of the connected screen
				@param		nHeight		The height in pixes of the connected screen
				@param		nDCPin		The binary I/O pin connected to the screen data
					or command select
				@param		nCSPin		The binary I/O pin connected to the screen
					chip select.
				@param		nResetPin	The binary I/O pin connected to the screen reset
			*/
			void BeginSPI(SPIClass *pSpi, uint8_t nWidth, uint8_t nHeight, uint8_t nDCPin, uint8_t nCSPin, uint8_t nResetPin = OLED1306_NOPIN);
			
			void Reset();
			void SendToScreen();
			void ClearDrawing();
			void DrawPixel(uint8_t nXCoord, uint8_t nYCoord, bool bSetOn);
			void DrawLine(uint8_t nXStart, uint8_t nYStart, uint8_t nXEnd, uint8_t nYEnd, bool bSetOn);
			void DrawImage_ByteMap(uint8_t nXLeft, uint8_t nYTop, uint8_t nWidth, uint8_t nHeight, const uint8_t *aImageData);
			void DrawImage_BitMap(uint8_t nXLeft, uint8_t nYTop, uint8_t nDataWidth, uint8_t nDataHeight, bool bDrawZeroes, const uint8_t *aImageData);
			void DrawImageDblSize_BitMap(uint8_t nXLeft, uint8_t nYTop, uint8_t nDataWidth, uint8_t nDataHeight, bool bDrawZeroes, const uint8_t *aImageData);
			void DrawText(uint8_t nXLeft, uint8_t nYTop, char *Text);
			void DrawTextDblSize(uint8_t nXLeft, uint8_t nYTop, char *Text);
			void InvertColors(bool bInvertOn);

		protected:
			/**	@brief		Width in pixels of the connected screen
			*/
			uint8_t cnScreenWidth;
			/**	@brief		Height in pixels of the connected screen
			*/
			uint8_t cnScreenHeight;

			/**	@brief		Chip select digital IO pin
				@details	Normally high, set to low to select the chip to receive
					SPI commands and data.
				@ingroup	oled1306
			*/
			uint8_t cnCSPin;

			/**	@brief		Chip select digital IO pin
				@details	Normally high, set to low to reset the chip.  It takes 
					10 milliseconds for the chip to finish restarting.
				@ingroup	oled1306
			*/
			uint8_t cnResetPin;

			/**	@brief		Place the chip in Data or Command mode
				@details	When High the chip will interpret bytes from the SPI 
					bus as data to be written to GDDRAM.  When Low the chip will 
					treat the SPI data as commands.
				@ingroup	oled1306
			*/
			uint8_t cnDataCmdPin;

			/**	@brief		Pointer to the allocated buffer holding the drawing
				@details	This buffer will be allocated at run time to hold the drawing 
					while it is being created until it is sent to the screen.  The blocks
					are arranged in 16 byte chunks.  The screen will be configured to 
					accept bytes from left to right until then top to bottom.

					However, each byte will be used vertically.  The least significant bit 
					in the topmost row of the screen, the most significant bit 7 rows below
					that.  Each byte spans 8 rows.  

					This could be done with each block being the full width of the screen,
					but it was broken into chunks in the event that different width screens
					were to be supported.
			*/
			sScreenBlock_t *caBlocks;

			/**	@brief		Number of data blocks in the allocated space
			*/
			uint8_t cnBlocksCnt;

			/**	@brief		Pointer to the SPI interface class for the Arduino 
			*/
			SPIClass *cpSpi;
			
			TwoWire *cpI2C;
			uint8_t cnI2CAddr;

			void SendRegisterValue(uint8_t nRegister, uint8_t nValue);
			void SendByte(uint8_t nByte);
			void SendDataBlock(uint16_t nNumBytes, void *pDataBlock, bool bIsDisplayData = false);

		private:
			bool DeviceSetup();
	};

/***** Globals     *****/


/***** Prototypes  *****/


/***** Functions   *****/
	SSD1306::SSD1306() {
		return;
	}

	SSD1306::~SSD1306() {
		if (caBlocks != NULL) {
			free(caBlocks);
		}
	}

	void SSD1306::BeginI2C(TwoWire *pI2C, bool bSA0Bit, uint8_t nWidth, uint8_t nHeight, uint8_t nResetPin) {
		//Intialize class variables
		cnScreenWidth = nWidth;
		cnScreenHeight = nHeight;
		cnCSPin = OLED1306_NOPIN;
		cnDataCmdPin = OLED1306_NOPIN;
		cnResetPin = nResetPin;
		cnBlocksCnt = cnScreenHeight * cnScreenWidth / 8 / 16; //Number of pixels divide by bytes then by block size
		cpSpi = NULL;
		cpI2C = pI2C;
		
		cnI2CAddr = OLED1306_I2CADDRBASE;
		
		if (bSA0Bit == true) {
			cnI2CAddr |= OLED1306_I2CSA0BIT;
		}
		
		DeviceSetup();
		
		return;
	}
	
	void SSD1306::BeginSPI(SPIClass *pSpi, uint8_t nWidth, uint8_t nHeight, uint8_t nDCPin, uint8_t nCSPin, uint8_t nResetPin) {
		//Intialize class variables
		cnScreenWidth = nWidth;
		cnScreenHeight = nHeight;
		cnCSPin = nCSPin;
		cnDataCmdPin = nDCPin;
		cnResetPin = nResetPin;
		cnBlocksCnt = cnScreenHeight * cnScreenWidth / 8 / 16; //Number of pixels divide by bytes then by block size
		cpSpi = pSpi;
		cpI2C = NULL;
		
		//Setup control pins
		pinMode(cnCSPin, OUTPUT);
		pinMode(cnDataCmdPin, OUTPUT);
		
		digitalWrite(cnCSPin, HIGH);
		digitalWrite(cnDataCmdPin, HIGH);
		
		DeviceSetup();

		return;
	}
	
	void SSD1306::ClearDrawing() {
		memset(caBlocks, 0, sizeof(sScreenBlock_t) * cnBlocksCnt);

		return;
	}

	void SSD1306::DrawPixel(uint8_t nXCoord, uint8_t nYCoord, bool bSetOn) {
		uint8_t nBlockIdx = 0;
		uint8_t nByteIdx = 0;
		uint8_t DrawBit;
		
		if ((nXCoord >= cnScreenWidth) || (nYCoord >= cnScreenHeight)) {
			//Some coordinate is out of range, unable to draw that
			return;
		}

		nBlockIdx = (nYCoord / 8) * 8; //This is the block row number (each row is 8 blocks)
		nBlockIdx += nXCoord / 16; //This is the block column number

		nByteIdx = nXCoord % 16;

		DrawBit = 1 << (nYCoord % 8);

		if (bSetOn == true) {
			caBlocks[nBlockIdx].nColBits[nByteIdx] |= DrawBit;
		} else {
			caBlocks[nBlockIdx].nColBits[nByteIdx] &= ~DrawBit;
		}
	}

	void SSD1306::DrawLine(uint8_t nXStart, uint8_t nYStart, uint8_t nXEnd, uint8_t nYEnd, bool bSetOn) {
		int16_t nRise, nRun;
		int8_t nXStep, nYStep;
		uint8_t nXCoord, nYCoord;
		float nYNew;

		if ((nXStart >= cnScreenWidth) || (nXEnd >= cnScreenWidth)) { //An X coordinate is out of bounds
			return;
		}

		if ((nYStart >= cnScreenHeight) || (nYEnd >= cnScreenHeight)) { //An X coordinate is out of bounds
			return;
		}

		nRun = nXEnd - nXStart;
		nRise = nYEnd - nYStart;

		if (nXEnd > nXStart) { //Line goes Left to Right
			nXStep = 1;
		} else { //Line goes from Right to Left
			nXStep = -1;
		}

		if (nYEnd > nYStart) { //Line goes Top to Bottom
			nYStep = 1;
		} else { //Line goes from Bottom to Top
			nYStep = -1;
		}

		nXCoord = nXStart;
		nYCoord = nYStart;
		DrawPixel(nXCoord, nYCoord, bSetOn); //Make sure the starting point is drawn
		while (nXCoord != nXEnd) {
			nXCoord += nXStep; //Always advance the X coordinate

			//Calculate the new Y coordinate
			nYNew = nYStart + (nRise * (nXCoord - nXStart) / nRun);

			//Draw the first pixel of the segment
			DrawPixel(nXCoord, nYCoord, bSetOn);
			while (nYCoord != (uint8_t)nYNew) { //Draw the vertical portion
				nYCoord += nYStep;
				DrawPixel(nXCoord, nYCoord, bSetOn);
			}
		}

		return;
	}

	void SSD1306::DrawImage_ByteMap(uint8_t nXLeft, uint8_t nYTop, uint8_t nWidth, uint8_t nHeight, const uint8_t *aImgData) {
		uint8_t nRowCtr, nColCtr, nXCoord, nYCoord;
		uint16_t nIndex;

		for (nRowCtr = 0; nRowCtr < nHeight; nRowCtr++) {
			//Set coords for first pixel in the row
			nXCoord = nXLeft;
			nYCoord = nYTop + nRowCtr;

			nIndex = nWidth * nRowCtr;
			for (nColCtr = 0; nColCtr < nWidth; nColCtr++) {
				if (aImgData[nIndex] == 0) {
					DrawPixel(nXCoord, nYCoord, false);
				} else {
					DrawPixel(nXCoord, nYCoord, true);
				}

				nXCoord += 1; //Advance to next pixel
				nIndex += 1; //Advance to next byte in image data
			}
		}

		return;
	}

	void SSD1306::DrawImage_BitMap(uint8_t nXLeft, uint8_t nYTop, uint8_t nDataWidth, uint8_t nDataHeight, bool bDrawZeroes, const uint8_t *aImageData) {
		uint8_t nRowCtr, nByteCtr, nXCoord, nYCoord, nCurrBit, nCurrByte;
		uint16_t nIndex;

		for (nRowCtr = 0; nRowCtr < nDataHeight; nRowCtr++) {
			//Set coords for the first pixel i nthe row
			nXCoord = nXLeft;
			nYCoord = nYTop + nRowCtr;

			nIndex = nDataWidth * nRowCtr;

			for (nByteCtr = 0; nByteCtr < nDataWidth; nByteCtr++) {
				nCurrBit = 0x80; //Mask to start with the left-most bit
				nCurrByte = aImageData[nIndex];

				while (nCurrBit != 0) {
					if ((nCurrByte & nCurrBit) != 0) { //This pixel is On
						DrawPixel(nXCoord, nYCoord, true);
					} else if (bDrawZeroes == true) { //This pixel is off
						DrawPixel(nXCoord, nYCoord, false);
					}

					nXCoord += 1;
					nCurrBit >>= 1;
				}
				nIndex += 1; //Finished that byte, get the next
			}
		}

		return;
	}

	void SSD1306::DrawImageDblSize_BitMap(uint8_t nXLeft, uint8_t nYTop, uint8_t nDataWidth, uint8_t nDataHeight, bool bDrawZeroes, const uint8_t *aImageData) {
		uint8_t nRowCtr, nByteCtr, nXCoord, nYCoord, nCurrBit, nCurrByte;
		uint16_t nIndex;

		for (nRowCtr = 0; nRowCtr < nDataHeight; nRowCtr++) {
			//Set coords for the first pixel i nthe row
			nXCoord = nXLeft;
			nYCoord = nYTop + nRowCtr * 2;

			nIndex = nDataWidth * nRowCtr;

			for (nByteCtr = 0; nByteCtr < nDataWidth; nByteCtr++) {
				nCurrBit = 0x80; //Mask to start with the left-most bit
				nCurrByte = aImageData[nIndex];

				while (nCurrBit != 0) {
					if ((nCurrByte & nCurrBit) != 0) { //This pixel is On
						DrawPixel(nXCoord, nYCoord, true);
						DrawPixel(nXCoord + 1, nYCoord, true);
						DrawPixel(nXCoord, nYCoord + 1, true);
						DrawPixel(nXCoord + 1, nYCoord + 1, true);
					} else if (bDrawZeroes == true) { //This pixel is off
						DrawPixel(nXCoord, nYCoord, false);
						DrawPixel(nXCoord + 1, nYCoord, false);
						DrawPixel(nXCoord, nYCoord + 1, false);
						DrawPixel(nXCoord + 1, nYCoord + 1, false);
					}

					nXCoord += 2;
					nCurrBit >>= 1;
				}
				nIndex += 1; //Finished that byte, get the next
			}
		}

		return;
	}

	void SSD1306::DrawTextDblSize(uint8_t nXLeft, uint8_t nYTop, char *Text) {
		uint16_t nCharCtr, nIndex;
		uint8_t nXCoord, nYCoord;
		char Letter;

		nXCoord = nXLeft;
		nYCoord = nYTop;
		nCharCtr = 0;
		while (Text[nCharCtr] != '\0') {
			Letter = Text[nCharCtr];

			if ((Letter >= 'A') && (Letter <= 'Z'))  {
				nIndex = 7 * (Letter - 'A');
				DrawImageDblSize_BitMap(nXCoord, nYCoord, 1, 7, false, &(aBitImg_CapLetters_5x7[nIndex]));

				nXCoord += 10 + 2; //Font has 5 pixel wide characters, 1 pixel between letters
			} else if ((Letter >= 'a') && (Letter <= 'z'))  {
				nIndex = 7 * (Letter - 'a');
				DrawImageDblSize_BitMap(nXCoord, nYCoord, 1, 7, false, &(aBitImg_LowLetters_5x7[nIndex]));

				nXCoord += 10 + 2; //Font has 5 pixel wide characters, 1 pixel between letters
			} else if ((Letter >= '0') && (Letter <= '9')) {
				nIndex = 7 * (Letter - '0');
				DrawImageDblSize_BitMap(nXCoord, nYCoord, 1, 7, false, &(aBitImg_Digits_5x7[nIndex]));

				nXCoord += 10 + 2; //Font has 5 pixel wide characters, 1 pixel between letters
			}else if (Letter == '\n') {
				nXCoord = nXLeft; //Reset to start of line
				nYCoord += 14 + 2; //Font has 7 pixel tall characters, 1 pixel between lines
			} else { //Not aailable in the font, insert a spce
				nXCoord += 10 + 2; //Font has 5 pixel wide characters, 1 pixel between letters
			}

			nCharCtr += 1;
		}

		return;
	}

	void SSD1306::InvertColors(bool bInvertOn) {
		if (bInvertOn == true) {
			SendByte(OLED_InverseDisplay);
		} else {
			SendByte(OLED_NormalDisplay);
		}
	}
	
	void SSD1306::DrawText(uint8_t nXLeft, uint8_t nYTop, char *Text) {
		uint16_t nCharCtr, nIndex;
		uint8_t nXCoord, nYCoord;
		char Letter;

		nXCoord = nXLeft;
		nYCoord = nYTop;
		nCharCtr = 0;
		while (Text[nCharCtr] != '\0') {
			Letter = Text[nCharCtr];

			if ((Letter >= 'A') && (Letter <= 'Z'))  {
				nIndex = 7 * (Letter - 'A');
				DrawImage_BitMap(nXCoord, nYCoord, 1, 7, false, &(aBitImg_CapLetters_5x7[nIndex]));

				nXCoord += 5 + 1; //Font has 5 pixel wide characters, 1 pixel between letters
			} else if ((Letter >= 'a') && (Letter <= 'z'))  {
				nIndex = 7 * (Letter - 'a');
				DrawImage_BitMap(nXCoord, nYCoord, 1, 7, false, &(aBitImg_LowLetters_5x7[nIndex]));

				nXCoord += 5 + 1; //Font has 5 pixel wide characters, 1 pixel between letters
			} else if ((Letter >= '0') && (Letter <= '9')) {
				nIndex = 7 * (Letter - '0');
				DrawImage_BitMap(nXCoord, nYCoord, 1, 7, false, &(aBitImg_Digits_5x7[nIndex]));

				nXCoord += 5 + 1; //Font has 5 pixel wide characters, 1 pixel between letters
			}else if (Letter == '\n') {
				nXCoord = nXLeft; //Reset to start of line
				nYCoord += 7 + 1; //Font has 7 pixel tall characters, 1 pixel between lines
			} else { //Not aailable in the font, insert a spce
				nXCoord += 5 + 1; //Font has 5 pixel wide characters, 1 pixel between letters
			}

			nCharCtr += 1;
		}

		return;
	}

	void SSD1306::SendRegisterValue(uint8_t nRegister, uint8_t nValue) {
		uint8_t nI2CDataCmd;
		
		if (cpSpi != NULL) {
			cpSpi->beginTransaction(SPISettings(OLED1306_CLKFREQ, MSBFIRST, SPI_MODE0)); //Max speed of device, most significant bit first, Mode 0: Output falling edge data capture rising edge 

			digitalWrite(cnDataCmdPin, LOW); //Put chip in command mode
			digitalWrite(cnCSPin, LOW); //Select the chip

			cpSpi->transfer(nRegister); //Send register location
			cpSpi->transfer(nValue);  //Send value to record into register

			digitalWrite(cnCSPin, HIGH); //Unselect the chip

			cpSpi->endTransaction();
		} else if (cpI2C != NULL) {
			nI2CDataCmd = 0x00; //Co = 0, D/C = 0
			
			cpI2C->setClock(400000UL);
			
			cpI2C->beginTransmission(cnI2CAddr);
			cpI2C->write(nI2CDataCmd);
			cpI2C->write(nRegister);
			cpI2C->endTransmission();
			
			cpI2C->beginTransmission(cnI2CAddr);
			cpI2C->write(nI2CDataCmd);
			cpI2C->write(nValue);
			cpI2C->endTransmission();
		}

		return;
	}

	void SSD1306::SendByte(uint8_t nByte) {
		uint8_t nI2CDataCmd;
		
		if (cpSpi != NULL) {
			cpSpi->beginTransaction(SPISettings(OLED1306_CLKFREQ, MSBFIRST, SPI_MODE0)); //Max speed of device, most significant bit first, Mode 0: Output falling edge data capture rising edge 

			digitalWrite(cnDataCmdPin, LOW); //Put chip in command mode
			digitalWrite(cnCSPin, LOW); //Select the chip

			cpSpi->transfer(nByte);

			digitalWrite(cnCSPin, HIGH); //Unselect the chip

			cpSpi->endTransaction();
		} else if (cpI2C != NULL) {
			nI2CDataCmd = 0x00; //Co = 0, D/C = 0
			
			cpI2C->setClock(400000UL);
			
			cpI2C->beginTransmission(cnI2CAddr);
			cpI2C->write(nI2CDataCmd);
			cpI2C->write(nByte);
			cpI2C->endTransmission();
		}

		return;
	}

	void SSD1306::SendDataBlock(uint16_t nNumBytes, void *pDataBlock, bool bIsDisplayData) {
		int nCtr, nBytesSent;
		uint8_t *aByteArray = (uint8_t *)pDataBlock;

		if (cpI2C != NULL) {
			cpI2C->setClock(400000UL);
			
			cpI2C->beginTransmission(cnI2CAddr);
			if (bIsDisplayData == false) {
				cpI2C->write(0x00);
			} else {
				cpI2C->write(OLED1306_I2CDCBIT);
			}
			
			nBytesSent = 0;
		}
		
		for (nCtr = 0; nCtr < nNumBytes; nCtr++) {
			if (cpSpi != NULL) {
				cpSpi->transfer((*aByteArray));
			} else if (cpI2C != NULL) {
				cpI2C->write((*aByteArray));
				nBytesSent += 1;
				
				if (nBytesSent >= OLED1306_I2CBUFFLEN) {
					cpI2C->endTransmission();
					cpI2C->beginTransmission(cnI2CAddr);
					if (bIsDisplayData == false) {
						cpI2C->write(0x00);
					} else {
						cpI2C->write(OLED1306_I2CDCBIT);
					}
					
					nBytesSent = 0;
				}
			}

			aByteArray += 1; //Advance to the next byte
			
		}
		
		if (cpI2C != NULL) {
			cpI2C->endTransmission();
		}

		return;
	}

	void SSD1306::Reset() {
		if (cnResetPin != OLED1306_NOPIN) { //Reset pin is connected, use that
			digitalWrite(cnResetPin, HIGH);
			delay(1);
			digitalWrite(cnResetPin, LOW);
			delay(1000);
			digitalWrite(cnResetPin, HIGH);
		} else { //No hardware reset, try a command
			SendByte(OLED_DisplayOff);
			
			ClearDrawing();
			SendToScreen();
			
			SendByte(OLED_DisplayOn);
		}

		return;
	}

	void SSD1306::SendToScreen() {
		uint8_t Cmds[3];

		if (cpSpi != NULL) {
			cpSpi->beginTransaction(SPISettings(OLED1306_CLKFREQ, MSBFIRST, SPI_MODE0)); //Max speed of device, most significant bit first, Mode 0: Output falling edge data capture rising edge 
			digitalWrite(cnDataCmdPin, LOW); //Command Mode
			digitalWrite(cnCSPin, LOW);
		}
		//All of this must be done in a single transaction or portions will be skipped
		
		//Make sure drawing always begins from top left corner
		Cmds[0] = OLED_SetPageAddress;
		Cmds[1] = 0x00; //Starting page
		Cmds[2] = 0xFF; //Ending page
		SendDataBlock(3, Cmds);

		Cmds[0] = OLED_SetColumnAddress;
		Cmds[1] = 0x00; //Starting column
		Cmds[2] = cnScreenWidth - 1; //Ending column
		SendDataBlock(3, Cmds);

		if (cpSpi != NULL) {
			digitalWrite(cnDataCmdPin, HIGH); //Data Mode
		}
		
		SendDataBlock(sizeof(sScreenBlock_t) * cnBlocksCnt, caBlocks, true);
		
		if (cpSpi != NULL) {
			digitalWrite(cnCSPin, HIGH);
			cpSpi->endTransaction();
		}
	
		return;
	}
	
	bool SSD1306::DeviceSetup() {
		caBlocks = (sScreenBlock_t *)malloc(sizeof(sScreenBlock_t) * cnBlocksCnt);
		if (caBlocks == NULL) {
			Serial.print("SSD1306 Driver: Drawing Buffer Allocation Failed!\n");
		}

		ClearDrawing();

		//Set reset pin mode
		if (cnResetPin != OLED1306_NOPIN) {
			pinMode(cnResetPin, OUTPUT);
			digitalWrite(cnResetPin, HIGH);
		}

		//Initialize the chip
		Reset();

		SendByte(OLED_DisplayOff);

		SendRegisterValue(OLED_DisplayClockDivide, OLED_ClockDivideDefault);
		SendRegisterValue(OLED_MultiplexRatio, cnScreenHeight - 1);
		SendRegisterValue(OLED_DisplayOffset, 0x00); //The first line is the start line
		SendByte(OLED_DisplayStartLine | 0x00); //Set start line to zero
		SendRegisterValue(OLED_SetChargePump, OLED_ChargePumpOn);
		SendRegisterValue(OLED_SetMemoryMode, 0x00);
		SendByte(OLED_SegmentRemapSegDriv);
		SendByte(OLED_ComScanDecr);
		
		if (cnScreenHeight > 32) {
			SendRegisterValue(OLED_SetComPins, 0x12);
		} else {
			SendRegisterValue(OLED_SetComPins, 0x02);
		}
		
		SendRegisterValue(OLED_SetContrast, 0xCF);

		SendRegisterValue(OLED_SetPreCharge, 0xF1); //Precharge 0x22 or 0xF1
		SendRegisterValue(OLED_SetVComDeselect, 0x40);
		SendByte(OLED_DeactivateScroll);
		
		SendByte(OLED_DisplayResume);
		SendByte(OLED_NormalDisplay);
		
		SendByte(OLED_DisplayOn);
		
		return true;
	}

#endif

