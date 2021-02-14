/**	@defgroup	GROUPNAME
	@brief		
	@details	v
	#Description
	
	#File Information
		File:	APA102Driver.h
		Author:	J. Beighel
		Date:	2021-02-02
*/

#ifndef __APA102Driver_H
	#define __APA102Driver_H

/*****	Includes	*****/
	#include <string.h>

	#include "CommonUtils.h"
	#include "SPIGeneralInterface.h"

/*****	Defines		*****/
	
	
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
	
	typedef struct sAPA102Info_t {
		sSPIIface_t *pSpi;		/**< Pointer to SPI interface */
		eAPA102Light_t eOrder;		/**< Light order to use for this set */
		uint32_t nLightNum;			/**< Number of lights being controlled */
		uint32_t anLights[64];		/**< Array of light colors to write */
	} sAPA102Info_t;

/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	eAPA102Return_t APA102Initialize(sAPA102Info_t *pDev, sSPIIface_t *pSpiDev);
	
	eAPA102Return_t APA102SetLightColor(sAPA102Info_t *pDev, uint32_t nLightID, uint8_t nRed, uint8_t nGreen, uint8_t nBlue);
	
	eAPA102Return_t APA102UpdateLights(sAPA102Info_t *pDev);

/*****	Functions	*****/


#endif

