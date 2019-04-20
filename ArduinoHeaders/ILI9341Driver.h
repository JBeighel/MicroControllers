/**	@defgroup	tftdriverILI9341
	@brief		
	@details	
		
*/

#ifndef __ILI9341Driver_H
	#define __ILI9341Driver_H

/***** Includes    *****/
	#include "ArduinoSPI.h"

/***** Constants   *****/
	#define ILI9341_SPIFREQ		25000000

/***** Definitions *****/
	typedef enum eILI3941Commands_t {

	} eILI3941Commands_t;

	class ILI9341Driver : protected ArduinoSPI {
	public:
		bool Initialize(uint8_t nCSPin);

	protected:

	private:

	};

/***** Globals     *****/


/***** Prototypes  *****/


/***** Functions   *****/
	bool ILI9341Driver::Initialize(uint8_t nCSPin) {
		ArduinoSPI::Initialize(ILI9341_SPIFREQ, SPI_MSBFIRST, nCSPin);

		return true;
	}

#endif

