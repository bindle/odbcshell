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


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark -
#pragma mark Functions

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
   int         status;
   SQLTCHAR    info[255];
   SQLSMALLINT len1;

   status = SQLGetInfo(cnf->hdbc, SQL_DM_VER, info, sizeof(info), &len1);
   if (status == SQL_SUCCESS)
      printf ("iODBC Driver Manager %s\n", info);

   return(0);
}

/* end of source */
