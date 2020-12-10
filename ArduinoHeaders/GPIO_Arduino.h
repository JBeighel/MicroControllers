/**	@defgroup	gpioarduino
	@brief		Implementation of the GPIO General Interface for Arduino
	@details	v0.4
		
*/

#ifndef __GPIOARDUINO
	#define __GPIOARDUINO

/***** Includes		*****/
	#include "CommonUtils.h"
	#include "GPIOGeneralInterface.h"


/***** Constants	*****/
	/**	@brief		Default GPIO hardware information object
		@ingroup	gpioarduino
	*/
	#define GPIO_HWINFO		NULL
	
	#define GPIO_INIT		ArduinoGPIOPortInitialize
	
	#define GPIO_CAPS		GPIOCap_SetPinMode | GPIOCap_ReadPinMode | GPIOCap_DigitalWrite | GPIOCap_DigitalRead | GPIOCap_PWMWrite | GPIOCap_AnalogWrite | GPIOCap_AnalogRead;
	
	#define TIME_INIT		ArduinoTimeIfaceInitialize
	
	#define TIME_CAPS		TimeCap_GetTicks | TimeCap_DelaySec | TimeCap_DelayMilliSec | TimeCap_DelayMicroSec;
	
	#define DELAYMILLISEC		delay
	
	#define DELAYMICROSEC		delayMicroseconds

	#undef ARDUINO_MODEL

	//Defines for Arduino boards are found in:
	//C:\Users\<Username>\AppData\Local\Arduino15\packages\<Vendor>\hardware\<Processor>\<Version>\boards.txt
	//Look for the setting name that ends with .board=
	//Prefix its value with ARDUINO_
	#ifdef ARDUINO_AVR_UNO	//Compiling for Arduino Uno
		#define ARDUINO_MODEL		"Arduino Uno"
		
		#error "Support for Arduino Uno incomplete"
	#endif
	
	#ifdef ARDUINO_SAM_DUE	//Compiling for Due
		#define ARDUINO_MODEL		"Arduino Due"
		
		#error "Support for Arduino Due incomplete"
	#endif
	
	#ifdef ARDUINO_SAMD_NANO_33_IOT	//Compiling for Nano IoT
		#define ARDUINO_MODEL		"Arduino Nano IoT"
	
		/**	@brief		Count of available GPIO pins
			@ingroup	gpioarduino
		*/
		#define ARDUINO_GPIOCNT		(sizeof(gArduinoGPIOList) / sizeof(uint8_t))
		
		#define ARDUINO_DACCNT		(sizeof(gArduinoDACList) / sizeof(uint8_t))
		
		#define ARDUINO_ADCCNT		(sizeof(gArduinoADCList) / sizeof(uint8_t))
		
		#define ARDUINO_PWMCNT		(sizeof(gArduinoPWMList) / sizeof(uint8_t))
		
		/**	@brief		Available bit depth for Analog Inputs
			@ingroup	gpioarduino
		*/
		#define ARDUINO_ADCBITDEPTH	12
		
		/**	@brief		Available bit depth for Analog Outputs
			@details	Arduino uses this same bit depth for PWM and DAC outputs.  So the values
				must match.
			@ingroup	gpioarduino
		*/
		#define ARDUINO_DACBITDEPTH	ARDUINO_PWMBITDEPTH
		
		/**	@brief		Available bit depth for PWM Outputs
			@ingroup	gpioarduino
		*/
		#define ARDUINO_PWMBITDEPTH	10
		
		/** @brief		Constant array of all pin numbers to use for GPIO
			@details	There are 22 potential GPIO pins, owever 7 are used by the peripheral buses
				Pin 0 and 1 are used for the UART.
				Pin 11 is MOSI, Pin 12 is MIS and Pin 13 are SCK; reserved for SPI
				Pin 18/A4 is SDA and Pin 19/A5 is SCL; reserved for I2C
			@ingroup	gpioarduino
		*/
		const uint8_t gArduinoGPIOList[] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 14, 15, 16, 17, 20, 21 };
		
		/** @brief		Constant array of all pin numbers that support Analog Input
			@ingroup	gpioarduino
		*/
		const uint8_t gArduinoADCList[] = { 14, 15, 16, 17, 18, 19, 20, 21 }; //14 is A0 through 21 is A7
		
		/** @brief		Constant array of all pin numbers that support Analog Ouput
			@ingroup	gpioarduino
		*/
		const uint8_t gArduinoDACList[] = { 14 }; //14 is A0
		
		/** @brief		Constant array of all pin numbers that support PWM Output
			@ingroup	gpioarduino
		*/
		const uint8_t gArduinoPWMList[] = { 2, 3, 5, 6, 9, 10, 11, 12, 16, 17, 19 }; //16 is A2, 17 is A3, and 19 is A5
	#endif
	
	#ifdef ARDUINO_SAMD_MKRZERO	//Compiling for Mkr Zero
		#define ARDUINO_MODEL		"Arduino Mkr Zero"
		
		#error "Support for Arduino Mkr Zero incomplete"
	#endif
	
	#ifdef ARDUINO_TRINKET_M0 //Compiling for Adafruit Trinket M0
		#define ARDUINO_MODEL		"Adafruit Trinket M0"
	
		/** @brief    Count of available GPIO pins
		  @ingroup  gpioarduino
		*/
		#define ARDUINO_GPIOCNT   (sizeof(gArduinoGPIOList) / sizeof(uint8_t))
		
		#define ARDUINO_DACCNT    (sizeof(gArduinoDACList) / sizeof(uint8_t))
		
		#define ARDUINO_ADCCNT    (sizeof(gArduinoADCList) / sizeof(uint8_t))
		
		#define ARDUINO_PWMCNT    (sizeof(gArduinoPWMList) / sizeof(uint8_t))
		
		/** @brief    Available bit depth for Analog Inputs
		  @ingroup  gpioarduino
		*/
		#define ARDUINO_ADCBITDEPTH 12
		
		/** @brief    Available bit depth for Analog Outputs
		  @details  Arduino uses this same bit depth for PWM and DAC outputs.  So the values
			must match.
		  @ingroup  gpioarduino
		*/
		#define ARDUINO_DACBITDEPTH ARDUINO_PWMBITDEPTH
		
		/** @brief    Available bit depth for PWM Outputs
		  @ingroup  gpioarduino
		*/
		#define ARDUINO_PWMBITDEPTH 10
		
		/** @brief    Constant array of all pin numbers to use for GPIO
		  @details  There are 5 potential GPIO pins, most are shared with the peripheral buses
			Pin 3 is RX and 4 is TX are used for the UART.
			Pin 0 is MOSI, Pin 1 is MISO and Pin 2 are SCK; reserved for SPI
			Pin 0 is SDA and Pin 2 is SCL; reserved for I2C
		  @ingroup  gpioarduino
		*/
		const uint8_t gArduinoGPIOList[] = { 0, 1, 2, 3, 4 };
		
		/** @brief    Constant array of all pin numbers that support Analog Input
		  @ingroup  gpioarduino
		*/
		const uint8_t gArduinoADCList[] = { 2, 3, 4 };
		
		/** @brief    Constant array of all pin numbers that support Analog Ouput
		  @ingroup  gpioarduino
		*/
		const uint8_t gArduinoDACList[] = {  };
		
		/** @brief    Constant array of all pin numbers that support PWM Output
		  @ingroup  gpioarduino
		*/
		const uint8_t gArduinoPWMList[] = { 0, 1, 2, 3, 4 };
	#endif
	
	#ifdef ARDUINO_NUCLEO_L412KB //Compiling for STM32 Nucleo-32 L412KB
		#define ARDUINO_MODEL		"STM32 Nucleo-32 L412KB"
		
		//#error "Support for STM32 Nucleo-32 L412KB incomplete"
		
		/**	@brief		Count of available GPIO pins
			@ingroup	gpioarduino
		*/
		#define ARDUINO_GPIOCNT		(sizeof(gArduinoGPIOList) / sizeof(uint8_t))
		
		#define ARDUINO_DACCNT		(sizeof(gArduinoDACList) / sizeof(uint8_t))
		
		#define ARDUINO_ADCCNT		(sizeof(gArduinoADCList) / sizeof(uint8_t))
		
		#define ARDUINO_PWMCNT		(sizeof(gArduinoPWMList) / sizeof(uint8_t))
		
		/**	@brief		Available bit depth for Analog Inputs
			@ingroup	gpioarduino
		*/
		#define ARDUINO_ADCBITDEPTH	12
		
		/**	@brief		Available bit depth for Analog Outputs
			@details	Arduino uses this same bit depth for PWM and DAC outputs.  So the values
				must match.
			@ingroup	gpioarduino
		*/
		#define ARDUINO_DACBITDEPTH	ARDUINO_PWMBITDEPTH
		
		/**	@brief		Available bit depth for PWM Outputs
			@ingroup	gpioarduino
		*/
		#define ARDUINO_PWMBITDEPTH	10
		
		/** @brief		Constant array of all pin numbers to use for GPIO
			@details	There are 22 potential GPIO pins, owever 7 are used by the peripheral buses
				Pin 0 and 1 are used for the UART.
				Pin 11 is MOSI, Pin 12 is MIS and Pin 13 are SCK; reserved for SPI
				Pin 18/A4 is SDA and Pin 19/A5 is SCL; reserved for I2C
			@ingroup	gpioarduino
		*/
		const uint8_t gArduinoGPIOList[] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 14, 15, 16, 17, 20, 21 };
		
		/** @brief		Constant array of all pin numbers that support Analog Input
			@ingroup	gpioarduino
		*/
		const uint8_t gArduinoADCList[] = { 14, 15, 16, 17, 18, 19, 20, 21 }; //14 is A0 through 21 is A7
		
		/** @brief		Constant array of all pin numbers that support Analog Ouput
			@ingroup	gpioarduino
		*/
		const uint8_t gArduinoDACList[] = { };
		
		/** @brief		Constant array of all pin numbers that support PWM Output
			@ingroup	gpioarduino
		*/
		const uint8_t gArduinoPWMList[] = { };
	#endif
	
	#ifndef ARDUINO_MODEL
		#error "No arduino model detected, unable to compile GPIO Interface for Arduino"
	#endif

/***** Definitions	*****/


/***** Constants	*****/


/***** Globals		*****/


/***** Prototypes 	*****/
	eGPIOReturn_t ArduinoGPIOPortInitialize(sGPIOIface_t *pIface, void *pHWInfo);

	eGPIOReturn_t ArduinoGPIOSetModeByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, eGPIOModes_t eMode);
	
	eGPIOReturn_t ArduinoGPIOReadModeByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, eGPIOModes_t *eMode);
	
	eGPIOReturn_t ArduinoGPIODigitalWriteByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, bool bState);
	
	eGPIOReturn_t ArduinoGPIODigitalReadByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, bool *bState);
	
	eGPIOReturn_t ArduinoGPIOPWMWriteByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, uint32_t nPWMValue);
	
	eGPIOReturn_t ArduinoGPIOAnalogWriteByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, uint32_t nAnaValue);
	
	eGPIOReturn_t ArduinoGPIOAnalogReadByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, uint32_t *nAnaValue);
	
	eReturn_t ArduinoTimeIfaceInitialize(sTimeIface_t *pTime);
	
	uint32_t ArduinoGetTicks(void);
	
	eReturn_t ArduinoDelaySeconds(uint32_t nDelayAmount);
	
	eReturn_t ArduinoDelayMilliSeconds(uint32_t nDelayAmount);
	
	eReturn_t ArduinoDelayMicroSeconds(uint32_t nDelayAmount);

/***** Functions	*****/

eGPIOReturn_t ArduinoGPIOPortInitialize(sGPIOIface_t *pIface, void *pHWInfo) {
	uint16_t nCtr;
	uint8_t nCurrPin;
	
	GPIOInterfaceInitialize(pIface); //Set a base sanity for the interface
	
	//Set pointers for all supported functions
	pIface->pfPortInit = &ArduinoGPIOPortInitialize;
	pIface->pfSetModeByPin = &ArduinoGPIOSetModeByPin;
	pIface->pfReadModeByPin = &ArduinoGPIOReadModeByPin;
	pIface->pfDigitalWriteByPin = &ArduinoGPIODigitalWriteByPin;
	pIface->pfDigitalReadByPin = &ArduinoGPIODigitalReadByPin;
	pIface->pfPWMWriteByPin = &ArduinoGPIOPWMWriteByPin;
	pIface->pfAnalogWriteByPin = &ArduinoGPIOAnalogWriteByPin;
	pIface->pfAnalogReadByPin = &ArduinoGPIOAnalogReadByPin;
	
	//Provide the capabilities of the available GPIO
	pIface->nPWMBitDepth = ARDUINO_PWMBITDEPTH;
	pIface->nAnaInBitDepth = ARDUINO_ADCBITDEPTH;
	pIface->nAnaOutBitDepth = ARDUINO_DACBITDEPTH;
	
	if (GPIO_IOCNT < ARDUINO_GPIOCNT) { //Can't provide more IO than the interface allows
		pIface->nGPIOCnt = GPIO_IOCNT;
	} else {
		pIface->nGPIOCnt = ARDUINO_GPIOCNT;
	}
	
	pIface->pHWInfo = pHWInfo;
	
	//Set the capabilities of all the IO points
	for (nCtr = 0; nCtr < pIface->nGPIOCnt; nCtr++) {
		pIface->aGPIO[nCtr].eCapabilities = (eGPIOModes_t)(GPIO_DigitalInput | GPIO_DigitalOutput | GPIO_InputPullup);
		
		if (NumberInArray_u8(gArduinoGPIOList[nCtr], gArduinoPWMList, ARDUINO_PWMCNT) == true) {
			pIface->aGPIO[nCtr].eCapabilities = (eGPIOModes_t)(pIface->aGPIO[nCtr].eCapabilities | GPIO_OutputPWM);
		}
		
		if (NumberInArray_u8(gArduinoGPIOList[nCtr], gArduinoADCList, ARDUINO_ADCCNT) == true) {
			pIface->aGPIO[nCtr].eCapabilities = (eGPIOModes_t)(pIface->aGPIO[nCtr].eCapabilities | GPIO_AnalogInput);
		}
		
		if (NumberInArray_u8(gArduinoGPIOList[nCtr], gArduinoDACList, ARDUINO_DACCNT) == true) {
			pIface->aGPIO[nCtr].eCapabilities = (eGPIOModes_t)(pIface->aGPIO[nCtr].eCapabilities | GPIO_AnalogOutput);
		}
		
		pIface->aGPIO[nCtr].eMode = GPIO_None;
		pIface->aGPIO[nCtr].pHWInfo = (void *)gArduinoGPIOList[nCtr];
	}
	
	//Initialize the necessary hardware
	analogReadResolution(pIface->nAnaInBitDepth);
	analogWriteResolution(pIface->nPWMBitDepth);  //Arduino uses this for PWM and DAC, so they better match
	
	return GPIO_Success;
}
	
eGPIOReturn_t ArduinoGPIOSetModeByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, eGPIOModes_t eMode) {
	uint8_t nIdx = IndexInArray_u8(nGPIOPin, gArduinoGPIOList, ARDUINO_GPIOCNT);
	
	if (nIdx >= ARDUINO_GPIOCNT) {
		return GPIOFail_InvalidPin;
	}
	
	if (CheckAllBitsInMask(pIface->aGPIO[nIdx].eCapabilities, eMode) == false) { 
		//Some feature requested is not valid for this pin
		return GPIOFail_Unsupported;
	}
	
	if ((CountSetBitsInInt32(eMode) > 1) && (eMode != GPIO_InputPullup)) {
		return GPIOFail_InvalidMode;
	}
	
	switch (eMode) {
		case GPIO_DigitalInput:
			pinMode(nGPIOPin, INPUT);
			break;
		case GPIO_DigitalOutput:
		case GPIO_OutputPWM: //PWM and Digital Output are handled the same
			pinMode(nGPIOPin, OUTPUT);
			break;
		case GPIO_InputPullup:
			pinMode(nGPIOPin, INPUT_PULLUP);
			break;
		case GPIO_AnalogInput:
			pinMode(nGPIOPin, INPUT);
			break;
		case GPIO_AnalogOutput:
			pinMode(nGPIOPin, OUTPUT);
			break;
		default: //Unknown or unrecognized settings
			return GPIOFail_Unsupported;
	}
	
	pIface->aGPIO[nIdx].eMode = eMode;
	
	return GPIO_Success;
}
	
eGPIOReturn_t ArduinoGPIOReadModeByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, eGPIOModes_t *eMode) {
	uint8_t nIdx = IndexInArray_u8(nGPIOPin, gArduinoGPIOList, ARDUINO_GPIOCNT);
	
	if (nIdx >= ARDUINO_GPIOCNT) {
		return GPIOFail_InvalidPin;
	}
	
	(*eMode) = pIface->aGPIO[nIdx].eMode;
	
	return GPIO_Success;
}

eGPIOReturn_t ArduinoGPIODigitalWriteByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, bool bState) {
	uint8_t nIdx = IndexInArray_u8(nGPIOPin, gArduinoGPIOList, ARDUINO_GPIOCNT);
	
	if (nIdx >= ARDUINO_GPIOCNT) {
		return GPIOFail_InvalidPin;
	}
	
	if (pIface->aGPIO[nIdx].eMode != GPIO_DigitalOutput) {
		return GPIOFail_InvalidOp;
	}
	
	digitalWrite(nGPIOPin, bState);
	
	return GPIO_Success;
}

eGPIOReturn_t ArduinoGPIODigitalReadByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, bool *bState) {
	if (digitalRead(nGPIOPin) == HIGH) {
		*bState = true;
	} else {
		*bState = false;
	}
	
	return GPIO_Success;
}

eGPIOReturn_t ArduinoGPIOPWMWriteByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, uint32_t nPWMValue) {
	uint8_t nIdx = IndexInArray_u8(nGPIOPin, gArduinoGPIOList, ARDUINO_GPIOCNT);
	
	if (nIdx >= ARDUINO_GPIOCNT) {
		return GPIOFail_InvalidPin;
	}
	
	if (pIface->aGPIO[nIdx].eMode != GPIO_OutputPWM) {
		return GPIOFail_InvalidOp;
	}
	
	analogWrite(nGPIOPin, nPWMValue);
	
	return GPIO_Success;
}

eGPIOReturn_t ArduinoGPIOAnalogWriteByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, uint32_t nAnaValue) {
	uint8_t nIdx = IndexInArray_u8(nGPIOPin, gArduinoGPIOList, ARDUINO_GPIOCNT);
	
	if (nIdx >= ARDUINO_GPIOCNT) {
		return GPIOFail_InvalidPin;
	}
	
	if (pIface->aGPIO[nIdx].eMode != GPIO_AnalogOutput) {
		return GPIOFail_InvalidOp;
	}
	
	analogWrite(nGPIOPin, nAnaValue);
	
	return GPIO_Success;
}

eGPIOReturn_t ArduinoGPIOAnalogReadByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, uint32_t *nAnaValue) {
	uint8_t nIdx = IndexInArray_u8(nGPIOPin, gArduinoGPIOList, ARDUINO_GPIOCNT);
	
	if (nIdx >= ARDUINO_GPIOCNT) {
		return GPIOFail_InvalidPin;
	}
	
	if (pIface->aGPIO[nIdx].eMode != GPIO_AnalogInput) {
		return GPIOFail_InvalidOp;
	}
	
	*nAnaValue = analogRead(nGPIOPin);
	
	return GPIO_Success;
}

eReturn_t ArduinoTimeIfaceInitialize(sTimeIface_t *pTime) {
	TimeInterfaceInitialize(pTime);
	
	pTime->pfGetTicks = &ArduinoGetTicks;
	pTime->pfDelaySeconds = &ArduinoDelaySeconds;
	pTime->pfDelayMilliSeconds = &ArduinoDelayMilliSeconds;
	pTime->pfDelayMicroSeconds = &ArduinoDelayMicroSeconds;
	
	return Success;
}

uint32_t ArduinoGetTicks(void) {
	return millis();
}

eReturn_t ArduinoDelaySeconds(uint32_t nDelayAmount) {
	delay(1000 * nDelayAmount);
	
	return Success;
}

eReturn_t ArduinoDelayMilliSeconds(uint32_t nDelayAmount) {
	delay(nDelayAmount);
	
	return Success;
}

eReturn_t ArduinoDelayMicroSeconds(uint32_t nDelayAmount) {
	delayMicroseconds(nDelayAmount);
	
	return Success;
}

#endif
