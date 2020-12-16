/**	@defgroup	ssd1675driver
	@brief		eINK Display Controller IC driver
	@details	v0.1
	#Description
		
		
	#File Information
		File:	SSD1675Driver.h
		Author:	J. Beighel
		Date:	12-15-2020
*/

#ifndef __SSD1675DRIVER_H
	#define __SSD1675DRIVER_H

/*****	Includes	*****/
	#include <stdio.h>

	#include "TimeGeneralInterface.h"
	#include "GPIOGeneralInterface.h"
	#include "SPIGeneralInterface.h"

/*****	Defines		*****/
	#define SSD1675_TIMECAPS	(TimeCap_DelayMilliSec)
	
	#define SSD1675_GPIOCAPS	(GPIOCap_DigitalWrite)
	
	#define SSD1675_SPICAPS		(SPI_BiDir1Byte)

/*****	Definitions	*****/
	typedef enum eSSD1675Return_t {
		SSD1675Warn_Unknown		= 1,	/**< An unknown but recoverable error happened during the operation */
		SSD1675_Success			= 0,	/**< The operation completed successfully */
		SSD1675Fail_Unknown		= -1,	/**< An unknown and unrecoverable error happened during the operation */
		SSD1675Fail_SPIError	= -2,	/**< An error occurred in the SPI bus */
	} eSSD1675Return_t;
	
	typedef enum eSSD1675Cmd_t {
		SSD1675Cmd_OutCont		= 0x01,	/**< Driver output control */
		SSD1675Cmd_GateVolt		= 0x03,	/**< Gate driving voltage control */
		SSD1675Cmd_SrcVolt		= 0x04,	/**< Source driving voltage control */
		SSD1675Cmd_InitCodeSet	= 0x08,	/**< Initial code setting OTP program */
		SSD1675Cmd_WriteInitCode= 0x09,	/**< Write register for initial code setting */
		SSD1675Cmd_ReadInitCode	= 0x0A,	/**< Read register for initial code setting */
		SSD1675Cmd_BoostStart	= 0x0C,	/**< Booster soft start control */
		SSD1675Cmd_GateScanStart= 0x0F,	/**< Gate scan start position */
		SSD1675Cmd_DeepSleep	= 0x10,	/**< Deep sleep mode */
		SSD1675Cmd_DataEntryMode= 0x11,	/**< Data entry mode setting */
		SSD1675Cmd_SWReset		= 0x12,	/**< Software reset */
		SSD1675Cmd_HVDetect		= 0x14,	/**< HV ready detect */
		SSD1675Cmd_VCIDetect	= 0x15,	/**< VCI detection */
		SSD1675Cmd_TempSensCont	= 0x18,	/**< Temperature sensor control */
		SSD1675Cmd_TempSensWr	= 0x1A,	/**< Temperature sensor control (write to register) */
		SSD1675Cmd_TempSensRd	= 0x1B,	/**< Temperature sensor control (read from register) */
		SSD1675Cmd_TempSensWrExt= 0x1C,	/**< Temperature sensor control (write to external) */
		SSD1675Cmd_MasterActiv	= 0x20,	/**< Master activation */
		SSD1675Cmd_DispUpCont1	= 0x21,	/**< Display update control 1 */
		SSD1675Cmd_DispUpCont2	= 0x22,	/**< Display update control 2 */
		SSD1675Cmd_RAMWrBlack	= 0x24,	/**< Write RAM Black/White (0x24) */
		SSD1675Cmd_RAMWrRed		= 0x26,	/**< Write RAM Red (0x26) */
		SSD1675Cmd_RAMRead		= 0x27,	/**< Read RAM */
		SSD1675Cmd_VCOMSense	= 0x28,	/**< VCOM Sense */
		SSD1675Cmd_VCOMSnseDur	= 0x29,	/**< VCOM sense duration */
		SSD1675Cmd_ProgVCOMOTP	= 0x2A,	/**< Program VCOM OTP */
		SSD1675Cmd_VCOMContWr	= 0x2B,	/**< Write register for VCOM control */
		SSD1675Cmd_VCOMRegWr	= 0x2C,	/**< Write VCOM register */
		SSD1675Cmd_DispOptRdOTP	= 0x2d,	/**< OTP Register read for display option */
		SSD1675Cmd_UserIDRd		= 0x2E,	/**< User ID read */
		SSD1675Cmd_StatusRead	= 0x2F,	/**< Status bit read */
		SSD1675Cmd_ProgWSOTP	= 0x30,	/**< Program OTP waveform setting */
		SSD1675Cmd_LoadWSOTP	= 0x31,	/**< Load waveform setting OTP */
		SSD1675Cmd_WriteLUTReg	= 0x32,	/**< Write LUT register */
		SSD1675Cmd_CRCCalc		= 0x34,	/**< CRC calculation */
		SSD1675Cmd_CRCStatRd	= 0x35,	/**< CRC status read */
		SSD1675Cmd_ProgOTPSel	= 0x36,	/**< Program OTP selection */
		SSD1675Cmd_DispOptRdReg	= 0x37,	/**< Write register for display option */
		SSD1675Cmd_UserIDWrReg	= 0x38,	/**< Write register for user ID */
		SSD1675Cmd_ProgModeOTP	= 0x39,	/**< OTP program mode */
		SSD1675Cmd_SetLinePer	= 0x3A,	/**< Set number of dummy line period */
		SSD1675Cmd_GateLidWidth	= 0x3B,	/**< Set gate line width */
		SSD1675Cmd_BrdrWaveCont	= 0x3C,	/**< Border waveform control */
		SSD1675Cmd_ReadRAMOpt	= 0x41,	/**< Read RAM option */
		SSD1675Cmd_RAMPosX		= 0x44,	/**< Set RAM X address start/end position */
		SSD1675Cmd_RAMPosY		= 0x45,	/**< Set RAM Y address start/end position */
		SSD1675Cmd_AutoWrRAMRed	= 0x46,	/**< Auto write Red RAM for regular pattern */
		SSD1675Cmd_AutoWrRAMBlck= 0x47,	/**< Auto write Black RAM for regular pattern */
		SSD1675Cmd_RAMCntX		= 0x4E,	/**< Set RAM X address counter */
		SSD1675Cmd_RAMCntY		= 0x4F,	/**< Set RAM Y address counter */
		SSD1675Cmd_AnaBlockCont	= 0x74,	/**< Set analog block control */
		SSD1675Cmd_DigBlockCont	= 0x7E,	/**< Set digital block control */
		SSD1675Cmd_NOP			= 0x7F,	/**< No operation */
	} eSSD1675Cmd_t;
	
	typedef enum eSSD1675OutCont_t {
		SSD1675OutCont_DataBytes	= 3,		/**< Number of data bytes this command uses */
		
		SSD1675OutCont_MUXMask		= 0x01FF,	/**< Bits used by the MUX gate lines setting */
		
		SSD1675OutCont_Gate1Zero	= 0x0000,	/**< Setting for gate order to be 0, 1, 2, 3 */
		SSD1675OutCont_Gate1One		= 0x0400,	/**< Setting for gate order to be 1, 0, 3, 2 */
		
		SSD1675OutCont_ScanNorm		= 0x0000,	/**< Gate scanning order 0, 1, 2, 3, ..., 295 */
		SSD1675OutCont_ScanEvenOdd	= 0x0200,	/**< Gate scanning order 0, 2, 4, ..., 294, 1, 3, 5, ..., 295 */
		
		SSD1675OutCont_ScanAsc		= 0x0000,	/**< Gate scanning order 0 to 295 */
		SSD1675OutCont_ScanDesc		= 0x0100,	/**< Gate scanning order 295 to 0 */
		
		SSD1675OutCont_Default		= 0x0127,	/**< Default setting */
	} eSSD1675OutCont_t;

	typedef struct sSSD1675Info_t {
		sTimeIface_t *pTime;
		sGPIOIface_t *pGpio;
		sSPIIface_t *pSpi;
		GPIOID_t nPinChipSel;
		GPIOID_t nPinDataCmd;
	} sSSD1675Info_t;

/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	eSSD1675Return_t SSD1675Initialize(sSSD1675Info_t *pObj, sTimeIface_t *pTime, sGPIOIface_t *pGpio, sSPIIface_t *pSpi, GPIOID_t nPinChipSel, GPIOID_t nPinDataCmd);

/*****	Functions	*****/


#endif
