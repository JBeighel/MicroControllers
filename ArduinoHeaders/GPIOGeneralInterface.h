/**	@defgroup	gpioiface
	@brief		General interface for using GPIO pins
	@details	v0.2
		
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
		#define GPIO_IOCNT		20
	#endif
	
	typedef struct sGPIOIface_t sGPIOIface_t; //Declarign this early, will define it later

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

	/**	@brief		Function type to use in order to get the current system tick count
		@return		The current cound of system ticks, this number will overflow and wrap 
						at some point
		@ingroup	gpioiface
	*/
	typedef uint32_t (*pfGetCurrentTicks_t)();
	
	/**	@brief		Function type to use in order to delay for a number of milliseconds
		@param		nDelayTime		Number of milliseconds to delay for
		@ingroup	gpioiface
	*/
	typedef void (*pfMilliSecondDelay_t)(uint32_t nDelayTime);
	
	typedef eGPIOReturn_t (*pfGPIOPortInitialize_t)(sGPIOIface_t *pIface, void *pHWInfo);
	
	typedef eGPIOReturn_t (*pfGPIOSetModeByPin_t)(sGPIOIface_t *pIface, uint16_t nGPIOPin, eGPIOModes_t eMode);
	
	typedef eGPIOReturn_t (*pfGPIOReadModeByPin_t)(sGPIOIface_t *pIface, uint16_t nGPIOPin, eGPIOModes_t *eMode);
	
	typedef eGPIOReturn_t (*pfGPIODigitalWriteByPin_t)(sGPIOIface_t *pIface, uint16_t nGPIOPin, bool bState);
	
	typedef eGPIOReturn_t (*pfGPIODigitalReadByPin_t)(sGPIOIface_t *pIface, uint16_t nGPIOPin, bool *bState);
	
	typedef eGPIOReturn_t (*pfGPIOPWMWriteByPin_t)(sGPIOIface_t *pIface, uint16_t nGPIOPin, uint32_t nPWMValue);
	
	typedef eGPIOReturn_t (*pfGPIOAnalogWriteByPin_t)(sGPIOIface_t *pIface, uint16_t nGPIOPin, uint32_t nAnaValue);
	
	typedef eGPIOReturn_t (*pfGPIOAnalogReadByPin_t)(sGPIOIface_t *pIface, uint16_t nGPIOPin, uint32_t *nAnaValue);

	typedef struct sGPIOInfo_t {
		eGPIOModes_t eCapabilities;
		eGPIOModes_t eMode;
		
		void *pHWInfo;
	} sGPIOInfo_t;

	typedef struct sGPIOIface_t {
		pfGPIOPortInitialize_t pfPortInit;
		pfGPIOSetModeByPin_t pfSetModeByPin;
		pfGPIOReadModeByPin_t pfReadModeByPin;
		pfGPIODigitalWriteByPin_t pfDigitalWriteByPin;
		pfGPIODigitalReadByPin_t pfDigitalReadByPin;
		pfGPIOPWMWriteByPin_t pfPWMWriteByPin;
		pfGPIOAnalogWriteByPin_t pfAnalogWriteByPin;
		pfGPIOAnalogReadByPin_t pfAnalogReadByPin;
		
		sGPIOInfo_t aGPIO[GPIO_IOCNT];
		
		uint8_t nPWMBitDepth;
		uint8_t nAnaInBitDepth;
		uint8_t nAnaOutBitDepth;
		uint8_t nGPIOCnt;
		
		void *pHWInfo;
	} sGPIOHWObj_t;
	

/***** Constants	*****/


/***** Globals		*****/


/***** Prototypes 	*****/
	eGPIOReturn_t GPIOInterfaceInitialize(sGPIOIface_t *pIface);
	
	eGPIOReturn_t GPIOPortInitialize(sGPIOIface_t *pIface, void *pHWInfo);
	
	eGPIOReturn_t GPIOSetModeByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, eGPIOModes_t eMode);
	
	eGPIOReturn_t GPIOReadModeByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, eGPIOModes_t *eMode);
	
	eGPIOReturn_t GPIODigitalWriteByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, bool bState);
	
	eGPIOReturn_t GPIODigitalReadByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, bool *bState);
	
	eGPIOReturn_t GPIOPWMWriteByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, uint32_t nPWMValue);
	
	eGPIOReturn_t GPIOAnalogWriteByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, uint32_t nAnaValue);
	
	eGPIOReturn_t GPIOAnalogReadByPin(sGPIOIface_t *pIface, uint16_t nGPIOPin, uint32_t *nAnaValue);

/***** Functions	*****/

	
#endif

