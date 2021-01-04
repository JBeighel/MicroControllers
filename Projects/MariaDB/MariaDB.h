/**	@defgroup	mariadb
	@brief		
	@details	v
	#Description
	
	#File Information
		File:	MariaDB.h
		Author:	J. Beighel
		Date:	2020-01-04
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

	/**	@brief		Enumeration of all return codes the SQL functions have
		@ingroup	mariadb
	*/
	typedef enum eSQLReturn_t {
		SQLWarn_NoRecord	= 1,	/**< Indicates that no record was available */
		SQLWarn_Unknown		= 1,	/**< An unknown but recoverable error happened during the operation */
		SQL_Success			= 0,	/**< The operation completed successfully */
		SQLFail_Unknown		= -1,	/**< An unknown and unrecoverable error happened during the operation */
		SQLFail_NotImplem	= -2,	/**< Function not implemented */
		SQLFail_RSNotFound	= -3,	/**< Recordset requeste could not be found */
		SQLFail_Memory		= -4,	/**< Memory allocation failed */
	} eSQLReturn_t;

	/**	@brief		Enumeration of all interpretted data types a record can have
		@ingroup	mariadb
	*/
	typedef enum eSQLDataType_t {
		SQLUnknown,		/**< Data type is unknown or can not be interpretted */
		SQLInteger,		/**< Data is a signed integer value */
		SQLDecimal,		/**< Data is a floating point number */
		SQLString,		/**< Data is a text string */
	} eSQLDataType_t;
	
	/**	@brief		Value of a single field of a database record
		@details	Values from database records will be interpretted into a set
			of simple data types.  Each true database type will be distilled down
			into one of the types represented in this structure.
		@ingroup	mariadb
	*/
	typedef struct sSQLValue_t {
		const char *FieldName;	/**< Name of this field from the query */
		eSQLDataType_t eType;	/**< Interpretted data type for this field */
		int32_t nInteger;		/**< Data when interpretted as a signed integer */
		float nDecimal;			/**< Data when interpretted as a floating point number */
		const char *String;		/**< Data represented as a string value */
	} sSQLValue_t;
	
	/**	@brief		A single record returned from a select query
		@ingroup	mariadb
	*/
	typedef struct sSQLRecord_t {
		uint32_t nRowNumber;
		uint32_t nNumValues;
		sSQLValue_t *aValues;
	} sSQLRecord_t;

	/**	@brief		Structure containing all information related to a record set
		@details	Select queries will return one or more records from the database.  
			Regardless of the data size this information is presented to the application
			as a record set.
		@ingroup	mariadb
	*/
	typedef struct sSQLRecordSet_t {
		MYSQL_RES *dbRecordSet;
		sSQLRecord_t Record;
		uint32_t nIndex;
		sSQLRecordSet_t *pRSNext;
	} sSQLRecordSet_t;
	
	/**	@brief		Structure holding all information on a database connection
		@ingroup	mariadb
	*/
	typedef struct sSQLConn_t {
		MYSQL dbConn;
		
		sSQLRecordSet_t *pRSRoot;
		uint32_t nRSCnt;
	} sSQLConn_t;
	
	/**	@brief		Datatype for exchanging recordset information with this module
		@ingroup	mariadb
	*/
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

