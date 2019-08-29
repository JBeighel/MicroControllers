/**	@defgroup	DS3231DRIVER
	@brief		Driver for the DS3231 Real Time CLock chip
	@details	
		
*/

#ifndef __DS3231DRIVER_H
	#define __DS3231DRIVER_H

/***** Includes		*****/
	#include <Wire.h>
	#include <time.h>

	#include "CommonUtils.h"

/***** Constants	*****/
	#define RTC_CENTURY			2000

	#define RTC_NOPIN			0xFF

	#define RTC_DS3231_ADDR		0x68

/***** Definitions	*****/
	typedef enum eDS3231Registers_t {
		RTC_Seconds				= 0x00,		/**< BCD coded value of the seconds for the time stamp */
		RTC_Minutes				= 0x01,		/**< BCD coded value of the minutes for the time stamp */
		RTC_Hours				= 0x02,		/**< Flags and BCD coded hours for the time stamp */
		RTC_Day					= 0x03,
		RTC_Date				= 0x04,
		RTC_MonthCentury		= 0x05,
		RTC_Year				= 0x06,
		RTC_Alarm1Sec			= 0x07,
		RTC_Alarm1Minutes		= 0x08,
		RTC_Alarm1Hours			= 0x09,
		RTC_Alarm1DayDate		= 0x0A,
		RTC_Alarm2Minutes		= 0x0B,
		RTC_Alarm2Hours			= 0x0C,
		RTC_Alarm2DayDate		= 0x0D,
		RTC_Control				= 0x0E,
		RTC_ControlStatus		= 0x0F,
		RTC_AgingOffset			= 0x10,
		RTC_TempMSB				= 0x11,		/**< Integer portion of the temperature in celsius, 8 bit signed int */
		RTC_TempLSB				= 0x12,		/**< Fraction portion of temperature in celsius, increments of 0.25 */
	} eDS3231Registers_t;

	typedef enum eDS3231_RegHours_t {
		RTC_Hours_1224			= 0x40,
		RTC_Hours_AMPM			= 0x20,
		RTC_Hours_BCD24Mask		= 0x3F,
		RTC_Hours_BCD12Mask		= 0x1F,
	} eDS3231_RegHours_t;

	typedef enum eDS3231_RegMonth_t {
		RTC_Month_Century		= 0x80,
		RTC_Month_BCDMask		= 0x1F,
	} eDS3231_RegMonth_t;

	typedef enum eDS3231_RegAlrmSeconds_t {
		RTC_AlrmSec_AM1			= 0x80,
		RTC_AlrmSec_BCDMask		= 0x7F,
	} eDS3231_RegAlrm1Seconds_t;

	typedef enum eDS3231_RegAlrmMinutes_t {
		RTC_AlrmMin_AM2			= 0x80,
		RTC_AlrmMin_BCDMask		= 0x7F,
	} eDS3231_RegAlrm1Minutes_t;

	typedef enum eDS3231_RegAlrmHours_t {
		RTC_AlrmHrs_AM3			= 0x80,
		RTC_AlrmHrs_1224		= 0x40,
		RTC_AlrmHrs_AMPM		= 0x20,
		RTC_AlrmHrs_BCD24Mask	= 0x3F,
		RTC_AlrmHrs_BCD12Mask	= 0x1F,
	} eDS3231_RegAlrmHours_t;

	typedef enum eDS3231_RegAlrmDay_t {
		RTC_AlrmDay_AM4			= 0x80,
		RTC_AlrmDay_DayDate		= 0x40,
		RTC_AlrmDay_BCDDayMask	= 0x0F,
		RTC_AlrmDay_BCDDateMask	= 0x3F,
	} eDS3231_RegAlrmDay_t;

	typedef enum eDS3231_RegControl_t {
		RTC_Ctrl_EOSC			= 0x80,
		RTC_Ctrl_BBSQW			= 0x40,
		RTC_Ctrl_CONV			= 0x20,
		RTC_Ctrl_RS2			= 0x10,
		RTC_Ctrl_RS1			= 0x08,
		RTC_Ctrl_INTCN			= 0x04,
		RTC_Ctrl_A2IE			= 0x02,
		RTC_Ctrl_A1IE			= 0x01,
	} eDS3231_RegControl_t;

	typedef enum eDS3231_RegCtrlStat_t {
		RTC_CtrlStat_OSF		= 0x80,
		RTC_CtrlStat_EN32KHz	= 0x08,
		RTC_CtrlStat_BSY		= 0x04,
		RTC_CtrlStat_A2F		= 0x02,
		RTC_CtrlStat_A1F		= 0x01,
	} eDS3231_RegCtrlStat_t;

	typedef enum eDS3231_RegTempLSB_t {
		RTC_TempLSB_00			= 0x00,
		RTC_TempLSB_25			= 0x40,
		RTC_TempLSB_50			= 0x80,
		RTC_TempLSB_75			= 0xC0,
	} eDS3231_RegTempLSB_t;

	typedef class cDS3231_t {
	public:
		cDS3231_t(uint8_t nIntPin, uint8_t nResetPin, TwoWire *pI2CWire);
		~cDS3231_t();
		bool Initialize();
		bool ReadDateAndTime(struct tm *tTime);
		bool SetDateAndTime(const struct tm *tTime);
		bool SetDateOnly(const struct tm *tTime);
		bool SetTimeOnly(const struct tm *tTime);

	protected:
		uint8_t I2CReadUint8Reg(uint8_t nDevAddr, uint8_t nRegAddr);
		bool I2CWriteUint8Reg(uint8_t nDevAddr, uint8_t nRegAddr, uint8_t nValue);

	private:
		uint8_t cnIntPin;
		uint8_t cnResetPin;

		TwoWire *cpI2CBus;
	};

/***** Globals		*****/


/***** Prototypes 	*****/
	#define BCD8toUint8(nBCD8)	((((nBCD8 & 0xF0) >> 4) * 10) + (nBCD8 & 0x0F))

	#define Uint8toBCD8(nUint8)	(((nUint8 / 10) << 4) + (nUint8 % 10))

	#define RTCHoursRegTo24HUint8(nRegVal)	((CheckAllBitsInMask(nRegVal, RTC_Hours_1224) == 1) ? (BCD8toUint8(RTC_Hours_BCD12Mask & nRegVal) + (12 * CheckAllBitsInMask(nRegVal, RTC_Hours_AMPM))) : (BCD8toUint8(RTC_Hours_BCD24Mask & nRegVal)))

/***** Functions	*****/
cDS3231_t::cDS3231_t(uint8_t nIntPin, uint8_t nResetPin, TwoWire *pI2CWire) {
	cnIntPin = nIntPin;
	cnResetPin = nResetPin;
	cpI2CBus = pI2CWire;

	return;
}

cDS3231_t::~cDS3231_t() {
	//No resources allocated, no cleanup to be done

	return;
}

bool cDS3231_t::Initialize() {
	//Set mode of GPIO pins
	if (cnResetPin != RTC_NOPIN) {
		pinMode(cnResetPin, OUTPUT);
	}

	if (cnIntPin != RTC_NOPIN) {
		pinMode(cnIntPin, INPUT_PULLUP);
	}

	//Prepare the I2C bus
	cpI2CBus->begin();

	//Power on reset needed?
	//The radio is hooked up I2C, configure it that way

	return true;
}

bool cDS3231_t::ReadDateAndTime(struct tm *tTime) {
	uint8_t	nByteRecv;

	nByteRecv = I2CReadUint8Reg(RTC_DS3231_ADDR, RTC_Seconds);
	tTime->tm_sec = BCD8toUint8(nByteRecv);

	nByteRecv = I2CReadUint8Reg(RTC_DS3231_ADDR, RTC_Minutes);
	tTime->tm_min = BCD8toUint8(nByteRecv);

	nByteRecv = I2CReadUint8Reg(RTC_DS3231_ADDR, RTC_Hours);
	tTime->tm_hour = RTCHoursRegTo24HUint8(nByteRecv);

	nByteRecv = I2CReadUint8Reg(RTC_DS3231_ADDR, RTC_Date);
	tTime->tm_mday = BCD8toUint8(nByteRecv);

	nByteRecv = I2CReadUint8Reg(RTC_DS3231_ADDR, RTC_Day);
	tTime->tm_wday = BCD8toUint8(nByteRecv) - 1; //The RTC gives 1-7, but the struct specifies 0-6

	nByteRecv = I2CReadUint8Reg(RTC_DS3231_ADDR, RTC_MonthCentury);
	tTime->tm_mon = BCD8toUint8(nByteRecv & RTC_Month_BCDMask);
	if (CheckAllBitsInMask(nByteRecv, RTC_Month_Century) == 1) {
		tTime->tm_year = 100;
	} else {
		tTime->tm_year = 0;
	}

	nByteRecv = I2CReadUint8Reg(RTC_DS3231_ADDR, RTC_Year);
	tTime->tm_year += BCD8toUint8(nByteRecv);

	//The RTC doesn't provide these last values in the tm struct
	tTime->tm_yday = 0;
	tTime->tm_isdst = 0;

	return true;
}

bool cDS3231_t::SetDateAndTime(const struct tm *tTime) {
	SetDateOnly(tTime);
	SetTimeOnly(tTime);

	return true;
}

bool cDS3231_t::SetDateOnly(const struct tm *tTime) {
	uint8_t nByteSend;
	uint16_t nYear;

	nByteSend = Uint8toBCD8(tTime->tm_mday);
	I2CWriteUint8Reg(RTC_DS3231_ADDR, RTC_Date, nByteSend);

	nByteSend = Uint8toBCD8(tTime->tm_wday);
	I2CWriteUint8Reg(RTC_DS3231_ADDR, RTC_Day, nByteSend);

	nYear = tTime->tm_year;
	if (nYear > 1900) {
		nYear -= 1900; //The RTC starts at year 1900
	}

	if (nYear >= 100) {
		nByteSend = Uint8toBCD8(tTime->tm_mon);
		nByteSend |= RTC_Month_Century;
		I2CWriteUint8Reg(RTC_DS3231_ADDR, RTC_MonthCentury, nByteSend);

		nYear -= 100; //Remove the years represented in the century flag
	} else {
		nByteSend = Uint8toBCD8(tTime->tm_mon);
		I2CWriteUint8Reg(RTC_DS3231_ADDR, RTC_MonthCentury, nByteSend);
	}
	nByteSend = Uint8toBCD8(nYear);
	I2CWriteUint8Reg(RTC_DS3231_ADDR, RTC_Year, nByteSend);

	return true;
}

bool cDS3231_t::SetTimeOnly(const struct tm *tTime) {
	uint8_t nByteSend;

	nByteSend = Uint8toBCD8(tTime->tm_sec);
	I2CWriteUint8Reg(RTC_DS3231_ADDR, RTC_Seconds, nByteSend);

	nByteSend = Uint8toBCD8(tTime->tm_min);
	I2CWriteUint8Reg(RTC_DS3231_ADDR, RTC_Minutes, nByteSend);

	nByteSend = Uint8toBCD8(tTime->tm_hour);
	ZeroAllBitsInMask(nByteSend, RTC_Hours_1224); //Set the RTC into 24 hour mode
	I2CWriteUint8Reg(RTC_DS3231_ADDR, RTC_Hours, nByteSend);

	return true;
}

uint8_t cDS3231_t::I2CReadUint8Reg(uint8_t nDevAddr, uint8_t nRegAddr) {
	uint8_t nByteRecv;

	cpI2CBus->beginTransmission(nDevAddr);
	Wire.write(nRegAddr);
	cpI2CBus->endTransmission();

	cpI2CBus->requestFrom(nDevAddr, (uint8_t)1); //This function assumes registers have 1 byte
	nByteRecv = cpI2CBus->read();

	return nByteRecv;
}

bool cDS3231_t::I2CWriteUint8Reg(uint8_t nDevAddr, uint8_t nRegAddr, uint8_t nValue) {
	cpI2CBus->beginTransmission(nDevAddr);
	Wire.write(nRegAddr);
	Wire.write(nValue);
	cpI2CBus->endTransmission();

	return true;
}

#endif

