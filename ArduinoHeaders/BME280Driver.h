/**	@defgroup	bme280driver
	@brief		
	@details	v1.0
		
*/

#ifndef __BME280Driver_H
	#define __BME280Driver_H

/***** Includes		*****/
	#include "CommonUtils.h"
	#include "I2CDevice.h"

	#define BME280_DEBUG
	#ifdef BME280_DEBUG
		#define DebugPrint(...)	Serial.print(__VA_ARGS__)
	#else
		#define DebugPrint(...)
	#endif

/***** Definitions	*****/
	/**	@brief		Enumeration of all registers in the BME280 
		@ingroup	bme280driver
	*/
	typedef enum eBME280Registers_t {
		BME280_DIG_T1_LSB		= 0x88,	/**< Least significant bits of temperature trim value 1 which is unsigned */
		BME280_DIG_T1_MSB		= 0x89,	/**< Most significant bits of temperature trim value 1 which is unsigned */
		BME280_DIG_T2_LSB		= 0x8A,	/**< Least significant bits of temperature trim value 2 which is signed */
		BME280_DIG_T2_MSB		= 0x8B,	/**< Most significant bits of temperature trim value 2 which is signed */
		BME280_DIG_T3_LSB		= 0x8C,	/**< Least significant bits of temperature trim value 3 which is signed */
		BME280_DIG_T3_MSB		= 0x8D,	/**< Most significant bits of temperature trim value 3 which is signed */
		
		BME280_DIG_P1_LSB		= 0x8E,	/**< Least significant bits of pressure trim value 1 which is unsigned */
		BME280_DIG_P1_MSB		= 0x8F,	/**< Most significant bits of pressure trim value 1 which is unsigned */
		BME280_DIG_P2_LSB		= 0x90,	/**< Least significant bits of pressure trim value 2 which is signed */
		BME280_DIG_P2_MSB		= 0x91,	/**< Most significant bits of pressure trim value 2 which is signed */
		BME280_DIG_P3_LSB		= 0x92,	/**< Least significant bits of pressure trim value 3 which is signed */
		BME280_DIG_P3_MSB		= 0x93,	/**< Most significant bits of pressure trim value 3 which is signed */
		BME280_DIG_P4_LSB		= 0x94,	/**< Least significant bits of pressure trim value 4 which is signed */
		BME280_DIG_P4_MSB		= 0x95,	/**< Most significant bits of pressure trim value 4 which is signed */
		BME280_DIG_P5_LSB		= 0x96,	/**< Least significant bits of pressure trim value 5 which is signed */
		BME280_DIG_P5_MSB		= 0x97,	/**< Most significant bits of pressure trim value 5 which is signed */
		BME280_DIG_P6_LSB		= 0x98,	/**< Least significant bits of pressure trim value 6 which is signed */
		BME280_DIG_P6_MSB		= 0x99,	/**< Most significant bits of pressure trim value 6 which is signed */
		BME280_DIG_P7_LSB		= 0x9A,	/**< Least significant bits of pressure trim value 7 which is signed */
		BME280_DIG_P7_MSB		= 0x9B,	/**< Most significant bits of pressure trim value 7 which is signed */
		BME280_DIG_P8_LSB		= 0x9C,	/**< Least significant bits of pressure trim value 8 which is signed */
		BME280_DIG_P8_MSB		= 0x9D,	/**< Most significant bits of pressure trim value 8 which is signed */
		BME280_DIG_P9_LSB		= 0x9E,	/**< Least significant bits of pressure trim value 9 which is signed */
		BME280_DIG_P9_MSB		= 0x9F,	/**< Most significant bits of pressure trim value 9 which is signed */
		
		BME280_DIG_H1			= 0xA1,	/**< Humidity trim value 1 which is unsigned */
		BME280_DIG_H2_LSB		= 0xE1,	/**< Least significant bits of Humidity trim value 2 which is signed */
		BME280_DIG_H2_MSB		= 0xE2,	/**< Most significant bits of Humidity trim value 2 which is signed */
		BME280_DIG_H3			= 0xE3,	/**< Humidity trim value 3 which is unsigned */
		BME280_DIG_H4_MSB		= 0xE4,	/**< Most significant bits of Humidity trim value 4 which is signed */
		BME280_DIG_H4_LSB		= 0xE5,	/**< 4 Least significant bits of Humidity trim value 4 which is signed */
		BME280_DIG_H5_LSB		= 0xE5,	/**< Most significant bits of Humidity trim value 5 which is signed */
		BME280_DIG_H5_MSB		= 0xE6,	/**< 4 Least significant bits of Humidity trim value 4 which is signed */
		
		BME280_DIG_H4_LSB_MASK	= 0x0F,	/**< Humidity trim value 4 MSB only uses some of the registers bytes, those bytes can be found using this mask */
		BME280_DIG_H4_LSB_SHIFT	= 4,	/**< The high bits of the humidity trim value 4 must be left shifted the amount */
		BME280_DIG_H5_LSB_MASK	= 0xF0,	/**< Humidity trim value 5 LSB only uses some of the registers bytes, those bytes can be found using this mask */
		BME280_DIG_H5_LSB_SHIFT	= 4,	/**< The masked register bits of humidity trim value 5 LSB must be right shifted this amount before becoming the LSB of the trim value.  In addition the high bits must be left shifted the amount */
		
		BME280_REG_ID			= 0xD0,	/**< Chip identifier, BMP280 should be 0x56 or 0x57 for samples, 0x58 for mass production or 0x60 for BME280 */
		BME280_REG_RESET		= 0xE0,	/**< Write 0xB6 to reset the device, all other values are ignored */
		BME280_REG_CTRL_HUM		= 0xF2,	/**< Sets humidity options for the device, takes effect after a write to CTRL_MEAS */
		BME280_REG_STATUS		= 0xF3,	/**< Reports status of the device */
		BME280_REG_CTRL_MEAS	= 0xF4,	/**< Sets pressure and temperature options of the device */
		BME280_REG_CONFIG		= 0xF5,	/**< Sets rate, filter, and interface optiosn of the device */
		BME280_REG_PRESS_MSB	= 0xF7,	/**< Most significant bits of pressure reading */
		BME280_REG_PRESS_LSB	= 0xF8,	/**< Least significant bits of pressure reading */
		BME280_REG_PRESS_XLSB	= 0xF9,	/**< Extra low bits of pressure reading, depends on temperature resolution */
		BME280_REG_TEMP_MSB		= 0xFA,	/**< Most significant bits of temperature reading */
		BME280_REG_TEMP_LSB		= 0xFB,	/**< Least significant bits of temperature reading */
		BME280_REG_TEMP_XLSB	= 0xFC,	/**< Extra low bits of temperature reading, depends on pressure resolution */
		BME280_REG_HUM_MSB		= 0xFD,	/**< Most significant bits of humidity reading */
		BME280_REG_HUM_LSB		= 0xFE,	/**< Least significant bits of humidity reading */
	} eBME280Registers_t;
	
	typedef enum eBME280_IDCodes_t {
		BME280_Unknown			= 0x00,
		BME280_BME280			= 0x60,
		BME280_BMP280Sample1	= 0x56,
		BME280_BMP280Sample2	= 0x57,
		BME280_BMP280			= 0x58,
	} eBME280_IDCodes_t;
	
	/**	@brief		Enumeration of all bit fields in the status register of the BME280 
		@ingroup	bme280driver
	*/
	typedef enum eBME280_RegStatus_t {
		BME280_STATUS_MEASURING	= 0x01,	/**< Set during conversion, cleared when results are in data registers */
		BME280_STATUS_IMUPDATE	= 0x08,	/**< Set when non-volatile settings are being copied into registers, cleared when copy completes */	

		BME280_STATUS_MASK		= 0x09,	/**< Mask to only allow bits used in the status register */
	} eBNE280_RegStatus_t;
	
	/**	@brief		Enumeration of all bit fields in the control measurement register of the BME280 
		@ingroup	bme280driver
	*/
	typedef enum eBME280_RegCrlMeas_t {
		BME280_CTMS_OVRSMP_SKIP		= 0x00,	/**< This setting will have the device not take this reading to saves on conversion time */
		BME280_CTMS_OVRSMP_x1		= 0x01,
		BME280_CTMS_OVRSMP_x2		= 0x02,
		BME280_CTMS_OVRSMP_x4		= 0x03,
		BME280_CTMS_OVRSMP_x8		= 0x04,
		BME280_CTMS_OVRSMP_x16		= 0x05,
		
		BME280_CTMS_OVRSMP_PSHIFT	= 2,	/**< The over sampling values should be written to CTRL_HUM without modification.  To write them to CTRL_MEAS for pressure they must be left shifted this many bytes */
		BME280_CTMS_OVRSMP_TSHIFT	= 5,	/**< The over sampling values should be written to CTRL_HUM without modification.  To write them to CTRL_MEAS for temperature they must be left shifted this many bytes */
		
		BME280_CTMS_MODE_SLEEP		= 0x00,	/**< Place the device in sleep mode */
		BME280_CTMS_MODE_FORCED		= 0x01,	/**< Place the device in forced sampling mode, takes single measurement */
		BME280_CTMS_MODE_NORMAL		= 0x03,	/**< Place the device in normal smapling mode, repeated measurement cycles */
	} eBME280_RegCrlMeas_t;
	
	/**	@brief		Enumeration of all bit fields in the configuration register of the BME280 
		@ingroup	bme280driver
	*/
	typedef enum eBME280_RegConfig_t {
		BME280_CFG_SB_HALF			= 0x00,	/**< 0.5 millisecond delay between samples in normal mode */
		BME280_CFG_SB_62			= 0x20,	/**< 62.6 millisecond delay between samples in normal mode */
		BME280_CFG_SB_125			= 0x40,	/**< 125 millisecond delay between samples in normal mode */
		BME280_CFG_SB_250			= 0x60,	/**< 250 millisecond delay between samples in normal mode */
		BME280_CFG_SB_500			= 0x80,	/**< 500 millisecond delay between samples in normal mode */
		BME280_CFG_SB_1000			= 0xA0,	/**< 1000 millisecond delay between samples in normal mode */
		BME280_CFG_SB_10			= 0xC0,	/**< 10 millisecond delay between samples in normal mode */
		BME280_CFG_SC_20			= 0xE0,	/**< 2- millisecond delay between samples in normal mode */
		BME280_CFG_SB_MASK			= 0xE0,	/**< Mask of all bits used in the standby setting */

		BME280_CFG_FILT_OFF			= 0x00,
		BME280_CFG_FILT_2			= 0x04,
		BME280_CFG_FILT_4			= 0x08,
		BME280_CFG_FILT_8			= 0x0C,
		BME280_CFG_FILT_16			= 0x10,
		BME280_CFG_FILT_MASK		= 0x1C,	/**< Mask of all bits used in the filter setting */
		
		BME280_CFG_SPI_EN			= 0x01,	/**< Enables communication over the SPI bus */
		BME280_CFG_SPI_DIS			= 0x00,	/**< Disables communication over the SPI bus */
	} eBME280_RegConfig_t;
	
	class cBME280_t : cI2CDevice_t {
		public: 
			cBME280_t(bool bSDOHigh, TwoWire *pI2CWire);
			~cBME280_t();
			
			bool CheckDevice();
			bool SetConfig();
			bool ReadCalibrationValues();
			bool ReadMeasurements(float *pnTemp, float *pnPress, float *pnHumid);
			
		protected:
			/**	@brief		Calubration values for hte temperature reading
				@details	Index 0 is an unsigned value
			*/
			int16_t cnTempCal[3];
			
			/**	@brief		Calubration values for hte pressure reading
				@details	Index 0 is an unsigned value
			*/
			int16_t cnPressureCal[9];
			
			/**	@brief		Calubration values for hte humidity reading
				@details	Index 0 is an unsigned 8 bit value
					Index 2 is an unsigned 8 bit value
			*/
			int16_t cnHumidityCal[5];
		
		private:
		
	};

/***** Constants	*****/
	#define BME280_BASEADDR			0x76
	
	#define BME280_SDOADDR			0x01

/***** Globals		*****/


/***** Prototypes 	*****/
	/**	@brief		Converts the 3 Temperature and Pressure registers into a 32bit reading
		@param		nMSB	The most significgant bits register value
		@param		nLSB	The least significgant bits register value
		@param		nXLSB	The extra low significgant bits register value
		@return		Thee 32 bit value that is the full temperature or pressure reading
		@ingroup	bme280driver
	*/
	#define BuildTempPressADCValue(nMSB, nLSB, nXLSB) (((nXLSB & 0xF0) >> 4) | (nLSB << 4) | (nMSB << 12))

	/**	@brief		Converts the 2 humidity registers into a 32bit reading
		@param		nMSB	The most significgant bits register value
		@param		nLSB	The least significgant bits register value
		@return		Thee 32 bit value that is the full humidity reading
		@ingroup	bme280driver
	*/
	#define BuildHumidityADCValue(nMSB, nLSB) ((uint32_t)nLSB | ((uint32_t)nMSB << 8))

/***** Functions	*****/
cBME280_t::cBME280_t(bool bSDOHigh, TwoWire *pI2CWire) : cI2CDevice_t((bSDOHigh == true) ? (BME280_BASEADDR | BME280_SDOADDR) : BME280_BASEADDR,  pI2CWire) {
	//All initialization takes place in base class
	return;
}

cBME280_t::~cBME280_t() {
	//No resources allocated, cleanup unnecessary
	return;
}

bool cBME280_t::CheckDevice() {
	uint8_t nIDReg;
	
	nIDReg = I2CReadUint8Reg(cnI2CAddr, BME280_REG_ID);
	
	if (nIDReg != BME280_BME280) { //Not a BME280 device
		return false;
	} else { //Device identifies as BME280
		return true;
	}
}

bool cBME280_t::SetConfig() {
	bool bResult;
	uint8_t nRegVal;
	
	//Enter sleep mode to set config
	nRegVal = (BME280_CTMS_OVRSMP_x1 << BME280_CTMS_OVRSMP_PSHIFT) | (BME280_CTMS_OVRSMP_x1 << BME280_CTMS_OVRSMP_TSHIFT) | BME280_CTMS_MODE_SLEEP;
	bResult = I2CWriteUint8Reg(cnI2CAddr, BME280_REG_CTRL_MEAS, nRegVal);
	if (bResult == false) {
		DebugPrint("Failed to set CTRL MEAS");
		return false;
	}
	
	bResult = I2CWriteUint8Reg(cnI2CAddr, BME280_REG_CTRL_HUM, BME280_CTMS_OVRSMP_x1);
	if (bResult == false) {
		DebugPrint("Failed to set CTRL HUM");
		return false;
	}
	
	nRegVal = BME280_CFG_SB_1000 | BME280_CFG_FILT_OFF | BME280_CFG_SPI_DIS;
	bResult = I2CWriteUint8Reg(cnI2CAddr, BME280_REG_CONFIG, nRegVal);
	if (bResult == false) {
		DebugPrint("Failed to set CONFIG");
		return false;
	}
	
	return true;
}

bool cBME280_t::ReadCalibrationValues() {
	bool bNeg;
	uint8_t nRegVal, nCtr;
	uint8_t anReadBytes[2];
	
	//Read all temperature calibration values, Values are read in little endian, make sure they are positioned correctly
	for (nCtr = 0; nCtr < sizeof(cnTempCal) / sizeof(int16_t); nCtr++) {
		nRegVal = BME280_DIG_T1_LSB + (nCtr * 2);
		I2CWriteData(cnI2CAddr, 1, &nRegVal);
		I2CReadData(cnI2CAddr, 2, anReadBytes);
		cnTempCal[nCtr] = (int16_t)(anReadBytes[0] | (anReadBytes[1] << 8));
	}

	//Read all pressure calibration values, Values are read in little endian, make sure they are positioned correctly
	for (nCtr = 0; nCtr < sizeof(cnPressureCal) / sizeof(int16_t); nCtr++) {
		nRegVal = BME280_DIG_P1_LSB + (nCtr * 2);
		I2CWriteData(cnI2CAddr, 1, &nRegVal);
		I2CReadData(cnI2CAddr, 2, anReadBytes);
		cnPressureCal[nCtr] = (int16_t)((uint16_t)anReadBytes[0] | ((uint16_t)anReadBytes[1] << 8));
	}

	//Read all humidity calibration values
	cnHumidityCal[0] = I2CReadUint8Reg(cnI2CAddr, BME280_DIG_H1);
	cnHumidityCal[2] = I2CReadUint8Reg(cnI2CAddr, BME280_DIG_H3);
	
	nRegVal = I2CReadUint8Reg(cnI2CAddr, BME280_DIG_H2_MSB);
	cnHumidityCal[1] = (int32_t)(nRegVal << 8);
	
	nRegVal = I2CReadUint8Reg(cnI2CAddr, BME280_DIG_H2_LSB);
	cnHumidityCal[1] = (int32_t)(((uint32_t)cnHumidityCal[1]) | nRegVal);
	
	nRegVal = I2CReadUint8Reg(cnI2CAddr, BME280_DIG_H4_MSB);
	cnHumidityCal[3] = (int32_t)(nRegVal << BME280_DIG_H4_LSB_SHIFT);
	
	nRegVal = I2CReadUint8Reg(cnI2CAddr, BME280_DIG_H4_LSB);
	nRegVal &= BME280_DIG_H4_LSB_MASK;
	cnHumidityCal[3] = (int32_t)(((uint32_t)cnHumidityCal[3]) | nRegVal);
	
	if ((cnHumidityCal[3] & 0x0800) == 0x0800) { //The value needs to be negative
		cnHumidityCal[3] |= 0xF000;
	}
	
	nRegVal = I2CReadUint8Reg(cnI2CAddr, BME280_DIG_H5_MSB);
	cnHumidityCal[4] = (int32_t)(nRegVal << BME280_DIG_H5_LSB_SHIFT);
	
	nRegVal = I2CReadUint8Reg(cnI2CAddr, BME280_DIG_H5_LSB);
	nRegVal &= BME280_DIG_H5_LSB_MASK;
	cnHumidityCal[4] = (int32_t)(((uint32_t)cnHumidityCal[4]) | nRegVal);
	
	if ((cnHumidityCal[4] & 0x0800) == 0x0800) { //The value needs to be negative
		cnHumidityCal[4] |= 0xF000;
	}
	
	return true;
}

bool cBME280_t::ReadMeasurements(float *pnTemp, float *pnPress, float *pnHumid) {
	bool bResult;
	uint8_t nMSBVal, nLSBVal, nXLSBVal;
	uint32_t nCtr;
	int32_t nTempFine, nIntTemp, nIntHum, nI32Var1, nI32Var2;
	int64_t nI64Var1, nI64Var2, nIntPress;

	//Set some base values in case the read fails
	*pnTemp = 0;
	*pnPress = 0;
	*pnHumid = 0;
	
	//Go to forced mode to trigger a sample conversion
	nMSBVal = (BME280_CTMS_OVRSMP_x1 << BME280_CTMS_OVRSMP_PSHIFT) | (BME280_CTMS_OVRSMP_x1 << BME280_CTMS_OVRSMP_TSHIFT) | BME280_CTMS_MODE_FORCED;
	bResult = I2CWriteUint8Reg(cnI2CAddr, BME280_REG_CTRL_MEAS, nMSBVal);
	if (bResult == false) {
		DebugPrint("Failed to set CTRL MEAS");
		return false;
	}
	
	//Wait for the device to finish any conversion
	nMSBVal = I2CReadUint8Reg(cnI2CAddr, BME280_REG_STATUS);
	nMSBVal &= BME280_STATUS_MASK;
	for (nCtr = 0; (nCtr < 2500) && (nMSBVal != 0); nCtr++) {
		delayMicroseconds(100);
		
		nMSBVal = I2CReadUint8Reg(cnI2CAddr, BME280_REG_STATUS);
		nMSBVal &= BME280_STATUS_MASK;
	}
	
	if (nMSBVal != 0) { //The device never became ready
		return false;
	}
	
	//Read the results of the last conversion
	nMSBVal = I2CReadUint8Reg(cnI2CAddr, BME280_REG_PRESS_MSB);
	nLSBVal = I2CReadUint8Reg(cnI2CAddr, BME280_REG_PRESS_LSB);
	nXLSBVal = I2CReadUint8Reg(cnI2CAddr, BME280_REG_PRESS_XLSB);
	nIntPress = BuildTempPressADCValue(nMSBVal, nLSBVal, nXLSBVal);

	nMSBVal = I2CReadUint8Reg(cnI2CAddr, BME280_REG_TEMP_MSB);
	nLSBVal = I2CReadUint8Reg(cnI2CAddr, BME280_REG_TEMP_LSB);
	nXLSBVal = I2CReadUint8Reg(cnI2CAddr, BME280_REG_TEMP_XLSB);
	nIntTemp = BuildTempPressADCValue(nMSBVal, nLSBVal, nXLSBVal);
	
	nMSBVal = I2CReadUint8Reg(cnI2CAddr, BME280_REG_HUM_MSB);
	nLSBVal = I2CReadUint8Reg(cnI2CAddr, BME280_REG_HUM_LSB);
	nIntHum = BuildHumidityADCValue(nMSBVal, nLSBVal);
	
	//Apply temperature calibration values
	nI32Var1 = (((nIntTemp >> 3) - (((uint32_t)cnTempCal[0]) << 1)) * cnTempCal[1]) >> 11;
	nI32Var2 = (((((nIntTemp >> 4) - ((uint32_t)cnTempCal[0])) * ((nIntTemp >> 4) - ((uint32_t)cnTempCal[0]))) >> 12) * cnTempCal[2]) >> 14;
	
	nTempFine = nI32Var1 + nI32Var2;
	*pnTemp = ((float)(((nTempFine * 5) + 128) >> 8)) / 100.0f;
	
	//Apply pressure calibration values
	nI64Var1 = ((int64_t)nTempFine) - 128000;
	nI64Var2 = nI64Var1 * nI64Var1 * (int64_t)cnPressureCal[5];
	nI64Var2 = nI64Var2 + ((nI64Var1 * (int64_t)cnPressureCal[4]) << 17);
	nI64Var2 = nI64Var2 + (((int64_t)cnPressureCal[3]) << 35);
	nI64Var1 = ((nI64Var1 * nI64Var1 * (int64_t)cnPressureCal[2]) >> 8) + ((nI64Var1 * (int64_t)cnPressureCal[1]) << 12);
	nI64Var1 = (((((int64_t)1) << 47) + nI64Var1) * (int64_t)(((int32_t)cnPressureCal[0]) & 0x0000FFFF)) >> 33;

	if (nI64Var1 == 0) {
		nIntPress = 0; //Avoid division by zero
	} else {

		nIntPress = 1048576 - nIntPress;

		nIntPress = (((nIntPress << 31) - nI64Var2) * 3125) / nI64Var1;

		nI64Var1 = (((int64_t)cnPressureCal[8]) * (nIntPress >> 13) * (nIntPress >> 13)) >> 25;
		nI64Var2 = (((int64_t)cnPressureCal[7]) * nIntPress) >> 19;

		nIntPress = ((nIntPress + nI64Var1 + nI64Var2) >> 8) + (((int64_t)cnPressureCal[6]) << 4);
	}
	
	*pnPress = (float)nIntPress / 256.0f;
	
	//Apply humidity calibration factors
	nI32Var1 = nTempFine - 76800;
	nI32Var1 = (((((nIntHum << 14) - (((int32_t)cnHumidityCal[3]) << 20) - (((int32_t)cnHumidityCal[4]) * nI32Var1)) + ((int32_t)16384)) >> 15) * (((((((nI32Var1 * ((int32_t)cnHumidityCal[5])) >> 10) * (((nI32Var1 * ((int32_t)cnHumidityCal[2])) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) * ((int32_t)cnHumidityCal[1]) + 8192) >> 14));

	nI32Var1 = (nI32Var1 - (((((nI32Var1 >> 15) * (nI32Var1 >> 15)) >> 7) * ((int32_t)cnHumidityCal[0])) >> 4));

	nI32Var1 = (nI32Var1 < 0) ? 0 : nI32Var1;
	nI32Var1 = (nI32Var1 > 419430400) ? 419430400 : nI32Var1;
	*pnHumid = (nI32Var1 >> 12);
	*pnHumid /= 1024.0f;
	
	return true;
}

#endif

