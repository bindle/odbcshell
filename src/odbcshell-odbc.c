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
 *  @file src/odbcshell-odbc.c ODBC management functions
 */
#include "odbcshell-odbc.h"

///////////////
//           //
//  Headers  //
//           //
///////////////
#ifdef PMARK
#pragma mark Headers
#endif

#include "odbcshell.h"

#include <stdio.h>
#include <string.h>

#include "odbcshell-print.h"



/////////////////
//             //
//  Functions  //
//             //
/////////////////
#ifdef PMARK
#pragma mark -
#pragma mark Functions
#endif

/// adds ODBC connection to list
/// @param[in]  cnf      pointer to configuration struct
/// @param[in]  connp    pointer to pointer to connection struct
int odbcshell_odbc_array_add(ODBCShell * cnf, ODBCShellConn * conn)
{
   void      * ptr;
   size_t      conns_size;

   conns_size  = sizeof(ODBCShellConn *);
   conns_size *= (cnf->conns_count + 1);

   if (!(ptr = realloc(cnf->conns, conns_size)))
   {
      fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
      return(-1);
   };
   cnf->conns = ptr;

   cnf->conns[cnf->conns_count] = conn;

   cnf->conns_count++;

   return(0);
}


/// retrieves an ODBC connection from the list
/// @param[in]  cnf      pointer to configuration struct
/// @param[in]  name     internal name of connection
int odbcshell_odbc_array_findindex(ODBCShell * cnf, const char * name)
{
   int i;
   if (!(cnf->conns))
      return(-1);
   for(i = 0; i < (int)cnf->conns_count; i++)
      if (!(strcasecmp(cnf->conns[i]->name, name)))
         return(i);
   return(-1);
}


/// removes an ODBC connection from the list
/// @param[in]  cnf      pointer to configuration struct
/// @param[in]  name     internal name of connection
int odbcshell_odbc_array_rm(ODBCShell * cnf, const char * name)
{
   int i;
   int conn_index;

   if (((conn_index = odbcshell_odbc_array_findindex(cnf, name))) == -1)
      return(0);

   for(i = (conn_index+1); i < (int)cnf->conns_count; i++)
      cnf->conns[i-1] = cnf->conns[i];

   cnf->conns_count--;

   return(0);
}


/// closes all ODBC connections
/// @param[in]  cnf      pointer to configuration struct
int odbcshell_odbc_close(ODBCShell * cnf)
{
   int i;

   for(i = 0;i < (int)cnf->conns_count; i++)
   {
      printf("closing connection \"%s\"\n", cnf->conns[i]->name);
      odbcshell_odbc_free(cnf, &cnf->conns[i]);
   };
   free(cnf->conns);
   cnf->conns = NULL;
   cnf->conns_count = 0;

   if (cnf->hdbc)
   {
      SQLDisconnect(cnf->hdbc);
      SQLFreeHandle(SQL_HANDLE_DBC, cnf->hdbc);
   };
   cnf->hdbc = NULL;

   if (cnf->henv)
      SQLFreeHandle(SQL_HANDLE_ENV, cnf->henv);
   cnf->henv = NULL;

   return(0);
}


/// connects to ODBC data source
/// @param[in]  cnf      pointer to configuration struct
/// @param[in]  dsn      data source to connect
/// @param[in]  name     internal name of connect
int odbcshell_odbc_connect(ODBCShell * cnf, const char * dsn,
   const char * name)
{
   SQLTCHAR         dsnOut[512];
   short            buflen;
   SQLRETURN        sts;
   ODBCShellConn  * conn;

   if (!(name))
      name = "default";
   if (!(strlen(name)))
      name = "default";

   if ((odbcshell_odbc_array_findindex(cnf, name)) >= 0)
   {
      odbcshell_error(cnf, "connection with name \"%s\" already exists\n", name);
      return(-1);
   };

   // allocates memory for storing internal connection information
   if (!(conn = malloc(sizeof(ODBCShellConn))))
   {
      fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
      return(-1);
   };
   memset(conn, 0, sizeof(ODBCShellConn));
   name = name ? name : "";
   if (!(conn->name = strdup(name)))
   {
      fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
      return(-1);
   };
   if (!(conn->dsn = (char *)strdup(dsn)))
   {
      fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
      return(-1);
   };

   // allocates iODBC handle for connection
   if (SQLAllocHandle(SQL_HANDLE_DBC, cnf->henv, &conn->hdbc) != SQL_SUCCESS)
   {
      odbcshell_odbc_errors("SQLAllocHandle", cnf, conn);
      odbcshell_odbc_free(cnf, &conn);
      return(-1);
   };
   SQLSetConnectOption(conn->hdbc, SQL_APPLICATION_NAME, (SQLULEN)PROGRAM_NAME);

   // connects to data source
   if (cnf->odbcprompt)
   {
      sts = SQLDriverConnect(conn->hdbc, 0, (SQLTCHAR *)conn->dsn, SQL_NTS,
            dsnOut, strlen(conn->dsn), &buflen, SQL_DRIVER_COMPLETE);
   } else {
      sts = SQLDriverConnect(conn->hdbc, 0, (SQLTCHAR *)conn->dsn, SQL_NTS,
            dsnOut, strlen(conn->dsn), &buflen, SQL_DRIVER_NOPROMPT);
   };
   if ((sts != SQL_SUCCESS) && (sts != SQL_SUCCESS_WITH_INFO))
   {
      odbcshell_odbc_errors("SQLDriverConnect", cnf, conn);
      odbcshell_odbc_free(cnf, &conn);
      return(-1);
   };

   // allocates statement handle
   sts = SQLAllocHandle(SQL_HANDLE_STMT, conn->hdbc, &conn->hstmt);
   if (sts != SQL_SUCCESS)
   {
      odbcshell_odbc_errors("SQLAllocHandle", cnf, conn);
      odbcshell_odbc_free(cnf, &conn);
      return(-1);
   };

   // adds connection to array
   if ((odbcshell_odbc_array_add(cnf, conn)))
   {
      odbcshell_odbc_free(cnf, &conn);
      return(-1);
   };

   odbcshell_odbc_update_current(cnf, conn);

   return(0);
}


/// disconnects a session
/// @param[in]  cnf      pointer to configuration struct
/// @param[in]  name     internal name of connect
int odbcshell_odbc_disconnect(ODBCShell * cnf, const char * name)
{
   int             conn_index;
   ODBCShellConn * conn;

   if (!(name))
   {
      if (!(cnf->current))
      {
         odbcshell_error(cnf, "not connected to a database\n");
         return(-1);
      };
      if (!(cnf->current->name))
         return(0);
      name = cnf->current->name;
   };

   if (cnf->current)
      if (!(strcasecmp(name, cnf->current->name)))
         cnf->current = NULL;

   if (((conn_index = odbcshell_odbc_array_findindex(cnf, name))) == -1)
   {
      odbcshell_error(cnf, "unknown connection \"%s\"\n", name);
      return(-1);
   };
   conn = cnf->conns[conn_index];
   odbcshell_odbc_array_rm(cnf, name);
   odbcshell_odbc_free(cnf, &conn);

   odbcshell_odbc_update_current(cnf, NULL);

   return(0);
}


/// displays iODBC errors
/// @param[in]  s        descriptive string
/// @param[in]  cnf      pointer to configuration struct
/// @param[in]  conn     pointer to connection struct
void odbcshell_odbc_errors(const char * s, ODBCShell * cnf,
   ODBCShellConn  * conn)
{
   int        i;
   HENV       henv;
   HDBC       hdbc;
   HSTMT      hstmt;
   SQLTCHAR   buff[512];
   SQLTCHAR   sqlstate[15];
   SQLINTEGER native_error;
   SQLRETURN  sts;

   henv         = cnf->henv;
   hdbc         = conn ? conn->hdbc  : cnf->hdbc;
   hstmt        = conn ? conn->hstmt : NULL;
   native_error = 0;

   // display statement errors
   for(i = 1; ((hstmt) && (i < 6)); i++)
   {
      sts = SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, i, sqlstate, &native_error, buff, sizeof(buff), NULL);
      if (!(SQL_SUCCEEDED(sts)))
         break;
      odbcshell_error(cnf, "%s: %s\n",  s, buff);
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
      odbcshell_error(cnf, "%s: %s\n",  s, buff);
      if (!(strcmp((char *)sqlstate, "IM003")))
         return;
   };

   // display environment errors
   for(i = 1; ((henv) && (i < 6)); i++)
   {
      sts = SQLGetDiagRec(SQL_HANDLE_ENV, henv, i, sqlstate, &native_error, buff, sizeof(buff), NULL);
      if (!(SQL_SUCCEEDED(sts)))
         break;
      odbcshell_error(cnf, "%s: %s\n",  s, buff);
      if (!(strcmp((char *)sqlstate, "IM003")))
         return;
   };

   return;
}


/// execute SQL statement
/// @param[in]  cnf      pointer to configuration struct
/// @param[in]  sql      SQL string to execute
int odbcshell_odbc_exec(ODBCShell * cnf, char * sql)
{
   int err;

   if (!(cnf->current))
   {
      odbcshell_error(cnf, "not connected to a database\n");
      return(-1);
   };

   // prepare SQL statement
   odbcshell_verbose(cnf, "preparing SQL statement...\n");
   err = SQLPrepare(cnf->current->hstmt, (SQLTCHAR *)sql, SQL_NTS);
   if (err != SQL_SUCCESS)
   {
      odbcshell_odbc_errors("SQLPrepare", cnf, cnf->current);
      return(-1);
   };

   // execute SQL statement
   odbcshell_verbose(cnf, "executing SQL statement...\n");
   err = SQLExecute(cnf->current->hstmt);
   if (err != SQL_SUCCESS)
   {
      odbcshell_odbc_errors("SQLExecute", cnf, cnf->current);
      if (err == SQL_SUCCESS_WITH_INFO)
         return(0);
      return(-1);
   };

   return(odbcshell_odbc_result(cnf));
}


/// frees resources from an iODBC connection
/// @param[in]  cnf      pointer to configuration struct
/// @param[in]  connp    pointer to pointer to connection struct
void odbcshell_odbc_free(ODBCShell * cnf, ODBCShellConn  ** connp)
{
   if (!(connp))
      return;

   if (cnf)
      if (cnf->current == (*connp))
         cnf->current = NULL;

   if ((*connp)->hstmt)
   {
      SQLCloseCursor((*connp)->hstmt);
      SQLFreeHandle(SQL_HANDLE_STMT, (*connp)->hstmt);
   };
   (*connp)->hstmt = NULL;

   if ((*connp)->hdbc)
   {
      SQLDisconnect((*connp)->hdbc);
      SQLFreeHandle(SQL_HANDLE_DBC, (*connp)->hdbc);
   };
   (*connp)->hdbc = NULL;

   if ((*connp)->name)
      free((*connp)->name);
   (*connp)->name = NULL;

   if ((*connp)->dsn)
      free((*connp)->dsn);
   (*connp)->dsn = NULL;

   free(*connp);
   (*connp) = NULL;

   return;
}


/// initializes ODBC library
/// @param[in]  cnf      pointer to configuration struct
int odbcshell_odbc_initialize(ODBCShell * cnf)
{
   if (SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_ENV, NULL, &cnf->henv) != SQL_SUCCESS)
      return(-1);

   SQLSetEnvAttr(cnf->henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER) SQL_OV_ODBC3,
                  SQL_IS_UINTEGER);

   if (SQLAllocHandle(SQL_HANDLE_DBC, cnf->henv, &cnf->hdbc) != SQL_SUCCESS)
      return(-1);

   SQLSetConnectOption(cnf->hdbc, SQL_APPLICATION_NAME, (SQLULEN)PROGRAM_NAME);

   return(0);
}


/// reconnects a session
/// @param[in]  cnf      pointer to configuration struct
/// @param[in]  name     internal name of connect
int odbcshell_odbc_reconnect(ODBCShell * cnf, const char * name)
{
   SQLTCHAR        dsnOut[512];
   short           buflen;
   SQLRETURN       sts;
   int             conn_index;
   ODBCShellConn * conn;

   if (!(name))
   {
      if (!(cnf->current))
         return(0);
      if (!(cnf->current->name))
         return(0);
      name = cnf->current->name;
   };

   if (((conn_index = odbcshell_odbc_array_findindex(cnf, name))) == -1)
      return(0);

   conn = cnf->conns[conn_index];

   SQLCloseCursor(conn->hstmt);
   SQLFreeHandle(SQL_HANDLE_STMT, conn->hstmt);
   conn->hstmt = NULL;
   SQLDisconnect(conn->hdbc);

   sts = SQLDriverConnect(conn->hdbc, 0, (SQLTCHAR *)conn->dsn, SQL_NTS, dsnOut,
      sizeof(dsnOut), &buflen, SQL_DRIVER_NOPROMPT);
   if ((sts != SQL_SUCCESS) && (sts != SQL_SUCCESS_WITH_INFO))
   {
      odbcshell_odbc_errors("SQLDriverConnect", cnf, conn);
      return(-1);
   }; 

   sts = SQLAllocHandle(SQL_HANDLE_STMT, conn->hdbc, &conn->hstmt);
   if (sts != SQL_SUCCESS)
   {
      odbcshell_odbc_errors("SQLAllocHandle", cnf, conn);
      return(-1);
   };

   return(0);
}


/// displays result from ODBC operation
/// @param[in]  cnf      pointer to configuration struct
int odbcshell_odbc_result(ODBCShell * cnf)
{
   int             err;
   SQLRETURN       sts;
   SQLTCHAR        fetchBuffer[1024];
   size_t          displayWidths[256];
   size_t          displayWidth;
   short           cols_count;
   short           col_index;
   SQLULEN         colPrecision;
   unsigned long   totalRows;
   unsigned long   totalSets;
   SQLLEN          nrows;
   ODBCShellColumn col;

   totalSets = 1;

   odbcshell_verbose(cnf, "preparing SQL results...\n");

   sts = SQL_SUCCESS;
   while (sts == SQL_SUCCESS)
   {
      // retrieve number of columns
      err = SQLNumResultCols(cnf->current->hstmt, &cols_count);
      if (err != SQL_SUCCESS)
      {
         odbcshell_odbc_errors("SQLNumResultCols", cnf, cnf->current);
         SQLCloseCursor(cnf->current->hstmt);
         return(-1);
      };
      if (cols_count == 0)
      {
         nrows = 0;
         SQLRowCount(cnf->current->hstmt, &nrows);
         printf("Statement executed. %ld rows affected.\n", (long)nrows);
         SQLCloseCursor(cnf->current->hstmt);
         return(0);
      };
      if (cols_count > 256)
      {
         cols_count = 256;
         odbcshell_error(cnf, "NOTE: Resultset truncated to %d columns.\n", 256);
      };

      // retrieve name of column
      for(col_index = 0; col_index < cols_count; col_index++)
      {
         memset(&col, 0, sizeof(ODBCShellColumn));
         err = SQLDescribeCol(cnf->current->hstmt, col_index+1, col.name,
                              sizeof(col.name), NULL, &col.type, &colPrecision,
                              &col.scale, &col.nullable);
         if (err != SQL_SUCCESS)
         {
            odbcshell_odbc_errors("SQLDescribeCol", cnf, cnf->current);
            SQLCloseCursor(cnf->current->hstmt);
            return(-1);
         };
         switch(col.type)
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
               if (col.scale > 0)
                  displayWidth = displayWidth + col.scale + 1;
               break;

            default:
               displayWidths[col_index] = 0;	/* skip other data types */
               continue;
         };

         if (displayWidth < strlen((char *)col.name))
            displayWidth = strlen((char *)col.name);
         if (displayWidth > sizeof(fetchBuffer) - 1)
            displayWidth = sizeof(fetchBuffer) - 1;
         displayWidths[col_index] = displayWidth;

         odbcshell_fprintf(cnf, "\"%s\"", col.name);
         if (col_index < (cols_count-1))
            odbcshell_fprintf(cnf, ",");
      };
      odbcshell_fprintf(cnf, "\n");

      totalRows = 0;
      while(1)
      {
         sts = SQLFetchScroll(cnf->current->hstmt, SQL_FETCH_NEXT, 1);
         if (sts == SQL_NO_DATA_FOUND)
            break;
         if (sts != SQL_SUCCESS)
         {
            odbcshell_odbc_errors("SQLFetchScroll", cnf, cnf->current);
            break;
         };

         for(col_index = 0; col_index < cols_count; col_index++)
         {
            sts = SQLGetData(cnf->current->hstmt, col_index+1, SQL_C_CHAR,
                             fetchBuffer, sizeof(fetchBuffer), &col.indicator);
            if ((sts != SQL_SUCCESS_WITH_INFO) && (sts != SQL_SUCCESS))
            {
               odbcshell_odbc_errors("SQLGetData", cnf, cnf->current);
               SQLCloseCursor(cnf->current->hstmt);
               return(-1);
            };
            if (col.indicator == SQL_NULL_DATA)
               fetchBuffer[0] = '\0';
            odbcshell_fprintf(cnf, "\"%s\"", fetchBuffer);
            if (col_index < (cols_count-1))
               odbcshell_fprintf(cnf, ",");
         };
         odbcshell_fprintf(cnf, "\n");
         totalRows++;
      };

      printf("\nresult set %lu returned %lu rows.\n\n", totalSets, totalRows);
      totalSets++;

      sts = SQLMoreResults(cnf->current->hstmt);
   };

   if (sts == SQL_ERROR)
   {
      odbcshell_odbc_errors("SQLMoreResults", cnf, cnf->current);
      SQLCloseCursor(cnf->current->hstmt);
      return(-1);
   };

   SQLCloseCursor(cnf->current->hstmt);

   return(0);
}


/// displays list of ODBC datatypes
/// @param[in]  cnf      pointer to configuration struct
int odbcshell_odbc_show_datatypes(ODBCShell * cnf)
{
   SQLRETURN       sts;

   if (!(cnf->current))
   {
      odbcshell_error(cnf, "not connected to a database\n");
      return(-1);
   };

   sts = SQLGetTypeInfo(cnf->current->hstmt, 0);
   if (sts != SQL_SUCCESS)
   {
      odbcshell_odbc_errors("SQLGetTypeInfo", cnf, cnf->current);
      return(-1);
   };

   return(odbcshell_odbc_result(cnf));
}


/// displays list of ODBC data sources
/// @param[in]  cnf      pointer to configuration struct
int odbcshell_odbc_show_dsn(ODBCShell * cnf)
{
   int err;
   SQLTCHAR    dsn[33];
   SQLTCHAR    desc[255];
   SQLSMALLINT len1;
   SQLSMALLINT len2;

   err = SQLDataSources(cnf->henv, SQL_FETCH_FIRST, dsn,  sizeof(dsn), &len1,
                                                    desc, sizeof(desc), &len2);
   if (err != SQL_SUCCESS)
   {
      odbcshell_error(cnf, "unable to list ODBC sources.\n");
      return(-1);
   };

   printf("%-32s   %-40s\n", "DSN:", "Driver:");
   while (err == SQL_SUCCESS)
   {
      printf("%-32s   %-40s\n", dsn, desc);
      err = SQLDataSources(cnf->henv, SQL_FETCH_NEXT, dsn,  sizeof(dsn), &len1,
                                                      desc, sizeof(desc), &len2);
   };

   return(0);
}


/// displays list of ODBC owners
/// @param[in]  cnf      pointer to configuration struct
int odbcshell_odbc_show_owners(ODBCShell * cnf)
{
   SQLRETURN       sts;
   unsigned char   strwild[2];

   strncpy((char *)strwild,  "%", 2);

   if (!(cnf->current))
   {
      odbcshell_error(cnf, "not connected to a database\n");
      return(-1);
   };

   sts = SQLTables(cnf->current->hstmt, NULL, 0, strwild, SQL_NTS, NULL, 0, NULL, 0);
   if (sts != SQL_SUCCESS)
   {
      odbcshell_odbc_errors("SQLGetTypeInfo", cnf, cnf->current);
      return(-1);
   };

   return(odbcshell_odbc_result(cnf));
}


/// displays list of ODBC tables
/// @param[in]  cnf      pointer to configuration struct
int odbcshell_odbc_show_tables(ODBCShell * cnf)
{
   SQLRETURN       sts;

   if (!(cnf->current))
   {
      odbcshell_error(cnf, "not connected to a database\n");
      return(-1);
   };

   sts = SQLTables(cnf->current->hstmt, NULL, 0, NULL, 0, NULL, 0, NULL, 0);
   if (sts != SQL_SUCCESS)
   {
      odbcshell_odbc_errors("SQLMoreResults", cnf, cnf->current);
      return(-1);
   };

   return(odbcshell_odbc_result(cnf));
}


/// displays list of ODBC types
/// @param[in]  cnf      pointer to configuration struct
int odbcshell_odbc_show_types(ODBCShell * cnf)
{
   SQLRETURN       sts;
   unsigned char   strwild[2];

   strncpy((char *)strwild,  "%", 2);

   if (!(cnf->current))
   {
      odbcshell_error(cnf, "not connected to a database\n");
      return(-1);
   };

   sts = SQLTables(cnf->current->hstmt, NULL, 0, NULL, 0, NULL, 0, strwild, SQL_NTS);
   if (sts != SQL_SUCCESS)
   {
      odbcshell_odbc_errors("SQLGetTypeInfo", cnf, cnf->current);
      return(-1);
   };

   return(odbcshell_odbc_result(cnf));
}


/// displays list of ODBC qualifiers
/// @param[in]  cnf      pointer to configuration struct
int odbcshell_odbc_show_qualifiers(ODBCShell * cnf)
{
   SQLRETURN       sts;
   unsigned char   strwild[2];

   strncpy((char *)strwild,  "%", 2);

   if (!(cnf->current))
   {
      odbcshell_error(cnf, "not connected to a database\n");
      return(-1);
   };

   sts = SQLTables(cnf->current->hstmt, strwild, SQL_NTS, NULL, 0, NULL, 0, NULL, 0);
   if (sts != SQL_SUCCESS)
   {
      odbcshell_odbc_errors("SQLMoreResults", cnf, cnf->current);
      return(-1);
   };

   return(odbcshell_odbc_result(cnf));
}


/// updates current connection
/// @param[in]  cnf      pointer to configuration struct
/// @param[in]  conn     pointer to connection struct
int odbcshell_odbc_update_current(ODBCShell * cnf, ODBCShellConn  * conn)
{
   if ( (!(conn)) && ((cnf->current)) )
      return(0);

   if ( (!(conn)) && (!(cnf->conns_count)) )
      return(0);

   if (!(conn))
      conn = cnf->conns[cnf->conns_count-1];

   cnf->current = conn;
   odbcshell_printf(cnf, "using connection \"%s\"\n", cnf->current->name);

   return(0);
}


/// switches active connection
/// @param[in]  cnf      pointer to configuration struct
/// @param[in]  name     internal name of connect
int odbcshell_odbc_use(ODBCShell * cnf, const char * name)
{
   int i;

   if (!(name))
   {
      printf("  Name:      DSN:\n");
      for(i = 0; i < (int)cnf->conns_count; i++)
      {
         if (!(strcasecmp(cnf->current->name, cnf->conns[i]->name)))
            printf("* %-10s %s\n", cnf->conns[i]->name, cnf->conns[i]->dsn);
         else
            printf("  %-10s %s\n", cnf->conns[i]->name, cnf->conns[i]->dsn);
      };
      return(0);
   };

   for(i = 0; i < (int)cnf->conns_count; i++)
   {
      if (!(strcasecmp(name, cnf->conns[i]->name)))
      {
         odbcshell_odbc_update_current(cnf, cnf->conns[i]);
         return(0);
      };
   };

   odbcshell_error(cnf, "use: unknown connection handle\n");

   return(-1);
}


/// displays ODBC version
/// @param[in]  cnf      pointer to configuration struct
int odbcshell_odbc_version(ODBCShell * cnf)
{
   int         i;
   SQLTCHAR    info[255];
   SQLSMALLINT len;
   SQLRETURN   sts;

   sts = SQLGetInfo(cnf->hdbc, SQL_DM_VER, info, sizeof(info), &len);
   if (sts == SQL_SUCCESS)
      printf("iODBC Driver Manager %s\n", info);

   for(i = 0; i < (int)cnf->conns_count; i++)
   {
      printf("connection: %s", cnf->conns[i]->name);
      sts = SQLGetInfo(cnf->conns[i]->hdbc, SQL_DRIVER_NAME, info, sizeof(info), &len);
      if (sts == SQL_SUCCESS)
      {
         printf(" (%s)", info);
         sts = SQLGetInfo(cnf->conns[i]->hdbc, SQL_DRIVER_VER, info, sizeof(info), &len);
         if (sts == SQL_SUCCESS)
            printf(" %s", info);
         printf("\n");
      };
   };

   return(0);
}

/* end of source */
