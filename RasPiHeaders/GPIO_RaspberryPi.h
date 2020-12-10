/**	@defgroup	gpioraspi
	@brief		GPIO General Interface implementation for Raspberry Pi
	@details	v0.1
	# Description #
	
	# Usage #
	
	# File Information #
		File:	GPIO_RaspberryPi.h
		Author:	J. Beighel
		Created:11-24-2020
*/

#ifndef __GPIORASPBERRYPI
	#define __GPIORASPBERRYPI

/*****	Includes	*****/
	#include <stdio.h>
	#include <stdlib.h>
	
	#include <fcntl.h>
	#include <sys/mman.h>
	#include <unistd.h>
	
	#include "CommonUtils.h"
	#include "GPIOGeneralInterface.h"

/*****	Definitions	*****/
	/**	@brief		Pointer to the GPIO object for the Raspberry Pi
		@ingroup	gpioraspi
	*/
	#define GPIO_HWINFO		((void *)RASPI_PERIPHBASE)
	
	/**	@brief		Function to call when initializing the Raspberry Pi GPIO interface
		@ingroup	gpioraspi
	*/
	#define GPIO_INIT		RasPiGPIOPortInitialize

	/**	@brief		Marker for the Raspberry Pi model 1 
		@ingroup	gpioraspi
	*/
	#define	RASPI_MODEL1		1
	
	/**	@brief		Marker for the Raspberry Pi model 2
		@ingroup	gpioraspi
	*/
	#define	RASPI_MODEL2		2
	
	/**	@brief		Marker for the Raspberry Pi model 3/3+
		@ingroup	gpioraspi
	*/
	#define	RASPI_MODEL3		3
	
	/**	@brief		Marker for the Raspberry Pi model 4
		@ingroup	gpioraspi
	*/
	#define	RASPI_MODEL4		4
	
	#ifndef RASPI_CURRMODEL
		/**	@brief		Compile time detection of the Raspberry Pi model 
			@ingroup	gpioraspi
		*/
		#define RASPI_CURRMODEL	RASPI_MODEL2
	#endif

	/**	@brief		Peripheral register address for the Raspberry Pi
		@ingroup	gpioraspi
	*/
	#if RASPI_CURRMODEL == RASPI_MODEL1
		#define RASPI_PERIPHBASE	0x20000000
	#else
		#define RASPI_PERIPHBASE	0x3F000000
	#endif
	
	/**	@brief		Offset from the peripheral register to the GPIO registers
		@ingroup	gpioraspi
	*/
	#define RASPI_GPIOOFFSET		0x00200000
	
	/**	@brief		Offset from the GPIO register to the Pin Mode register
		@ingroup	gpioraspi
	*/
	#define RASPI_GPIOMODE		0x00000000
	
	/**	@brief		Offset from the GPIO register to the Output Set register
		@ingroup	gpioraspi
	*/
	#define RASPI_GPIOSET		0x00000007
	
	/**	@brief		Offset from the GPIO register to the Output Clear register
		@ingroup	gpioraspi
	*/
	#define RASPI_GPIOCLEAR		0x0000000A
	
	/**	@brief		Offset from the GPIO register to the Input Read register
		@ingroup	gpioraspi
	*/
	#define RASPI_GPIOREAD		0x0000000D
	
	/**	@brief		Offset from the GPIO register to the Output Pull Up/Down register
		@ingroup	gpioraspi
	*/
	#define RASPI_GPIOPUPDN		0x00000025
	
	/**	@brief		Offset from the GPIO register to the Output Pull Up/Down clock register
		@ingroup	gpioraspi
	*/
	#define RASPI_GPIOPUPDNCLK	0x00000026
	
	/**	@brief		Size of the block of RAM needed for the GPIO registers
		@ingroup	gpioraspi
	*/
	#define RASPI_BLOCKSIZE		4096

	/**	@brief		Number of GPIO pins available on the Raspberry Pi
		@ingroup	gpioraspi
	*/
	#define RASPI_PINCOUNT		28
	#if (GPIO_IOCNT < RASPI_PINCOUNT)
		//Can't support more pins than the interface allows
		#undef RASPI_PINCOUNT
		#define RASPI_PINCOUNT	GPIO_IOCNT
	#endif

	/**	@brief		Variable type to use for the register pointers
		@details	The size, uint32, is necessary for the pointer arithmentic
			needed to navigate the registers.
			
			Volatile is needed so that the optimizer will always read from the 
			memory address rather than relying on a processor buffer.
		@ingroup	gpioraspi
	*/
	typedef volatile uint32_t *	pRasPiRegType_t;

	/**	@brief		Enumeration of GPIO Pin Mode bits
		@details	Mode registers allocate three bits to a single GPIO pin.
			This allows each 32 bit register to configure 10 pins.  To move 
			from pin to pin you must shift the settings by 3 pins per register.
			
			The mode registers have consecutive addresses, so moving to the 
			next register will give seting for the next 10 pins.
			
			To get the register for zero based index Pin A you calculate the 
			register as:
				Mode Reg = RASPI_PERIPHBASE + RASPI_GPIOOFFSET + RASPI_GPIOMODE + (A / 10)
			
			To get to the settings for Pin A from that register:
				Mode = (RegVal >> ((A % 10) * 3)) & 0x07
		@ingroup	gpioraspi
	*/
	typedef enum eRasPiGPIOMode_t {
		RasPi_GPIOInput			= 0x00,	/**< Flags to specify a pin as an input */
		RasPi_GPIOOutput		= 0x01,	/**< Flags to specify a pin as an output */
		
		RasPi_GPIORegPins		= 10,	/**< Mode registers hold information for this many pins */
		RasPi_GPIOPinShift		= 3,	/**< Number of bits to shift to reach values for the next pin */
		RasPi_GPIOPinMask		= 0x07,	/**< Mask of all bits related to a single pin */
	} eRasPiGPIOMode_t;

/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	/**	@brief		Function to initialize the Raspberry Pi GPIO hardware
		@param		pIface		Pointer to interface object to prepart for use 
			to interact with the hardware.
		@param		pHWInfo		Pointer to the object containing all of the 
			hardware information needed to operate the GPIO pins
		@ingroup	gpioraspi
	*/
	eGPIOReturn_t RasPiGPIOPortInitialize(sGPIOIface_t *pIface, void *pHWInfo);
	

/*****	Functions	*****/


#endif

