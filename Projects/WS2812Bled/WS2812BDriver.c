/**	File:	WS2812BDriver.c
	Author:	J. Beighel
	Date:	2021-02-02
*/

/*****	Includes	*****/


/*****	Defines		*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	WS2812BPositionRedByte(eOrder, nRed)		((nRed & WS2812B_ColorMask) << ((eOrder & WS2812B_ROrderMask) >> WS2812B_ROrderShift))
	
	WS2812BPositionBlueByte(eOrder, nBlue)		((nBlue & WS2812B_ColorMask) << ((eOrder & WS2812B_BOrderMask) >> WS2812B_BOrderShift))
	
	WS2812BPositionGreenByte(eOrder, nGreen)	((nGreen & WS2812B_ColorMask) << ((eOrder & WS2812B_GOrderMask) >> WS2812B_GOrderShift))

/*****	Functions	*****/
eWS2812BReturn_t WS2812BInitialize(sWS2812BInfo_t *pDev, sTimeIface_t *pTime, sGPIOIface_t * pGPIO, GPIOID_t nCtrlPin) {
	pDev->pTime = pTime;
	pDev->pGpio = pGPIO;
	pDev->nPinNum = nCtrlPin;
	pDev->eOrder = WS2812B_BRGOrder;
	
	memset(pDev->anLights, 0, sizeof(uint32_t) * 64);
	
	return Success;
}

eWS2812BReturn_t WS2812BSetLightColor(sWS2812BInfo_t *pDev, uint32_t nLightID, uint8_t nRed, uint8_t nGree, uint8_t nBlue) {
	uint32_t nLightVal = 0;
	
	nLightVal |= WS2812BPositionRedByte(pDev->eOrder, nRed);
	nLightVal |= WS2812BPositionGreenByte(pDev->eOrder, nGreen);
	nLightVal |= WS2812BPositionBlueByte(pDev->eOrder, nBlue);
	
	pDev->anLights[nLightID] = nLightVal;
	
	return Success;
}

eWS2812BReturn_t WS2812BUpdateLights(sWS2812BInfo_t *pDev) {
	uint32_t nBitCtr; nLightCtr, nValue;
	
	//The LED's require pretty good timing, so this block can't really be
	//Interrupted without screwing up some of the lights
	
	for (nLightCtr = 0; nLightCtr < 64; nLightCtr++) {
		nValue = pDev->anLights[nLightCtr];
		
		for (nBitCtr = 0; nBitCtr < WS2812B_NumBits; nBitCtr++) {
			//Sending the high bit first, 
			if ((nValue & (0x01 << (WS2812B_NumBits - 1))) == 0) { //Use zero timing
				//Set pin high
				pDev->pGpio->pfGPIODigitalWriteByPin_t(pDev->pGpio, pDev->nPinNum, true);
				pDev->pTime->pfDelay100NanoSeconds(WS2812B_0High100NSec);
				//Set pin low
				pDev->pGpio->pfGPIODigitalWriteByPin_t(pDev->pGpio, pDev->nPinNum, false);
				pDev->pTime->pfDelay100NanoSeconds(WS2812B_0Low100NSec);
			} else { //Use 1 timing
				//Set pin high
				pDev->pGpio->pfGPIODigitalWriteByPin_t(pDev->pGpio, pDev->nPinNum, true);
				pDev->pTime->pfDelay100NanoSeconds(WS2812B_1High100NSec);
				//Set pin low
				pDev->pGpio->pfGPIODigitalWriteByPin_t(pDev->pGpio, pDev->nPinNum, false);
				pDev->pTime->pfDelay100NanoSeconds(WS2812B_1Low100NSec);
			}
			
			nValue <<= 1; //Sending from highest bit to lowest
		}
	}
	
	return Success;
}