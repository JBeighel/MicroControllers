/**	@defgroup	apa102
	@brief		Driver for the APA102 addressable LED
	@details	v0.1
	These addressable lights get their color values shifted in over the SPI bus.  The command 
	starts with 4 start frame 0x00 bytes.  Then starting with the first light send out 4 bytes
	to give them color information.  The first byte is 0xFF, which can be 0x111xxxxx where the 
	x's represent a global brightness.  The next byte is for Blue, a byte for Green, and a 
	byte for red.  The BGR order may not be a match for all strings of lights, so some testing
	and config settings to get this correct may be needed.  After 4 bytes for each light in the 
	string, the end frame is a 1 bit for every 2 lights.  For example a string of 64 lights
	needs 32 1 bits (64 / 2 = 32).  Another option is (Number of Lights + 15) / 16 to calculate
	the number of 0xFF bytes to send, round up to nearest 16 then divide to get the whole numer
		
*/

#ifndef __APA102DRIVER_H
	#define __APA102DRIVER_H

/***** Includes		*****/
	#include <SPI.h>

/***** Constants	*****/
	#define ADA102_CLKFREQ		8000000

/***** Definitions	*****/
	/**	@brief		Enumeration of all color orders the lights may expect the commands in
		@details	Using two bits per color.  Bits 4 & 5 encode what color is sent in the first
		byte.  Bits 2 & 3 encode the second color byte.  Bits 0 & 1 encode the third color byte.
		The value zero is for red, 1 for green, and 2 for blue.
	*/
	typedef enum eAPA102_LightOrder_t {
		APA102_ColRed	= 0x00,
		APA102_ColGreen	= 0x01,
		APA102_ColBlue	= 0x02,

		APA102_1ColShift= 0,
		APA102_2ColShift= 2,
		APA102_3ColShift= 4,
		APA102_ColMask	= 0x03,
		
		APA102_RGB		= (APA102_ColRed << APA102_3ColShift) | (APA102_ColGreen << APA102_2ColShift) | (APA102_ColBlue << APA102_1ColShift),
		APA102_GBR		= (APA102_ColGreen << APA102_3ColShift) | (APA102_ColBlue << APA102_2ColShift) | (APA102_ColRed << APA102_1ColShift),
		APA102_BRG		= (APA102_ColBlue << APA102_3ColShift) | (APA102_ColRed << APA102_2ColShift) | (APA102_ColGreen << APA102_1ColShift),
		APA102_RBG		= (APA102_ColRed << APA102_3ColShift) | (APA102_ColBlue << APA102_2ColShift) | (APA102_ColGreen << APA102_1ColShift),
		APA102_GRB		= (APA102_ColGreen << APA102_3ColShift) | (APA102_ColRed << APA102_2ColShift) | (APA102_ColBlue << APA102_1ColShift),
		APA102_BGR		= (APA102_ColBlue << APA102_3ColShift) | (APA102_ColGreen << APA102_2ColShift) | (APA102_ColRed << APA102_1ColShift),
		APA102_MONO		= APA102_RGB,
	};

	typedef enum eLightColors_t {
		Color_DimRed		= 0x070000,
		Color_MaxRed		= 0xFF0000,
		Color_DimOrange		= 0x070300,
		Color_DimYellow		= 0x070700,
		Color_DimGreen		= 0x000700,
		Color_MaxGreen		= 0x00FF00,
		Color_DimBlue		= 0x000007,
		Color_MaxBlue		= 0x0000FF,
		Color_DimPurple		= 0x070007,
		Color_DimWhite		= 0x070707,
		Color_MaxWhite		= 0xFFFFFF,
		Color_Off			= 0x000000,

		Color_RedShift		= 16,
		Color_GreenShift	= 8,
		Color_BlueShift		= 0,
		Color_SingleMask	= 0x0000FF,
	} eLightColors_t;

	typedef struct sLEDInfo_t {
		eAPA102_LightOrder_t	eColorOrder;
		uint8_t					aColors[3];
	} sLEDInfo_t;

	typedef class cADA102_t {
	public:
		cADA102_t(SPIClass *pSPIObj, uint16_t nNumLights, eAPA102_LightOrder_t eDefColorOrder);
		~cADA102_t();
		bool Initialize();
		bool SetLightColorOrder(uint16_t nLightIdx, eAPA102_LightOrder_t eOrder);
		bool SetLightColor(uint16_t nLightIdx, eLightColors_t eColor);
		bool Clear();
		bool UpdateDisplay();

	protected:

	private:
		SPIClass *cpSPIBus;
		sLEDInfo_t *cpLightList;
		uint16_t cnLightNum;
		eAPA102_LightOrder_t ceColorOrder;
	};

/***** Globals		*****/


/***** Prototypes 	*****/
	#define CustomLightColor(nRed, nGreen, nBlue)	((eLightColors_t)((nRed << Color_RedShift) | (nGreen << Color_GreenShift) | (nBlue << Color_BlueShift)))

	#define GetColorRedByte(eColor)					((eColor >> Color_RedShift) & Color_SingleMask)
	#define GetColorBlueByte(eColor)				((eColor >> Color_BlueShift) & Color_SingleMask)
	#define GetColorGreenByte(eColor)				((eColor >> Color_GreenShift) & Color_SingleMask)

	#define GetOrder1stIdx(eOrder)					(((uint16_t)eOrder >> (uint16_t)APA102_1ColShift) & (uint16_t)APA102_ColMask)
	#define GetOrder2ndIdx(eOrder)					(((uint16_t)eOrder >> (uint16_t)APA102_2ColShift) & (uint16_t)APA102_ColMask)
	#define GetOrder3rdIdx(eOrder)					(((uint16_t)eOrder >> (uint16_t)APA102_3ColShift) & (uint16_t)APA102_ColMask)

/***** Functions	*****/
cADA102_t::cADA102_t(SPIClass *pSPIObj, uint16_t nNumLights, eAPA102_LightOrder_t eDefColorOrder) {
	uint16_t nCtr;

	cpSPIBus = pSPIObj;
	cnLightNum = nNumLights;
	ceColorOrder = eDefColorOrder;

	cpLightList = new sLEDInfo_t[cnLightNum];

	for (nCtr = 0; nCtr < cnLightNum; nCtr++) {
		cpLightList[nCtr].eColorOrder = ceColorOrder;
		cpLightList[nCtr].aColors[0] = 0x00;
		cpLightList[nCtr].aColors[1] = 0x00;
		cpLightList[nCtr].aColors[2] = 0x00;
	}

	return;
}

cADA102_t::~cADA102_t() {
	delete cpLightList;

	return;
}

bool cADA102_t::Initialize() {
	

	return;
}

bool cADA102_t::SetLightColorOrder(uint16_t nLightIdx, eAPA102_LightOrder_t eOrder) {
	if (nLightIdx >= cnLightNum) {
		return false;
	}

	cpLightList[nLightIdx].eColorOrder = eOrder;

	return true;
}

bool cADA102_t::SetLightColor(uint16_t nLightIdx, eLightColors_t eColor) {
	eAPA102_LightOrder_t eColorIdx;
	uint8_t aColorBytes[3];

	if (nLightIdx >= cnLightNum) {
		return false;
	}

	aColorBytes[0] = GetColorRedByte(eColor);
	aColorBytes[1] = GetColorGreenByte(eColor);
	aColorBytes[2] = GetColorBlueByte(eColor);

	eColorIdx = GetOrder1stIdx(cpLightList[nLightIdx].eColorOrder);
	cpLightList[nLightIdx].aColors[0] = aColorBytes[eColorIdx];

	eColorIdx = GetOrder2ndIdx(cpLightList[nLightIdx].eColorOrder);
	cpLightList[nLightIdx].aColors[1] = aColorBytes[eColorIdx];

	eColorIdx = GetOrder3rdIdx(cpLightList[nLightIdx].eColorOrder);
	cpLightList[nLightIdx].aColors[2] = aColorBytes[eColorIdx];

	return true;
}

bool cADA102_t::Clear() {
	uint16_t nCtr;

	for (nCtr = 0; nCtr < cnLightNum; nCtr++) {
		cpLightList[nCtr].aColors[0] = 0;
		cpLightList[nCtr].aColors[1] = 0;
		cpLightList[nCtr].aColors[2] = 0;
	}

	return true;
}

bool cADA102_t::UpdateDisplay() {
	uint16_t nCtr;

	cpSPIBus->beginTransaction(SPISettings(ADA102_CLKFREQ, MSBFIRST, SPI_MODE0));

	//Start frame
	for (nCtr = 0; nCtr < 4; nCtr++) {
		cpSPIBus->transfer(0x00);
	}
	
	//Color data
	for (nCtr = 0; nCtr < cnLightNum; nCtr++) {
		cpSPIBus->transfer(0xFF); //Global brightness value, fully bright
		cpSPIBus->transfer(cpLightList[nCtr].aColors[0]);
		cpSPIBus->transfer(cpLightList[nCtr].aColors[1]);
		cpSPIBus->transfer(cpLightList[nCtr].aColors[2]);
	}

	//End Frame
	for (nCtr = 0; nCtr < (cnLightNum + 15) / 16; nCtr++) {
		cpSPIBus->transfer(0xFF);
		cpSPIBus->transfer(0xFF);
		cpSPIBus->transfer(0xFF);
		cpSPIBus->transfer(0xFF);
	}

	cpSPIBus->endTransaction();
	Serial.println(" ");
}

#endif
