/**	File:	APA102Driver.c
	Author:	J. Beighel
	Date:	2021-02-02
*/

/*****	Includes	*****/
	#include "APA102Driver.h"

/*****	Defines		*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	#define APA102PositionRedByte(eOrder, nRed)		((nRed & APA102_ColorMask) << ((eOrder & APA102_ROrderMask) >> APA102_ROrderShift))
	
	#define APA102PositionBlueByte(eOrder, nBlue)		((nBlue & APA102_ColorMask) << ((eOrder & APA102_BOrderMask) >> APA102_BOrderShift))
	
	#define APA102PositionGreenByte(eOrder, nGreen)	((nGreen & APA102_ColorMask) << ((eOrder & APA102_GOrderMask) >> APA102_GOrderShift))

/*****	Functions	*****/
eAPA102Return_t APA102Initialize(sAPA102Info_t *pDev, sSPIIface_t *pSpiDev) {
	pDev->pSpi = pSpiDev;
	
	pDev->eOrder = APA102_RBGOrder;
	pDev->nLightNum = 11;
	memset(pDev->anLights, 0, sizeof(uint32_t) * pDev->nLightNum);
	
	return Success;
}

eAPA102Return_t APA102SetLightColor(sAPA102Info_t *pDev, uint32_t nLightID, uint8_t nRed, uint8_t nGreen, uint8_t nBlue) {
	uint32_t nLightVal = 0;
	
	nLightVal |= APA102PositionRedByte(pDev->eOrder, nRed);
	nLightVal |= APA102PositionGreenByte(pDev->eOrder, nGreen);
	nLightVal |= APA102PositionBlueByte(pDev->eOrder, nBlue);
	
	pDev->anLights[nLightID] = nLightVal;
	
	return Success;
}

eAPA102Return_t APA102UpdateLights(sAPA102Info_t *pDev) {
	uint32_t nLightCtr;
	uint8_t nByte, nDump;
	
	pDev->pSpi->pfBeginTransfer(pDev->pSpi);
	
	//Start frame
	nByte = 0x00;
	for (nLightCtr = 0; nLightCtr < 4; nLightCtr++) {
		pDev->pSpi->pfTransferByte(pDev->pSpi, nByte, &nDump);
	}
	
	//Write the 3 byte color code for each light
	for (nLightCtr = 0; nLightCtr < pDev->nLightNum; nLightCtr++) {
		nByte = 0xFF; //Global brightness, fully on
		pDev->pSpi->pfTransferByte(pDev->pSpi, nByte, &nDump);
		
		//Write lowest byte
		nByte = pDev->anLights[nLightCtr] & 0xFF;
		pDev->pSpi->pfTransferByte(pDev->pSpi, nByte, &nDump);
		
		//Write middle byte
		nByte = (pDev->anLights[nLightCtr] >> 8) & 0xFF;
		pDev->pSpi->pfTransferByte(pDev->pSpi, nByte, &nDump);
		
		//Write highest byte
		nByte = (pDev->anLights[nLightCtr] >> 16) & 0xFF;
		pDev->pSpi->pfTransferByte(pDev->pSpi, nByte, &nDump);
	}
	
	//End Frame
	nByte = 0xFF;
	for (nLightCtr = 0; nLightCtr < (pDev->nLightNum + 15) / 16; nLightCtr++) {
		pDev->pSpi->pfTransferByte(pDev->pSpi, nByte, &nDump);
		pDev->pSpi->pfTransferByte(pDev->pSpi, nByte, &nDump);
		pDev->pSpi->pfTransferByte(pDev->pSpi, nByte, &nDump);
		pDev->pSpi->pfTransferByte(pDev->pSpi, nByte, &nDump);
	}
	
	pDev->pSpi->pfEndTransfer(pDev->pSpi);
	
	return Success;
}