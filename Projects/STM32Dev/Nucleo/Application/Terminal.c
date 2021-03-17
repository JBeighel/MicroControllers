/**	File:	Terminal.c
	Author:	J. Beighel
	Date:	2021-03-17
*/

/*****	Includes	*****/


/*****	Defines		*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
eReturn_t IOCnctReadByteNoImplement(sIOObject_t *pIOObj, uint8_t *pnByte);
eReturn_t IOCnctWriteByteNoImplement(sIOObject_t *pIOObj, uint8_t nByte);
eReturn_t IOCnctWriteTextNoImplement(sIOObject_t *pIOObj, char *pText);
eReturn_t IOCnctReadDataNoImplement(sIOObject_t *pIOObj, uint8_t *pnDataBuff, uint32_t nBuffSize, uint32_t *pnReadSize);
eReturn_t IOCnctWriteDataNoImplement(sIOObject_t *pIOObj, uint8_t *pnData, uint32_t nDataLen);

/*****	Functions	*****/
eReturn_t IOCnctObjectInitialize(sIOObject_t *pIOObj) {
	pIOObj->pfReadByte = &IOCnctReadByteNoImplement;
	pIOObj->pfWriteByte = &IOCnctWriteByteNoImplement;
	pIOObj->pfReadData = &IOCnctReadDataNoImplement;
	pIOObj->pfWriteData = &IOCnctWriteDataNoImplement;
	pIOObj->pfWriteText = &IOCnctWriteTextNoImplement;
	
	pIOObj->pHWInfo = NULL;
	
	return Success;
}

eReturn_t IOCnctReadByteNoImplement(sIOObject_t *pIOObj, uint8_t *pnByte) {
	return Fail_NotImplem;
}

eReturn_t IOCnctWriteByteNoImplement(sIOObject_t *pIOObj, uint8_t nByte)  {
	return Fail_NotImplem;
}

eReturn_t IOCnctWriteTextNoImplement(sIOObject_t *pIOObj, char *pText) {
	return Fail_NotImplem;
}

eReturn_t IOCnctReadDataNoImplement(sIOObject_t *pIOObj, uint8_t *pnDataBuff, uint32_t nBuffSize, uint32_t *pnReadSize) {
	return Fail_NotImplem;
}

eReturn_t IOCnctWriteDataNoImplement(sIOObject_t *pIOObj, uint8_t *pnData, uint32_t nDataLen) {
	return Fail_NotImplem;
}