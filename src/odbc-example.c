/*
 *  ODBC Shell
 *  Copyright (C) 2011 Bindle Binaries <syzdek@bindlebinaries.com>.
 *
 *  @BINDLE_BINARIES_BSD_LICENSE_START@
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Bindle Binaries nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL BINDLE BINARIES BE LIABLE FOR
 *  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 *
 *  @BINDLE_BINARIES_BSD_LICENSE_END@
 */
/**
 *  @file src/odbc-example.c ODBC example client
 */
/*
 *  Simple Build:
 *     gcc -W -Wall -O2 -c odbc-example.c
 *     gcc -W -Wall -O2 -lodbc -o odbc-example odbc-example.o
 *
 *  GNU Libtool Build:
 *     libtool --mode=compile gcc -W -Wall -g -O2 -c odbc-example.c
 *     libtool --mode=link    gcc -W -Wall -g -O2 -o odbc-example odbc-example.lo
 *
 *  GNU Libtool Install:
 *     libtool --mode=install install -c odbc-example /usr/local/bin/odbc-example
 *
 *  GNU Libtool Clean:
 *     libtool --mode=clean rm -f odbc-example.lo odbc-example
 */

///////////////
//           //
//  Headers  //
//           //
///////////////
#ifdef PMARK
#pragma mark Headers
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include <sql.h>
#include <sqlext.h>
#include <sqlucode.h>
#include <iodbcext.h>


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#ifdef PMARK
#pragma mark -
#pragma mark Prototypes
#endif

// main statement
int main(int argc, char * argv[]);

// disconnects and frees handles
void my_free(HENV henv, HDBC hdbc, HSTMT hstmt);

// displays errors
void my_errors(const char * s, HENV henv, HDBC hdbc, HSTMT hstmt);

// displays result
int my_result(HENV henv, HDBC hdbc, HSTMT hstmt);


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#ifdef PMARK
#pragma mark -
#pragma mark Functions
#endif

/// main statement
/// @param[in]  argc  number of arguments passed to program
/// @param[in]  argv  array of arguments passed to program
int main(int argc, char * argv[])
{
   int             err;
   short           buflen;
   HENV            henv;
   HDBC            hdbc;
   HSTMT           hstmt;
   SQLTCHAR        dsn[512];
   SQLTCHAR        desc[512];
   SQLTCHAR        driverInfo[255];
   SQLSMALLINT     len1;
   SQLSMALLINT     len2;
   SQLUSMALLINT    funcs[SQL_API_ODBC3_ALL_FUNCTIONS_SIZE];
   unsigned char   strwild[2];

   henv  = NULL;
   hdbc  = NULL;
   hstmt = NULL;
   strncpy((char *)strwild,  "%", 2);

   if (argc > 3)
   {
      fprintf(stderr, "Usage: %s [DSN [SQL]]\n", argv[0]);
      return(1);
   };

   // initialize ODBC API
   if (SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_ENV, NULL, &henv) != SQL_SUCCESS)
   {
      my_errors("SQLAllocHandle", henv, hdbc, hstmt);
      return(1);
   };
   SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER) SQL_OV_ODBC3, SQL_IS_UINTEGER);

   // allocate connection handle
   if (SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc) != SQL_SUCCESS)
   {
      my_errors("SQLAllocHandle", henv, hdbc, hstmt);
      my_free(henv, hdbc, hstmt);
      return(1);
   };
   SQLSetConnectOption(hdbc, SQL_APPLICATION_NAME, (SQLULEN)argv[0]);

   // displays driver manager version
   err = SQLGetInfo(hdbc, SQL_DM_VER, driverInfo, sizeof(driverInfo), &len1);
   if (err == SQL_SUCCESS)
      printf("Driver Manager: %s\n", driverInfo);

   // displays list of ODBC data sources
   if (argc == 1)
   {
      printf("\n");
      err = SQLDataSources(henv, SQL_FETCH_FIRST, dsn,  sizeof(dsn), &len1,
                                                  desc, sizeof(desc), &len2);
      if (err != SQL_SUCCESS)
      {
         fprintf(stderr, "%s: unable to list ODBC sources.\n", argv[0]);
         return(0);
      };
      fprintf(stderr, "%-32s   %-40s\n", "DSN:", "Driver:");
      while(err == SQL_SUCCESS)
      {
         fprintf(stderr, "%-32s   %-40s\n", dsn, desc);
         err = SQLDataSources(henv, SQL_FETCH_NEXT, dsn,  sizeof(dsn), &len1,
                                                    desc, sizeof(desc), &len2);
      };
      my_free(henv, hdbc, hstmt);
      printf("\n");
      return(0);
   };

   // connects to database
   err = SQLDriverConnect(hdbc, 0, (SQLCHAR *) argv[1], SQL_NTS,
                          dsn, sizeof(dsn), &buflen, SQL_DRIVER_COMPLETE);
   if ((err != SQL_SUCCESS) && (err != SQL_SUCCESS_WITH_INFO))
   {
      my_errors("SQLDriverConnect", henv, hdbc, hstmt);
      my_free(henv, hdbc, hstmt);
      return(1);
   };

   // prints driver information
   err = SQLGetInfo(hdbc, SQL_DRIVER_VER, driverInfo, sizeof(driverInfo), &len1);
   if (err == SQL_SUCCESS)
   {
      printf("Driver: %s", driverInfo);
      err = SQLGetInfo(hdbc, SQL_DRIVER_NAME, driverInfo, sizeof(driverInfo), &len1);
      if (err == SQL_SUCCESS)
         printf(" (%s)", driverInfo);
      printf("\n\n");
   };

   // prints list of supported functions
   SQLGetFunctions(hdbc, SQL_API_ODBC3_ALL_FUNCTIONS, funcs);

   if (argc == 2)
      return(0);

   // allocates statement handle
   err = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
   if (err != SQL_SUCCESS)
   {
      my_errors("SQLAllocHandle", henv, hdbc, hstmt);
      my_free(henv, hdbc, hstmt);
      return(1);
   };

   // disconnect and reconnect to database
   SQLCloseCursor(hstmt);
   SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
   hstmt = NULL;
   SQLDisconnect(hdbc);
   err = SQLDriverConnect(hdbc, 0, dsn, SQL_NTS, dsn, sizeof(dsn), &buflen, SQL_DRIVER_NOPROMPT);
   if ((err != SQL_SUCCESS) && (err != SQL_SUCCESS_WITH_INFO))
   {
      my_errors("SQLDriverConnect", henv, hdbc, hstmt);
      my_free(henv, hdbc, hstmt);
      return(1);
   }; 
   err = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
   if (err != SQL_SUCCESS)
   {
      my_errors("SQLAllocHandle", henv, hdbc, hstmt);
      my_free(henv, hdbc, hstmt);
      return(1);
   };

   // queries the database for tables
   if (!(strcmp(argv[2], "tables")))
   {
      err = SQLTables(hstmt, NULL, 0, NULL, 0, NULL, 0, NULL, 0);
      if (err != SQL_SUCCESS)
      {
         my_errors("SQLTables", henv, hdbc, hstmt);
         my_free(henv, hdbc, hstmt);
         return(1);
      };
      if ((my_result(henv, hdbc, hstmt)))
      {
         my_free(henv, hdbc, hstmt);
         return(1);
      };
      my_free(henv, hdbc, hstmt);
      return(0);
   };

   // queries the database for qualifiers
   if (!(strcmp(argv[2], "qualifiers")))
   {
      err = SQLTables(hstmt, strwild, SQL_NTS, NULL, 0, NULL, 0, NULL, 0);
      if (err != SQL_SUCCESS)
      {
         my_errors("SQLTables", henv, hdbc, hstmt);
         my_free(henv, hdbc, hstmt);
         return(1);
      };
      if ((my_result(henv, hdbc, hstmt)))
      {
         my_free(henv, hdbc, hstmt);
         return(1);
      };
      my_free(henv, hdbc, hstmt);
      return(0);
   };

   // queries the database for owners
   if (!(strcmp(argv[2], "owners")))
   {
      err = SQLTables(hstmt, NULL, 0, strwild, SQL_NTS, NULL, 0, NULL, 0);
      if (err != SQL_SUCCESS)
      {
         my_errors("SQLTables", henv, hdbc, hstmt);
         my_free(henv, hdbc, hstmt);
         return(1);
      };
      if ((my_result(henv, hdbc, hstmt)))
      {
         my_free(henv, hdbc, hstmt);
         return(1);
      };
      my_free(henv, hdbc, hstmt);
      return(0);
	};

   // queries the database for types
   if (!(strcmp(argv[2], "types")))
   {
      err = SQLTables(hstmt, NULL, 0, NULL, 0, NULL, 0, strwild, SQL_NTS);
      if (err != SQL_SUCCESS)
      {
         my_errors("SQLTables", henv, hdbc, hstmt);
         my_free(henv, hdbc, hstmt);
         return(1);
      };
      if ((my_result(henv, hdbc, hstmt)))
      {
         my_free(henv, hdbc, hstmt);
         return(1);
      };
      my_free(henv, hdbc, hstmt);
      return(0);
	};

   // queries the database for datatypes
   if (!(strcmp(argv[2], "datatypes")))
   {
      err = SQLGetTypeInfo(hstmt, 0);
      if (err != SQL_SUCCESS)
      {
         my_errors("SQLGetTypeInfo", henv, hdbc, hstmt);
         my_free(henv, hdbc, hstmt);
         return(1);
      };
      if ((my_result(henv, hdbc, hstmt)))
      {
         my_free(henv, hdbc, hstmt);
         return(1);
      };
      my_free(henv, hdbc, hstmt);
      return(0);
   };

   // prepare SQL statement
   err = SQLPrepare(hstmt, (SQLTCHAR *)argv[2], SQL_NTS);
   if (err != SQL_SUCCESS)
   {
      my_errors("SQLPrepare", henv, hdbc, hstmt);
      my_free(henv, hdbc, hstmt);
      return(1);
   };

   // execute SQL statement
   err = SQLExecute(hstmt);
   if (err != SQL_SUCCESS)
   {
      my_errors("SQLExecute", henv, hdbc, hstmt);
      my_free(henv, hdbc, hstmt);
      return(1);
      if (err == SQL_SUCCESS_WITH_INFO)
      {
         my_free(henv, hdbc, hstmt);
         return(0);
      };
   };      

   // display results
   if ((my_result(henv, hdbc, hstmt)))
   {
      my_free(henv, hdbc, hstmt);
      return(1);
   };

   // frees resources
   my_free(henv, hdbc, hstmt);

   return(0);
}


/// displays errors
void my_errors(const char * s, HENV henv, HDBC hdbc, HSTMT hstmt)
{
   int        i;
   SQLTCHAR   buff[512];
   SQLTCHAR   sqlstate[15];
   SQLINTEGER native_error;
   SQLRETURN  sts;

   native_error = 0;

   // display statement errors
   for(i = 1; ((hstmt) && (i < 6)); i++)
   {
      sts = SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, i, sqlstate, &native_error, buff, sizeof(buff), NULL);
      if (!(SQL_SUCCEEDED(sts)))
         break;
      fprintf(stderr, "%s: %s\n",  s, buff);
      if (!(strcmp((char *)sqlstate, "IM003")))
         return;
   };

   // display connection errors
   for(i = 1; ((hdbc) && (i < 6)); i++)
   {
      sts = SQLGetDiagRec(SQL_HANDLE_DBC, hdbc, i, sqlstate,
                           &native_error, buff, sizeof(buff), NULL);
      if (!(SQL_SUCCEEDED(sts)))
         break;
      fprintf(stderr, "%s: %s\n",  s, buff);
      if (!(strcmp((char *)sqlstate, "IM003")))
         return;
   };

   // display environment errors
   for(i = 1; ((henv) && (i < 6)); i++)
   {
      sts = SQLGetDiagRec(SQL_HANDLE_ENV, henv, i, sqlstate, &native_error, buff, sizeof(buff), NULL);
      if (!(SQL_SUCCEEDED(sts)))
         break;
      fprintf(stderr, "%s: %s\n",  s, buff);
      if (!(strcmp((char *)sqlstate, "IM003")))
         return;
   };

   return;
}


/// disconnects and frees handles
void my_free(HENV henv, HDBC hdbc, HSTMT hstmt)
{
   if (hstmt)
   {
      SQLCloseCursor(hstmt);
      SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
   };

   if (hdbc)
   {
      SQLDisconnect(hdbc);
      SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
   };

   if (henv)
      SQLFreeHandle(SQL_HANDLE_ENV, henv);

   return;
}


/// displays result
int my_result(HENV henv, HDBC hdbc, HSTMT hstmt)
{
   int             err;
   SQLRETURN       sts;
   SQLTCHAR        fetchBuffer[1024];
   size_t          displayWidths[32];
   size_t          displayWidth;
   short           numCols;
   short           colNum;
   SQLTCHAR        colName[50];
   SQLSMALLINT     colType;
   SQLULEN         colPrecision;
   SQLLEN          colIndicator;
   SQLSMALLINT     colScale;
   SQLSMALLINT     colNullable;
   unsigned long   totalRows;
   unsigned long   totalSets;
   SQLLEN          nrows;

   totalSets = 1;

   sts = SQL_SUCCESS;
    while(sts == SQL_SUCCESS)
   {
      // retrieve number of columns
      err = SQLNumResultCols(hstmt, &numCols);
      if (err != SQL_SUCCESS)
      {
         my_errors("SQLExecute", henv, hdbc, hstmt);
         SQLCloseCursor(hstmt);
         return(1);
      };
      if (numCols == 0)
      {
         nrows = 0;
         SQLRowCount(hstmt, &nrows);
         printf("Statement executed. %ld rows affected.\n", (long)nrows);
         SQLCloseCursor(hstmt);
         return(1);
      };
      if (numCols > 32)
      {
         numCols = 32;
         fprintf(stderr, "NOTE: Resultset truncated to %d columns.\n", 32);
      };

      // retrieve name of column
      for(colNum = 1; colNum <= numCols; colNum++)
      {
         err = SQLDescribeCol(hstmt, colNum, (SQLTCHAR *) colName, sizeof(colName), NULL,
                              &colType, &colPrecision, &colScale, &colNullable);
         if (err != SQL_SUCCESS)
         {
            my_errors("SQLExecute", henv, hdbc, hstmt);
            SQLCloseCursor(hstmt);
            return(1);
         };
         switch(colType)
         {
            case SQL_VARCHAR:
            case SQL_CHAR:
            case SQL_WVARCHAR:
            case SQL_WCHAR:
            case SQL_GUID:
               displayWidth = colPrecision;
               break;

            case SQL_BINARY:
               displayWidth = colPrecision * 2;
               break;

            case SQL_LONGVARCHAR:
            case SQL_WLONGVARCHAR:
            case SQL_LONGVARBINARY:
               displayWidth = 30;	/* show only first 30 */
               break;

            case SQL_BIT:
               displayWidth = 1;
               break;

            case SQL_TINYINT:
            case SQL_SMALLINT:
            case SQL_INTEGER:
            case SQL_BIGINT:
               displayWidth = colPrecision + 1;	/* sign */
               break;

            case SQL_DOUBLE:
            case SQL_DECIMAL:
            case SQL_NUMERIC:
            case SQL_FLOAT:
            case SQL_REAL:
               displayWidth = colPrecision + 2;	/* sign, comma */
            break;

#ifdef SQL_TYPE_DATE
            case SQL_TYPE_DATE:
#endif
            case SQL_DATE:
               displayWidth = 10;
               break;

#ifdef SQL_TYPE_TIME
            case SQL_TYPE_TIME:
#endif
            case SQL_TIME:
               displayWidth = 8;
               break;

#ifdef SQL_TYPE_TIMESTAMP
            case SQL_TYPE_TIMESTAMP:
#endif
            case SQL_TIMESTAMP:
               displayWidth = 19;
               if (colScale > 0)
                  displayWidth = displayWidth + colScale + 1;
               break;

            default:
               displayWidths[colNum - 1] = 0;	/* skip other data types */
               continue;
         };

         if (displayWidth < strlen((char *)colName))
            displayWidth = strlen((char *)colName);
         if (displayWidth > sizeof(fetchBuffer) - 1)
            displayWidth = sizeof(fetchBuffer) - 1;
         displayWidths[colNum - 1] = displayWidth;

         printf("\"%s\"", colName);
         if (colNum < numCols)
            printf(",");
      };
      printf("\n");

      totalRows = 0;
      while(1)
      {
         sts = SQLFetchScroll(hstmt, SQL_FETCH_NEXT, 1);
         if (sts == SQL_NO_DATA_FOUND)
            break;
         if (sts != SQL_SUCCESS)
         {
            my_errors("SQLFetchScroll", henv, hdbc, hstmt);
            break;
         };

         for(colNum = 1; colNum <= numCols; colNum++)
         {
            sts = SQLGetData(hstmt, colNum, SQL_C_CHAR, fetchBuffer,
                             sizeof(fetchBuffer), &colIndicator);
            if ((sts != SQL_SUCCESS_WITH_INFO) && (sts != SQL_SUCCESS))
            {
               my_errors("SQLFetchScroll", henv, hdbc, hstmt);
               SQLCloseCursor(hstmt);
               return(1);
            };
            if (colIndicator == SQL_NULL_DATA)
               fetchBuffer[0] = '\0';
            printf("\"%s\"", fetchBuffer);
            if (colNum < numCols)
               printf(",");
         };
         printf("\n");
         totalRows++;
      };

      printf("\nresult set %lu returned %lu rows.\n\n", totalSets, totalRows);
      totalSets++;

      sts = SQLMoreResults(hstmt);
   };

   if (sts == SQL_ERROR)
   {
      my_errors("SQLMoreResults", henv, hdbc, hstmt);
      SQLCloseCursor(hstmt);
      return(1);
   };

   SQLCloseCursor(hstmt);

   return(0);
}

/* end of source */
