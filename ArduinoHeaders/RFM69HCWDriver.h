/**	@defgroup	rfm69hcwdriver
	@brief		Driver for the RFM69HCW LoRa radio module
	@details	
		
*/

#ifndef __RFM69HCWDRIVER
	#define __RFM69HCWDRIVER

/***** Includes		*****/


/***** Definitions	*****/
	enum eRFM69HCW_Registers_t {
		LORA_Fifo			= 0x00,	/**< FIFO Read/Write access */
		LORA_OpMode			= 0x01,	/**< Operating modes of the transceiver */
		LORA_DataModul		= 0x02,	/**< Data operation mode and Modulation settings */
		LORA_BitrateMSB		= 0x03,	/**< Bit Rate setting, most significant bits */
		LORA_BitrateLSB1	= 0x04,	/**< Bit Rate setting, least significant bits */
		LORA_FDevMSB		= 0x05,	/**< Frequency deviation setting, most significant bits */
		LORA_FDevLSB		= 0x06,	/**< Frequency deviation setting, least significnat bits */
		LORA_RFFreqMSB		= 0x07,	/**< RF carrier frequency, most significant bits */
		LORA_RFFreqMid		= 0x08,	/**< RF carrier frequency, intermediate bits */
		LORA_RFFreqLSB		= 0x09,	/**< RF carrier frequency, least significant bits */
		LORA_Osc1			= 0x0A,	/**< RC oscillators settings */
		LORA_AFCControl		= 0x0B,	/**< AFC control in low modulation index situations */
		LORA_Listen1		= 0x0D,	/**< Listen mode settings */
		LORA_Listen2		= 0x0E,	/**< Listen mode idle duration */
		LORA_Listen3		= 0x0F,	/**< Listen mode Rx duration */
		LORA_Version		= 0x10,
		LORA_PALevel		= 0x11,	/**< PA selection and output power control */
		LORA_PARamp			= 0x12,	/**< Control of the PA ramp time in FSK mode */
		LORA_OCP			= 0x13,	/**< Over current protection control */
		LORA_LNA			= 0x18,	/**< LNA settings */
		LORA_RxBW			= 0x19,	/**< Channel filter BW control */
		LORA_AFCBW			= 0x1A,	/**< Channel filter BW control during the AFC routine */
		LORA_OOKPeak		= 0x1B,	/**< OOK demodulator selection and control in peak mode */
		LORA_OOKAvg			= 0x1C,	/**< Average threshold control of the OOK demodulator */
		LORA_OOKFix			= 0x1D,	/**< Fixed threshold control of the OOK demodulator */
		LORA_AFCFEI			= 0x1E,	/**< AFC and FEI control and status */
		LORA_AFCMSB			= 0x1F,	/**< MSB of the frequency correction of the AFC */
		LORA_AFCLSB			= 0x20,	/**< LSB of the frequency correction of the AFC */
		LORA_FEIMSB			= 0x21,	/**< MSB of the calculated frequency error */
		LORA_FEILSB			= 0x22,	/**< LSB of the calculated frequency error */
		LORA_RSSIConfig		= 0x23,	/**< RSSI related settings */
		LORA_RSSIValue		= 0x24,	/**< RSSI value in dBm */
		LORA_DIOMapping1	= 0x25,	/**< Mapping pins DIO0 to DIO3 */
		LORA_DIOMapping2	= 0x26,	/**< Mapping pins DIO4 to DIO5, Clock out frequency */
		LORA_IRQFlags1		= 0x27,	/**< Status register, PLL Lock state, Timeout, RSSI > threshold */
		LORA_IRQFlags2		= 0x28,	/**< Status register, FIFO handling flags */
		LORA_RSSIThresh		= 0x29,	/**< RSSI threshold control */
		LORA_RxTimeout1		= 0x2A,	/**< Timeout duration between Rx request and RSSI detection */
		LORA_RxTimeout2		= 0x2B,	/**< Timeout duration between RSSI detection and PayloadReady */
		LORA_PreambleMSB	= 0x2C,	/**< Preamble length Most Significant Bits */
		LORA_PreambleLSB	= 0x2D,	/**< Preamble length Least Significant Bits */
		LORA_SyncConfig		= 0x2E,	/**< Sync word recognition control */
		LORA_SyncValue1		= 0x2F,	/**< Sync word byte 1 */
		LORA_SyncValue2		= 0x30,	/**< Sync word byte 2 */
		LORA_SyncValue3		= 0x31,	/**< Sync word byte 3 */
		LORA_SyncValue4		= 0x32,	/**< Sync word byte 4 */
		LORA_SyncValue5		= 0x33,	/**< Sync word byte 5 */
		LORA_SyncValue6		= 0x34,	/**< Sync word byte 6 */
		LORA_SyncValue7		= 0x35,	/**< Sync word byte 7 */
		LORA_SyncValue8		= 0x36,	/**< Sync word byte 8 */
		LORA_RacketConfig1	= 0x37,	/**< Packet mode settings */
		LORA_PayloadLength	= 0x38,	/**< Payload length setting */
		LORA_NodeAdrs		= 0x39,	/**< Node address */
		LORA_BroadcastAdrs	= 0x3A,	/**< Broadcast address */
		LORA_AutoModes		= 0x3B,	/**< Auto modes settings */
		LORA_FifoThresh		= 0x3C,	/**< Fifo threshold, Tx start condition */
		LORA_PacketConfig2	= 0x3D,	/**< Packet mode settings */
		LORA_AESKey01		= 0x3E,	/**< Cypher key byte 01 */
		LORA_AESKey02		= 0x3F,	/**< Cypher key byte 02 */
		LORA_AESKey03		= 0x40,	/**< Cypher key byte 03 */
		LORA_AESKey04		= 0x41,	/**< Cypher key byte 04 */
		LORA_AESKey05		= 0x42,	/**< Cypher key byte 05 */
		LORA_AESKey06		= 0x43,	/**< Cypher key byte 06 */
		LORA_AESKey07		= 0x44,	/**< Cypher key byte 07 */
		LORA_AESKey08		= 0x45,	/**< Cypher key byte 08 */
		LORA_AESKey09		= 0x46,	/**< Cypher key byte 09 */
		LORA_AESKey10		= 0x47,	/**< Cypher key byte 10 */
		LORA_AESKey11		= 0x48,	/**< Cypher key byte 11 */
		LORA_AESKey12		= 0x49,	/**< Cypher key byte 12 */
		LORA_AESKey13		= 0x4A,	/**< Cypher key byte 13 */
		LORA_AESKey14		= 0x4B,	/**< Cypher key byte 14 */
		LORA_AESKey15		= 0x4C,	/**< Cypher key byte 15 */
		LORA_AESKey16		= 0x4D,	/**< Cypher key byte 16 */
		LORA_Temp1			= 0x4E,	/**< Temperature sensor control */
		LORA_Temp2			= 0x4F,	/**< Temperature readout */
		LORA_TestLNA		= 0x58,	/**< Sensitivity boost */
		LORA_TestPA1		= 0x5A,	/**< High power PA settings 1 */
		LORA_TestPA2		= 0x5A,	/**< High power PA settings 2 */
		LORA_TestDagc		= 0x6F,	/**< Fading margin improvement */
		LORA_TestAFC		= 0x71,	/**< AFC offset for low modulateion index AFC */
	};

/***** Constants	*****/


/***** Globals		*****/


/***** Prototypes 	*****/


/***** Functions	*****/


#endif

