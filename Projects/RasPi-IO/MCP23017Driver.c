/**	File:	MCP23017Driver.c
	Author:	J. Beighel
	Date:	11-28-2020
*/

#ifndef __NAME_H
	#define __NAME_H

/*****	Includes	*****/
	#include "MCP23017Driver.h"

/*****	Constants	*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/


/*****	Functions	*****/
eMCP23017Returns_t MCP23017Initialize(sMCP23017Info_t *pDev, eMCP23017Address_t eAddr, sI2CIface_t *pI2CIface) {
	uint8_t nIOConVal, nRegVal, nCtr;
	eI2CReturns_t nResult;
	
	//Setup the device object
	pDev->nAddr = (uint8_t)eAddr;
	pDev->pI2C = pI2CIface;
	
	//Set the initial configuration for the IO Expander.  Listing a bit sets it to 1, excluing it sets it to zero.
	nIOConVal = MCP23017_BANK | MCP23017_SEQOP | MCP23017_HAEN;
	pDev->pI2C->pfI2CWriteUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_B0_IOCON, nIOConVal); //Write to both possible IOCON addresses just to be sure
	pDev->pI2C->pfI2CWriteUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_IOCON, nIOConVal);
	
	pDev->pI2C->pfI2CReadUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_IOCON, &nRegVal);
	if (nRegVal != nIOConVal) { //Device did not store the values set
		return MCP23017Fail_Unknown;
	}
	
	//Set all pins to Inputs with pull ups
	pDev->pI2C->pfI2CWriteUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_IODIRA, MCP23017_Pin_All);
	pDev->pI2C->pfI2CWriteUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_IODIRB, MCP23017_Pin_All);
	pDev->pI2C->pfI2CWriteUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_IPOLA, MCP23017_Pin_None);
	pDev->pI2C->pfI2CWriteUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_IPOLA, MCP23017_Pin_None);
	pDev->pI2C->pfI2CWriteUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_GPPUA, MCP23017_Pin_All);
	pDev->pI2C->pfI2CWriteUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_GPPUB, MCP23017_Pin_All);
	pDev->pI2C->pfI2CWriteUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_OLATA, MCP23017_Pin_None); //Not outputs by default, but have a predictable level when set
	pDev->pI2C->pfI2CWriteUint8Reg(pDev->pI2C, pDev->nAddr, MCP23017_OLATA, MCP23017_Pin_None);
	
	return MCP23017_Success;
}

#endif

