/**	@defgroup	terminal	Generic terminal interfaces
	@brief		Generic terminal user interface
	@details	v0.1
	#Description
	
	#File Information
		File:	Terminal.h
		Author:	J. Beighel
		Date:	2021-03-17
*/

#ifndef __TERMINAL_H
	#define __TERMINAL_H

/*****	Includes	*****/
	#include "CommonUtils.h"

/*****	Defines		*****/


/*****	Definitions	*****/
	typedef struct sIOConnect_t;
	
	typedef eIO

	typedef eReturn_t (*pfIOCnctReadByte_t)(sIOConnect_t *pIOObj, uint8_t *pnByte);
	typedef eReturn_t (*pfIOCnctWriteByte_t)(sIOConnect_t *pIOObj, uint8_t nByte);
	typedef eReturn_t (*pfIOCnctWriteText_t)(sIOConnect_t *pIOObj, char *pText);
	typedef eReturn_t (*pfIOCnctReadData_t)(sIOConnect_t *pIOObj, uint8_t *pnDataBuff, uint32_t nBuffSize, uint32_t *pnReadSize);
	typedef eReturn_t (*pfIOCnctWriteData_t)(sIOConnect_t *pIOObj, uint8_t *pnData, uint32_t nDataLen);

	typedef struct sIOConnect_t {
		pfIOCnctReadByte_t pfReadByte;
		pfIOCnctWriteByte_t pfWriteByte;
		pfIOCnctReadData_t pfReadData;
		pfIOCnctWriteData_t pfWriteData;
		
		pfIOCnctWriteText_t pfWriteText;
		
		void *pHWInfo;
	} sIOConnect_t;

/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	eReturn_t IOCnctObjectInitialize(sIOObject_t *pIOObj);

/*****	Functions	*****/


#endif

