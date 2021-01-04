/**	@defgroup	mariadb
	@brief		
	@details	v
	#Description
	
	#File Information
		File:	MariaDB.h
		Author:	J. Beighel
		Date:	12-23-2020
*/

#ifndef __MARIADB_H
	#define __MARIADB_H

/*****	Includes	*****/
	#include <stdint.h>
	#include <stddef.h>
	#include <stdlib.h>

	#include <mariadb/mysql.h>
	
	#include "CommonUtils.h"

/*****	Defines		*****/
	

/*****	Definitions	*****/
	typedef struct sSQLRecordSet_t sSQLRecordSet_t;

	typedef enum eSQLReturn_t {
		SQLWarn_NoRecord	= 1,	/**< Indicates that no record was available */
		SQLWarn_Unknown		= 1,	/**< An unknown but recoverable error happened during the operation */
		SQL_Success			= 0,	/**< The operation completed successfully */
		SQLFail_Unknown		= -1,	/**< An unknown and unrecoverable error happened during the operation */
		SQLFail_NotImplem	= -2,	/**< Function not implemented */
		SQLFail_RSNotFound	= -3,	/**< Recordset requeste could not be found */
		SQLFail_Memory		= -4,	/**< Memory allocation failed */
	} eSQLReturn_t;

	typedef enum eSQLDataType_t {
		SQLUnknown,
		SQLInteger,
		SQLDecimal,
		SQLString,
	} eSQLDataType_t;
	
	typedef struct sSQLValue_t {
		const char *FieldName;
		eSQLDataType_t eType;
		int32_t nInteger;
		float nDecimal;
		const char *String;
	} sSQLValue_t;
	
	typedef struct sSQLRecord_t {
		uint32_t nRowNumber;
		uint32_t nNumValues;
		sSQLValue_t *aValues;
	} sSQLRecord_t;
	
	typedef struct sSQLRecordSet_t {
		MYSQL_RES *dbRecordSet;
		sSQLRecord_t Record;
		uint32_t nIndex;
		sSQLRecordSet_t *pRSNext;
	} sSQLRecordSet_t;
	
	typedef struct sSQLConn_t {
		MYSQL dbConn;
		
		sSQLRecordSet_t *pRSRoot;
		uint32_t nRSCnt;
	} sSQLConn_t;
	
	typedef sSQLRecordSet_t *hRecSethandle_t;

/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	eSQLReturn_t SQLInitialize(sSQLConn_t *pSQLConn, const char *Server, const char *Username, const char *Password);
	
	eSQLReturn_t SQLShutdown(sSQLConn_t *pSQLConn);
	
	eSQLReturn_t SQLQuerySelect(sSQLConn_t *pSQLConn, const char *Query, hRecSethandle_t *hRecSet);
	
	eSQLReturn_t SQLReleaseRecordSet(sSQLConn_t *psConn, hRecSethandle_t hRecSet);
	
	eSQLReturn_t SQLQueryModify(sSQLConn_t *pSQLConn, const char *Query, uint32_t *pnRecsAffected);
	
	eSQLReturn_t SQLRecordSetNext(sSQLConn_t *pSQLConn, hRecSethandle_t hRecSet, sSQLRecord_t *pRecord);
	

/*****	Functions	*****/


#endif

