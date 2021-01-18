/**	@defgroup	us2066driver
	@brief		Driver for the US2066 OLED/PLED Segment/Common Driver with Controller
	@details	v0.2
	#Description
		This controller is used in the New Haven OLED 4x20 character displays.  
		It functions almost identically to the TC1602A controller so that driver
		was used as the base for this.  The only change was the row bases values.
		
	#File Information
		File:	US2066Driver.h
		Author:	J. Beighel
		Date:	2021-01-18
*/

#ifndef __US2066DRIVER
	#define __US2066DRIVER

/*****	Includes	*****/
	#include "CommonUtils.h"
	#include "TimeGeneralInterface.h"
	#include "GPIOGeneralInterface.h"
	#include "SPIGeneralInterface.h"

/*****	Constants	*****/
	/**	@brief		GPIO Interface Capabilities needed by the US2066 Driver
		@ingroup	us2066driver
	*/
	#define US2066_GPIOCAPS	(GPIOCap_DigitalWrite | GPIOCap_SetPinMode)
	
	/**	@brief		Time Interface Capabilities needed by the US2066 Driver
		@ingroup	us2066driver
	*/
	#define US2066_TIMECAPS	(TimeCap_DelayMicroSec | TimeCap_DelayMilliSec)

	/**	@brief		Delay from stting data pins to leading edge of clock
		@details	Time in microseconds
		@ingroup	us2066driver
	*/
	#define US2066_ENPULSEPREUSEC	10
	
	/**	@brief		Delay from leading edge of clock to falling edge
		@details	Time in microseconds
		Pulse only needs 150 nanoseconds
		@ingroup	us2066driver
	*/
	#define US2066_ENPULSEONUSEC	2
	
	/**	@brief		Delay from trailing edge of clock to hold data pins
		@details	Time in microseconds
		Cycle needs 400 nanoseconds
		@ingroup	us2066driver
	*/
	#define US2066_ENPULSEPOSTUSEC	10
	
	/**	@brief		Delay from trailing edge of clock to hold data pins
		@details	Time in milliseconds
		Data sheet calls for 100 microseconds to 15 milliseconds
		@ingroup	us2066driver
	*/
	#define US2066_PULSEINITMSEC	20
	
	/**	@brief		Delay to allow screen clear command to complete
		@details	Time in milliseconds
		Data sheet calls for 3 milliseconds
		@ingroup	us2066driver
	*/
	#define US2066_DELAYCLEARMSEC	5
	
	/**	@brief		Delay to allow set cursor position command to complete
		@details	Time in microseconds
		@ingroup	us2066driver
	*/
	#define US2066_CURSORPOSMSEC	750

/*****	Definitions	*****/
	typedef enum eUS2066Return_t {
		US2066Warn_Unknown			= 1,	/**< An unknown but recoverable 
									error happened during the operation */
		US2066_Success				= 0,	/**< The operation completed 
									successfully */
		US2066Fail_Unknown			= -1,	/**< An unknown and unrecoverable 
									error happened during the operation */
		US2066Fail_InvalisPos		= -2,	/**< A cursor position given was 
									invalid */
	} eUS2066Return_t;

	typedef enum eUS2066Cmd_t {
		US2066_FS_4DB_1LINE_5X8		= 0x20,
		US2066_FS_4DB_1LINE_5X10	= 0x24,
		US2066_FS_4DB_2LINE_5X8		= 0x28,
		US2066_FS_4DB_2LINE_5X10	= 0x2C,
		US2066_FS_8DB_1LINE_5X8		= 0x30,
		US2066_FS_8DB_1LINE_5X10	= 0x34,
		US2066_FS_8DB_2LINE_5X8		= 0x38,
		US2066_FS_8DB_2LINE_5X10	= 0x3C,
		
		US2066_CLEARDISPLAY			= 0x01,
		US2066_RETURNHOME			= 0x02,
		US2066_ENTRYMODESETBASE		= 0x04, //LCD and OLED flip curosr advance direction
		US2066_EMS_FLIPCURSORADV	= 0x02,
		US2066_DISPLAYON			= 0x0C,
		US2066_DISPLAYOFF			= 0x08,
		
		US2066_SETCGRAMBASE			= 0x20,	/**< Character generator RAM */
		US2066_SETCGRAMADDRMASK		= 0x1F,	/**< Display data RAM */
		
		US2066_SETDDRAMBASE			= 0x80,
		US2066_SETDDRAMADDRMASK		= 0x7F,
		
		US2066_READREG_BUSYFFLAG	= 0x80,
		US2066_READREG_ADDRMASK		= 0x7F,
	} eUS2066Cmd_t;
	
	typedef struct sUS2066Info_t {
		sTimeIface_t *pTime;
		sGPIOIface_t *pGPIO;
		sSPIIface_t *pSPI;
		GPIOID_t nRegSelPin;
		GPIOID_t nRdWrPin;
		GPIOID_t nEnablePin;	/**< Enable pin in parallel mode and chip select in SPI mode */
		GPIOID_t anDataPins[8];
		uint8_t anRowBases[4];
		uint8_t nColCnt;
		uint8_t nRowCnt;
		bool bUse8DataPins;
	} sUS2066Info_t;

/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	/**	@brief		Initialize the display driver chip
		@details	Sets up the pins used to transfer data to the driver chip 
			as well as initializes the chip to prepare it for use.
		@param		pDev			Pointer to the device information structure
		@param		pTime			Pointer to time interface object to use
		@param		pGpio			Pointer to GPIO interface object to use
		@param		bFlipCursorDir	Some versions advance thr cursor different 
			directions after drawing a character.  This will reverse the 
			direction the cursor moves
		@param		nColCnt			Number of character columns the display has
		@param		nRowCnt			Number of character rows the display has
		@param		nEnPin			GPIO pin connected to Enable pin
		@param		nReadWritePin	GPIO pin connected to Read/Write pin
		@param		nRegSelPin		GPIO pin connected to register select pin
		@param		nData4			GPIO pin connected to data bit 4
		@param		nData5			GPIO pin connected to data bit 5
		@param		nData6			GPIO pin connected to data bit 6
		@param		nData7			GPIO pin connected to data bit 7
		@return		US2066_Success on success or a code indicating the error
		@ingroup	us2066driver
	*/
	eUS2066Return_t US2066Init4Data(sUS2066Info_t *pDev, sTimeIface_t *pTime, sGPIOIface_t *pGpio, bool bFlipCursorDir, uint8_t nColCnt, uint8_t nRowCnt, GPIOID_t nEnPin, GPIOID_t nReadWritePin, GPIOID_t nRegSelPin, GPIOID_t nData4, GPIOID_t nData5, GPIOID_t nData6, GPIOID_t nData7);
	
	eUS2066Return_t US2066Init8Data(sUS2066Info_t *pDev, sTimeIface_t *pTime, sGPIOIface_t *pGpio, bool bFlipCursorDir, uint8_t nColCnt, uint8_t nRowCnt, GPIOID_t nEnPin, GPIOID_t nReadWritePin, GPIOID_t nRegSelPin, GPIOID_t nData0, GPIOID_t nData1, GPIOID_t nData2, GPIOID_t nData3, GPIOID_t nData4, GPIOID_t nData5, GPIOID_t nData6, GPIOID_t nData7);
	
	eUS2066Return_t US2066InitSPI(sUS2066Info_t *pDev, sTimeIface_t *pTime, sGPIOIface_t *pGpio, sSPIIface_t *pSpi, uint8_t nColCnt, uint8_t nRowCnt, GPIOID_t nChipSelPin);
	
	/**	@brief		Clears all text displayed on the screen
		@param		pDev	Pointer to the peripheral object
		@return		US2066_Success on success or a code indicating the error
		@ingroup	us2066driver
	*/
	eUS2066Return_t US2066ClearDisplay(sUS2066Info_t *pDev);
	
	/**	@brief		Turns teh display on
		@param		pDev	Pointer to the peripheral object
		@return		US2066_Success on success or a code indicating the error
		@ingroup	us2066driver
	*/
	eUS2066Return_t US2066DisplayOn(sUS2066Info_t *pDev);
	
	/**	@brief		Turns teh display off
		@details	Does not clear the display, when it is turned back on the
			previous contents will be shown
		@param		pDev	Pointer to the peripheral object
		@return		US2066_Success on success or a code indicating the error
		@ingroup	us2066driver
	*/
	eUS2066Return_t US2066DisplayOff(sUS2066Info_t *pDev);
	
	eUS2066Return_t US2066SetCursorPosition(sUS2066Info_t *pDev, uint8_t nCol, uint8_t nRow);
	
	eUS2066Return_t US2066PrintCharacter(sUS2066Info_t *pDev, char Letter);

/*****	Functions	*****/


#endif

