/**	@defgroup	rc522driver
	@brief		Peripheral driver for the RC522 RFID reader
	@details	v0.1
	# Description #

	# Usage #
		The device determines the bus it will be using by checking the I2C and EA pins on startup.  If
		the I2C pin is high, then the I2C bus will be used.  If I2C is low and EA is high then SPI will
		be used.  If both I2C and EA pins are low then UART will be used.

	# File Information #
		File:	RC522Driver.h
		Author:	J. Beighel
		Created:2021-07-22
*/

#ifndef __RC522DRIVER_H
	#define __RC522DRIVER_H

/*****	Includes	*****/
	#include "CommonUtils.h"
	#include "GPIOGeneralInterface.h"
	#include "SPIGeneralInterface.h"
	#include "I2CGeneralInterface.h"

/*****	Definitions	*****/
	/**	@brief		SPI interface capabilities required by the RC522 driver
		@ingroup	rc522driver
	*/
	#define RC522_SPICAPS		(SPI_BeginTransfer | SPI_EndTransfer | SPI_BiDir1Byte)
	
	#define RC522_DEFMODWIDTH	0x26

	typedef enum eRC522Return_t {
		RC522Warn_Unknown		= 1,	/**< An unknown but recoverable error happened during the operation */
		RC522_Success			= 0,	/**< The operation completed successfully */
		RC522Fail_Unknown		= -1,	/**< An unknown and unrecoverable error happened during the operation */
		RC522Fail_NotFound		= -2,	/**< The device did not respond to self identification requests */
		RC522Fail_BusFailure	= -3,	/**< The communications bus reported a failure */
		RC522Fail_WriteVerify	= -4,	/**< The device did no properly store a written value */
		RC522Fail_Timeout	 	= -5,	/**< Timeout while waiting for a command to complete */
		RC522Fail_DeviceError	= -6,	/**< The peripheral device reported an error */
		RC522Fail_BuffSize		= -7,	/**< A buffer was too small to hold the requested data */
		RC522Fail_CRC			= -8,	/**< Received CRC does not match calculated CRC */
	} eRC522Return_t;

	typedef enum eRC522Addr_t {
		RC522AddrBase			= 0x28,	/**< Used when pin EA is low */
		RC522Addr2				= 0x04,
		RC522Addr1				= 0x02,
		RC522Addr0				= 0x01,

		RC522Addr3				= 0x08,	/**< Only valid if pin EA is high */
		RC522Addr4				= 0x10,	/**< Only valid if pin EA is high */
		RC522Addr5				= 0x20,	/**< Only valid if pin EA is high */
	} eRC522Addr_t;

	/**	@brief		Enumeration of all registers in the RFRC522 peripheral
		@ingroup	rc522driver
	*/
	typedef enum eRC522Reg_t {
		//Command and status
		RC522Reg_Command		= 0x01, /**< Issue commands, start and stop execution */
		RC522Reg_ComIEn			= 0x02,
		RC522Reg_DivIEn			= 0x03,
		RC522Reg_ComIrq			= 0x04,	/**< Command IRQ flags */
		RC522Reg_DivIrq			= 0x05,	/**< Additional IRQ bits */
		RC522Reg_Error			= 0x06,	/**< Errors detected during last command */
		RC522Reg_Status1		= 0x07,
		RC522Reg_Status2		= 0x08,
		RC522Reg_FIFOData		= 0x09,	/**< Used to read/write data to 64 byte FIFO */
		RC522Reg_FIFOLevel		= 0x0A,	/**< Number of bytes in the FIFO */
		RC522Reg_WaterLevel		= 0x0B,
		RC522Reg_Control		= 0x0C,	/**< Control bits */
		RC522Reg_BitFraming		= 0x0D,	/**< Adjustments for bit-oriented frames */
		RC522Reg_Coll			= 0x0E,

		//Command
		RC522Reg_Mode			= 0x11,
		RC522Reg_TxMode			= 0x12,	/**< Transmission mode settings */
		RC522Reg_RxMode			= 0x13,	/**< Receive mode settings */
		RC522Reg_TxControl		= 0x14,	/**< Controls logical behavior of antenna pins */
		RC522Reg_TxASK			= 0x15,	/**< Transmit and modulation settings */
		RC522Reg_TxSel			= 0x16,
		RC522Reg_RxSel			= 0x17,
		RC522Reg_RxThreshold	= 0x18,
		RC522Reg_Demod			= 0x19,

		RC522Reg_MfTx			= 0x1C,
		RC522Reg_MfRx			= 0x1D,

		RC522Reg_SerialSpeed	= 0x1F,

		//Configuration
		RC522Reg_CRCMSB			= 0x21,	/**< MSB of CRC calculation */
		RC522Reg_CRCLSB			= 0x22,	/**< LSB of CRC calculation */

		RC522Reg_ModWidth		= 0x24,	/**< Sets modulation width */

		RC522Reg_RFCfg			= 0x26,
		RC522Reg_GsN			= 0x27,
		RC522Reg_CWGsP			= 0x28,
		RC522Reg_ModGsP			= 0x29,
		RC522Reg_TMode			= 0x2A,	/**< Timer and Prescaler register */
		RC522Reg_TPrescaler		= 0x2B,
		RC522Reg_TReloadMSB		= 0x2C,	/**< 16 bit timer reload value, most significant bits */
		RC522Reg_TReloadLSB		= 0x2D,	/**< 16 bit timer reload value, least significant bits */
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

	/**	@brief		Enumeration of all values in the version register
		@ingroup	rc522driver
	*/
	typedef enum eRC522RegVersion_t {
		RC522RegVer_ChipMask	= 0xF0,
		RC522RegVer_ChipMFRC522	= 0x90,

		RC522RegVer_SWMask		= 0x0F,
		RC522RegVer_SWVer1		= 0x01,
		RC522RegVer_SWVer2		= 0x02,
	} eRC522RegVersion_t;

	/**	@brief		Values in the Transmission mode register
		@ingroup	rc522driver
	*/
	typedef enum eRC522RegTxRxMode_t {
		RC522RegTxRxMode_CRC	= 0x80,	/**< Enables CRC calculation, can only be 0 at 106kBd */
		RC522RegTxRxMode_106kBd	= 0x00,	/**< Radio communication at 106kBd */
		RC522RegTxRxMode_212kBd	= 0x10,	/**< Radio communication at 212kBd */
		RC522RegTxRxMode_424kBd	= 0x20,	/**< Radio communication at 424kBd */
		RC522RegTxRxMode_848kBd	= 0x30,	/**< Radio communication at 848kBd */
		RC522RegTxRxMode_InvMod	= 0x04,	/**< Invert modulation of data (TxMode only) */
		RC522RegTxRxMode_RxNoErr= 0x04,	/**< Invalid received data stream (less than 4 bits) will be ignored */
	} eRC522RegTxRxMode_t;

	/**	@brief		Values in the timer and prescaler mode register
		@ingroup	rc522driver
	*/
	typedef enum eRC522RegTMode_t {
		RC522RegTMode_Auto		= 0x80,	/**< Timer restarts at end of transmission */
		RC522RegTMode_NonGated	= 0x00,
		RC522RegTMode_MFINGated	= 0x20,
		RC522RegTMode_AUX1Gated	= 0x40,
		RC522RegTMode_Restart	= 0x10,
		RC522RegTMode_PrescaleHighMask= 0x0F,	/**< Mask for the high bits of the prescaler value */
	} eRC522RegTMode_t;

	/**	@brief		Transmit and modulation settings in TxASK register
		@ingroup	rc522driver
	*/
	typedef enum eRC522RegTxASK_t {
		RC522RegTxASK_Force100ASK	= 0x40,	/**< Force 100% ASK modulation */
	} eRC522RegTxASK_t;

	typedef enum eRC522RegMode_t {
		RC522RegMode_MSBFirst	= 0x80,
		RC522RegMode_TxWaitRF	= 0x20,
		RC522RegMode_PolMFIn	= 0x08,
		RC522RegMode_CRCPre0000	= 0x00,
		RC522RegMode_CRCPre6363	= 0x01,
		RC522RegMode_CRCPreA671	= 0x10,
		RC522RegMode_CRCPreFFFF	= 0x11,
	} eRC522RegMode_t;
	
	/**	@brief		Values in the TxControl register, behavior of antenna driver pins
		@ingroup	rc522driver
	*/
	typedef enum eRC522RegTxCtrl_t {
		RC522RegTxCtrl_InvTx2RFOn	= 0x80,	/**< Output signal on TX2 Inverted when driver TX2 enabled */
		RC522RegTxCtrl_InvTx1RFOn	= 0x40,	/**< Output signal on TX1 Inverted when driver TX1 enabled */
		RC522RegTxCtrl_InvTx2RFOff	= 0x20,	/**< Output signal on TX2 Inverted when driver TX2 disabled */
		RC522RegTxCtrl_InvTx1RFOff	= 0x10,	/**< Output signal on TX1 Inverted when driver TX1 disabled */
		RC522RegTxCtrl_Tx2CW		= 0x08,	/**< Output signal on TX2 continuously delivers unmodulated 13.56 MHz energy carrier */
		RC522RegTxCtrl_Tx2RFEn		= 0x02,	/**< Output signal on pin TX2 delivers the 13.56 MHz energy carrier odulated by the transmission data */
		RC522RegTxCtrl_Tx1RFEn		= 0x01,	/**< Output signal on pin TX1 delivers the 13.56 MHz energy carrier odulated by the transmission data */
	} eRC522RegTxCtrl_t;
	
	/**	@brief		Values in the command register
		@ingroup	rc522driver
	*/
	typedef enum eRC522RegCmd_t {
		RC522RegCmd_RcvOff			= 0x20,	/**< Switch off analog part of receiver */
		RC522RegCmd_PowerDown		= 0x10,	/**< Enter soft power-down mode */
		
		RC522RegCmd_CmdMask			= 0x0F,	/**< Mask of all bits used by commands */
		
		RC522RegCmd_CmdIdle			= 0x00,	/**< Cancel current command, no action */
		RC522RegCmd_CmdMem			= 0x01,	/**< Stores 25 bytes into internal buffer */
		RC522RegCmd_CmdGenRndID		= 0x02,	/**< Generate random 10 byte ID number */
		RC522RegCmd_CmdCalcCRC		= 0x03,	/**< Activate CRC coprocessor or perform self test */
		RC522RegCmd_CmdTransmit		= 0x04,	/**< Transmit data from FIFO buffer */
		RC522RegCmd_CmdNoCmdChg		= 0x07,	/**< No command change, modify bits in command register */
		RC522RegCmd_CmdReceive		= 0x08,	/**< Activates the receiver circuit */
		RC522RegCmd_CmdTransceive	= 0x0C,	/**< Transmits data from FIFo buffer then antenna and activates receiver */
		RC522RegCmd_CmdMFAuthent	= 0x0E,	/**< Performs MIFARE standard authentication as a reader */
		RC522RegCmd_CmdSoftReset	= 0x0F,	/**< Resets the MFRC522 */
	} eRC522RegCmd_t;
	
	/**	@brief		Values in the ComIrq register, interrupt request bits
		@ingroup	rc522driver
	*/
	typedef enum eRC522RegComIrq_t {
		RC522RegComIrq_AllIrq	= 0x7F, /**< Mask of all IRQ bits */
		RC522RegComIrq_Set1		= 0x80,	/**< 1 indicates set bits are set, 0 inidcates set bits are clear */
		RC522RegComIrq_Tx		= 0x40,	/**< Last bit of transmitted data was sent */
		RC522RegComIrq_Rx		= 0x20,	/**< Receiver detected valid data stream */
		RC522RegComIrq_Idle		= 0x10,	/**< Command terminates, entered Idle */
		RC522RegComIrq_HiAlert	= 0x08,	/**< Status1Reg's HiAlert bit is set */
		RC522RegComIrq_LoAlert	= 0x04,	/**< Status1Reg's LoAlert bit is set */
		RC522RegComIrq_Err		= 0x02,	/**< Any bit in ErrorReg is set */
		RC522RegComIrq_Timer	= 0x01,	/**< Timer coutner has decremented to zero */
	} eRC522RegComIrq_t;
	
	typedef enum eRC522RegDivIrq_t {
		RC522RegDivIrq_Set2			= 0x80,	/**< 1 indicates set bits are set, 0 inidcates set bits are clear */
		RC522RegDivIrq_MfinActive	= 0x10,	/**< MFIN active, set when rising of falling edge detected */
		RC522RegDivIrq_CRC			= 0x04,	/**< The CalcCRC command is complete */
	} eRC522RegDivIrq_t;
	
	/**	@brief		Values in the FIFO Level register, FIFO data levels
		@ingroup	rc522driver
	*/
	typedef enum eRC522RegFIFOLvl_t {
		RC522RegFIFOLvl_FlushBuff	= 0x80, /**< Clears internal FIFO buffer and flag bits */
		RC522RegFIFOLvl_LvlMask		= 0x7F, /**< Mask of all bits used to report bytes stored i FIFO */
	} eRC522RegFIFOLvl_t;
	
	typedef enum eRC522RegBitFrm_t {
		RC522RegBitFrm_StartSend	= 0x80,	/**< Starts trnsmission of data (tranceive cmd only ) */
		RC522RegBitFrm_RxAlignMask	= 0x70,	/**< Bits used to hold first bit position of received data in FIFO */
		RC522RegBitFrm_RxAlignShift	= 4,	/**< Number of bits to shift decimal value into RX Align space */
		RC522RegBitFrm_TxLastBitsMask= 0x07,/**< Number of bits to transmit in the last byte (0 for all 8) */
		RC522RegBitFrm_TxRx8Bits	= 0x00,	/**< Used to set all 8 bits used for TX and RX data */
	} eRC522RegBitFrm_t;
	
	/**	@brief		Error conditions detected when carrying out commands
		@ingroup	rc522driver
	*/
	typedef enum eRC522RegErr_t {
		RC522RegErr_Write		= 0x80,	/**< Darta written to FIFO by host during receive */
		RC522RegErr_Temp		= 0x40,	/**< Internal temperature sensor detects over heating */
		RC522RegErr_BuffOverFl	= 0x10,	/**< FIFO buffer overflow */
		RC522RegErr_Collision	= 0x08,	/**< Bit-collision dtected */
		RC522RegErr_CRC			= 0x04,	/**< CRC calculation failed */
		RC522RegErr_Parity		= 0x02,	/**< Parity check failed */
		RC522RegErr_Protocol	= 0x01,	/**< RF protocol error detected */
	} eRC522RegErr_t;

	/**	@brief		Mescellaneous control registers
		@ingroup	rc522driver
	*/
	typedef enum eRC522RegCtrl_t {
		RC522RegCtrl_TStopNow	= 0x80,	/**< Timer stops immediately */
		RC522RegCtrl_TStartNow	= 0x40,	/**< Timer starts immediately */
		RC522RegCtrl_RxLastBits = 0x07,	/**< Indicates number of valid bits in the last RX byte, 0 means all 8 */
	} eRC522RegCtrl_t;

	typedef struct sRC522Obj_t {
		sSPIIface_t *pSpi;
		sI2CIface_t *pI2C;
		sGPIOIface_t *pGPIO;
		GPIOID_t nCSPin;
		GPIOID_t nRstPin;
		uint8_t nI2CAddr;
	} sRC522Obj_t;

/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	eRC522Return_t RC522InitializeSPI(sRC522Obj_t *pRC522, sSPIIface_t *pSpiObj, sGPIOIface_t *pGPIOObj, GPIOID_t nChipSelectPin, GPIOID_t nResetPin);

	eRC522Return_t RC522InitializeI2C(sRC522Obj_t *pRC522, sI2CIface_t *pI2CObj, sGPIOIface_t *pGPIOObj, eRC522Addr_t eAddrPins, GPIOID_t nResetPin);

	//PICC_IsNewCardPresent
	eRC522Return_t RC522IsCardPresent(sRC522Obj_t *pRC522, bool *pbCardDetected);

	//asume 4 byte serial; PICC_ReadCardSerial
	eRC522Return_t RC522ReadCardSerial(sRC522Obj_t *pRC522, uint8_t *paSerial);

/*****	Functions	*****/


#endif
