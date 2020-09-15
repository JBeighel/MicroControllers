/*
	File:	TF02Driver.c
	Author:	J. Beighel
	Date:	09-15-2020
*/


/*****	Includes	*****/
	#include "TF02Driver.h"

/*****	Constants	*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/


/*****	Functions	*****/

eTF02Returns_t TF02Initialize(sTF02Device_t *pTF02Obj, sUARTIface_t *pUART) {
	pTF02Obj->pUART = pUART;
	
	return TF02_Success;
}