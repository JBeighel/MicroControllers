/**	@defgroup	ifacespi
	@brief		Abstract class providing an interface to the SPI hardware
	@details	v0.1
		Intended as a generic wraper around the SPI bus interface of a microcontroller.
		It should mask the low level interface needed to control the SPI bus from the
		application end of reading and writing bytes to that bus.

		It has a small internal circular buffer to hold bytes received from the SPI bus.
		Incoming data can be stored there temporarily until it is needed.  Data to be 
		sent will be written to the port immediately with no buffering.

		The virtual functions are intended to be the actual mechanisms for interacting 
		with the SPI bus.  SendByte() should write the requested byte to the SPI bus.
		ReadByte() by default will return one of the bytes in the buffer, if this is 
		not overridden then it is expected that WriteToBuffer() will be used to 
		populate the buffer.  To override ReadByte() it will need to hold whatever 
		commands are needed to retrieve data from the SPI bus.

		It is expected that a function will need to be added to read data from the SPI
		bus and addit to the buffer via WriteToBuffer() when this class is inheritted.  
		This function may need to be part of an interrupt or called periodically to
		check for data from the bus.
*/

#ifndef __IFACESPI_H
	#define __IFACESPI_H

/***** Includes    *****/


/***** Constants   *****/
	#ifndef SPI_BUFFSIZE
		#define SPI_BUFFSIZE 16
	#endif

/***** Definitions *****/
	typedef enum eSPIDataOrder_t {
		SPI_MSBFIRST,
		SPI_LSBFIRST,
	} eSPIDataOrder_t;


	class iSPIHandler {
	public:
		iSPIHandler();
		~iSPIHandler();

		bool Initialize(uint32_t nBusClockFreq, eSPIDataOrder_t eDataOrder, uint8_t nCSPin);
		virtual bool BeginTransfer();
		virtual bool EndTransfer();
		virtual bool SendByte(uint8_t nByte);
		virtual bool ReadByte(uint8_t *pnByte);

	protected:
		uint16_t cnBuffReadIdx;
		uint16_t cnBuffWriteIdx;
		uint8_t caSpiBuffer[SPI_BUFFSIZE];
		eSPIDataOrder_t ceDataOrder;
		uint32_t cnBusFreq;
		uint8_t cnChipSelectPin;

		uint16_t BytesInBuffer();
		bool WriteToBuffer(uint8_t nByte);
		bool ReadFromBuffer(uint8_t *pnByte);

	private:

	};

/***** Globals     *****/


/***** Prototypes  *****/


/***** Functions   *****/
	iSPIHandler::iSPIHandler() {
		cnBuffReadIdx = 0;
		cnBuffWriteIdx = 0;

		memset(caSpiBuffer, 0, sizeof(uint8_t) * SPI_BUFFSIZE);

		return;
	}

	iSPIHandler::~iSPIHandler() {
		return;
	}

	bool iSPIHandler::WriteToBuffer(uint8_t nByte) {
		caSpiBuffer[cnBuffWriteIdx] = nByte;

		cnBuffWriteIdx += 1;
		if (cnBuffWriteIdx >= SPI_BUFFSIZE) { //At end of buffer, start over
			cnBuffWriteIdx = 0;
		}

		if (cnBuffReadIdx == cnBuffWriteIdx) {
			//The writes just caught the reads, buffer over flow so lose a byte
			cnBuffReadIdx += 1;
			return false;
		}

		return true;
	}

	bool iSPIHandler::ReadFromBuffer(uint8_t *pnByte) {
		if (cnBuffReadIdx == cnBuffWriteIdx) {
			//Buffer underflow, we don't have any data to read
			(*pnByte) = 0;
			return false;
		}

		(*pnByte) = caSpiBuffer[cnBuffReadIdx];

		cnBuffReadIdx += 1;

		if (cnBuffReadIdx >= SPI_BUFFSIZE) { //At end of buffer, start over
			cnBuffReadIdx = 0;
		}
	}

	bool iSPIHandler::ReadByte(uint8_t *pnByte) {
		return ReadFromBuffer(pnByte);
	}

	bool iSPIHandler::Initialize(uint32_t nBusClockFreq, eSPIDataOrder_t eDataOrder, uint8_t nCSPin) {
		cnBusFreq = nBusClockFreq;
		ceDataOrder = eDataOrder;

		cnChipSelectPin = nCSPin;
		pinMode(cnChipSelectPin, OUTPUT);
		digitalWrite(cnChipSelectPin, HIGH); //Deselect the chip

		return;
	}

	uint16_t iSPIHandler::BytesInBuffer() {
		uint16_t nNumBytes;

		if (cnBuffReadIdx == cnBuffWriteIdx) {
			nNumBytes = 0;
		} else if (cnBuffReadIdx < cnBuffWriteIdx) {
			nNumBytes = cnBuffWriteIdx - cnBuffReadIdx;
		} else { //Read is > Write
			nNumBytes = SPI_BUFFSIZE - cnBuffReadIdx; //bytes till end of buffer
			nNumBytes += cnBuffWriteIdx; //bytes at beginning of buffer
		}

		return nNumBytes;
	}

#endif

