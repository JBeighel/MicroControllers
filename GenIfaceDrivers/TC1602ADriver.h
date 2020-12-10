/**	@defgroup	tc1602adriver
	@brief		
	@details	v0.2
	#Description
	
	#File Information
		File:	TC1602ADriver.h
		Author:	J. Beighel
		Date:	12-10-2020
*/

#ifndef __TC1602ADRIVER
	#define __TC1602ADRIVER

/*****	Includes	*****/
	#include "CommonUtils.h"
	#incldue "TimeGeneralInterface.h"
	#include "GPIOGeneralInterface.h"

/*****	Constants	*****/
	/**	@brief		GPIO Interface Capabilities needed by the TC1602A Driver
		@ingroup	tc1602adriver
	*/
	#define TC1602A_GPIOCAPS	(GPIOCap_DigitalWrite | GPIOCap_SetPinMode)
	
	/**	@brief		Time Interface Capabilities needed by the TC1602A Driver
		@ingroup	tc1602adriver
	*/
	#define TC1602A_TIMECAPS	(TimeCap_DelayMicroSec | TimeCap_DelayMilliSec)

	/**	@brief		Delay from stting data pins to leading edge of clock
		@details	Time in microseconds
		@ingroup	tc1602adriver
	*/
	#define TC1602A_ENPULSEPREUSEC	10
	
	/**	@brief		Delay from leading edge of clock to falling edge
		@details	Time in microseconds
		Pulse only needs 150 nanoseconds
		@ingroup	tc1602adriver
	*/
	#define TC1602A_ENPULSEONUSEC	2
	
	/**	@brief		Delay from trailing edge of clock to hold data pins
		@details	Time in microseconds
		Cycle needs 400 nanoseconds
		@ingroup	tc1602adriver
	*/
	#define TC1602A_ENPULSEPOSTUSEC	10
	
	/**	@brief		Delay from trailing edge of clock to hold data pins
		@details	Time in milliseconds
		Data sheet calls for 100 microseconds to 15 milliseconds
		@ingroup	tc1602adriver
	*/
	#define TC1602A_PULSEINITMSEC	20
	
	/**	@brief		Delay to allow screen clear command to complete
		@details	Time in milliseconds
		Data sheet calls for 3 milliseconds
		@ingroup	tc1602adriver
	*/
	#define TC1602A_DELAYCLEARMSEC	5
	
	/**	@brief		Delay to allow set cursor position command to complete
		@details	Time in microseconds
		@ingroup	tc1602adriver
	*/
	#define TC1602A_CURSORPOSMSEC	750

/*****	Definitions	*****/
	typedef enum eTC1602AReturn_t {
		TC1602AWarn_Unknown			= 1,	/**< An unknown but recoverable 
									error happened during the operation */
		TC1602A_Success				= 0,	/**< The operation completed 
									successfully */
		TC1602AFail_Unknown			= -1,	/**< An unknown and unrecoverable 
									error happened during the operation */
		TC1602AFail_InvalisPos		= -2,	/**< A cursor position given was 
									invalid */
	} eTC1602AReturn_t;

	typedef enum eTC1602ACmd_t {
		TC1602A_FS_4DB_1LINE_5X8	= 0x20,
		TC1602A_FS_4DB_1LINE_5X10	= 0x24,
		TC1602A_FS_4DB_2LINE_5X8	= 0x28,
		TC1602A_FS_4DB_2LINE_5X10	= 0x2C,
		TC1602A_FS_8DB_1LINE_5X8	= 0x30,
		TC1602A_FS_8DB_1LINE_5X10	= 0x34,
		TC1602A_FS_8DB_2LINE_5X8	= 0x38,
		TC1602A_FS_8DB_2LINE_5X10	= 0x3C,
		
		TC1602A_CLEARDISPLAY		= 0x01,
		TC1602A_RETURNHOME			= 0x02,
		TC1602A_ENTRYMODESETBASE	= 0x04, //LCD and OLED flip curosr advance direction
		TC1602A_EMS_FLIPCURSORADV	= 0x02,
		TC1602A_DISPLAYON			= 0x0C,
		TC1602A_DISPLAYOFF			= 0x08,
		
		TC1602A_SETCGRAMBASE		= 0x20,	/**< Character generator RAM */
		TC1602A_SETCGRAMADDRMASK	= 0x1F,	/**< Display data RAM */
		
		TC1602A_SETDDRAMBASE		= 0x80,
		TC1602A_SETDDRAMADDRMASK	= 0x7F,
		
		TC1602A_READREG_BUSYFFLAG	= 0x80,
		TC1602A_READREG_ADDRMASK	= 0x7F,
	} eTC1602ACmd_t;
	
	typedef struct sTC1602AInfo_t {
		sTimeIface_t *pTime;
		sGPIOIface_t *pGPIO;
		uint16_t nRegSelPin;
		uint16_t nRdWrPin;
		uint16_t nEnablePin;
		uint16_t anDataPins[8];
		uint8_t anRowBases[4];
		uint8_t nColCnt;
		uint8_t nRowCnt;
		bool bUse8DataPins;
	} sTC1602AInfo_t;

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
		@ingroup	tc1602adriver
	*/
	eTC1602AReturn_t TC1602AInit4Data(sTC1602AInfo_t *pDev, sTimeIface_t *pTime, sGPIOIface_t *pGpio, bool bFlipCursorDir, uint8_t nColCnt, uint8_t nRowCnt, uint16_t nEnPin, uint16_t nReadWritePin, uint16_t nRegSelPin, uint16_t nData4, uint16_t nData5, uint16_t nData6, uint16_t nData7);
	
	eTC1602AReturn_t TC1602AInit8Data(sTC1602AInfo_t *pDev, sTimeIface_t *pTime, sGPIOIface_t *pGpio, bool bFlipCursorDir, uint8_t nColCnt, uint8_t nRowCnt, uint16_t nEnPin, uint16_t nReadWritePin, uint16_t nRegSelPin, uint16_t nData0, uint16_t nData1, uint16_t nData2, uint16_t nData3, uint16_t nData4, uint16_t nData5, uint16_t nData6, uint16_t nData7);
	
	eTC1602AReturn_t TC1602AInitClearDisplay(sTC1602AInfo_t *pDev);
	
	eTC1602AReturn_t TC1602AInitDisplayOn(sTC1602AInfo_t *pDev);
	
	eTC1602AReturn_t TC1602AInitDisplayOff(sTC1602AInfo_t *pDev);
	
	eTC1602AReturn_t TC1602AInitSetCursorPosition(sTC1602AInfo_t *pDev, uint8_t nCol, uint8_t nRow);
	
	eTC1602AReturn_t TC1602AInitPrintCharacter(sTC1602AInfo_t *pDev, char Letter);

/*****	Functions	*****/


#endif

