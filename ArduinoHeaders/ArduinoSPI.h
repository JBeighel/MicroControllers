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
	class cArduinoSPI_t : public iSPIHandler_t {
	public:
		cArduinoSPI_t();
		~cArduinoSPI_t();
		
		bool Initialize(uint32_t nBusClockFreq, eSPIDataOrder_t eDataOrder);
		bool SendByte(uint8_t nByte);
		bool ByteArrived(uint8_t nByte);
		bool BeginTransfer(uint8_t nCSPin);
		bool EndTransfer();

		#ifndef ARDUINOSPI_USEINTERRUPT //The interrupt will do the reads, if its dissabled add another method
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

	cArduinoSPI_t::cArduinoSPI_t() {

		return;
	}
	
	cArduinoSPI_t::~cArduinoSPI_t() {

		return;
	}

	bool cArduinoSPI_t::Initialize(uint32_t nBusClockFreq, eSPIDataOrder_t eDataOrder) {
		iSPIHandler_t::Initialize(nBusClockFreq, eDataOrder);

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

	bool cArduinoSPI_t::ByteArrived(uint8_t nByte) {
		WriteToBuffer(nByte);
	}

	bool cArduinoSPI_t::SendByte(uint8_t nByte) {
		uint8_t nByteRead;

		nByteRead = SPI.transfer(nByte);

		WriteToBuffer(nByteRead);

		return true;
	}

	bool cArduinoSPI_t::BeginTransfer(uint8_t nCSPin) {
		cnChipSelectPin = nCSPin;
		
		if (cnChipSelectPin != SPI_NOPIN) {
			digitalWrite(cnChipSelectPin, LOW); //Select the chip
		}
		SPI.beginTransaction(cSPISettings);

		return true;
	}

	bool cArduinoSPI_t::EndTransfer() {
		SPI.endTransaction();
		
		if (cnChipSelectPin != SPI_NOPIN) {
			digitalWrite(cnChipSelectPin, HIGH); //Deselect the chip
		}
		
		return true;
	}

	#ifndef ARDUINOSPI_USEINTERRUPT
		bool cArduinoSPI_t::ReadByte(uint8_t *pnByte) {
			if (BytesInBuffer() > 0) {
				return ReadFromBuffer(pnByte);
			} else {
				(*pnByte) = SPI.transfer(0xFF); //Sends a dummy byte to trigger the read
				return true;
			}
		}
	#endif

#endif

