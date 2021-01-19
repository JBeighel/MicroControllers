/**	File:	US2066Driver.c
	Author:	J. Beighel
	Date:	2021-01-19
*/

/*****	Includes	*****/
	#include "US2066Driver.h"

/*****	Constants	*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	/**	@brief		Sends a byte of data over the data pins to the LCD controller
		@details	Blocks while setting the data pins and the delays for pusling
			the hardware.
		@ingroup	us2066driver
	*/
	eUS2066Return_t US2066WriteByte(sUS2066Info_t *pDev, bool bSendToRam, uint8_t nByte);

/*****	Functions	*****/
eUS2066Return_t US2066Init4Data(sUS2066Info_t *pDev, sTimeIface_t *pTime, sGPIOIface_t *pGpio, bool bFlipCursorDir, uint8_t nColCnt, uint8_t nRowCnt, uint16_t nEnPin, uint16_t nReadWritePin, uint16_t nRegSelPin, uint16_t nData4, uint16_t nData5, uint16_t nData6, uint16_t nData7) {
	return US2066Init8Data(pDev, pTime, pGpio, bFlipCursorDir, nColCnt, nRowCnt, nEnPin, nReadWritePin, nRegSelPin, GPIO_NOPIN, GPIO_NOPIN, GPIO_NOPIN,GPIO_NOPIN, nData4, nData5, nData6, nData7);
}

eUS2066Return_t US2066Init8Data(sUS2066Info_t *pDev, sTimeIface_t *pTime, sGPIOIface_t *pGpio, bool bFlipCursorDir, uint8_t nColCnt, uint8_t nRowCnt, uint16_t nEnPin, uint16_t nReadWritePin, uint16_t nRegSelPin, uint16_t nData0, uint16_t nData1, uint16_t nData2, uint16_t nData3, uint16_t nData4, uint16_t nData5, uint16_t nData6, uint16_t nData7) {
	uint8_t nCtr;
	
	//Set all module properties
	pDev->pTime = pTime;
	pDev->pGPIO = pGpio;
	pDev->pSPI = NULL;
	pDev->nRegSelPin = nRegSelPin;
	pDev->nRdWrPin = nReadWritePin;
	pDev->nEnablePin = nEnPin;
	pDev->nColCnt = nColCnt;
	pDev->nRowCnt = nRowCnt;
	pDev->nResetPin = GPIO_NOPIN;
	
	if (nData0 == GPIO_NOPIN) { //If 8 pins aren't set, only use 4
		pDev->bUse8DataPins = false;
	} else {
		pDev->bUse8DataPins = true;
	}
	
	pDev->anDataPins[0] = nData0;
	pDev->anDataPins[1] = nData1;
	pDev->anDataPins[2] = nData2;
	pDev->anDataPins[3] = nData3;
	pDev->anDataPins[4] = nData4;
	pDev->anDataPins[5] = nData5;
	pDev->anDataPins[6] = nData6;
	pDev->anDataPins[7] = nData7;
	
	pDev->anRowBases[0] = 0x00;
	pDev->anRowBases[1] = 0x20;
	pDev->anRowBases[2] = 0x40;
	pDev->anRowBases[3] = 0x60;
	
	//Initialize the data pins
	pDev->pGPIO->pfSetModeByPin(pDev->pGPIO, pDev->nRegSelPin, GPIO_DigitalOutput);
	pDev->pGPIO->pfSetModeByPin(pDev->pGPIO, pDev->nRdWrPin, GPIO_DigitalOutput);
	pDev->pGPIO->pfSetModeByPin(pDev->pGPIO, pDev->nEnablePin, GPIO_DigitalOutput);
	
	for (nCtr = 0; nCtr < 8; nCtr++) {
		if ((nCtr >= 4) || (pDev->bUse8DataPins == true)) {
			pDev->pGPIO->pfSetModeByPin(pDev->pGPIO, pDev->anDataPins[nCtr], GPIO_DigitalOutput);
			pDev->pGPIO->pfDigitalWriteByPin(pDev->pGPIO, pDev->anDataPins[nCtr], false);
		}
	}
	
	//Setup the output pins for the 1602 startup
	pDev->pGPIO->pfDigitalWriteByPin(pDev->pGPIO, pDev->nRdWrPin, false);
	pDev->pGPIO->pfDigitalWriteByPin(pDev->pGPIO, pDev->nRegSelPin, false);

	pDev->pGPIO->pfDigitalWriteByPin(pDev->pGPIO, pDev->anDataPins[4], true);
	pDev->pGPIO->pfDigitalWriteByPin(pDev->pGPIO, pDev->anDataPins[5], true);
	pDev->pGPIO->pfDigitalWriteByPin(pDev->pGPIO, pDev->anDataPins[6], false);
	pDev->pGPIO->pfDigitalWriteByPin(pDev->pGPIO, pDev->anDataPins[7], false);
	
	//Per the data sheet, pulse this 4 times
	for (nCtr = 0; nCtr < 3; nCtr++) {
		//Delay to let peripheral do its thing
		pDev->pTime->pfDelayMilliSeconds(US2066_PULSEINITMSEC);
		
		//Pulse the LCD to have it read data
		pDev->pTime->pfDelayMicroSeconds(US2066_ENPULSEPREUSEC);
		pDev->pGPIO->pfDigitalWriteByPin(pDev->pGPIO, pDev->nEnablePin, true);
		pDev->pTime->pfDelayMicroSeconds(US2066_ENPULSEONUSEC);
		pDev->pGPIO->pfDigitalWriteByPin(pDev->pGPIO, pDev->nEnablePin, false);
		pDev->pTime->pfDelayMicroSeconds(US2066_ENPULSEPOSTUSEC);
	}
	
	//Tell the peripheral how many data lines to use
	if (pDev->bUse8DataPins == true) { //Send byte 0x0C (what is there now)
		pDev->pTime->pfDelayMicroSeconds(US2066_ENPULSEPREUSEC);
		pDev->pGPIO->pfDigitalWriteByPin(pDev->pGPIO, pDev->nEnablePin, true);
		pDev->pTime->pfDelayMicroSeconds(US2066_ENPULSEONUSEC);
		pDev->pGPIO->pfDigitalWriteByPin(pDev->pGPIO, pDev->nEnablePin, false);
		pDev->pTime->pfDelayMicroSeconds(US2066_ENPULSEPOSTUSEC);
		
		US2066WriteByte(pDev, false, US2066_FS_8DB_2LINE_5X8);
	} else { //Send byte 0x04
		pDev->pGPIO->pfDigitalWriteByPin(pDev->pGPIO, pDev->anDataPins[4], false);
		
		pDev->pTime->pfDelayMicroSeconds(US2066_ENPULSEPREUSEC);
		pDev->pGPIO->pfDigitalWriteByPin(pDev->pGPIO, pDev->nEnablePin, true);
		pDev->pTime->pfDelayMicroSeconds(US2066_ENPULSEONUSEC);
		pDev->pGPIO->pfDigitalWriteByPin(pDev->pGPIO, pDev->nEnablePin, false);
		pDev->pTime->pfDelayMicroSeconds(US2066_ENPULSEPOSTUSEC);
		
		US2066WriteByte(pDev, false, US2066_FS_4DB_2LINE_5X8);
	}
	
	US2066WriteByte(pDev, false, US2066_DISPLAYOFF);
	US2066ClearDisplay(pDev);
	
	if (bFlipCursorDir == true) {
		US2066WriteByte(pDev, false, US2066_ENTRYMODESETBASE | US2066_EMS_FLIPCURSORADV);
	} else {
		US2066WriteByte(pDev, false, US2066_ENTRYMODESETBASE);
	}
	
	pDev->pTime->pfDelayMilliSeconds(US2066_PULSEINITMSEC);
	US2066WriteByte(pDev, false, US2066_DISPLAYON);
	
	return US2066_Success;
}

eUS2066Return_t US2066InitSPI(sUS2066Info_t *pDev, sTimeIface_t *pTime, sGPIOIface_t *pGpio, sSPIIface_t *pSpi, uint8_t nColCnt, uint8_t nRowCnt, GPIOID_t nChipSelPin, GPIOID_t nResetPin) {
	uint16_t nCtr;
	
	//Set all module properties
	pDev->pTime = pTime;
	pDev->pGPIO = pGpio;
	pDev->pSPI = pSpi;
	pDev->nRegSelPin = GPIO_NOPIN;
	pDev->nRdWrPin = GPIO_NOPIN;
	pDev->bUse8DataPins = false;
	
	pDev->nColCnt = nColCnt;
	pDev->nRowCnt = nRowCnt;
	pDev->nEnablePin = nChipSelPin; //Using this to hold chip select
	pDev->nResetPin = nResetPin;
	
	pDev->anDataPins[0] = GPIO_NOPIN;
	pDev->anDataPins[1] = GPIO_NOPIN;
	pDev->anDataPins[2] = GPIO_NOPIN;
	pDev->anDataPins[3] = GPIO_NOPIN;
	pDev->anDataPins[4] = GPIO_NOPIN;
	pDev->anDataPins[5] = GPIO_NOPIN;
	pDev->anDataPins[6] = GPIO_NOPIN;
	pDev->anDataPins[7] = GPIO_NOPIN;
	
	pDev->anRowBases[0] = 0x00;
	pDev->anRowBases[1] = 0x20;
	pDev->anRowBases[2] = 0x40;
	pDev->anRowBases[3] = 0x60;
	
	pDev->pGPIO->pfSetModeByPin(pDev->pGPIO, pDev->nEnablePin, GPIO_DigitalOutput);
	pDev->pGPIO->pfDigitalWriteByPin(pDev->pGPIO, pDev->nEnablePin, true);
	
	pDev->pGPIO->pfSetModeByPin(pDev->pGPIO, pDev->nResetPin, GPIO_DigitalOutput);
	pDev->pGPIO->pfDigitalWriteByPin(pDev->pGPIO, pDev->nResetPin, true);
	
	//Reset the device
	pDev->pTime->pfDelayMicroSeconds(US2066_RESTARTUSEC);
	pDev->pGPIO->pfDigitalWriteByPin(pDev->pGPIO, pDev->nResetPin, false); //3 uSec
	pDev->pTime->pfDelayMicroSeconds(US2066_RESTARTUSEC);
	
	pDev->pGPIO->pfDigitalWriteByPin(pDev->pGPIO, pDev->nResetPin, true); //100 uSec
	pDev->pTime->pfDelayMicroSeconds(US2066_RESTARTUSEC);
	
	//Set the device configuration
	for (nCtr = 0; nCtr < 3; nCtr++) {
		pDev->pTime->pfDelayMilliSeconds(US2066_PULSEINITMSEC);
		US2066WriteByte(pDev, false, US2066_DISPLAYON);
	}
	
	US2066WriteByte(pDev, false, US2066_DISPLAYOFF);
	US2066ClearDisplay(pDev);
		
	//US2066WriteByte(pDev, false, US2066_ENTRYMODESETBASE | US2066_EMS_FLIPCURSORADV);
	US2066WriteByte(pDev, false, US2066_ENTRYMODESETBASE);
	
	pDev->pTime->pfDelayMilliSeconds(US2066_PULSEINITMSEC);
	US2066WriteByte(pDev, false, US2066_DISPLAYON);
	
	return US2066_Success;
}

eUS2066Return_t US2066ClearDisplay(sUS2066Info_t *pDev) {
	US2066WriteByte(pDev, false, US2066_CLEARDISPLAY);
	pDev->pTime->pfDelayMilliSeconds(US2066_DELAYCLEARMSEC);

	return US2066_Success;
}

eUS2066Return_t US2066DisplayOn(sUS2066Info_t *pDev) {
	US2066WriteByte(pDev, false, US2066_DISPLAYON);

	return US2066_Success;
}
	
eUS2066Return_t US2066DisplayOff(sUS2066Info_t *pDev) {
	US2066WriteByte(pDev, false, US2066_DISPLAYOFF);

	return US2066_Success;
}
	
eUS2066Return_t US2066SetCursorPosition(sUS2066Info_t *pDev, uint8_t nCol, uint8_t nRow) {
	uint8_t nAddr;
	
	if (nRow >= pDev->nRowCnt) {
		return US2066Fail_InvalisPos;
	}
	
	if (nRow >= pDev->nRowCnt) {
		return US2066Fail_InvalisPos;
	}
	
	nAddr = US2066_SETDDRAMBASE | (pDev->anRowBases[nRow] + nCol);
	US2066WriteByte(pDev, false, nAddr);
	pDev->pTime->pfDelayMicroSeconds(US2066_CURSORPOSMSEC);
	
	return US2066_Success;
}

eUS2066Return_t US2066PrintCharacter(sUS2066Info_t *pDev, char Letter) {
	US2066WriteByte(pDev, true, Letter);

	return US2066_Success;
}

eUS2066Return_t US2066WriteByte(sUS2066Info_t *pDev, bool bSendToRam, uint8_t nByte) {
	uint8_t nCurrBit;
	uint8_t nCtr;
	
	if (pDev->pSPI == NULL) { //Operating on a parallel bus
		//Set the Read/Write to Write
		pDev->pGPIO->pfDigitalWriteByPin(pDev->pGPIO, pDev->nRdWrPin, false);
		
		if (bSendToRam == true) { //Send data to display RAM
			pDev->pGPIO->pfDigitalWriteByPin(pDev->pGPIO, pDev->nRegSelPin, true);
		} else { //Send data to a register
			pDev->pGPIO->pfDigitalWriteByPin(pDev->pGPIO, pDev->nRegSelPin, false);
		}
		
		if (pDev->bUse8DataPins == true) { //1 write, 8 data lines
			//Set all data pins
			nCurrBit = 0x01;
			for (nCtr = 0; nCtr < 8; nCtr++) {
				if ((nCurrBit & nByte) == nCurrBit) {
					pDev->pGPIO->pfDigitalWriteByPin(pDev->pGPIO, pDev->anDataPins[nCtr], true);
				} else {
					pDev->pGPIO->pfDigitalWriteByPin(pDev->pGPIO, pDev->anDataPins[nCtr], false);
				}
				
				nCurrBit <<= 1;
			}
			
			//Pulse the LCD to read the data
			pDev->pTime->pfDelayMicroSeconds(US2066_ENPULSEPREUSEC);
			pDev->pGPIO->pfDigitalWriteByPin(pDev->pGPIO, pDev->nEnablePin, true);
			pDev->pTime->pfDelayMicroSeconds(US2066_ENPULSEONUSEC);
			pDev->pGPIO->pfDigitalWriteByPin(pDev->pGPIO, pDev->nEnablePin, false);
			pDev->pTime->pfDelayMicroSeconds(US2066_ENPULSEPOSTUSEC);
		} else { //2 writes, 4 data line
			//High nibble
			nCurrBit = 0x10;
			for (nCtr = 4; nCtr < 8; nCtr++) {
				if ((nCurrBit & nByte) == nCurrBit) {
					pDev->pGPIO->pfDigitalWriteByPin(pDev->pGPIO, pDev->anDataPins[nCtr], true);
				} else {
					pDev->pGPIO->pfDigitalWriteByPin(pDev->pGPIO, pDev->anDataPins[nCtr], false);
				}
				
				nCurrBit <<= 1;
			}
			
			//Pulse the LCD to read the data
			pDev->pTime->pfDelayMicroSeconds(US2066_ENPULSEPREUSEC);
			pDev->pGPIO->pfDigitalWriteByPin(pDev->pGPIO, pDev->nEnablePin, true);
			pDev->pTime->pfDelayMicroSeconds(US2066_ENPULSEONUSEC);
			pDev->pGPIO->pfDigitalWriteByPin(pDev->pGPIO, pDev->nEnablePin, false);
			pDev->pTime->pfDelayMicroSeconds(US2066_ENPULSEPOSTUSEC);
			
			//Low nibble
			nCurrBit = 0x01;
			for (nCtr = 4; nCtr < 8; nCtr++) {
				if ((nCurrBit & nByte) == nCurrBit) {
					pDev->pGPIO->pfDigitalWriteByPin(pDev->pGPIO, pDev->anDataPins[nCtr], true);
				} else {
					pDev->pGPIO->pfDigitalWriteByPin(pDev->pGPIO, pDev->anDataPins[nCtr], false);
				}
				
				nCurrBit <<= 1;
			}
			
			//Pulse the LCD to read the data
			pDev->pTime->pfDelayMicroSeconds(US2066_ENPULSEPREUSEC);
			pDev->pGPIO->pfDigitalWriteByPin(pDev->pGPIO, pDev->nEnablePin, true);
			pDev->pTime->pfDelayMicroSeconds(US2066_ENPULSEONUSEC);
			pDev->pGPIO->pfDigitalWriteByPin(pDev->pGPIO, pDev->nEnablePin, false);
			pDev->pTime->pfDelayMicroSeconds(US2066_ENPULSEPOSTUSEC);
		}
	} else { //SPI interface is set, use that (no send to RAM handling needed)
		pDev->pGPIO->pfDigitalWriteByPin(pDev->pGPIO, pDev->nEnablePin, false); //Chip select
		pDev->pSPI->pfBeginTransfer(pDev->pSPI);
	
		nCurrBit = US2066_SPISTART; //Start of transfer
		nCurrBit |= US2066_SPIWRITE; //Read/Write bit = Write
		
		if (bSendToRam == true) {
			nCurrBit |= US2066_SPIDATA; //Data/Command bit = Data
		} else {
			nCurrBit |= US2066_SPICOMMAND; //Data/Command bit = Command
		}
		
		//The device gets the value with LSB first
		nByte = ReverseBitsInUInt8(nByte);
		
		pDev->pSPI->pfTransferByte(pDev->pSPI, nCurrBit, &nCtr); //Send transfer start byte
		
		nCurrBit = 0xF0 & nByte; //Write high nibble fist (lower order bits)
		pDev->pSPI->pfTransferByte(pDev->pSPI, nCurrBit, &nCtr); //Writing only, discard read 
		
		nCurrBit = (0x0F & nByte) << 4; //Write low nibble (higher order bits)
		pDev->pSPI->pfTransferByte(pDev->pSPI, nCurrBit, &nCtr); //Writing only, discard read 
		
		pDev->pSPI->pfEndTransfer(pDev->pSPI);
		pDev->pGPIO->pfDigitalWriteByPin(pDev->pGPIO, pDev->nEnablePin, true); //Chip select
		pDev->pTime->pfDelayMicroSeconds(US2066_ENPULSEPOSTUSEC);
	}
	
	return US2066_Success;
}