/**	@defgroup	apa102driver
	@brief		Driver for the APA102 addressable tricolor LED
	@details	v0.3
	#Description
	
	#File Information
		File:	APA102Driver.h
		Author:	J. Beighel
		Date:	2021-07-28
*/

#ifndef __APA102Driver_H
	#define __APA102Driver_H

/*****	Includes	*****/
	#include <string.h>

	#include "CommonUtils.h"
	#include "SPIGeneralInterface.h"

/*****	Defines		*****/
	#define APA102_SPICAPS		(SPI_BiDir1Byte)
	
	#ifndef APA102_NUMLIGHTS
		/**	@brief		Number of lights the driver will store information about
			@ingroup	apa102driver
		*/
		#define APA102_NUMLIGHTS	64
	#endif
	
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
		APA102_GBROrder			= 0x00001008,	/**< Bit shifts to position bytes in GBR order */
		
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

		APA102_Black			= 0x00000000,
		APA102_BrightWhite		= 0x00FFFFFF,
		APA102_BrightRed		= 0x00FF0000,
		APA102_BrightYellow		= 0x00FFFF00,
		APA102_BrightGreen		= 0x0000FF00,
		APA102_BrightCyan		= 0x0000FFFF,
		APA102_BrightBlue		= 0x000000FF,
		APA102_BrightPink		= 0x00FF00FF,
		
		APA102_Red				= 0x00440000,
		APA102_Yellow			= 0x00444400,
		APA102_Green			= 0x00004400,
		APA102_Cyan				= 0x00004444,
		APA102_Blue				= 0x00000044,
		APA102_Pink				= 0x00440044,
		APA102_White			= 0x00444444,
		
		APA102_DimRed			= 0x00110000,
		APA102_DimOrange		= 0x00110500,
		APA102_DimYellow		= 0x00111100,
		APA102_DimGreen			= 0x00001100,
		APA102_DimAqua			= 0x00001505,
		APA102_DimCyan			= 0x00001111,
		APA102_DimBlue			= 0x00000011,
		APA102_DimPurple		= 0x00050011,
		APA102_DimPink			= 0x00110011,
		APA102_DimWhite			= 0x00111111,
	} eAPA102Color_t;

	typedef struct sAPA102Info_t {
		sSPIIface_t *pSpi;			/**< Pointer to SPI interface */
		eAPA102Light_t eOrder;		/**< Light order to use for this set */
		uint32_t nLightNum;			/**< Number of lights being controlled */
		uint32_t anLights[APA102_NUMLIGHTS];	/**< Array of light colors to write */
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
	
	/**	@brief		Sets the order of colors the device expects in commands
		@details	Different manufacturers specify different color orders in 
			the bytes sent over the data line.  This allows you to configure 
			the order of bits sent to the light.
		@param		pDev		The APA102 instance to work on
		@param		eOrder		Order of bits expected by the device
		@return		Success if the color order is updated, or a code indicating 
			the nature of the failure
		@ingroup	apa102driver
	 */
	eAPA102Return_t APA102SetColorOrder(sAPA102Info_t *pDev, eAPA102Light_t eOrder);

/*****	Functions	*****/


#endif

