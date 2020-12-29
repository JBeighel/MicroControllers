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
	#include <mariadb/mysql.h>

/*****	Defines		*****/
	

/*****	Definitions	*****/
	typedef enum eSQLDataType_t {
		SQLInteger,
		SQLDecimal,
		SQLString,
	} eSQLDataType_t;
	
	typedef struct sSQLRecord_t {
		uint32_t nRowNumber;
		uint32_t nNumValues;
		sSQLValue_t *aValues;
	} sSQLRecord_t;

	typedef struct sSQLValue_t {
		char *FieldName;
		eSQLDataType_t eType;
		int32_t nInteger;
		float fDecimal;
		char *String;
	} sSQLValue_t;
	
	typedef struct sSQLConn_t {
		MYSQL dbConn;
		MYSQL_RES *RecordSet;
	} sSQLConn_t;

/*****	Constants	*****/


/*****	Globals		*****/


/*****	Prototypes 	*****/


/*****	Functions	*****/


#endif

