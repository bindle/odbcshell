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
 *  @file src/odbcshell-variables.c ODBC Shell global variables
 */
#include "odbcshell-variables.h"

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
#include <strings.h>


/////////////////
//             //
//  Variables  //
//             //
/////////////////
#ifdef PMARK
#pragma mark Variables
#endif

/// @brief numeric values for command names
ODBCShellOption odbcshell_cmd_strings[] =
{
   { ODBCSHELL_CMD_ODBC,        1, -1, "ALTER",      "internal SQL command (data definition)",        NULL },
   { ODBCSHELL_CMD_ODBC,        1, -1, "BEGIN",      "internal SQL command (transaction controls)",   NULL },
   { ODBCSHELL_CMD_CLEAR,       1,  1, "CLEAR",      "clears screen",                                 (const char *[2]){"clear", NULL} },
   { ODBCSHELL_CMD_CLOSE,       1,  1, "CLOSE",      "closes output file",                            (const char *[2]){"close", NULL} },
   { ODBCSHELL_CMD_ODBC,        1, -1, "COMMIT",     "internal SQL command (transaction controls)",   NULL },
   { ODBCSHELL_CMD_CONNECT,     2,  3, "CONNECT",    "connects to a database",                        (const char *[3]){"connect \"DSN=My Database;UID=John Doe;PWD=password\"", "connect name \"DSN=My Database;UID=John Doe;PWD=password\"", NULL} },
   { ODBCSHELL_CMD_ODBC,        1, -1, "CREATE",     "internal SQL command (data definition)",         NULL },
   { ODBCSHELL_CMD_ODBC,        1, -1, "DELETE",     "internal SQL command (data manipulation)",       NULL },
   { ODBCSHELL_CMD_DISCONNECT,  1,  2, "DISCONNECT", "disconnects from a database",                    (const char *[3]){"disconnect", "disconnect name", NULL} },
   { ODBCSHELL_CMD_ODBC,        1, -1, "DROP",       "internal SQL command (data definition)",         NULL },
   { ODBCSHELL_CMD_ECHO,        1, -1, "ECHO",       "prints arguments to screen",                     (const char *[4]){"echo \"string\"", "echo \"string1\" \"string2\"", "echo \"string1\" \"string2\" \"stringN\"", NULL} },
   { ODBCSHELL_CMD_QUIT,        1,  1, "EXIT",       "exits ODBC Shell",                               (const char *[2]){"exit", NULL} },
   { ODBCSHELL_CMD_ODBC,        1, -1, "GRANT",      "internal SQL command (data control)",            NULL },
   { ODBCSHELL_CMD_HELP,        1,  2, "HELP",       "displays help information",                      (const char *[4]){"help", "help topic", "help topic subtopic", NULL} },
   { ODBCSHELL_CMD_ODBC,        1, -1, "INSERT",     "internal SQL command (data manipulation)",       NULL },
   { ODBCSHELL_CMD_ODBC,        1, -1, "MERGE",      "internal SQL command (data manipulation)",       NULL },
   { ODBCSHELL_CMD_OPEN,        1,  2, "OPEN",       "opens file to write results",                    (const char *[3]){"open", "open filename", NULL} },
   { ODBCSHELL_CMD_QUIT,        1,  1, "LOGOUT",     "exits ODBC Shell",                               (const char *[2]){"logout", NULL} },
   { ODBCSHELL_CMD_QUIT,        1,  1, "QUIT",       "exits ODBC Shell",                               (const char *[2]){"quit", NULL} },
   { ODBCSHELL_CMD_RECONNECT,   1,  2, "RECONNECT",  "reconnects to a database",                       (const char *[3]){"reconnect", "reconnect name", NULL} },
   { ODBCSHELL_CMD_RESET,       1,  1, "RESET",      "resets internal configuration",                  (const char *[2]){"reset", NULL} },
   { ODBCSHELL_CMD_ODBC,        1, -1, "REVOKE",     "internal SQL command (data control)",            NULL },
   { ODBCSHELL_CMD_ODBC,        1, -1, "ROLLBACK",   "internal SQL command (transaction controls)",    NULL },
   { ODBCSHELL_CMD_ODBC,        1, -1, "SAVE",       "internal SQL command (transaction controls)",    NULL },
   { ODBCSHELL_CMD_ODBC,        1, -1, "SAVEPOINT",  "internal SQL command (transaction controls)",    NULL },
   { ODBCSHELL_CMD_SEND,        2, -1, "SEND",       "send statement to ODBC data source",             (const char *[2]){"send SQL_statement", NULL} },
   { ODBCSHELL_CMD_SET,         1,  3, "SET",        "sets configuration option",                      (const char *[4]){"set", "set option", "set option value", NULL} },
   { ODBCSHELL_CMD_SETENV,      1,  3, "SETENV",     "displays and sets environment variables",        (const char *[4]){"setenv", "setenv variable", "setenv variable name", NULL} },
   { ODBCSHELL_CMD_ODBC,        1, -1, "SELECT",     "internal SQL command (queries)",                 NULL },
   { ODBCSHELL_CMD_SHOW,        2,  2, "SHOW",       "shows database information",                     (const char *[7]){"show dsn", "show tables", "show qualifiers", "show owners", "show types", "show datatypes", NULL} },
   { ODBCSHELL_CMD_SOURCE,      2,  2, "SOURCE",     "imports odbc script",                            (const char *[2]){"source filename", NULL} },
   { ODBCSHELL_CMD_ODBC,        1, -1, "START",      "internal SQL command (transaction controls)",    NULL },
   { ODBCSHELL_CMD_ODBC,        1, -1, "TRUNCATE",   "internal SQL command (data definition)",         NULL },
   { ODBCSHELL_CMD_UNSET,       2,  2, "UNSET",      "unsets configuration option",                    (const char *[2]){"unset option", NULL} },
   { ODBCSHELL_CMD_UNSETENV,    1,  2, "UNSETENV",   "unsets environment variables",                   (const char *[2]){"unsetenv variable", NULL} },
   { ODBCSHELL_CMD_ODBC,        1, -1, "UPDATE",     "internal SQL command (data manipulation)",       NULL },
   { ODBCSHELL_CMD_USE,         1,  2, "USE",        "switches active connection",                     (const char *[3]){"use", "use name", NULL} },
   { ODBCSHELL_CMD_VERSION,     1,  1, "VERSION",    "displays version information",                   (const char *[2]){"version", NULL} },
   { -1, -1, -1, NULL, NULL, NULL }
};


/// @brief numeric values for configuration options
ODBCShellOption odbcshell_opt_strings[] =
{
   { ODBCSHELL_OPT_CONFFILE,  1,  1, "conffile",   "configuration file used to set initial settings", NULL },
   { ODBCSHELL_OPT_CONTINUE,  1,  1, "continue",   "continue if non-fatal errors are encountered", NULL },
   { ODBCSHELL_OPT_FORMAT,    1,  1, "format",     "output format of results (CSV, Fixed)", NULL },
   { ODBCSHELL_OPT_HISTFILE,  1,  1, "histfile",   "file used for saving command history", NULL },
   { ODBCSHELL_OPT_HISTORY,   1,  1, "history",    "enable history file", NULL },
   { ODBCSHELL_OPT_NOSHELL,   1,  1, "noshell",    "disable calling external programs/scripts", NULL },
   { ODBCSHELL_OPT_ODBCPROMPT,1,  1, "odbcprompt", "allow ODBC driver to prompt for information", NULL },
   { ODBCSHELL_OPT_PROMPT,    1,  1, "prompt",     "prompt used within ODBC Shell", NULL },
   { ODBCSHELL_OPT_SILENT,    1,  1, "silent",     "do not display non-fatal messages", NULL },
   { ODBCSHELL_OPT_VERBOSE,   1,  1, "verbose",    "display verbose messages", NULL },
   { -1, -1, -1, NULL, NULL, NULL }
};


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#ifdef PMARK
#pragma mark -
#pragma mark Functions
#endif

/// @brief looks up option by name
/// @param opts   array of optins to search
/// @param name   name to use as lookup key
ODBCShellOption * odbcshell_lookup_opt_by_name(ODBCShellOption * opts,
   const char * name)
{
   unsigned u;
   if (!(opts))
      return(NULL);
   for(u = 0; opts[u].name; u++)
      if (!(strcasecmp(name, opts[u].name)))
         return(&opts[u]);
   return(NULL);
}


/// @brief looks up option by value
/// @param opts   array of optins to search
/// @param val    value to use as lookup key
ODBCShellOption * odbcshell_lookup_opt_by_value(ODBCShellOption * opts,
   int val)
{
   unsigned u;
   if (!(opts))
      return(NULL);
   for(u = 0; opts[u].name; u++)
      if (val == opts[u].val)
         return(&opts[u]);
   return(NULL);
}

/* end of source */
