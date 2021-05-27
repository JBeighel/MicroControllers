/**	@defgroup	database
	@brief		
	@details	v0.1
	#Description
	
	#File Information
		File:	???.c
		Author:	J. Beighel
		Date:	MM-DD-YYYY
*/

#ifndef __DATABASE_H
	#define __DATABASE_H

/*****	Includes	*****/
	#include <string.h>

	#include "CommonUtils.h"

/*****	Defines		*****/
	/**	@brief		Number of characters (including terminator) reserved for value names
	 *	@ingroup	database
	 */
	#define DB_NAMELENGTH		50

	/**	@brief		Number of characters (including terminator) reserved for property values
	 *	@ingroup	database
	 */
	#define	DB_PROPERTYLENGTH	50

/*****	Definitions	*****/
	/**	@brief		Type used to store analog measurement values
	 *	@details	This is intended to set the maximum precision that analog
	 *		measurements can be stored at.
	 *	@ingroup	database
	 */
	typedef float DBAnalogVal_t;

	typedef enum eDBUnits_t {
		Unitless,
		Volts,
		Amps,
		Watts,
		Vars,
		VA,
		DegreesCelsius,
		DegreesFarenheit,
		DegreesAngle,
		Radians,
		Percentage,
	} eDBUnits_t;

	/**	@brief		Enumeration of all methods of monitoring a digital input
	 *	@details	The counter value will monitor an input to either count the
	 *		number of state transitions that occur or to record the time
	 *		between specified state transitions.
	 *	@ingroup	database
	 */
	typedef enum eDBCounterType_t {
		DBCntr_None,			/**< No input timing or counts are being made */

		DBCntr_TmrTrueToFalse,	/**< Ticks from positive edge to negative edge (True pulse duration) */
		DBCntr_TmrFalseToTrue,	/**< Ticks from negative edge to positive edge (False pulse duration) */
		DBCntr_TmrTrueToTrue,	/**< Ticks from a positive edge to the next positive edge (True to true duration) */
		DBCntr_TmrFalseToFalse,	/**< Ticks from a negative edge to the next negative edge (False to false duration) */

		DBCntr_CntPosEdges,		/**< Number of positive edges detected */
		DBCntr_CntNegEdges,		/**< Number of negative edges detected */
		DBCntr_CntToggles,		/**< Number of input changes detected */
	} eDBCounterType_t;

	typedef enum eDBDataType_t {
		DB_NoType,			/**< Unknown data type */
		DB_AnalogPoint,		/**< Measurement point that records a range of values */
		DB_DigitalPoint,	/**< Measurement point that records only true and false states */
		DB_DeviceProperty,	/**< Textual property reported for user convenience */
		DB_DeviceSetting,	/**< Configuration item for the device */
	} eDBDataType_t;

	typedef enum eDBFlags_t {
		DBFlag_MaskNone		= 0x0,			/**< Mask for no flags */
		DBFlag_MaskAll		= 0xFFFFFFFF,	/**< Mask for all flags (forces type to 4 byte size) */

		DBFlag_NoReading	= 0x0002,		/**< No reading has been made since device powered on */
		DBFlag_LocalForce	= 0x0010,		/**< Value is forced by this device */
		DBFlag_RemoteForce	= 0x0008,		/**< Value is forced by reporting device (this device relays value) */
		DBFlag_MeasVal		= 0x0080,
		DBFlag_Online		= 0x0001,
		DBFlag_CommLost		= 0x0004,

		DBFlag_Input		= 0x0200,		/**< Value is treated as if it were from a measured input */
		DBFlag_ReadOnly		= 0x0400,		/**< Value can only be read from */
	} eDBFlags_t;

	/**	@brief		Structure containing all identifying elements of a data value
	 *	@details	Each value will have two unique identifies.
	 *		The first is its database index.  This value will be its index in
	 *		the array of values containing the entire database.
	 *		The second is the combination of data type and type index.  Each
	 *		data type will have its own subset of values.  In this subset each
	 *		value will have a unique index.
	 *
	 *		The textual name is for user convenience.  It is not intended to
	 *		identifying the point programmatically.
	 *	@ingroup	database
	 */
	typedef struct sDBDataID_t {
		uint32_t nDBIdx;				/**< Index of this value within the entire database */
		uint32_t nTypeIdx;				/**< Index of this value within in type */
		eDBDataType_t eType;			/**< Data type contained in this value */
		char strName[DB_NAMELENGTH];	/**< Textual name of the value */
	} sDBDataID_t;

	typedef struct sDBAnalogPoint_t {
		sDBDataID_t ID;

		//Values
		DBAnalogVal_t nCurrValue;		/**< Current measurement reported */
		DBAnalogVal_t nPriorValue;		/**< Prior measurement reported */
		DBAnalogVal_t nForcedValue;		/**< If locally forced, this is the value reported */
		eDBFlags_t eCurrFlags;			/**< Current flags reported */
		eDBFlags_t ePriorFlags;			/**< Prior flags reported */

		//Settings
		DBAnalogVal_t nChangeThresh;	/**< Threshold for adjacent readings change */
		DBAnalogVal_t nHighThresh;		/**< Threshold for exceptional high reading */
		DBAnalogVal_t nLowThresh;		/**< Threshold for exceptional low reading */

		eDBUnits_t eUnits;				/**< Units this measurement is reported in */
		DBAnalogVal_t nRangeMin;		/**< Minimum reading expected by this measurement */
		DBAnalogVal_t nRangeMax;		/**< Maximum reading expected by this measurement */
	} sDBAnalogPoint_t;

	typedef struct sDBDigitalPoint_t {
		sDBDataID_t ID;

		//Values
		bool bCurrValue;				/**< Current measurement reported */
		bool bPriorValue;				/**< Prior measurement reported */
		bool bForcedValue;				/**< If locally forced, this is the value reported */
		eDBFlags_t eCurrFlags;			/**< Current flags reported */
		eDBFlags_t ePriorFlags;			/**< Prior flags reported */
		uint32_t tCounterValue;			/**< Current counter value reported */

		//Settings
		uint32_t tLastTrue;				/**< Tick when last change to true occurred */
		uint32_t tLastFalse;			/**< Tick when last change to false occurred */
		uint32_t tLastToggle;			/**< Tick when last value change occurred */
		uint32_t tDebounceStart;		/**< Tick when debounce filter began */

		uint32_t tDebounceTicks;		/**< Number of ticks a value must persist to be accepted */
		eDBCounterType_t eCounterType;	/**< Current counter/timer monitoring this point */
	} sDBDigitalPoint_t;

	typedef struct sDBSetting_t {
		sDBDataID_t ID;

		//Values
		union {
			int32_t Int;
			float Float;
		} uValue;

		//Validation Values
		bool bIsInt;			/**< True if this setting uses an integer value, false for float */
		float nDefault;			/**< Default setting value */
		float nRangeMin;		/**< Minimum valid setting value that */
		float nRangeMax;		/**< Maximum valid setting value that */
		int32_t nFilterMask;	/**< Filter of valid bits that can be set, for flag values */
	} sDBSetting_t;

	typedef struct sDBProperty_t {
		sDBDataID_t ID;

		//Values
		char strValue[DB_PROPERTYLENGTH];
	} sDBProperty_t;

	//Property should be largest to allow maximum space for user defined value names
	STATIC_ASSERT(DBPropertySize_vsAnalog, sizeof(sDBProperty_t) >= sizeof(sDBAnalogPoint_t));
	STATIC_ASSERT(DBPropertySize_vsDigital, sizeof(sDBProperty_t) >= sizeof(sDBDigitalPoint_t));
	STATIC_ASSERT(DBPropertySize_vsSetting, sizeof(sDBProperty_t) >= sizeof(sDBSetting_t));

	typedef union uDBValue_t {
		sDBAnalogPoint_t Analog;
		sDBDigitalPoint_t Digital;
		sDBSetting_t Setting;
		sDBProperty_t Property;
	} uDBValue_t;

	/**	@brief		Structure containing pointers to all event handlers
	 *	@ingroup	database
	 */
	typedef struct sDBEvents_t {
		void (*pfDBValueChange)(eDBDataType_t eType, uint32_t nTypeIdx);
	} sDBEvents_t;

	typedef struct sDBDataObject_t {
		uint32_t nDBIdx;
		uint32_t nTypeIdx;
		eDBDataType_t eType;

		uDBValue_t *pValue;
		sDBEvents_t Events;
	} sDBDataObject_t;

/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/


/*****	Functions	*****/
	eReturn_t DBInitialize();

#endif

