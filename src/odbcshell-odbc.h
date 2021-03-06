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
 *  @file src/odbcshell-odbc.h ODBC management functions
 */
#ifndef _ODBCSHELL_SRC_ODBCSHELL_ODBC_H
#define _ODBCSHELL_SRC_ODBCSHELL_ODBC_H 1

///////////////
//           //
//  Headers  //
//           //
///////////////
#ifdef PMARK
#pragma mark Headers
#endif

#include "odbcshell.h"


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#ifdef PMARK
#pragma mark -
#pragma mark Prototypes
#endif

// adds ODBC connection to list
int odbcshell_odbc_array_add(ODBCShell * cnf, ODBCShellConn * conn);

// retrieves an ODBC connection from the list
int odbcshell_odbc_array_findindex(ODBCShell * cnf, const char * name);

// removes an ODBC connection from the list
int odbcshell_odbc_array_rm(ODBCShell * cnf, const char * name);

// closes all ODBC connections
int odbcshell_odbc_close(ODBCShell * cnf);

// connects to ODBC data source
int odbcshell_odbc_connect(ODBCShell * cnf, const char * dsn,
   const char * name);

// disconnects a session
int odbcshell_odbc_disconnect(ODBCShell * cnf, const char * name);

// displays iODBC errors
void odbcshell_odbc_errors(const char * s, ODBCShell * cnf,
   ODBCShellConn  * conn);

// execute SQL statement
int odbcshell_odbc_exec(ODBCShell * cnf, char * sql);

// frees resources from an iODBC connection
void odbcshell_odbc_free(ODBCShell * cnf, ODBCShellConn  ** connp);

// initializes ODBC library
int odbcshell_odbc_initialize(ODBCShell * cnf);

// reconnects a session
int odbcshell_odbc_reconnect(ODBCShell * cnf, const char * name);

// displays result from ODBC operation
int odbcshell_odbc_result(ODBCShell * cnf);

// displays result from ODBC operation as CSV output
int odbcshell_odbc_result_csv(ODBCShell * cnf, SQLLEN * row_countp);

// displays result from ODBC operation as Fixed Width output
int odbcshell_odbc_result_fixedwidth(ODBCShell * cnf, SQLLEN * row_countp);

// displays result from ODBC operation as XML output
int odbcshell_odbc_result_xml(ODBCShell * cnf, SQLLEN * row_countp);

// displays list of ODBC datatypes
int odbcshell_odbc_show_datatypes(ODBCShell * cnf);

// displays list of ODBC data sources
int odbcshell_odbc_show_dsn(ODBCShell * cnf);

// displays list of ODBC owners
int odbcshell_odbc_show_owners(ODBCShell * cnf);

// displays list of ODBC tables
int odbcshell_odbc_show_tables(ODBCShell * cnf);

// displays list of ODBC types
int odbcshell_odbc_show_types(ODBCShell * cnf);

// displays list of ODBC qualifiers
int odbcshell_odbc_show_qualifiers(ODBCShell * cnf);

// updates current connection
int odbcshell_odbc_update_current(ODBCShell * cnf, ODBCShellConn  * conn);

// switches active connection
int odbcshell_odbc_use(ODBCShell * cnf, const char * name);

// displays ODBC version
int odbcshell_odbc_version(ODBCShell * cnf);

#endif
/* end of header */
