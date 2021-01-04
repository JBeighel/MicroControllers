/**	File:	MariaDB.c
	Author:	J. Beighel
	Date:	12-23-2020
*/

/*****	Includes	*****/
	#include "MariaDB.h"

/*****	Defines		*****/


/*****	Definitions	*****/


/*****	Constants	*****/
	const char *gpcBlank = "";

/*****	Globals		*****/


/*****	Prototypes 	*****/
	eSQLReturn_t SQLFreeRecordSet(sSQLConn_t *pSQLConn, uint32_t nIndex);

/*****	Functions	*****/
eSQLReturn_t SQLInitialize(sSQLConn_t *pSQLConn, const char *Server, const char *Username, const char *Password) {
	MYSQL *dbResult;
	
	//Initialize module variables
	pSQLConn->pRSRoot = NULL;
	pSQLConn->nRSCnt = 0;
	
	//Attempt DB connection
	dbResult = mysql_init(&(pSQLConn->dbConn));
	if (dbResult == NULL) {
		return SQLFail_Unknown;
	}
	
	dbResult = mysql_real_connect(&(pSQLConn->dbConn), Server, Username, Password, NULL, 0, NULL, 0);
	if (dbResult == NULL) {
		return SQLFail_Unknown;
	}
	
	return SQL_Success;
}
	
eSQLReturn_t SQLShutdown(sSQLConn_t *pSQLConn) {
	while (pSQLConn->pRSRoot != NULL) {
		SQLFreeRecordSet(pSQLConn, pSQLConn->pRSRoot->nIndex);
	}
	
	//Close DB Connnection
	mysql_close(&(pSQLConn->dbConn));
	
	return SQL_Success;
}

eSQLReturn_t SQLQuerySelect(sSQLConn_t *pSQLConn, const char *Query, hRecSethandle_t *hRecSet) {
	sSQLRecordSet_t *pRSNew, *pRSLast;
	int nResult, nFieldCnt, nCtr;
	MYSQL_RES *dbRecSet;
	MYSQL_FIELD *dbField;
	
	//Lets submit the query and see if that works
	//The query, field count, and store resutls  functions are linked
	//inserting  another query will screw one of them up
	nResult = mysql_query(&(pSQLConn->dbConn), Query);
	if (nResult != 0) { //nResult holds an error code
		return SQLFail_Unknown;
	}
	
	nFieldCnt = mysql_field_count(&(pSQLConn->dbConn));
	
	dbRecSet = mysql_store_result(&(pSQLConn->dbConn));
	if (dbRecSet == NULL) {
		return SQLFail_Unknown;
	}
	
	//Need a new recordset item
	pRSNew = malloc(sizeof(sSQLRecordSet_t));
	if (pRSNew == NULL) {
		return SQLFail_Memory;
	}
	
	pRSNew->dbRecordSet = dbRecSet;
	pRSNew->pRSNext = NULL;
	
	pRSNew->Record.nRowNumber = (uint32_t)-1; //Invalid row number, since no record loaded
	pRSNew->Record.nNumValues = nFieldCnt;
	pRSNew->Record.aValues = malloc(sizeof(sSQLValue_t) * nFieldCnt);
	if (pRSNew->Record.aValues == NULL) {
		free(pRSNew);
		mysql_free_result(dbRecSet);
		
		return SQLFail_Memory;
	}
	
	//Add it to the end of the list
	if (pSQLConn->pRSRoot == NULL) { //First item in the list
		pSQLConn->pRSRoot = pRSNew;
		pRSNew->nIndex = 1;
	} else {
		//Find the end of the list
		pRSLast = pSQLConn->pRSRoot;
		while(pRSLast->pRSNext != NULL) {
			pRSLast = pRSLast->pRSNext;
		}
		
		pRSLast->pRSNext = pRSNew;
		pRSNew->nIndex = pRSLast->nIndex + 1;
	}
	
	//Look up the field names in the recordset
	for (nCtr = 0; nCtr < nFieldCnt; nCtr++) {
		//Default/blank field values
		pRSNew->Record.aValues[nCtr].nInteger = 0;
		pRSNew->Record.aValues[nCtr].nDecimal = 0;
		pRSNew->Record.aValues[nCtr].String = gpcBlank;
		
		//Look up field details
		dbField = mysql_fetch_field_direct(dbRecSet, nCtr);
		if (dbField == NULL) {
			pRSNew->Record.aValues[nCtr].FieldName = gpcBlank;
			pRSNew->Record.aValues[nCtr].eType = SQLUnknown;
		} else {
			pRSNew->Record.aValues[nCtr].FieldName = dbField->name;
			
			switch (dbField->type) {
				case MYSQL_TYPE_TINY:
				case MYSQL_TYPE_SHORT:
				case MYSQL_TYPE_INT24:
				case MYSQL_TYPE_LONG:
				case MYSQL_TYPE_LONGLONG:
					pRSNew->Record.aValues[nCtr].eType = SQLInteger;
					break;
				case MYSQL_TYPE_BIT: //Boolean, treat as integer for now
					pRSNew->Record.aValues[nCtr].eType = SQLInteger;
					break;
				case MYSQL_TYPE_DECIMAL:
				case MYSQL_TYPE_DOUBLE:
				case MYSQL_TYPE_FLOAT:
					pRSNew->Record.aValues[nCtr].eType = SQLDecimal;
					break;
				case MYSQL_TYPE_STRING:
				case MYSQL_TYPE_VAR_STRING:
				case MYSQL_TYPE_BLOB:
				case MYSQL_TYPE_ENUM:
				case MYSQL_TYPE_SET:
					pRSNew->Record.aValues[nCtr].eType = SQLString;
					break;
				case MYSQL_TYPE_DATE: //Date and time formats will be strings for now
				case MYSQL_TYPE_DATETIME:
				case MYSQL_TYPE_TIME:
				case MYSQL_TYPE_TIMESTAMP:
				case MYSQL_TYPE_YEAR:
					pRSNew->Record.aValues[nCtr].eType = SQLString;
					break;
				case MYSQL_TYPE_GEOMETRY: //Not sure how to handle geometry
				case MYSQL_TYPE_NULL: //Not sure what a null field is
				default:
					pRSNew->Record.aValues[nCtr].FieldName = gpcBlank;
					pRSNew->Record.aValues[nCtr].eType = SQLUnknown;
					break;
			}
		}
	}
	
	//Recordset is all set for use
	*hRecSet = pRSNew;
	
	return SQL_Success;
}

eSQLReturn_t SQLQueryModify(sSQLConn_t *pSQLConn, const char *Query, uint32_t *pnRecsAffected) {
	int32_t nResult;
	uint64_t nRecCnt;
	
	//Submit the query
	//The query, and record count functions are linked
	//inserting  another query will screw up the count
	nResult = mysql_query(&(pSQLConn->dbConn), Query);
	if (nResult != 0) { //nResult holds an error code
		return SQLFail_Unknown;
	}
	
	nRecCnt = mysql_affected_rows(&(pSQLConn->dbConn));
	
	if (nRecCnt < UINT32_MAXVALUE) {
		*pnRecsAffected = (uint32_t)nRecCnt;
	} else {
		*pnRecsAffected = UINT32_MAXVALUE;
	}
	
	return SQL_Success;
}

eSQLReturn_t SQLRecordSetNext(sSQLConn_t *pSQLConn, hRecSethandle_t hRecSet, sSQLRecord_t *pRecord) {
	MYSQL_ROW dbRow;
	int nCtr;
	
	dbRow = mysql_fetch_row(hRecSet->dbRecordSet);
	if (dbRow == NULL) { //No more records
		hRecSet->Record.nRowNumber = (uint32_t)-1; //Invalid row number, since no record loaded
		
		//Delete all values since this isn't a valid record
		for (nCtr = 0; nCtr < hRecSet->Record.nNumValues; nCtr++) {
			hRecSet->Record.aValues[nCtr].nInteger = 0;
			hRecSet->Record.aValues[nCtr].nDecimal = 0;
			hRecSet->Record.aValues[nCtr].String = NULL;
		}
		
		return SQLWarn_NoRecord;
	}
	
	//Update all of the record values with the new database
	hRecSet->Record.nRowNumber += 1;
	for (nCtr = 0; nCtr < hRecSet->Record.nNumValues; nCtr++) {
		//MySQL gives the string format only
		hRecSet->Record.aValues[nCtr].String = dbRow[nCtr];
		
		hRecSet->Record.aValues[nCtr].nDecimal = strtof(dbRow[nCtr], NULL);
		hRecSet->Record.aValues[nCtr].nInteger = (int32_t)hRecSet->Record.aValues[nCtr].nDecimal;
	}
	
	//Return the record values
	pRecord->nRowNumber = hRecSet->Record.nRowNumber;
	pRecord->nNumValues = hRecSet->Record.nNumValues;
	pRecord->aValues = hRecSet->Record.aValues;
	
	return SQL_Success;
}

eSQLReturn_t SQLReleaseRecordSet(sSQLConn_t *psConn, hRecSethandle_t hRecSet) {
	return SQLFreeRecordSet(psConn, hRecSet->nIndex);
}

eSQLReturn_t SQLFreeRecordSet(sSQLConn_t *pSQLConn, uint32_t nIndex) {
	sSQLRecordSet_t *pPrevRS, *pCurrRS;
	
	//Find the recordset to be freed
	pPrevRS = NULL;
	pCurrRS = pSQLConn->pRSRoot;
	while (pCurrRS != NULL) {
		if (pCurrRS->nIndex == nIndex) { //Found the record set, stop looking
			break;
		}
		
		//Get next recordset
		pPrevRS = pCurrRS;
		pCurrRS = pCurrRS->pRSNext;
	}
	
	if (pCurrRS == NULL) { //The index requested doesn't exist
		return SQLFail_RSNotFound;
	}
	
	//Clean up any database objects
	mysql_free_result(pCurrRS->dbRecordSet);
	
	//Update the linked list
	if (pPrevRS == NULL) { //Fix the root node
		pSQLConn->pRSRoot = pCurrRS->pRSNext;
	} else {
		pPrevRS->pRSNext = pCurrRS->pRSNext;
	}
	
	//Release allocated memory
	if (pCurrRS->Record.aValues != NULL) {
		free(pCurrRS->Record.aValues);
		pCurrRS->Record.aValues = NULL;
	}
	
	free(pCurrRS);
	
	return SQL_Success;
}