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
		uint32_t nRowNumber;	/**< Row number of the recordset that this record contains */
		uint32_t nNumValues;	/**< Number of values, or fields, in this record */	
		sSQLValue_t *aValues;	/**< Array of values in this record */
	} sSQLRecord_t;

	
	
	/**	@brief		Structure holding all information on a database connection
		@ingroup	mariadb
	*/
	typedef struct sSQLConn_t {
		MYSQL dbConn;			/**< Database connector object */
		
		sSQLRecordSet_t *pRSRoot;	/**< Root of the recordset linked list */
		uint32_t nRSCnt;		/**< Count of open recordsets through this connection */
	} sSQLConn_t;
	
	/**	@brief		Datatype for exchanging recordset information with this module
		@ingroup	mariadb
	*/
	typedef sSQLRecordSet_t *hRecSethandle_t;

/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/
	/**	@brief		Used to initialize the database connection
		@param		pSQLConn		Pointer to SQL object to initialize and connect with
		@param		Server			Database server address to connect to 
		@param		Username		Username to authenticate as with the server
		@param		Password		Password to authenticate with
		@return		SQL_Success on success or a code indicating the failure type
		@ingroup	mariadb
	*/
	eSQLReturn_t SQLInitialize(sSQLConn_t *pSQLConn, const char *Server, const char *Username, const char *Password);
	
	/**	@brief		Used to close the database connection and release resources
		@param		pSQLConn		Pointer to SQL object to work with with
		@return		SQL_Success on success or a code indicating the failure type
		@ingroup	mariadb
	*/
	eSQLReturn_t SQLShutdown(sSQLConn_t *pSQLConn);
	
	/**	@brief		Submit a select query and retrieve any records returned from it
		@param		pSQLConn		Pointer to SQL connection object to use
		@param		Query			Text of the select query
		@param		hRecSet			Returns the handle to the recordset object holding the returned data
		@return		SQL_Success on success or a code indicating the failure type
		@ingroup	mariadb
	*/
	eSQLReturn_t SQLQuerySelect(sSQLConn_t *pSQLConn, const char *Query, hRecSethandle_t *hRecSet);
	
	/**	@brief		Release all resources used in storing a database recordset
		@param		pSQLConn		Pointer to SQL connection object to use
		@param		hRecSet			Recordset object to release
		@return		SQL_Success on success or a code indicating the failure type
		@ingroup	mariadb
	*/
	eSQLReturn_t SQLReleaseRecordSet(sSQLConn_t *psConn, hRecSethandle_t hRecSet);
	
	/**	@brief		Submit a query to modify an object in the database
		@param		pSQLConn		Pointer to SQL connection object to use
		@param		Query			Text of the SQL query
		@param		pnRecsAffected	Returns the number of rows affected by the query
		@return		SQL_Success on success or a code indicating the failure type
		@ingroup	mariadb
	*/
	eSQLReturn_t SQLQueryModify(sSQLConn_t *pSQLConn, const char *Query, uint32_t *pnRecsAffected);
	
	/**	@brief		Retrieves the next record in the recordset object
		@param		pSQLConn		Pointer to SQL connection object to use
		@param		hRecSet			Handle to the recordset object to retrieve from
		@param		pRecord			Returns a structure containing all the data in the record
		@return		SQL_Success on success or a code indicating the failure type
		@ingroup	mariadb
	*/
	eSQLReturn_t SQLRecordSetNext(sSQLConn_t *pSQLConn, hRecSethandle_t hRecSet, sSQLRecord_t *pRecord);
	

/*****	Functions	*****/


#endif

