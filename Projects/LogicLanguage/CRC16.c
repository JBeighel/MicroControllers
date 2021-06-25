/*	File:	CRC16.h
	Author:	J. Beighel
	Date:	2021-06-24
*/
#include "CRC16.h"

/***** Protected Functions 	*****/

/** Macro to retrieve the initial CRC value from a CRC algorithm type
	@param Type A CRCTypes value that specifies the CRC algorithm type
	@ingroup	crc16
 */
#define GetInitialCRC16(Type) ((uint16_t)((Type & 0xFFFF0000) >> 16))

/** Macro to retrieve the CRC polynomial value from a CRC algorithm type
	@param Type A CRCTypes value that specifies the CRC algorithm type
	@ingroup	crc16
 */
#define GetPolynomialCRC16(Type) ((uint16_t)(Type & 0x0000FFFF))

/** Calculates a CRC table entry for a specific CRC algorithm type
	@param Index the table entry number to calculate
	@param Type The CRC algorithm type the table is for
	@return The calculated CRC table entry
	@ingroup	crc16
*/
uint16_t GetCRC16TableEntry(uint16_t Index, eCRC16Types_t Type);

/***** Protected Variables 	*****/

/***** Function Code 		*****/
uint16_t GetCRC16TableEntry(uint16_t Index, eCRC16Types_t Type) {
	uint16_t TableEntry, BitCtr, Polynomial;

	Polynomial = GetPolynomialCRC16(Type);
	TableEntry = 0;

	for (BitCtr = 0; BitCtr < 8; BitCtr++) {
		if (((TableEntry ^ Index) & 0x0001) == 1) {
			TableEntry = (uint16_t)((TableEntry >> 1) ^ Polynomial);
		} else {
			TableEntry >>= 1;
		}

		Index >>= 1;
	}

	return TableEntry;
}

crc16_t ContinueCalculateCRC16Byte(eCRC16Types_t Type, uint8_t Byte, crc16_t PreviousCRC) {
	uint16_t TableEntry, TableIndex;

	TableIndex = (PreviousCRC ^ Byte) & 0x00FF;

	#ifdef CRC16_USETABLE
		switch (Type) {
			case CRC_Modbus	:
				TableEntry = ModbusCrc16Table[TableIndex];
				break;
			case CRC_DNP :
				TableEntry = DnpCrc16Table[TableIndex];
				break;
			default: //No prebuilt table, have to generate it
				TableEntry = GetCRC16TableEntry(TableIndex, Type);
				break;
		}
	#else
		TableEntry = GetCRC16TableEntry(TableIndex, Type);
	#endif

	return (PreviousCRC >> 8) ^ TableEntry;
}

crc16_t ContinueCalculateCRC16Buffer(eCRC16Types_t Type, void *DataBuffer, uint32_t DataLength, crc16_t PreviousCRC) {
	uint32_t Ctr;
	uint8_t Byte;

	if ((PreviousCRC != 0) && (Type == CRC_DNP)) { //Revert the DNP CRC to the format used by the calculations
		PreviousCRC = FlipBytesUInt16(PreviousCRC);
		PreviousCRC = ~PreviousCRC;
	}

	for (Ctr = 0; Ctr < DataLength; Ctr++) {
		Byte = ((uint8_t *)DataBuffer)[Ctr];
		PreviousCRC = ContinueCalculateCRC16Byte(Type, Byte, PreviousCRC);
	}

	if (Type == CRC_DNP) {
		PreviousCRC = ~PreviousCRC;
		PreviousCRC = FlipBytesUInt16(PreviousCRC);
	}

	return PreviousCRC;
}

crc16_t CalculateCRC16(eCRC16Types_t Type, void *DataBuffer, uint32_t DataLength) {
	uint16_t CrcVal;

	CrcVal = GetInitialCRC16(Type);

	return ContinueCalculateCRC16Buffer(Type, DataBuffer, DataLength, CrcVal);
}
