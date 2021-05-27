/**	File:	???.c
	Author:	J. Beighel
	Date:	MM-DD-YYYY
*/

/*****	Includes	*****/
	#include "Database.h"

/*****	Defines		*****/


/*****	Definitions	*****/
	#define DB_NUMANALOGS		5
	#define DB_NUMDIGITALS		5
	#define DB_NUMSETTINGS		(sizeof(cDefSettings) / sizeof(sDBSetting_t))
	#define DB_NUMPROPERTIES	(sizeof(cDefProps) / sizeof(sDBProperty_t))
	
	#define DB_NUMOBJECTS		(DB_NUMANALOGS + DB_NUMDIGITALS + DB_NUMSETTINGS + DB_NUMPROPERTIES)

/*****	Constants	*****/
	//Similar constant to hold default analog thresholds/units/ranges and digital counter/debounce settings
	const sDBEvents_t cDefDigitalEvents = {
			.pfDBValueChange = NULL,
	};
	
	const sDBEvents_t cDefAnalogEvents = {
			.pfDBValueChange = NULL,
	};

	const sDBEvents_t cDefNoEvents = {
			.pfDBValueChange = NULL,
	};

	const sDBAnalogPoint_t cDBDefAnalog = {
		.nCurrValue = 0,
		.nPriorValue = 0,
		.nForcedValue = 0,
		.eCurrFlags = DBFlag_NoReading | DBFlag_MeasVal | DBFlag_Input,
		.ePriorFlags = DBFlag_MaskNone,
		.nChangeThresh = UINT32_MAXVALUE,
		.nHighThresh = UINT32_MAXVALUE,
		.nLowThresh = 0,
		.eUnits = Unitless,
		.nRangeMin = 0,
		.nRangeMax = UINT32_MAXVALUE,
	};
	
	const sDBDigitalPoint_t cDBDefDigital = {
		.bCurrValue = false,
		.bPriorValue = false,
		.bForcedValue = false,
		.eCurrFlags = DBFlag_NoReading | DBFlag_MeasVal | DBFlag_Input,
		.ePriorFlags = DBFlag_MaskNone,
		.tDebounceTicks = 10,
		.eCounterType = DBCntr_None,
		.tLastTrue = 0,
		.tLastFalse = 0,
		.tLastToggle = 0,
		.tDebounceStart = 0,
		.tCounterValue = 0,
	};

	//Probably should be set in some other header, to allow customization by the application
	const sDBSetting_t cDefSettings[] = {
		{ .ID.nDBIdx = 0, .ID.eType = DB_DeviceSetting, .ID.nTypeIdx = 0, .ID.strName = "Setting", .uValue.Int = 0, .bIsInt = true, .nDefault = 0, .nRangeMin = -100, .nRangeMax = 100, .nFilterMask = UINT32_MAXVALUE, },
	};
	
	const sDBProperty_t cDefProps[] = {
		{ .ID.nDBIdx = 0, .ID.eType = DB_DeviceProperty, .ID.nTypeIdx = 0, .ID.strName = "Model", .strValue = "Test123", },
	};

/*****	Globals		*****/
	/**	@brief		Array holding all database values
	 *	@details	The index of the array must be the database index of the
	 *		database value.
	 *
	 *		This array should be written to non-volatile memory as arranged in
	 *		this array.  It can be loaded in a similar fashion, however some
	 *		checking should be done to ensure that extra or missing points are
	 *		discovered and correctly indexed.
	 *
	 *		The memory for this array should be allocated upon database
	 *		initialization.  It should only be freed when the application shuts
	 *		down.
	 *	@ingroup	database
	 */
	uDBValue_t gaValues[DB_NUMOBJECTS];

	/**	@brief		Array holding all database values
	 *	@details	The index of the array must be the database index of the
	 *		database value.
	 *
	 *		This array should not be written to non-volatile memory.  It is to
	 *		be loaded from file as the correct order of all database values.
	 *
	 *		Setting validation values should be overwritten when loaded to
	 *		ensure changes to setting usage are accounted for.
	 *
	 *		The memory for this array should be allocated upon database
	 *		initialization.  It should only be freed when the application shuts
	 *		down.
	 *	@ingroup	database
	 */
	sDBDataObject_t gaObjects[DB_NUMOBJECTS];

/*****	Prototypes 	*****/


/*****	Functions	*****/
eReturn_t DBInitialize() {
	uint32_t nCtr, nStartOffset;

	//Prepare the database arrays, Analog points first
	nStartOffset = 0;
	for (nCtr = nStartOffset; nCtr < nStartOffset + DB_NUMANALOGS; nCtr++) {
		//Initialize database object identifiers
		gaObjects[nCtr].nDBIdx = nCtr;
		gaObjects[nCtr].eType = DB_AnalogPoint;
		gaObjects[nCtr].nTypeIdx = nCtr - nStartOffset;
		gaObjects[nCtr].pValue = &(gaValues[nCtr]);

		//Copy in default event handlers
		memcpy(&(gaObjects[nCtr].Events), &cDefAnalogEvents, sizeof(sDBEvents_t));

		//Copy in default analog point values
		memcpy(&(gaValues[nCtr].Analog), &cDBDefAnalog, sizeof(sDBAnalogPoint_t));

		//Give data value the correct identifiers
		gaValues[nCtr].Analog.ID.nDBIdx = gaObjects[nCtr].nDBIdx;
		gaValues[nCtr].Analog.ID.eType = gaObjects[nCtr].eType;
		gaValues[nCtr].Analog.ID.nTypeIdx = gaObjects[nCtr].nTypeIdx;
	}

	//Prepare Digital points
	nStartOffset = DB_NUMANALOGS;
	for (nCtr = nStartOffset; nCtr < nStartOffset + DB_NUMDIGITALS; nCtr++) {
		//Initialize database object identifiers
		gaObjects[nCtr].nDBIdx = nCtr;
		gaObjects[nCtr].eType = DB_DigitalPoint;
		gaObjects[nCtr].nTypeIdx = nCtr - nStartOffset;
		gaObjects[nCtr].pValue = &(gaValues[nCtr]);

		//Copy in default event handlers
		memcpy(&(gaObjects[nCtr].Events), &cDefDigitalEvents, sizeof(sDBEvents_t));

		//Copy in default digital point values
		memcpy(&(gaValues[nCtr].Digital), &cDBDefDigital, sizeof(sDBDigitalPoint_t));

		//Give data value the correct identifiers
		gaValues[nCtr].Digital.ID.nDBIdx = gaObjects[nCtr].nDBIdx;
		gaValues[nCtr].Digital.ID.eType = gaObjects[nCtr].eType;
		gaValues[nCtr].Digital.ID.nTypeIdx = gaObjects[nCtr].nTypeIdx;
	}

	//Prepare Settings (Applies all default settings)
	nStartOffset = DB_NUMANALOGS + DB_NUMDIGITALS;
	for (nCtr = nStartOffset; nCtr < nStartOffset + DB_NUMPROPERTIES; nCtr++) {
		//Initialize database object identifiers
		gaObjects[nCtr].nDBIdx = nStartOffset;
		gaObjects[nCtr].eType = DB_DeviceSetting;
		gaObjects[nCtr].nTypeIdx = nCtr - nStartOffset;
		gaObjects[nCtr].pValue = &(gaValues[nCtr]);

		//Copy in default event handlers
		memcpy(&(gaObjects[nCtr].Events), &cDefNoEvents, sizeof(sDBEvents_t));

		//Copy in default setting values
		memcpy(&(gaValues[nCtr].Setting), &(cDefSettings[nCtr - nStartOffset]), sizeof(sDBSetting_t));

		//Give data value the correct identifiers
		gaValues[nCtr].Setting.ID.nDBIdx = gaObjects[nCtr].nDBIdx;
		gaValues[nCtr].Setting.ID.eType = gaObjects[nCtr].eType;
		gaValues[nCtr].Setting.ID.nTypeIdx = gaObjects[nCtr].nTypeIdx;
	}

	//Prepare properties
	nStartOffset = DB_NUMANALOGS + DB_NUMDIGITALS + DB_NUMPROPERTIES;
	for (nCtr = nStartOffset; nCtr < nStartOffset + DB_NUMSETTINGS; nCtr++) {
		//Initialize database object identifiers
		gaObjects[nCtr].nDBIdx = nCtr;
		gaObjects[nCtr].eType = DB_DeviceProperty;
		gaObjects[nCtr].nTypeIdx = nCtr - nStartOffset;
		gaObjects[nCtr].pValue = &(gaValues[nCtr]);

		//Copy in default event handlers
		memcpy(&(gaObjects[nCtr].Events), &cDefNoEvents, sizeof(sDBEvents_t));

		//Copy in default property values
		memcpy(&(gaValues[nCtr].Property), &(cDefProps[nCtr - nStartOffset]), sizeof(sDBProperty_t));

		//Give data value the correct identifiers
		gaValues[nCtr].Setting.ID.nDBIdx = gaObjects[nCtr].nDBIdx;
		gaValues[nCtr].Setting.ID.eType = gaObjects[nCtr].eType;
		gaValues[nCtr].Setting.ID.nTypeIdx = gaObjects[nCtr].nTypeIdx;
	}

	return Success;
}
