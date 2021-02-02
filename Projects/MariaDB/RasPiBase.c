/**	File:	RasPiBase.c
	Author:	J. Beighel
	Date:	12-17-2020
*/

/*****	Includes	*****/
	//Genereral use libraries
	#include "CommonUtils.h"
	#include "TimeGeneralInterface.h"
	#include "GPIOGeneralInterface.h"
	#include "I2CGeneralInterface.h"
	#include "SPIGeneralInterface.h"
	#include "UARTGeneralInterface.h"
	
	//Pin Support, implementation libraries
	#include "GPIO_RaspberryPi.h"
	#include "I2C_RaspberryPi.h"
	#include "SPI_RaspberryPi.h"
	#include "UART_RaspberryPi.h"
	
	//Driver libraries
	#include <mariadb/mysql.h>
	
	#include "MariaDB.h"

/*****	Defines		*****/


/*****	Definitions	*****/
	

/*****	Constants	*****/


/*****	Globals		*****/
	sTimeIface_t gTime;
	sGPIOIface_t gGPIO;
	sI2CIface_t gI2C;
	sSPIIface_t gSPI;
	sUARTIface_t gUART;
	
	MYSQL gDB;

/*****	Prototypes 	*****/
	int MySQLDirect();
	int MySQLLibrary();

/*****	Functions	*****/
eReturn_t BoardInit(void) {
	int eResult;
	
	//Init processor (pin support work)
	eResult = TIME_INIT(&gTime);
	if (eResult != Success) {
		return Fail_Unknown;
	}
	
	eResult = GPIO_INIT(&gGPIO, GPIO_HWINFO);
	if (eResult != Success) {
		return Fail_Unknown;
	}
	
	eResult = I2C_1_PORTINIT(&gI2C, true, 100000, I2C_1_HWINFO);
	if (eResult != GPIO_Success) {
		return Fail_Unknown;
	}
	
	eResult = SPI_1_PORTINIT(&gSPI, SPI_1_HWINFO, 5000000, SPI_MSBFirst, SPI_Mode0);
	if (eResult != SPI_Success) {
		return Fail_Unknown;
	}
	
	eResult = UART_PORTINITIALIZE(&gUART, 9600, UART_8None1, UART_1_HWINFO);
	if (eResult != UART_Success) {
		return Fail_Unknown;
	}
	
	//Init peripherals (board support work)
	
	
	return Success;
}

int main(int nArgCnt, char **aArgVals) {
	if (BoardInit() != Success) {
		printf("Board initialization failed.\r\n");
		return 1;
	}
	
	for (int nCtr = 0; nCtr < 10000; nCtr++) {
		if (MySQLLibrary() != 0) {
			printf("Library fail\r\n");
			return 1;
		}
	}
	
	return MySQLDirect();
}

int MySQLLibrary() {
	sSQLConn_t dbConn;
	eSQLReturn_t eResult;
	sSQLRecord_t dbRec;
	hRecSethandle_t hRecSet;
	int nCtr;
	
	eResult = SQLInitialize(&dbConn, "localhost", "jason", "ecvx5858");
	if (eResult != SQL_Success) {
		printf("SQL Lib init failed: %d\r\n", eResult);
		return 1;
	}
	
	eResult = SQLQuerySelect(&dbConn, "SELECT * FROM Test.User", &hRecSet);
	if (eResult != SQL_Success) {
		printf("SQL Lib select failed: %d\r\n", eResult);
		return 1;
	}
	
	eResult = SQLRecordSetNext(&dbConn, hRecSet, &dbRec);
	while (eResult != SQLWarn_NoRecord) {
		printf("Record %d: ", dbRec.nRowNumber);
		
		for (nCtr = 0; nCtr < dbRec.nNumValues; nCtr++) {
			printf("Val %d-%s, ", nCtr, dbRec.aValues[nCtr].String);
		}
		
		printf("\r\n");
		
		//Get the next record
		eResult = SQLRecordSetNext(&dbConn, hRecSet, &dbRec);
	}
	
	printf("End of results\r\n");
	SQLReleaseRecordSet(&dbConn, hRecSet);
	
	SQLShutdown(&dbConn);
	printf("SQL Lib work complete\r\n");
	return 0;
}

int MySQLDirect() {
	MYSQL *dbResult;
	MYSQL_RES *QueryVals;
	MYSQL_FIELD *Field;
	MYSQL_ROW RSRow;
	int nResult, nRowCtr, nFieldCtr, nFieldMax;
	
	
	dbResult = mysql_init(&gDB);
	if (dbResult == NULL) {
		printf("MySQL/MariaDB Init failed\r\n");
		return 1;
	}
	
	//No port, pipe, or flags (should go with defaults?)
	dbResult = mysql_real_connect(&gDB, "localhost", "jason", "ecvx5858", "Test", 0, NULL, 0);
	if (dbResult == NULL) {
		printf("MySQL/MariaDB Connect failed\r\n");
		return 1;
	}
	
	//create table User (ID INT NOT NULL AUTO_INCREMENT PRIMARY KEY, Name CHAR(30), NumValue INT);
	
	// int mysql_query(MYSQL * mysql, const char * query); returns 0 on success
	// MYSQL_RES * mysql_store_result(MYSQL * mysql); gets the result localeconv
	// my_ulonglong mysql_num_rows(MYSQL_RES * ); rows in result
	// unsigned int mysql_num_fields(MYSQL_RES * );  fields in result
	// MYSQL_FIELD * mysql_fetch_field(MYSQL_RES * result);  gets description of a field in record set (auto-advances to next field)
	// MYSQL_FIELD * mysql_fetch_field_direct(MYSQL_RES * res, unsigned int fieldnr); Pick a field to describe
	// MYSQL_ROW mysql_fetch_row(MYSQL_RES * result); get a row from the result
	// All values are returned in string form (even numbers), except that if a value in a row is NULL, it is represented in the MYSQL_ROW structure by a C NULL pointer.
	
	//After all records read
	// void mysql_free_result(MYSQL_RES * result);  clean up the stored results
	
	//nResult = mysql_query(&gDB, "SHOW TABLES;");
	nResult = mysql_query(&gDB, "SELECT * FROM Test.User");
	if (nResult != 0) {
		printf("Query error\r\n");
		return 1;
	}
	
	QueryVals = mysql_store_result(&gDB);
	//QueryVals = mysql_use_result(&gDB);
	if (QueryVals == NULL) {
		printf("Failed to store results\r\n");
		return 1;
	}
	
	//This field count is accurate and can indicate a working query
	printf("DB Query returned %d fields\r\n", mysql_field_count(&gDB));
	nFieldMax = mysql_field_count(&gDB);
	//Row and field counts are just wrong, doesn't seem usable at all
	printf("RS Query returned %d rows with %d fields\r\n", mysql_num_rows(QueryVals), mysql_num_fields(QueryVals));
	
	nRowCtr = 0;
	do {
		RSRow = mysql_fetch_row(QueryVals);
		if (RSRow == NULL) {
			printf("Unable to fetch a row\r\n");
		} else {
			printf("Row %d: ", nRowCtr);
			for (nFieldCtr = 0; nFieldCtr < nFieldMax; nFieldCtr++) {				
				printf("  F%d: '%s'", nFieldCtr, RSRow[nFieldCtr]);
				
				Field = mysql_fetch_field_direct(QueryVals, nFieldCtr);
				
				if (Field == NULL) {
					printf(" TypeErr");
				} else {
					switch (Field->type) {
						case MYSQL_TYPE_TINY:
						case MYSQL_TYPE_SHORT:
						case MYSQL_TYPE_INT24:
						case MYSQL_TYPE_LONG:
						case MYSQL_TYPE_LONGLONG:
							printf(" Integer");
							break;
						case MYSQL_TYPE_DECIMAL:
						case MYSQL_TYPE_DOUBLE:
						case MYSQL_TYPE_FLOAT:
							printf(" Float");
							break;
						case MYSQL_TYPE_STRING:
						case MYSQL_TYPE_VAR_STRING:
						case MYSQL_TYPE_BLOB:
							printf(" String");
							break;
						case MYSQL_TYPE_ENUM:
						case MYSQL_TYPE_SET:
							printf(" Set");
							break;
						case MYSQL_TYPE_DATE:
						case MYSQL_TYPE_DATETIME:
						case MYSQL_TYPE_TIME:
						case MYSQL_TYPE_TIMESTAMP:
						case MYSQL_TYPE_YEAR:
							printf(" Time");
							break;
						case MYSQL_TYPE_GEOMETRY:
							printf(" Geometry");
							break;
						case MYSQL_TYPE_BIT:
							printf(" Bit");
							break;
						case MYSQL_TYPE_NULL:
							printf(" Null");
							break;
						default:
							printf(" Unknown");
							break;
					}
				}
			}
			
			printf("\r\n");
		}
		
		nRowCtr += 1;
	} while (RSRow != NULL);
	
	mysql_free_result(QueryVals);
	mysql_close(&gDB);
	
	return 0;
}