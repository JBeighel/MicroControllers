/**	@defgroup	rc522driver
	@brief		Peripheral driver for the RC522 RFID reader
	@details	v0.1
	# Description #

	# Usage #

	# File Information #
		File:	RC522Driver.h
		Author:	J. Beighel
		Created:09-04-2020
*/

#ifndef __RC522DRIVER_H
	#define __RC522DRIVER_H

/*****	Includes	*****/
	#include "GPIOGeneralInterface.h"
	#include "SPIGeneralInterface.h"

/*****	Definitions	*****/
	/**	@brief		SPI interface capabilities required by the RC522 driver
		@ingroup	rc522driver
	*/
	#define RC522_SPICAPS	(SPI_BeginTransfer | SPI_EndTransfer | SPI_BiDir1Byte)

	typedef enum eRC522Return_t {
		RC522Warn_Unknown		= 1,	/**< An unknown but recoverable error happened during the operation */
		RC522_Success			= 0,	/**< The operation completed successfully */
		RC522Fail_Unknown		= -1,	/**< An unknown and unrecoverable error happened during the operation */
		RC522Fail_NotFound		= -2,	/**< The device did not respond to self identification requests */
		RC522Fail_BusFailure	= -3,	/**< The communications bus reported a failure */
	} eRC522Return_t;

	typedef enum eRC522Addr_t {
		RC522AddrBase			= 0x28,
		RC522Addr2				= 0x04,
		RC522Addr1				= 0x02,
		RC522Addr0				= 0x01,
	} eRC522Addr_t;

	typedef enum eRC522Reg_t {
		//Command and status
		RC522Reg_Command		= 0x01,
		RC522Reg_ComIEn			= 0x02,
		RC522Reg_DivIEn			= 0x03,
		RC522Reg_ComIrq			= 0x04,
		RC522Reg_DivIrq			= 0x05,
		RC522Reg_Error			= 0x06,
		RC522Reg_Status1		= 0x07,
		RC522Reg_Status2		= 0x08,
		RC522Reg_FIFOData		= 0x09,
		RC522Reg_FIFOLevel		= 0x0A,
		RC522Reg_WaterLevel		= 0x0B,
		RC522Reg_Control		= 0x0C,
		RC522Reg_BitFraming		= 0x0D,
		RC522Reg_Coll			= 0x0E,

		//Command
		RC522Reg_Mode			= 0x11,
		RC522Reg_TxMode			= 0x12,
		RC522Reg_RxMode			= 0x13,
		RC522Reg_TxControl		= 0x14,
		RC522Reg_TXASK			= 0x15,
		RC522Reg_TxSel			= 0x16,
		RC522Reg_RxSel			= 0x17,
		RC522Reg_RxThreshold	= 0x18,
		RC522Reg_Demod			= 0x19,

		RC522Reg_MfTx			= 0x1C,
		RC522Reg_MfRx			= 0x1D,

		RC522Reg_SerialSpeed	= 0x1F,

		//Configuration
		RC522Reg_CRCMSB			= 0x21,
		RC522Reg_CRCLSB			= 0x22,

		RC522Reg_ModWidth		= 0x24,

		RC522Reg_RFCfg			= 0x26,
		RC522Reg_GsN			= 0x27,
		RC522Reg_CWGsP			= 0x28,
		RC522Reg_ModGsP			= 0x29,
		RC522Reg_TMode			= 0x2A,
		RC522Reg_TPrescaler		= 0x2B,
		RC522Reg_TReloadMSB		= 0x2C,
		RC522Reg_TReloadLSB		= 0x2D,
		RC522Reg_TCntrValMSB	= 0x2E,
		RC522Reg_TCntrValLSB	= 0x2F,

		//Test register
		RC522Reg_TestSel1		= 0x30,
		RC522Reg_TestSel2		= 0x31,
		RC522Reg_TestPinEn		= 0x33,
		RC522Reg_TestPinVal		= 0x34,
		RC522Reg_TestBus		= 0x35,
		RC522Reg_AutoTest		= 0x36,
		RC522Reg_Version		= 0x37,
		RC522Reg_AnalogTest		= 0x36,
		RC522Reg_TestDAC1		= 0x39,
		RC522Reg_TestDAC2		= 0x3A,
		RC522Reg_TestADC		= 0x3B,

		RC522Reg_SPIRead		= 0x80,
		RC522Reg_SPIWrite		= 0x00,
		RC522Reg_SPILShift		= 1,
	} eRC522Reg_t;

	typedef enum eRC522RegVersion_t {
		RC522RegVer_ChipMask	= 0xF0,
		RC522RegVer_ChipMFRC522	= 0x90,

		RC522RegVer_SWMask		= 0x0F,
		RC522RegVer_SWVer1		= 0x01,
		RC522RegVer_SWVer2		= 0x02,
	} eRC522RegVersion_t;

	typedef struct sRC522Obj_t {
		sSPIIface_t *pSpi;
		sGPIOIface_t *pGPIO;
		uint16_t nCSPin;
	} sRC522Obj_t;

/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	eRC522Return_t RC522Initialize(sRC522Obj_t *pRC522, sSPIIface_t *pSpiObj, sGPIOIface_t *pGPIOObj, uint16_t nChipSelectPin);

/*****	Functions	*****/


#endif
