/**	File:	PinSupport.c
	Author:	J. Beighel
	Date:	2021-03-02
*/

/*****	Includes	*****/
#include "PinSupport.h"

/*****	Defines		*****/


/*****	Definitions	*****/


/*****	Constants	*****/


/*****	Globals		*****/
	sGPIOIface_t gGpioA;
	sGPIOIface_t gGpioB;
	sGPIOIface_t gGpioH;

	sI2CIface_t gI2C1;
	sI2CIface_t gI2C3;

	sUARTIface_t gUart1;
	sUARTIface_t gUart2;

	sSPIIface_t gSpi1;

	sTimeIface_t gTime;

/*****	Prototypes 	*****/
	void GPIA03IntHandler(sGPIOIface_t *pIface, GPIOID_t nPin, void *pParam);

/*****	Functions	*****/
eReturn_t PinSetup() {
	//Set up all IO pins
	GPIO_INIT(&gGpioA, GPIO_A_HWINFO);
	GPIO_INIT(&gGpioB, GPIO_B_HWINFO);
	GPIO_INIT(&gGpioH, GPIO_H_HWINFO);

	//Set up all buses
	I2C_INIT(&gI2C1, true, 100000, I2C_1_HWINFO);
	I2C_INIT(&gI2C3, true, 100000, I2C_3_HWINFO);

	UART_INIT(&gUart1, 115200, UART_8None1, UART_1_HWINFO);
	UART_INIT(&gUart2, 115200, UART_8None1, UART_2_HWINFO);

	SPI_INIT(&gSpi1, SPI_1_HWINFO, 4000000, SPI_MSBFirst, SPI_Mode3);

	TIME_INIT(&gTime);

	//Arrange interrupts
	gGpioA.pfSetInterrupt(&gGpioA, GPI_A03_1_Pin, &GPIA03IntHandler, true, NULL);

	return Success;
}

void GPIA03IntHandler(sGPIOIface_t *pIface, GPIOID_t nPin, void *pParam) {
	int nCtr;

	nCtr = 0;
	nCtr += 1;

	return;
}
