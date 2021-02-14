/**	@defgroup	GROUPNAME
	@brief		
	@details	v
	#Description
	
	#File Information
		File:	WS2812BDriver.h
		Author:	J. Beighel
		Date:	2021-02-02
*/

#ifndef __WS2812BDriver_H
	#define __WS2812BDriver_H

/*****	Includes	*****/
	#include "CommonUtils.h"
	#include "TimeGeneralInterface.h"
	#include "GPIOGeneralInterface.h"

/*****	Defines		*****/
	#define WS2812B_0High100NSec	4 //400
	#define WS2812B_0Low100NSec		8 //800
	#define WS2812B_1High100NSec	8 //800
	#define WS2812B_1Low100NSec		4 //400
	
	#define WS2812B_ResetUSec		50

/*****	Definitions	*****/
	typedef eReturn_t	eWS2812BReturn_t;
	
	typedef enum eWS2812BLight_t {
		WS2812B_BOrderMask			= 0x00FF0000,	/**< Mask of order to get Blue bytes */
		WS2812B_BOrderShift			= 16,			/**< Number of LShifts to get Blue order */
		WS2812B_ROrderMask			= 0x0000FF00,
		WS2812B_ROrderShift			= 8,
		WS2812B_GOrderMask			= 0x000000FF,
		WS2812B_GOrderShift			= 0,
		
		WS2812B_BRGOrder			= 0x00100800,	/**< Bit shifts to position RGB bytes in BRG order */
		
		WS2812B_ColorMask			= 0xFF,
		
		WS2812B_NumBits				= 24,			/**< Number of bits to send for each LED */
	} eWS2812BLight_t;
	
	typedef struct sWS2812BInfo_t {
		sTimeIface_t *pTime;		/**< Pointer to time interface */
		sGPIOIface_t *pGpio;		/**< Pointer to GPIO interface */
		GPIOID_t nPinNum;			/**< Pin connected to light string */
		eWS2812BLight_t eOrder;		/**< Light order to use for this set */
		uint32_t nLightNum;			/**< Number of lights being controlled */
		uint32_t anLights[64];		/**< Array of light colors to write */
	} sWS2812BInfo_t;

/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	eWS2812BReturn_t WS2812BInitialize(sWS2812BInfo_t *pDev, sTimeIface_t *pTime, sGPIOIface_t * pGPIO, GPIOID_t nCtrlPin);
	
	eWS2812BReturn_t WS2812BSetLightColor(sWS2812BInfo_t *pDev, uint32_t nLightID, uint8_t nRed, uint8_t nGree, uint8_t nBlue);
	
	eWS2812BReturn_t WS2812BUpdateLights(sWS2812BInfo_t *pDev);

/*****	Functions	*****/


#endif

