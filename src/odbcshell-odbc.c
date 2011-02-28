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
#pragma mark Headers

#include "odbcshell.h"

#include <string.h>


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark -
#pragma mark Functions

/// adds ODBC connection to list
/// @param[in]  cnf      pointer to configuration struct
/// @param[in]  connp    pointer to pointer to connection struct
int odbcshell_odbc_conn_add(ODBCShell * cnf, ODBCShellConn * conn)
{
   void      * ptr;
   size_t      conns_size;

   conns_size  = sizeof(ODBCShellConn *);
   conns_size *= (cnf->conns_count + 1);

   if (!(ptr = realloc(cnf->conns, conns_size)))
   {
      fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
      return(1);
   };
   cnf->conns = ptr;

   cnf->conns[cnf->conns_count] = conn;

   cnf->conns_count++;

   return(0);
}


/// retrieves an ODBC connection from the list
/// @param[in]  cnf      pointer to configuration struct
/// @param[in]  name     internal name of connection
int odbcshell_odbc_conn_findindex(ODBCShell * cnf, const char * name)
{
   int i;
   if (!(cnf->conns))
      return(-1);
   for(i = 0; i < (int)cnf->conns_count; i++)
      if (!(strcasecmp(cnf->conns[i]->name, name)))
         return(i);
   return(-1);
}


// frees resources from an iODBC connection
/// @param[in]  cnf      pointer to configuration struct
/// @param[in]  connp    pointer to pointer to connection struct
void odbcshell_odbc_conn_free(ODBCShell * cnf, ODBCShellConn  ** connp)
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


/// removes an ODBC connection from the list
/// @param[in]  cnf      pointer to configuration struct
/// @param[in]  name     internal name of connection
int odbcshell_odbc_conn_rm(ODBCShell * cnf, const char * name)
{
   int i;
   int conn_index;

   if (((conn_index = odbcshell_odbc_conn_findindex(cnf, name))) == -1)
      return(0);

   for(i = (conn_index+1); i < (int)cnf->conns_count; i++)
      cnf->conns[i-1] = cnf->conns[i];

   cnf->conns_count--;

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

   if ((odbcshell_odbc_conn_findindex(cnf, dsn)) >= 0)
   {
      fprintf(stderr, "%s: connection %s already established\n", PROGRAM_NAME, name);
      return(0);
   };

   // allocates memory for storing internal connection information
   if (!(conn = malloc(sizeof(ODBCShellConn))))
   {
      fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
      return(1);
   };
   memset(conn, 0, sizeof(ODBCShellConn));
   name = name ? name : "";
   if (!(conn->name = strdup(name)))
   {
      fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
      return(1);
   };
   if (!(conn->dsn = (char *)strdup(dsn)))
   {
      fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
      return(1);
   };

   // allocates iODBC handle for connection
   if (SQLAllocHandle(SQL_HANDLE_DBC, cnf->henv, &conn->hdbc) != SQL_SUCCESS)
   {
      odbcshell_odbc_errors("SQLAllocHandle", cnf, conn);
      odbcshell_odbc_conn_free(cnf, &conn);
      return(0);
   };
   SQLSetConnectOption(conn->hdbc, SQL_APPLICATION_NAME, (SQLULEN)PROGRAM_NAME);

   // connects to data source
   sts = SQLDriverConnect(conn->hdbc, 0, (SQLTCHAR *)conn->dsn, SQL_NTS, dsnOut,
                          strlen(conn->dsn), &buflen, SQL_DRIVER_COMPLETE);
   if ((sts != SQL_SUCCESS) && (sts != SQL_SUCCESS_WITH_INFO))
   {
      odbcshell_odbc_errors("SQLDriverConnect", cnf, conn);
      odbcshell_odbc_conn_free(cnf, &conn);
      return(0);
   };

   if ((odbcshell_odbc_conn_add(cnf, conn)))
   {
      odbcshell_odbc_conn_free(cnf, &conn);
      return(1);
   };
   cnf->current = conn;

   return(0);
}


/// disconnects a session
int odbcshell_odbc_disconnect(ODBCShell * cnf, const char * name)
{
   int             conn_index;
   ODBCShellConn * conn;

   if (cnf->current)
      if (!(strcasecmp(name, cnf->current->name)))
         cnf->current = NULL;

   if (((conn_index = odbcshell_odbc_conn_findindex(cnf, name))) == -1)
      return(0);

   conn = cnf->conns[conn_index];
   odbcshell_odbc_conn_rm(cnf, name);
   odbcshell_odbc_conn_free(cnf, &conn);

   if ((!(cnf->current)) && (cnf->conns_count))
      cnf->current = cnf->conns[0];

   return(0);
}


/// displays iODBC errors
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
      sts = SQLGetDiagRec (SQL_HANDLE_STMT, hstmt, i, sqlstate, &native_error, buff, sizeof(buff), NULL);
      if (!(SQL_SUCCEEDED(sts)))
         break;
      fprintf (stderr, "%s: %s\n",  s, buff);
      if (!(strcmp((char *)sqlstate, "IM003")))
         return;
   };

   // display connection errors
   for(i = 1; ((hdbc) && (i < 6)); i++)
   {
      sts = SQLGetDiagRec (SQL_HANDLE_DBC, hdbc, i, sqlstate,
                           &native_error, buff, sizeof(buff), NULL);
      if (!(SQL_SUCCEEDED(sts)))
         break;
      fprintf (stderr, "%s: %s\n",  s, buff);
      if (!(strcmp((char *)sqlstate, "IM003")))
         return;
   };

   // display environment errors
   for(i = 1; ((henv) && (i < 6)); i++)
   {
      sts = SQLGetDiagRec (SQL_HANDLE_ENV, henv, i, sqlstate, &native_error, buff, sizeof(buff), NULL);
      if (!(SQL_SUCCEEDED(sts)))
         break;
      fprintf (stderr, "%s: %s\n",  s, buff);
      if (!(strcmp((char *)sqlstate, "IM003")))
         return;
   };

   return;
}


/// initializes ODBC library
/// @param[in]  cnf      pointer to configuration struct
int odbcshell_odbc_initialize(ODBCShell * cnf)
{
   if (SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_ENV, NULL, &cnf->henv) != SQL_SUCCESS)
      return(-1);

   SQLSetEnvAttr (cnf->henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER) SQL_OV_ODBC3,
                  SQL_IS_UINTEGER);

   if (SQLAllocHandle(SQL_HANDLE_DBC, cnf->henv, &cnf->hdbc) != SQL_SUCCESS)
      return(-1);

   SQLSetConnectOption(cnf->hdbc, SQL_APPLICATION_NAME, (SQLULEN)PROGRAM_NAME);

   return(0);
}


/// displays list of ODBC data sources
/// @param[in]  cnf      pointer to configuration struct
int odbcshell_odbc_list_dsn(ODBCShell * cnf)
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
      fprintf(stderr, "%s: unable to list ODBC sources.\n", PROGRAM_NAME);
      return(0);
   };

   fprintf (stderr, "%-32s   %-40s\n", "DSN:", "Driver:");
   while (err == SQL_SUCCESS)
   {
      fprintf (stderr, "%-32s   %-40s\n", dsn, desc);
      err = SQLDataSources(cnf->henv, SQL_FETCH_NEXT, dsn,  sizeof(dsn), &len1,
                                                      desc, sizeof(desc), &len2);
   };

   return(0);
}


/// displays ODBC version
/// @param[in]  cnf      pointer to configuration struct
int odbcshell_odbc_version(ODBCShell * cnf)
{
   SQLTCHAR    info[255];
   SQLSMALLINT len;
   SQLRETURN   sts;

   sts = SQLGetInfo(cnf->hdbc, SQL_DM_VER, info, sizeof(info), &len);
   if (sts == SQL_SUCCESS)
      printf("iODBC Driver Manager %s\n", info);

   if (cnf->current)
   {
      if (cnf->current->name)
         printf("Connection: %s\n", cnf->current->name);
      sts = SQLGetInfo(cnf->current->hdbc, SQL_DRIVER_VER, info, sizeof(info), &len);
      if (sts == SQL_SUCCESS)
      {
         printf ("Driver: %s", info);
         sts = SQLGetInfo (cnf->current->hdbc, SQL_DRIVER_NAME, info, sizeof(info), &len);
         if (sts == SQL_SUCCESS)
            printf(" (%s)", info);
         printf("\n");
      };
   };

   return(0);
}

/* end of source */
