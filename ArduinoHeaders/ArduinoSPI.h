/**	@defgroup	arduinospi
	@brief		Wrapping the default Arduino SPI cuntions with the SPI interface handler
	@details	v0.1
		
*/

#ifndef __ARDUINOSPI_H
	#define __ARDUINOSPI_H

/***** Includes    *****/
	#include <SPI.h>

	#include "IfaceSpi.h"

/***** Constants   *****/


/***** Definitions *****/
	class ArduinoSPI : public iSPIHandler {
	public:
		bool Initialize(uint32_t nBusClockFreq, eSPIDataOrder_t eDataOrder, u8 nCSPin);
		bool SendByte(uint8_t nByte);
		bool ByteArrived(uint8_t nByte);
		bool BeginTransfer();
		bool EndTransfer();

		#ifndef ARDUINOSPI_USEINTERRUPT
			bool ReadByte(uint8_t *pnByte);
		#endif

	protected:
		SPISettings cSPISettings;

	private:

	};

/***** Globals     *****/
	#ifdef ARDUINOSPI_USEINTERRUPT
		ArduinoSPI *gpSPIBus;
	#endif

/***** Prototypes  *****/


/***** Functions   *****/
	#ifdef ARDUINOSPI_USEINTERRUPT
		/**	@brief		Interrupt handler for the SPI bus on Arduino, called on data arrival
			@details	Accepts the byte that arrived, then basses it along to the SPI bus 
				class.
			@ingroup	arduinospi
		*/
		ISR (SPI_STC_vect) {
			uint8_t Recv = SPDR; //Can also assign SPDR a value to send a byte from this interrupt

			gSPIBus->ByteArrived(Recv);

			return;
		}
	#endif

	bool ArduinoSPI::Initialize(uint32_t nBusClockFreq, eSPIDataOrder_t eDataOrder, u8 nCSPin) {
		iSPIHandler::Initialize(nBusClockFreq, eDataOrder, nCSPin);

		SPI.begin();

		if (eDataOrder == SPI_MSBFIRST) { //Max speed of device, most significant bit first, Mode 0: Output falling edge data capture rising edge 
			cSPISettings = SPISettings(cnBusFreq, MSBFIRST, SPI_MODE0);
		} else {
			cSPISettings = SPISettings(cnBusFreq, LSBFIRST, SPI_MODE0);
		}

		#ifdef ARDUINOSPI_USEINTERRUPT
			gpSPIBus = &this;
		#endif
	}

	bool ArduinoSPI::ByteArrived(uint8_t nByte) {
		WriteToBuffer(nByte);
	}

	bool ArduinoSPI::SendByte(uint8_t nByte) {
		uint8_t nByteRead;

		nByteRead = SPI.transfer(nByte);

		WriteToBuffer(nByteRead);

		return true;
	}

	bool ArduinoSPI::BeginTransfer() {
		digitalWrite(cnChipSelectPin, LOW); //Select the chip
		SPI.beginTransaction(cSPISettings);

		return true;
	}

	bool ArduinoSPI::EndTransfer() {
		SPI.endTransaction();
		digitalWrite(cnChipSelectPin, HIGH); //Deselect the chip

		return true;
	}

	#ifndef ARDUINOSPI_USEINTERRUPT
		bool ArduinoSPI::ReadByte(uint8_t *pnByte) {
			if (BytesInBuffer() > 0) {
				return ReadFromBuffer(pnByte);
			} else {
				(*pnByte) = SPI.transfer(0xFF); //Sends a dummy byte to trigger the read
				return true;
			}
		}
	#endif

#endif

