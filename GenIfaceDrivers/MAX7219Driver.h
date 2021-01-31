/**	@defgroup	max7219driver
	@brief		Peripheral driver for MAX 7219 7-segment LED display driver
	@details	v0.1
	#Description
		Requires SPI mode 0
	
	#File Information
		File:	MAX7219Driver.h
		Author:	J. Beighel
		Date:	2021-01-26
*/

#ifndef __MAX7219DRIVER_H
	#define __MAX7219DRIVER_H

/*****	Includes	*****/
	#include "CommonUtils.h"
	#include "GPIOGeneralInterface.h"
	#include "SPIGeneralInterface.h"


/*****	Defines		*****/
	#define MAX7219_GPIOCAPS	(GPIOCap_DigitalWrite)
	#define MAX7219_SPICAPS		(SPI_BiDir1Byte)
	
	#define MAX7219_MAXDIGITS	8

/*****	Definitions	*****/
	typedef eReturn_t eMAX7219Return_t;

	typedef enum eMAX7219Reg_t {
		MAX7219_NOOP		= 0x00,	/**< No operation */
		MAX7219_DIGIT0		= 0x01,	/**< Set output value for digit 0 */
		MAX7219_DIGIT1		= 0x02,	/**< Set output value for digit 1 */
		MAX7219_DIGIT2		= 0x03,	/**< Set output value for digit 2 */
		MAX7219_DIGIT3		= 0x04,	/**< Set output value for digit 3 */
		MAX7219_DIGIT4		= 0x05,	/**< Set output value for digit 4 */
		MAX7219_DIGIT5		= 0x06,	/**< Set output value for digit 5 */
		MAX7219_DIGIT6		= 0x07,	/**< Set output value for digit 6 */
		MAX7219_DIGIT7		= 0x08,	/**< Set output value for digit 7 */
		MAX7219_DECODEMODE	= 0x09,	/**< Set decoder mode for all digits */
		MAX7219_INTENSITY	= 0x0A,	/**< Brightness control for all LEDs */
		MAX7219_SCANLIMIT	= 0x0B,	/**< Sets how many digits to display */
		MAX7219_SHUTDOWN	= 0x0C,	/**< Enables/Disables outputs */
		MAX7219_DISPLAYTEST	= 0x0F,	/**< Enables/Disables test of LED display */
	} eMAX7219Reg_t;
	
	typedef enum eMAX7219RegDigit_t {
		MAX7219DIGIT_CodeMask= 0x8F,	/**< Mask of all bits for code B font */
		MAX7219DIGIT_Code0	= 0x00,	/**< Use Code B font for 0 */
		MAX7219DIGIT_Code1	= 0x01,	/**< Use Code B font for 1 */
		MAX7219DIGIT_Code2	= 0x02,	/**< Use Code B font for 2 */
		MAX7219DIGIT_Code3	= 0x03,	/**< Use Code B font for 3 */
		MAX7219DIGIT_Code4	= 0x04,	/**< Use Code B font for 4 */
		MAX7219DIGIT_Code5	= 0x05,	/**< Use Code B font for 5 */
		MAX7219DIGIT_Code6	= 0x06,	/**< Use Code B font for 6 */
		MAX7219DIGIT_Code7	= 0x07,	/**< Use Code B font for 7 */
		MAX7219DIGIT_Code8	= 0x08,	/**< Use Code B font for 8 */
		MAX7219DIGIT_Code9	= 0x09,	/**< Use Code B font for 9 */
		MAX7219DIGIT_CodeDash= 0x0A,	/**< Use Code B font for a dash */
		MAX7219DIGIT_CodeE	= 0x0B,	/**< Use Code B font for E */
		MAX7219DIGIT_CodeH	= 0x0C,	/**< Use Code B font for H */
		MAX7219DIGIT_CodeL	= 0x0D,	/**< Use Code B font for L */
		MAX7219DIGIT_CodeP	= 0x0E,	/**< Use Code B font for P */
		MAX7219DIGIT_CodeSpace= 0x0F,	/**< Use Code B font for space */
		MAX7219DIGIT_DecPt	= 0x80,	/**< When decoding this turns on the decimal point*/
		
		MAX7219DIGIT_SegA	= 0x40,	/**< Segment A */
		MAX7219DIGIT_SegB	= 0x20,	/**< Segment B */
		MAX7219DIGIT_SegC	= 0x10,	/**< Segment C */
		MAX7219DIGIT_SegD	= 0x08,	/**< Segment D */
		MAX7219DIGIT_SegE	= 0x04,	/**< Segment E */
		MAX7219DIGIT_SegF	= 0x02,	/**< Segment F */
		MAX7219DIGIT_SegG	= 0x01,	/**< Segment G */
		MAX7219DIGIT_SegDP	= 0x80,	/**< Segment Decimal Point */
	} eMAX7219RegDigit_t;
	
	/**	@brief		Set bits in this register to apply code B decoding to each digit
		@ingroup	max7219driver
	*/
	typedef enum eMAX7219RegDecode_t {
		MAX7219_DecodeNone	= 0x00,	/**< Deisable decoding for all digits */
		MAX7219_DecodeDig0	= 0x01,	/**< Bit for digit 0 */
		MAX7219_DecodeDig1	= 0x02,	/**< Bit for digit 1 */
		MAX7219_DecodeDig2	= 0x04,	/**< Bit for digit 2 */
		MAX7219_DecodeDig3	= 0x08,	/**< Bit for digit 3 */
		MAX7219_DecodeDig4	= 0x10,	/**< Bit for digit 4 */
		MAX7219_DecodeDig5	= 0x20,	/**< Bit for digit 5 */
		MAX7219_DecodeDig6	= 0x40,	/**< Bit for digit 6 */
		MAX7219_DecodeDig7	= 0x80,	/**< Bit for digit 7 */
	} eMAX7219RegDecode_t;
	
	typedef enum eMAX7219RegIntensity_t {
		MAX7219INTEN_Mask	= 0x0F,	/**< Mask of bits used to set the intensity */
		MAX7219INTEN_Min	= 0x01,	/**< Minimum intensity value */
		MAX7219INTEN_Max	= 0x0F,	/**< Maximum intensity value */
	} eMAX7219RegIntensity_t;
	
	typedef enum eMAX7219RegScan_t {
		MAX7219SCAN_Mask	= 0x07,	/**< Mask of bits used to set the scan limit */
		MAX7219SCAN_Dig0	= 0x00,	/**< Apply outputs for digit 0 only */
		MAX7219SCAN_Dig0to1	= 0x01,	/**< Apply outputs for digits 0 through 1 */
		MAX7219SCAN_Dig0to2	= 0x02,	/**< Apply outputs for digits 0 through 2 */
		MAX7219SCAN_Dig0to3	= 0x03,	/**< Apply outputs for digits 0 through 3 */
		MAX7219SCAN_Dig0to4	= 0x04,	/**< Apply outputs for digits 0 through 4 */
		MAX7219SCAN_Dig0to5	= 0x05,	/**< Apply outputs for digits 0 through 5 */
		MAX7219SCAN_Dig0to6	= 0x06,	/**< Apply outputs for digits 0 through 6 */
		MAX7219SCAN_DigAll	= 0x07,	/**< Apply outputs for all digits, 0 through 7 */
	} eMAX7219RegScan_t;
	
	typedef enum eMAX7219RegShutDwn_t {
		MAX7219SHUTDWN_Norm	= 0x01,	/**< Normal operation, display is on */
		MAX7219SHUTDWN_Off	= 0x00,	/**< Shutdown, display is off */
	} eMAX7219RegShutDwn_t;
	
	typedef enum eMAX7219RegDispTst_t {
		MAX7219DISPTST_On	= 0x01,	/**< Testign display, all outputs are on */
		MAX7219DISPTST_Off	= 0x00,	/**< Normal operation */
	} eMAX7219RegDispTst_t;
	
	typedef enum eMAX7219Led_t {
		MAX7219Led_SegA		= 0x40,	/**< Segment A */
		MAX7219Led_SegB		= 0x20,	/**< Segment B */
		MAX7219Led_SegC		= 0x10,	/**< Segment C */
		MAX7219Led_SegD		= 0x08,	/**< Segment D */
		MAX7219Led_SegE		= 0x04,	/**< Segment E */
		MAX7219Led_SegF		= 0x02,	/**< Segment F */
		MAX7219Led_SegG		= 0x01,	/**< Segment G */
		MAX7219Led_SegDP	= 0x80,	/**< Segment Decimal Point */
		
		MAX7219Led_0		= 0x80,
		MAX7219Led_1		= 0x40,
		MAX7219Led_2		= 0x20,
		MAX7219Led_3		= 0x10,
		MAX7219Led_4		= 0x08,
		MAX7219Led_5		= 0x04,
		MAX7219Led_6		= 0x02,
		MAX7219Led_7		= 0x01,
	} eMAX7219Led_t;
	
	typedef struct sMAX7219Info_t {
		sGPIOIface_t *pGpio;
		sSPIIface_t *pSpi;
		GPIOID_t nChipSelect;
	} sMAX7219Info_t;

/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/


/*****	Functions	*****/

eMAX7219Return_t MAX7219Initialize(sMAX7219Info_t *pDev, uint8_t nNumDigits, GPIOID_t nChipSel, sGPIOIface_t *pGpioDev, sSPIIface_t *pSpiDev);

eMAX7219Return_t MAX7219SetDigitLEDs(sMAX7219Info_t *pDev, uint8_t nDigit, eMAX7219Led_t eLights);

eMAX7219Return_t MAX7219SetIntensity(sMAX7219Info_t *pDev, uint8_t nLevel);

#endif

