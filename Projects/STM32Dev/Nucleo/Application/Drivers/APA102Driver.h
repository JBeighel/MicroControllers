/**	@defgroup	apa102driver
	@brief		Driver for the APA102 addressable tricolor LED
	@details	v0.1
	#Description
	
	#File Information
		File:	APA102Driver.h
		Author:	J. Beighel
		Date:	2021-03-02
*/

#ifndef __APA102Driver_H
	#define __APA102Driver_H

/*****	Includes	*****/
	#include <string.h>

	#include "CommonUtils.h"
	#include "SPIGeneralInterface.h"

/*****	Defines		*****/
	#define APA102_SPICAPS		(SPI_BiDir1Byte)
	
/*****	Definitions	*****/
	typedef eReturn_t	eAPA102Return_t;
	
	typedef enum eAPA102Light_t {
		APA102_ROrderMask		= 0x00FF0000,
		APA102_ROrderShift		= 16,
		APA102_GOrderMask		= 0x0000FF00,
		APA102_GOrderShift		= 8,
		APA102_BOrderMask		= 0x000000FF,	/**< Mask of order to get Blue bytes */
		APA102_BOrderShift		= 0,			/**< Number of LShifts to get Blue order */
		
		APA102_BRGOrder			= 0x00000810,	/**< Bit shifts to position bytes in BRG order */
		APA102_RBGOrder			= 0x00100800,	/**< Bit shifts to position bytes in RBG order */
		
		APA102_ColorMask		= 0xFF,
		
		APA102_NumBits			= 24,			/**< Number of bits to send for each LED */
	} eAPA102Light_t;
	
	typedef enum eAPA102Color_t {
		APA102_RedMask			= 0x00FF0000,
		APA102_RedShift			= 16,
		APA102_GreenMask		= 0x0000FF00,
		APA102_GreenShift		= 8,
		APA102_BlueMask			= 0x000000FF,
		APA102_BlueShift		= 0,

		APA102_BrightRed		= 0x00FF0000,
		APA102_BrightGreen		= 0x0000FF00,
		APA102_BrightBlue		= 0x000000FF,
		APA102_DimRed			= 0x00440000,
		APA102_DimGreen			= 0x00004400,
		APA102_DimBlue			= 0x00000044,
	} eAPA102Color_t;

	typedef struct sAPA102Info_t {
		sSPIIface_t *pSpi;			/**< Pointer to SPI interface */
		eAPA102Light_t eOrder;		/**< Light order to use for this set */
		uint32_t nLightNum;			/**< Number of lights being controlled */
		uint32_t anLights[64];		/**< Array of light colors to write */
	} sAPA102Info_t;

/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	eAPA102Return_t APA102Initialize(sAPA102Info_t *pDev, sSPIIface_t *pSpiDev);
	
	/**	@brief		Sets the color for a specific light in the string
		@param		pDev		The APA102 instance to work on
		@param		nLightID	The index of the light in the string
		@param		nRed		The brightness of red in the light
		@param		nGreen		The brightness of green in the light
		@param		nBlue		The brightness of blue in the light
		@return		Success if the light is updated, or a code indicating the
			nature of the failure
		@ingroup	apa102driver
	 */
	eAPA102Return_t APA102SetLightColorBytes(sAPA102Info_t *pDev, uint32_t nLightID, uint8_t nRed, uint8_t nGreen, uint8_t nBlue);
	
	/**	@brief		Sets the color for a specific light in the string
		@param		pDev		The APA102 instance to work on
		@param		nLightID	The index of the light in the string
		@param		nColor		Indicates the brightness of all three colors in
			the light.  It takes the format 0xXXRRGGBB where XX are ignored, RR
			is the brightness of red, GG is the brightness of green, and BB is
			the brightness of blue
		@return		Success if the light is updated, or a code indicating the
			nature of the failure
		@ingroup	apa102driver
	 */
	eAPA102Return_t APA102SetLightColor(sAPA102Info_t *pDev, uint32_t nLightID, uint32_t nColor);

	eAPA102Return_t APA102UpdateLights(sAPA102Info_t *pDev);

/*****	Functions	*****/


#endif

