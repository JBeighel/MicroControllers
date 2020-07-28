/**	@defgroup	scl3300
	@brief		DSPI Driver for Inclinometer
	@details	
		
*/

#ifndef __SCL3300-D01
	#define __SCL3300-D01

	#define SCL3300_DEBUG
	#ifdef SCL3300_DEBUG
		#define DebugPrint(...)	Serial.print(__VA_ARGS__)
	#else
		#define DebugPrint(...)
	#endif

/***** Includes		*****/
	#include "IfaceSpi.h"

/***** Definitions	*****/
	#define SCL3300_CRC_INIT	0xFF
	#define SCL3300_CRC_POLY	0x1D
	#define SCL3300_CRC_XOROUT	0xFF
	
	#define SCL3300_WHOAMI_RESP	0x00C1

	typedef enum eSCL3300Op_t {
		SCL3300_OP_Mask		= 0x80,
		
		SCL3300_OP_Read		= 0x00,
		SCL3300_OP_Write	= 0x80,
	} eSCL3300Op_t;
	
	typedef enum eSCL3300RetStatus_ {
		SCL3300_RS_Mask		= 0x03,
		
		SCL3300_RS_MOSI		= 0x00,
		SCL3300_RS_Startup	= 0x00,
		SCL3300_RS_Normal	= 0x01,
		SCL3300_RS_SelfTest	= 0x02,
		SCL3300_RS_Error	= 0x03,
	} eSCL3300RetStatus_t;
	
	typedef enum eSCL3300RegCmd_t {
		SCL3300_Cmd_ModeMask= 0x0003,
		SCL3300_Cmd_Mode1	= 0x0000,
		SCL3300_Cmd_Mode2	= 0x0001,
		SCL3300_Cmd_Mode3	= 0x0002,
		SCL3300_Cmd_Mode4	= 0x0003,
		
		SCL3300_Cmd_PwrMask	= 0x0004,
		SCL3300_Cmd_PwrWake	= 0x0000,
		SCL3300_Cmd_PwrDown	= 0x0004,
		
		SCL3300_Cmd_Reset	= 0x0020,
	} eSCL3300RegCmd_t;
	
	typedef enum eSCL3300RegBank_t {
		SCL3300_Bank_Mask	= 0x0001,
		SCL3300_Bank_0		= 0x0000,
		SCL3300_Bank_1		= 0x0001,
	} eSCL3300RegBank_t;
	
	typedef enum eSCL3300RegAngCtrl_t {
		SCL3300_Ang_Mask	= 0x1F,
		SCL3300_Ang_EnAll	= 0x1F,
	} eSCL3300RegAngCtrl_t;
	
	typedef enum eSCL3300RegAddr_t {
		SCL3300_Reg_Mask	= 0x7C,
		
		SCL3300_Reg_ACCX	= 0x04, /**< Read only, Banks 0 or 1 */
		SCL3300_Reg_ACCY	= 0x08, /**< Read only, Banks 0 or 1 */
		SCL3300_Reg_ACCZ	= 0x0C, /**< Read only, Banks 0 or 1 */
		SCL3300_Reg_STO		= 0x10, /**< Read only, Banks 0 or 1 */
		SCL3300_Reg_ANGCTRL	= 0x30, /**< Write only, Banks 0 */
		SCL3300_Reg_ANGX	= 0x24, /**< Read only, Banks 0 */
		SCL3300_Reg_ANGY	= 0x28, /**< Read only, Banks 0 */
		SCL3300_Reg_ANGZ	= 0x2C, /**< Read only, Banks 0 */
		SCL3300_Reg_Temp	= 0x14, /**< Read only, Banks 0 or 1 */
		SCL3300_Reg_Status	= 0x18, /**< Read only, Banks 0 or 1 */
		SCL3300_Reg_ERRFLAG1= 0x1C, /**< Read only, Banks 0 */
		SCL3300_Reg_ERRFLAG2= 0x20, /**< Read only, Banks 0 */
		SCL3300_Reg_CMD		= 0x34, /**< Read or Write, Banks 0 */
		SCL3300_Reg_WHOAMI	= 0x40, /**< Read only, Banks 0 */
		SCL3300_Reg_SERIAL1	= 0x64, /**< Read only, Banks 1 */
		SCL3300_Reg_SERIAL2	= 0x68, /**< Read only, Banks 1 */
		SCL3300_Reg_BANK	= 0x7C, /**< Read or Write, Banks 0 or 1 */
	} eSCL3300RegAddr_t;

	class cSCL3300_t {
		public:
			cSCL3300_t(iSPIHandler_t *pSpiDev);
			~cSCL3300_t();
			
			bool Initialize(int8_t nCSPin);
			
			bool BuildReadMessage(eSCL3300RegAddr_t eReg, uint8_t *pMsg);
			bool BuildWriteMessage(eSCL3300RegAddr_t eReg, uint16_t nData, uint8_t *pMsg);
			bool SendMsgGetResp(uint8_t pSendMsg[4], uint8_t *pRecvMsg);
			bool SendMsg(uint8_t pSendMsg[4]);
			bool ParseMessage(const uint8_t pMsg[4], eSCL3300RegAddr_t *peReg, eSCL3300RetStatus_t *peStatus, uint16_t *pnData);
			bool ReadAngles(int16_t *nX, int16_t *nY, int16_t *nZ);
			
		protected:
		
		private:
			iSPIHandler_t *cpSpiDev;
			uint8_t cnChipSelectPin;
			uint8_t caReadStatus[4];
	};

/***** Constants	*****/


/***** Globals		*****/


/***** Prototypes 	*****/


/***** Functions	*****/
cSCL3300_t::cSCL3300_t(iSPIHandler_t *pSpiDev) {
	cpSpiDev = pSpiDev;
	
	BuildReadMessage(SCL3300_Reg_Status, caReadStatus);
	
	return;
}

cSCL3300_t::~cSCL3300_t() {
	return;
}

bool cSCL3300_t::Initialize(int8_t nCSPin) {
	uint8_t aSendMsg[4], aRecvMsg[4];
	uint16_t nData;
	bool bResult;
	eSCL3300RegAddr_t eReg;
	eSCL3300RetStatus_t eStatus;
	
	cnChipSelectPin = nCSPin;
	
	pinMode(cnChipSelectPin, OUTPUT);
	digitalWrite(cnChipSelectPin, HIGH); //Unselected at start
	
	delay(10);
	BuildReadMessage(SCL3300_Reg_STO, aSendMsg);
	SendMsgGetResp(aSendMsg, aRecvMsg);
	bResult = ParseMessage(aRecvMsg, &eReg, &eStatus, &nData);
	if (bResult == false) {
		DebugPrint("Read self test parse fail");
		return false;
	}
	
	DebugPrint("Rd Self Test Status ");
	DebugPrint(eStatus, HEX);
	DebugPrint(" Reg ");
	DebugPrint(eReg, HEX);
	DebugPrint(" Data ");
	DebugPrint((int16_t)nData);
	DebugPrint("\n");
	
	//set measurement mode
	BuildWriteMessage(SCL3300_Reg_CMD, SCL3300_Cmd_Reset, aSendMsg);
	digitalWrite(9, HIGH);
	SendMsg(aSendMsg);
	digitalWrite(9, LOW);
	ParseMessage(aRecvMsg, &eReg, &eStatus, &nData);
	
	DebugPrint("Reset Cmd Status ");
	DebugPrint(eStatus, HEX);
	DebugPrint(" Reg ");
	DebugPrint(eReg, HEX);
	DebugPrint(" Data ");
	DebugPrint(nData, HEX);
	DebugPrint("\n");
	
	//10 ms wait
	delay(10);
	//set measurement mode
	BuildWriteMessage(SCL3300_Reg_CMD, SCL3300_Cmd_Mode3, aSendMsg);
	SendMsgGetResp(aSendMsg, aRecvMsg);
	ParseMessage(aRecvMsg, &eReg, &eStatus, &nData);
	//Initial command has indeterminate results, ignore them
	
	DebugPrint("Mode1 Cmd Status ");
	DebugPrint(eStatus, HEX);
	DebugPrint(" Reg ");
	DebugPrint(eReg, HEX);
	DebugPrint(" Data ");
	DebugPrint(nData, HEX);
	DebugPrint("\n");
	
	//wait 5ms
	delay(5);
	//read status RS 11
	BuildReadMessage(SCL3300_Reg_Status, aSendMsg);
	SendMsgGetResp(aSendMsg, aRecvMsg);
	bResult = ParseMessage(aRecvMsg, &eReg, &eStatus, &nData);
	if (bResult == false) {
		DebugPrint("Read status #1 parse fail");
		return false;
	}
	
	DebugPrint("Rd #1 Status ");
	DebugPrint(eStatus, HEX);
	DebugPrint(" Reg ");
	DebugPrint(eReg, HEX);
	DebugPrint(" Data ");
	DebugPrint(nData, HEX);
	DebugPrint("\n");
	
	//Read status RS 11
	BuildReadMessage(SCL3300_Reg_Status, aSendMsg);
	SendMsgGetResp(aSendMsg, aRecvMsg);
	bResult = ParseMessage(aRecvMsg, &eReg, &eStatus, &nData);
	if (bResult == false) {
		DebugPrint("Read status #2 parse fail");
		return false;
	}
	
	DebugPrint("Rd #2 Status ");
	DebugPrint(eStatus, HEX);
	DebugPrint(" Reg ");
	DebugPrint(eReg, HEX);
	DebugPrint(" Data ");
	DebugPrint(nData, HEX);
	DebugPrint("\n");
	
	//read status RS 01  indicates proper startup
	BuildReadMessage(SCL3300_Reg_Status, aSendMsg);
	digitalWrite(9, HIGH);
	SendMsgGetResp(aSendMsg, aRecvMsg);
	digitalWrite(9, LOW);
	bResult = ParseMessage(aRecvMsg, &eReg, &eStatus, &nData);
	if (bResult == false) {
		DebugPrint("Read status #3 parse fail");
		return false;
	}
	
	DebugPrint("Rd #3 Status ");
	DebugPrint(eStatus);
	DebugPrint(" Reg ");
	DebugPrint(eReg, HEX);
	DebugPrint(" Data ");
	DebugPrint(nData, HEX);
	DebugPrint("\n");
	
	if (eStatus != SCL3300_RS_Normal) { //Sensor should be in normal state indicating its ready for use
		DebugPrint(aRecvMsg[0], HEX);
		DebugPrint(" ");
		DebugPrint(aRecvMsg[1], HEX);
		DebugPrint(" ");
		DebugPrint(aRecvMsg[2], HEX);
		DebugPrint(" ");
		DebugPrint(aRecvMsg[3], HEX);
		DebugPrint(" ");
		DebugPrint("Read status #3 bad status fail\n");
		//return false;
	}
	
	//write ang_ctrl enables angle outputs
	BuildWriteMessage(SCL3300_Reg_ANGCTRL, SCL3300_Ang_EnAll, aSendMsg);
	digitalWrite(9, HIGH);
	SendMsgGetResp(aSendMsg, aRecvMsg);
	digitalWrite(9, LOW);
	bResult = ParseMessage(aRecvMsg, &eReg, &eStatus, &nData);
	
	DebugPrint("Enable Angle Status ");
	DebugPrint(eStatus);
	DebugPrint(" Reg ");
	DebugPrint(eReg, HEX);
	DebugPrint(" Data ");
	DebugPrint(nData, HEX);
	DebugPrint("\n");
	
	if (bResult == false) {
		DebugPrint(aRecvMsg[0], HEX);
		DebugPrint(" ");
		DebugPrint(aRecvMsg[1], HEX);
		DebugPrint(" ");
		DebugPrint(aRecvMsg[2], HEX);
		DebugPrint(" ");
		DebugPrint(aRecvMsg[3], HEX);
		DebugPrint(" ");
		DebugPrint("Set Angle Enable parse fail\n");
		//return false;
	}
	
	//May as well check what we're talking to
	digitalWrite(9, HIGH);
	BuildReadMessage(SCL3300_Reg_WHOAMI, aSendMsg);
	SendMsgGetResp(aSendMsg, aRecvMsg);
	bResult = ParseMessage(aRecvMsg, &eReg, &eStatus, &nData);
	digitalWrite(9, LOW);
	if (bResult == false) {
		DebugPrint("Set Who am I parse fail\n");
		return false;
	}
	
	if (nData != SCL3300_WHOAMI_RESP) { //It isn't who its supposed to be?
		DebugPrint(aRecvMsg[0], HEX);
		DebugPrint(" ");
		DebugPrint(aRecvMsg[1], HEX);
		DebugPrint(" ");
		DebugPrint(aRecvMsg[2], HEX);
		DebugPrint(" ");
		DebugPrint(aRecvMsg[3], HEX);
		DebugPrint(" Data:");
		DebugPrint(nData, HEX);
		
		DebugPrint("Set Who am I data fail\n");
		return false;
	}
	
	return true;
}

bool cSCL3300_t::BuildReadMessage(eSCL3300RegAddr_t eReg, uint8_t *pMsg) {
	pMsg[0] = SCL3300_OP_Read | eReg | SCL3300_RS_MOSI;
	pMsg[1] = 0x00; //Don't sent data for reads
	pMsg[2] = 0x00;
	
	pMsg[3] = CalculateCRC8(SCL3300_CRC_POLY, SCL3300_CRC_XOROUT, SCL3300_CRC_INIT, pMsg, 3);
	
	return true;
}

bool cSCL3300_t::BuildWriteMessage(eSCL3300RegAddr_t eReg, uint16_t nData, uint8_t *pMsg) {
	pMsg[0] = SCL3300_OP_Write | eReg | SCL3300_RS_MOSI;
	pMsg[1] = (nData & 0xFF00) >> 8;
	pMsg[2] = nData & 0xFF;
	
	pMsg[3] = CalculateCRC8(SCL3300_CRC_POLY, SCL3300_CRC_XOROUT, SCL3300_CRC_INIT, pMsg, 3);
	
	return true;
}

bool cSCL3300_t::SendMsgGetResp(uint8_t pSendMsg[4], uint8_t *pRecvMsg) {
	uint8_t nCtr;
	
	//Send out message over, requesting data
	cpSpiDev->BeginTransfer(cnChipSelectPin);
	
	for (nCtr = 0; nCtr < 4; nCtr++) {
		cpSpiDev->SendByte(pSendMsg[nCtr]);
	}
	
	cpSpiDev->EndTransfer();
	delayMicroseconds(15);
	
	cpSpiDev->EmptyReadBuffer();

	//Read back the response from the sensor
	cpSpiDev->BeginTransfer(cnChipSelectPin);
	
	for (nCtr = 0; nCtr < 4; nCtr++) {
		cpSpiDev->SendByte(caReadStatus[nCtr]);
		cpSpiDev->ReadByte(&(pRecvMsg[nCtr]));
	}
	
	cpSpiDev->EndTransfer();
	
	return true;
}

bool cSCL3300_t::SendMsg(uint8_t pSendMsg[4]) {
	uint8_t nCtr;
	
	//Send out message over, requesting data
	cpSpiDev->BeginTransfer(cnChipSelectPin);
	
	for (nCtr = 0; nCtr < 4; nCtr++) {
		cpSpiDev->SendByte(pSendMsg[nCtr]);
	}
	
	cpSpiDev->EndTransfer();
	delayMicroseconds(15);
	
	cpSpiDev->EmptyReadBuffer();
	
	return true;
}

bool cSCL3300_t::ParseMessage(const uint8_t pMsg[4], eSCL3300RegAddr_t *peReg, eSCL3300RetStatus_t *peStatus, uint16_t *pnData) {
	uint8_t nCrc;
	
	nCrc = CalculateCRC8(SCL3300_CRC_POLY, SCL3300_CRC_XOROUT, SCL3300_CRC_INIT, pMsg, 3);
	
	if (nCrc != pMsg[3]) {
		DebugPrint("CRC FAIL Msg ");
		DebugPrint(pMsg[3], HEX);
		DebugPrint(" calc ");
		DebugPrint(nCrc, HEX);
		DebugPrint("\n");
		return false;
	}
	
	(*peReg) = (eSCL3300RegAddr_t)(pMsg[0] & SCL3300_Reg_Mask);
	(*peStatus) = (eSCL3300RetStatus_t)(pMsg[0] & SCL3300_RS_Mask);
	
	(*pnData) = pMsg[1] << 8;
	(*pnData) |= pMsg[2];
	
	return true;
}

bool cSCL3300_t::ReadAngles(int16_t *nX, int16_t *nY, int16_t *nZ) {
	uint8_t aSendMsg[4], aRecvMsg[4];
	uint16_t nData;
	bool bResult;
	eSCL3300RegAddr_t eReg;
	eSCL3300RetStatus_t eStatus;
	
	//read status Angle X
	BuildReadMessage(SCL3300_Reg_ANGX, aSendMsg);
	digitalWrite(9, HIGH);
	SendMsgGetResp(aSendMsg, aRecvMsg);
	digitalWrite(9, LOW);
	bResult = ParseMessage(aRecvMsg, &eReg, &eStatus, &nData);
	if (bResult == false) {
		DebugPrint("Read Angle X parse fail");
		return false;
	}
	
	*nX = (int16_t)nData;
	
	//read status Angle X
	BuildReadMessage(SCL3300_Reg_ANGY, aSendMsg);
	SendMsgGetResp(aSendMsg, aRecvMsg);
	bResult = ParseMessage(aRecvMsg, &eReg, &eStatus, &nData);
	if (bResult == false) {
		DebugPrint("Read Angle Y parse fail");
		return false;
	}
	
	*nY = (int16_t)nData;
	
	//read status Angle X
	BuildReadMessage(SCL3300_Reg_ANGZ, aSendMsg);
	SendMsgGetResp(aSendMsg, aRecvMsg);
	bResult = ParseMessage(aRecvMsg, &eReg, &eStatus, &nData);
	if (bResult == false) {
		DebugPrint("Read Angle Z parse fail");
		return false;
	}
	
	*nZ = (int16_t)nData;
	
	return true;
}

#endif

