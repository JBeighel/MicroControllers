/**	@defgroup	samdtimer
	@brief		Timer module for the Arduino models using a SAMD processor
	@details	v0.1
		This module was tested on the Nano IoT.
		
*/

#ifndef __ARDUINOSAMD_TIMER
	#define __ARDUINOSAMD_TIMER
	
	#ifndef ARDUINO_ARCH_SAMD //deficned by the libraries associated with this architecture
		#error SAMD Timer library is only for use with that specific processor architecture
	#endif
	
	#define TIMER_DEBUG
	#ifdef TIMER_DEBUG
		#define DebugPrint(...)	Serial.print(__VA_ARGS__)
	#else
		#define DebugPrint(...)
	#endif

/***** Includes		*****/


/***** Definitions	*****/
	/**	@brief		Number of timers this class will provide
		@details	There is code in place to handle 4 timers.  However the interrupt vector for 
			TC5 never fires, so that is not working.  Disabling this here until more time can be 
			devoted to it.
		@ingroup	samdtimer
	*/
	#define	TIMERS_AVAIL	2

	typedef enum eTimerFrequency_t {
		Timer_SAMD_48MHz	= 1,
		Timer_SAMD_24MHz	= 2,
		Timer_SAMD_16MHz	= 3,
		Timer_SAMD_12MHz	= 4,
		Timer_SAMD_8MHz		= 6,
		Timer_SAMD_6MHz		= 8,
		Timer_SAMD_4MHz		= 12,
		Timer_SAMD_3MHz		= 16,
		Timer_SAMD_2MHz		= 24,
		Timer_SAMD_1MHz		= 48,
	} eTimerFrequency_t;
	
	typedef enum eTimerNum_t {
		Timer_1				= 0,
		Timer_2				= 1,
		Timer_3				= 2,
		Timer_4				= 3,
	} eTimerNum_t;
	
	typedef void (*pfTimerCallback_t)(void);
	
	class iTimerModule_t {
		public:
			iTimerModule_t();
			~iTimerModule_t();
			
			uint32_t GetTick(eTimerNum_t eTimer);
			bool SetCallback(eTimerNum_t eTimer, pfTimerCallback_t pfCallback);
			bool SetTick(eTimerNum_t eTimer, uint32_t tMicroSec);
			
			virtual bool Enable(eTimerNum_t eTimer, bool bSetEnable) = 0;
			virtual bool IsEnabled(eTimerNum_t eTimer) = 0;
			
		protected:
			/**	@brief		Number of counts that equals 1 microsecond
				@details	Set by interface implementing class
			*/
			uint32_t ctCountsPerMicroSec;
			
			/**	@brief		Number of counts immediately before the counter overflows
				@details	Set by interface implementing class
			*/
			uint32_t cnCountsAtOverflow;
			
			uint32_t canTimerTick[TIMERS_AVAIL];
			
			pfTimerCallback_t cafCallbacks[TIMERS_AVAIL];
			
			bool HandleTimerTick(eTimerNum_t eTimer);
			
			virtual uint32_t GetTimerCount(eTimerNum_t eTimer) = 0;
			virtual bool SetNextCountMatch(eTimerNum_t eTimer, uint32_t nCountMatch) = 0;
		
		private:
		
	};
	
	class cSAMDTimer_t : public iTimerModule_t {
		public:
			cSAMDTimer_t();
			~cSAMDTimer_t();
			
			bool Begin();
			bool Enable(eTimerNum_t eTimer, bool bSetEnable);
			bool IsEnabled(eTimerNum_t eTimer);
			
			bool InterruptHandler();
		
		protected:
			uint32_t GetTimerCount(eTimerNum_t eTimer);
			bool SetNextCountMatch(eTimerNum_t eTimer, uint32_t nCountMatch);
		
		public:
	};

/***** Constants	*****/


/***** Globals		*****/
cSAMDTimer_t gTimer = cSAMDTimer_t();

/***** Prototypes 	*****/


/***** Functions	*****/
void TimerBeginInterrupt(eTimerFrequency_t eFreq) {
	//Set up the generic clock (GCLK4) used to clock timers
	//Divide the 48MHz clock source by eFreq
	//Select Generic Clock (GCLK) 4
	REG_GCLK_GENDIV = GCLK_GENDIV_DIV((uint8_t)eFreq) | GCLK_GENDIV_ID(4);            
	while (GCLK->STATUS.bit.SYNCBUSY); //Wait for synchronization

	//Set the duty cycle to 50/50 HIGH/LOW | Enable GCLK4 | set 48MKHz source | use GLCK4
	REG_GCLK_GENCTRL = GCLK_GENCTRL_IDC | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL48M | GCLK_GENCTRL_ID(4);
	while (GCLK->STATUS.bit.SYNCBUSY); //Wait for synchronization

	//Feed GCLK4 to TC4 and TC5
	//Enable GCLK4 to TC4 and TC5 | Select GCLK4 | Feed the GCLK4 to TC4 and TC5
	REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK4 | GCLK_CLKCTRL_ID_TC4_TC5;
	while (GCLK->STATUS.bit.SYNCBUSY); //Wait for synchronization

	REG_TC4_CTRLA |= TC_CTRLA_MODE_COUNT32; //Set the counter to 8-bit mode
	while (GCLK->STATUS.bit.SYNCBUSY); //Wait for synchronization

	REG_TC4_COUNT32_CC0 = 0x00; //Set the TC4 CC0 register to some arbitary value
	while (GCLK->STATUS.bit.SYNCBUSY); //Wait for synchronization
	REG_TC4_COUNT32_CC1 = 0x00; //Set the TC4 CC1 register to some arbitary value
	while (GCLK->STATUS.bit.SYNCBUSY); //Wait for synchronization
	//REG_TC4_COUNT8_PER = 0xFF; //Set the PER (period) register to its maximum value
	while (GCLK->STATUS.bit.SYNCBUSY); //Wait for synchronization

	//NVIC_DisableIRQ(TC4_IRQn);
	//NVIC_ClearPendingIRQ(TC4_IRQn);
	
	NVIC_SetPriority(TC4_IRQn, 0); //Set the Nested Vector Interrupt Controller (NVIC) priority for TC4 to 0 (highest)
	NVIC_EnableIRQ(TC4_IRQn); //Connect TC4 to Nested Vector Interrupt Controller (NVIC)

	REG_TC4_INTFLAG |= TC_INTFLAG_MC1 | TC_INTFLAG_MC0 | TC_INTFLAG_OVF; //Clear the interrupt flags
	REG_TC4_INTENSET = TC_INTENSET_MC1 | TC_INTENSET_MC0 | TC_INTENSET_OVF;//Enable TC4 interrupts
	
	// REG_TC4_INTENCLR = TC_INTENCLR_MC1 | TC_INTENCLR_MC0 | TC_INTENCLR_OVF; //Disable TC4 interrupts

	//Set prescaler to 64, 16MHz/64 = 256kHz | Enable TC4
	REG_TC4_CTRLA |= TC_CTRLA_PRESCALER_DIV1 | TC_CTRLA_ENABLE;
	while (GCLK->STATUS.bit.SYNCBUSY); //Wait for synchronization
	
	return;
}

void TimerSetMatch0(uint8_t nMatchCount) {
	REG_TC4_COUNT8_CC0 = nMatchCount; //Set the TC4 CC0 register to some arbitary value
	while (GCLK->STATUS.bit.SYNCBUSY); //Wait for synchronization
	
	return;
}

void TimerSetMatch1(uint8_t nMatchCount) {
	REG_TC4_COUNT8_CC1 = nMatchCount; //Set the TC4 CC0 register to some arbitary value
	while (GCLK->STATUS.bit.SYNCBUSY); //Wait for synchronization
	
	return;
}

iTimerModule_t::iTimerModule_t() {
	uint32_t nCtr;
	
	//Set initial values in all class variables
	ctCountsPerMicroSec = 0;
	cnCountsAtOverflow = 0;
	
	for (nCtr = 0; nCtr < TIMERS_AVAIL; nCtr++) {
		canTimerTick[nCtr] = 0;
	}
	
	return;
}

iTimerModule_t::~iTimerModule_t() {
	//No work to be done, inheritting classes are expected to shut down the timers
	return;
}

uint32_t iTimerModule_t::GetTick(eTimerNum_t eTimer) {
	return canTimerTick[eTimer];
}

bool iTimerModule_t::SetCallback(eTimerNum_t eTimer, pfTimerCallback_t pfCallback) {
	if (eTimer >= TIMERS_AVAIL) {
		return false;
	}
	
	cafCallbacks[eTimer] = pfCallback;
	
	return true;
}

bool iTimerModule_t::SetTick(eTimerNum_t eTimer, uint32_t tMicroSec) {
	uint32_t nNewMatch;
	
	if (eTimer >= TIMERS_AVAIL) {
		return false;
	}
	
	nNewMatch = GetTimerCount(eTimer);
	nNewMatch += tMicroSec * ctCountsPerMicroSec;
	
	SetNextCountMatch(eTimer, nNewMatch);
	
	canTimerTick[eTimer] = tMicroSec;
	
	return true;
}

bool iTimerModule_t::HandleTimerTick(eTimerNum_t eTimer) {
	if (eTimer >= TIMERS_AVAIL) {
		return false;
	}
	
	SetTick(eTimer, canTimerTick[eTimer]);
	
	if (cafCallbacks[eTimer]!= NULL) {
		cafCallbacks[eTimer]();
	}
	
	return true;
}

cSAMDTimer_t::cSAMDTimer_t() {
	//No class variables to initialize
	return;
}

bool cSAMDTimer_t::Begin() {
	DebugPrint("Timer: SAMD constructor\n");

	//Using generic clock 4 (GCLK4) as the timer to interrupt on
	//Set the clock to 1MHz so that counts directly equal microseconds
	REG_GCLK_GENDIV = GCLK_GENDIV_DIV((uint8_t)Timer_SAMD_1MHz) | GCLK_GENDIV_ID(4);            
	while (GCLK->STATUS.bit.SYNCBUSY); //Wait for synchronization
	
	//Give the clok a 50/50 duty cycle, Enable GCLK4, and feed GCLK4 to TC4 and TC5
	REG_GCLK_GENCTRL = GCLK_GENCTRL_IDC | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL48M | GCLK_GENCTRL_ID(4);
	while (GCLK->STATUS.bit.SYNCBUSY); //Wait for synchronization
	
	//Enable and select GLK4, feed it to TC4 and TC5
	REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK4 | GCLK_CLKCTRL_ID_TC4_TC5;
	while (GCLK->STATUS.bit.SYNCBUSY); //Wait for synchronization
	
	//Set TC4 and TC5 to 32 bit 
	TC4->COUNT32.CTRLA.reg |= TC_CTRLA_MODE_COUNT32;
	while (GCLK->STATUS.bit.SYNCBUSY); //Wait for synchronization
	
	TC5->COUNT32.CTRLA.reg |= TC_CTRLA_MODE_COUNT32;
	while (GCLK->STATUS.bit.SYNCBUSY); //Wait for synchronization

	//Set match registers to a default value
	TC4->COUNT32.CC[0].reg = 0;
	while (GCLK->STATUS.bit.SYNCBUSY); //Wait for synchronization
	TC4->COUNT32.CC[1].reg = 0;
	while (GCLK->STATUS.bit.SYNCBUSY); //Wait for synchronization

	TC5->COUNT32.CC[0].reg = 0;
	while (GCLK->STATUS.bit.SYNCBUSY); //Wait for synchronization
	TC5->COUNT32.CC[1].reg = 0;
	while (GCLK->STATUS.bit.SYNCBUSY); //Wait for synchronization
	
	//Set interrupt priorirty and enable them for use
	NVIC_SetPriority(TC4_IRQn, 0); //Set the Nested Vector Interrupt Controller (NVIC) priority to 0 (highest)
	NVIC_EnableIRQ(TC4_IRQn); //Connect to Nested Vector Interrupt Controller (NVIC)
	
	NVIC_SetPriority(TC5_IRQn, 0); //Set the Nested Vector Interrupt Controller (NVIC) priority to 0 (highest)
	NVIC_EnableIRQ(TC5_IRQn); //Connect to Nested Vector Interrupt Controller (NVIC)
	
	//Clear any interrupt flags
	TC4->COUNT32.INTFLAG.reg = TC_INTFLAG_OVF | TC_INTFLAG_MC0 | TC_INTFLAG_MC1;
	TC5->COUNT32.INTFLAG.reg = TC_INTFLAG_OVF | TC_INTFLAG_MC0 | TC_INTFLAG_MC1;
	
	TC4->COUNT32.INTENCLR.reg = TC_INTENCLR_MC1 | TC_INTENCLR_MC0 | TC_INTENCLR_OVF; //Start with interrupts disabled
	TC5->COUNT32.INTENCLR.reg = TC_INTENCLR_MC1 | TC_INTENCLR_MC0 | TC_INTENCLR_OVF; //Start with interrupts disabled
	
	//TC5->COUNT32.INTENSET.reg = TC_INTENSET_MC1 | TC_INTENSET_MC0 | TC_INTENSET_OVF; //Start with interrupts disabled
	
	//Set prescaler to 1, don't subdivide the clock | Enable TC4
	TC4->COUNT32.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1 | TC_CTRLA_ENABLE;
	while (GCLK->STATUS.bit.SYNCBUSY); //Wait for synchronization
	
	TC5->COUNT32.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1 | TC_CTRLA_ENABLE;
	while (GCLK->STATUS.bit.SYNCBUSY); //Wait for synchronization
	
	//Interrupts at timer level are disabled by default
	Enable(Timer_1, false);
	Enable(Timer_2, false);
	Enable(Timer_3, false);
	Enable(Timer_4, false);
	
	//Update the variables describing the timer
	ctCountsPerMicroSec = 1;
	cnCountsAtOverflow = 0xFFFFFFFF;

	return true;
}

cSAMDTimer_t::~cSAMDTimer_t() {
	Enable(Timer_1, false); //Disable all timers before closing down
	Enable(Timer_2, false);
	Enable(Timer_3, false);
	Enable(Timer_4, false);
	
	return; 
}

bool cSAMDTimer_t::Enable(eTimerNum_t eTimer, bool bSetEnable) {
	//Set the flag to enable the match register for the timer interrupt
	switch (eTimer) {
		case Timer_1:
			if (bSetEnable == true) {
				TC4->COUNT32.INTENSET.reg |= TC_INTENSET_MC0;
			} else {
				TC4->COUNT32.INTENCLR.reg = TC_INTENSET_MC0;
			}
			break;
		case Timer_2:
			if (bSetEnable == true) {
				TC4->COUNT32.INTENSET.reg |= TC_INTENSET_MC1;
			} else {
				TC4->COUNT32.INTENCLR.reg = TC_INTENSET_MC1;
			}
			break;
		case Timer_3:
			if (bSetEnable == true) {
				TC5->COUNT32.INTENSET.reg |= TC_INTENSET_MC0;
			} else {
				TC5->COUNT32.INTENCLR.reg = TC_INTENSET_MC0;
			}
			break;
		case Timer_4:
			if (bSetEnable == true) {
				TC5->COUNT32.INTENSET.reg |= TC_INTENSET_MC1;
			} else {
				TC5->COUNT32.INTENCLR.reg = TC_INTENSET_MC1;
			}
			break;
		default: 
			return false;
	}
	
	//Reset the interval to be sure we aren't past it
	SetTick(eTimer, canTimerTick[eTimer]);
	
	return true;
}

bool cSAMDTimer_t::IsEnabled(eTimerNum_t eTimer) {
	switch (eTimer) {
		case Timer_1:
			if (TC4->COUNT32.INTENSET.reg && TC_INTENSET_MC0 != 0) {
				return true;
			} else {
				return false;
			}
		case Timer_2:
			if (TC4->COUNT32.INTENSET.reg && TC_INTENSET_MC1 != 0) {
				return true;
			} else {
				return false;
			}
		case Timer_3:
			if (TC5->COUNT32.INTENSET.reg && TC_INTENSET_MC0 != 0) {
				return true;
			} else {
				return false;
			}
		case Timer_4:
			if (TC5->COUNT32.INTENSET.reg && TC_INTENSET_MC1 != 0) {
				return true;
			} else {
				return false;
			}
		default: 
			return false;
	}
}

bool cSAMDTimer_t::InterruptHandler() {
	//Check TC4 interrupts
	if (TC4->COUNT32.INTFLAG.bit.OVF == 1) { //Overflow interrupt on TC4
		//Nothing uses this interrupt, so just clear it
		TC4->COUNT32.INTFLAG.reg = TC_INTFLAG_OVF; //Write the the interrupt flag we want to clear
	}
	
	if (TC4->COUNT32.INTFLAG.bit.MC0 && TC4->COUNT32.INTENSET.bit.MC0) { //Match 1 interrupt on TC4
		//This is Timer_2, so raise that event_callback
		HandleTimerTick(Timer_1);
		
		TC4->COUNT32.INTFLAG.reg = TC_INTFLAG_MC0;
	}
	
	if (TC4->COUNT32.INTFLAG.bit.MC1 && TC4->COUNT32.INTENSET.bit.MC1) { //Match 1 interrupt on TC4
		//This is Timer_2, so raise that event_callback
		HandleTimerTick(Timer_2);
		
		TC4->COUNT32.INTFLAG.reg = TC_INTFLAG_MC1;
	}
	
	//Check TC5 interrupts
	if (TC5->COUNT32.INTFLAG.bit.OVF && TC5->COUNT32.INTENSET.bit.OVF) { //Overflow interrupt on TC5
		//Nothing uses this interrupt, so just clear it
		TC5->COUNT32.INTFLAG.reg = TC_INTFLAG_OVF; //Write the the interrupt flag we want to clear
	}
	
	if (TC5->COUNT32.INTFLAG.bit.MC0 && TC5->COUNT32.INTENSET.bit.MC0) { //Match 0 interrupt on TC5
		//This is Timer_3, so raise that event_callback
		HandleTimerTick(Timer_3);
		
		TC5->COUNT32.INTFLAG.reg = TC_INTFLAG_MC0;
	}
	
	if (TC5->COUNT32.INTFLAG.bit.MC1 && TC5->COUNT32.INTENSET.bit.MC1) { //Match 1 interrupt on TC5
		//This is Timer_4, so raise that event_callback
		HandleTimerTick(Timer_4);
		
		TC5->COUNT32.INTFLAG.reg = TC_INTFLAG_MC1;
	}
	
	return true;
}

uint32_t cSAMDTimer_t::GetTimerCount(eTimerNum_t eTimer) {
	switch (eTimer) {
		case Timer_1:
		case Timer_2:
			return TC4->COUNT32.COUNT.reg;
		case Timer_3:
		case Timer_4:
			return TC5->COUNT32.COUNT.reg;
		default: 
			return 0;
	}
}

bool cSAMDTimer_t::SetNextCountMatch(eTimerNum_t eTimer, uint32_t nCountMatch) {
	switch (eTimer) {
		case Timer_1:
			TC4->COUNT32.CC[0].reg = nCountMatch;
			break;
		case Timer_2:
			TC4->COUNT32.CC[1].reg = nCountMatch;
			break;
		case Timer_3:
			TC5->COUNT32.CC[0].reg = nCountMatch;
			break;
		case Timer_4:
			TC5->COUNT32.CC[1].reg = nCountMatch;
			break;
		default: 
			return false;
	}
}

void TC4_Handler() { //Timer 4 interrupt handle
	gTimer.InterruptHandler();
	//TC4->COUNT32.INTFLAG.reg = TC_INTFLAG_OVF | TC_INTFLAG_MC0 | TC_INTFLAG_MC1;
	return;
}

void TC5_Handler() { //Timer 5 interrupt handle
	DebugPrint("TC5 Int\n");
	gTimer.InterruptHandler();
	//TC4->COUNT32.INTFLAG.reg = TC_INTFLAG_OVF | TC_INTFLAG_MC0 | TC_INTFLAG_MC1;
	return;
}

#endif

