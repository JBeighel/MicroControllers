/*
	File:	CommonUtils.h
	Author:	J. Beighel
	Date:	2021-01-26
*/

/***** Includes		*****/
	#include "CommonUtils.h"


/***** Constants	*****/


/***** Definitions	*****/


/***** Globals		*****/


/***** Prototypes 	*****/


/***** Functions	*****/
bool NumberInArray_u8(uint8_t nNumber, const uint8_t *aList, uint32_t nListCnt) {
	uint32_t nCtr;

	for (nCtr = 0; nCtr < nListCnt; nCtr++) {
		if (nNumber == aList[nCtr]) {
			return true;
		}
	}

	return false;
}

bool NumberInArray_u16(uint16_t nNumber, const uint16_t *aList, uint32_t nListCnt) {
	uint32_t nCtr;

	for (nCtr = 0; nCtr < nListCnt; nCtr++) {
		if (nNumber == aList[nCtr]) {
			return true;
		}
	}

	return false;
}

uint32_t IndexInArray_u8(uint8_t nNumber, const uint8_t *aList, uint32_t nListCnt) {
	uint32_t nCtr;

	for (nCtr = 0; nCtr < nListCnt; nCtr++) {
		if (nNumber == aList[nCtr]) {
			return nCtr;
		}
	}

	return nListCnt;
}

uint32_t IndexInArray_u16(uint16_t nNumber, const uint16_t *aList, uint32_t nListCnt) {
	uint32_t nCtr;

	for (nCtr = 0; nCtr < nListCnt; nCtr++) {
		if (nNumber == aList[nCtr]) {
			return nCtr;
		}
	}

	return nListCnt;
}

uint8_t ContinueCalcCRC8(uint8_t nPolynomial, uint8_t nByte, uint8_t nPrevCRC) {
	uint16_t nBitCtr;
	uint8_t nBit;
	uint8_t nTemp;

	for (nBitCtr = 0; nBitCtr < 8; nBitCtr++) {
		nBit = nByte & 0x80; //Get the highest bit

		nTemp = nPrevCRC & 0x80;
		if (nBit != 0) {
			nTemp ^= 0x80;
		}

		nPrevCRC <<= 1;

		if (nTemp != 0) {
			nPrevCRC ^= nPolynomial;
		}

		nByte <<= 1;
	}

	return nPrevCRC;
}

uint8_t CalculateCRC8(uint8_t nPolynomial, uint8_t nInit, uint8_t nXOROut, const uint8_t *pData, uint32_t nDataLen) {
	uint8_t nCrc;
	uint32_t nCtr;

	nCrc = nInit;

	for (nCtr = 0; nCtr < nDataLen; nCtr++) {
		nCrc = ContinueCalcCRC8(nPolynomial, pData[nCtr], nCrc);
	}

	nCrc ^= nXOROut;

	return nCrc;
}

uint32_t CountSetBitsInInt32(uint32_t nVal) {
	//SWAR algorithm
	nVal = nVal - ((nVal >> 1) & 0x55555555); //Break into 1 counts of 2 bit chunks
	nVal = (nVal & 0x33333333) + ((nVal >> 2) & 0x33333333); //Increase to 1 counts of 4 bit chunks
	nVal = (nVal + (nVal >> 4)) & 0x0F0F0F0F; //Increase to 1 counts of 8 bit chunks
	nVal = (nVal * 0x01010101) >> 24; //Sum all chunks into the highest byte, then shift down to lowest

	return nVal;
}

uint16_t CountSetBitsInInt16(uint32_t nVal) {
	//SWAR algorithm
	nVal = nVal - ((nVal >> 1) & 0x5555); //Break into 1 counts of 2 bit chunks
	nVal = (nVal & 0x3333) + ((nVal >> 2) & 0x3333); //Increase to 1 counts of 4 bit chunks
	nVal = (nVal + (nVal >> 4)) & 0x0F0F; //Increase to 1 counts of 8 bit chunks
	nVal = (nVal * 0x0101) >> 8; //Sum all chunks into the highest byte, then shift down to lowest

	return nVal;
}

uint8_t ReverseBitsInUInt8(uint8_t nVal) {
	uint8_t nCtr = 0, nRev = 0;
	
	for (nCtr = 0; nCtr < 8; nCtr++) {
		nRev <<= 1;
		nRev |= nVal & 0x01;
		nVal >>= 1;
	}
	
	return nRev;
}
