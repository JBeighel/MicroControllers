/**	@defgroup	gpioiface
	@brief		General interface for using GPIO pins
	@details	v0.6
	#Description
	
	#File Information
		File:	GPIOGeneralInterface.h
		Author:	J. Beighel
		Date:	2021-03-04
*/

#ifndef __gpioiface
	#define __gpioiface

/***** Includes		*****/
	#include <stdint.h>
	#include <stdbool.h>
	#include <string.h>

/***** Definitions	*****/
	
	#ifndef GPIO_IOCNT
		/**	@brief		Number of GPIO points to allocate space for
			@ingroup	gpioiface
		*/
		#define GPIO_IOCNT		30
	#endif
	
	/**	@brief		Value to use for an index to indicate this pin is not in use
		@ingroup	gpioiface
	*/
	#define GPIO_NOPIN			(0xFFFF)
	
	typedef struct sGPIOIface_t sGPIOIface_t; //Declarign this early, will define it later
	
	/**	@brief		Convenience type, defines value used to identify GPIO pins
		@ingroup	gpioiface
	*/
	typedef uint16_t GPIOID_t;
		
	typedef enum eGPIOCapabilities_t {
		GPIOCap_None			= 0x0000,
		GPIOCap_SetPinMode		= 0x0001,
		GPIOCap_ReadPinMode		= 0x0002,
		GPIOCap_DigitalWrite	= 0x0004,
		GPIOCap_DigitalRead		= 0x0008,
		GPIOCap_PWMWrite		= 0x0010,
		GPIOCap_AnalogWrite		= 0x0020,
		GPIOCap_AnalogRead		= 0x0040,
		GPIOCap_SetInterrupt	= 0x0080,
	} eGPIOCapabilities_t;
	
	/**	@brief		Enumeration of all GPIO interface return values
		@ingroup	gpioiface
	*/
	typedef enum eGPIOReturn_t {
		GPIOWarn_Unknown		= 1,	/**< An unknown but recoverable error happened during the operation */
		GPIO_Success			= 0,	/**< The operation completed successfully */
		GPIOFail_Unknown		= -1,	/**< An unknown and unrecoverable error happened during the operation */
		GPIOFail_Unsupported	= -2,	/**< The requested operation is not supported by this device */
		GPIOFail_InvalidPin		= -3,	/**< The requested pin does not exist */
		GPIOFail_InvalidMode	= -4,	/**< The requested pin mode is invalid */
		GPIOFail_InvalidOp		= -5,	/**< A request to read or write to a GPIO in a mode that doesn't allow this */
	} eGPIOReturn_t;
	
	/**	@brief		Enumeration of all possible GPIO modes
		@ingroup	gpioiface
	*/
	typedef enum eGPIOModes_t {
		GPIO_None				= 0x00,
		GPIO_DigitalInput		= 0x01,
		GPIO_DigitalOutput		= 0x02,
		GPIO_OutputPWM			= 0x04,
		GPIO_InputPullup		= 0x09,
		GPIO_AnalogInput		= 0x10,
		GPIO_AnalogOutput		= 0x20,
	} eGPIOModes_t;

	typedef void (*pfGPIOInterrupt_t)(sGPIOIface_t *pIface, GPIOID_t nPin, void *pParam);

	typedef eGPIOReturn_t (*pfGPIOPortInitialize_t)(sGPIOIface_t *pIface, void *pHWInfo);
	
	typedef eGPIOReturn_t (*pfGPIOSetModeByPin_t)(sGPIOIface_t *pIface, GPIOID_t nGPIOPin, eGPIOModes_t eMode);
	
	typedef eGPIOReturn_t (*pfGPIOReadModeByPin_t)(sGPIOIface_t *pIface, GPIOID_t nGPIOPin, eGPIOModes_t *eMode);
	
	typedef eGPIOReturn_t (*pfGPIODigitalWriteByPin_t)(sGPIOIface_t *pIface, GPIOID_t nGPIOPin, bool bState);
	
	typedef eGPIOReturn_t (*pfGPIODigitalReadByPin_t)(sGPIOIface_t *pIface, GPIOID_t nGPIOPin, bool *bState);
	
	typedef eGPIOReturn_t (*pfGPIOPWMWriteByPin_t)(sGPIOIface_t *pIface, GPIOID_t nGPIOPin, uint32_t nPWMValue);
	
	typedef eGPIOReturn_t (*pfGPIOAnalogWriteByPin_t)(sGPIOIface_t *pIface, GPIOID_t nGPIOPin, uint32_t nAnaValue);
	
	typedef eGPIOReturn_t (*pfGPIOAnalogReadByPin_t)(sGPIOIface_t *pIface, GPIOID_t nGPIOPin, uint32_t *nAnaValue);

	typedef eGPIOReturn_t (*pfGPIOSetInterrupt_t)(sGPIOIface_t *pIface, GPIOID_t nGPIOPin, pfGPIOInterrupt_t pHandler, bool bEnable, void *pParam);

	typedef struct sGPIOInfo_t {
		eGPIOModes_t eCapabilities;
		eGPIOModes_t eMode;
		
		void *pHWInfo;
	} sGPIOInfo_t;

	typedef struct sGPIOIface_t {
		eGPIOCapabilities_t ePortCapabilities;

		pfGPIOPortInitialize_t pfPortInit;
		pfGPIOSetModeByPin_t pfSetModeByPin;
		pfGPIOReadModeByPin_t pfReadModeByPin;
		pfGPIODigitalWriteByPin_t pfDigitalWriteByPin;
		pfGPIODigitalReadByPin_t pfDigitalReadByPin;
		pfGPIOPWMWriteByPin_t pfPWMWriteByPin;
		pfGPIOAnalogWriteByPin_t pfAnalogWriteByPin;
		pfGPIOAnalogReadByPin_t pfAnalogReadByPin;
		pfGPIOSetInterrupt_t pfSetInterrupt;
		
		sGPIOInfo_t aGPIO[GPIO_IOCNT];
		
		uint8_t nPWMBitDepth;
		uint8_t nAnaInBitDepth;
		uint8_t nAnaOutBitDepth;
		uint8_t nGPIOCnt;
		
		void *pHWInfo;
	} sGPIOIface_t;
		

/***** Constants	*****/


/***** Globals		*****/


/***** Prototypes 	*****/
	eGPIOReturn_t GPIOInterfaceInitialize(sGPIOIface_t *pIface);
	

/***** Functions	*****/

	
#endif

