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
 *  @file src/odbcshell-commands.c ODBC Shell commands
 */
#include "odbcshell-commands.h"

///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark Headers

#include "odbcshell.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "odbcshell-commands.h"
#include "odbcshell-options.h"
#include "odbcshell-print.h"
#include "odbcshell-variables.h"
#include "odbcshell-odbc.h"


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark -
#pragma mark Functions

/// clears the screen
int odbcshell_cmd_clear(void)
{
   printf("\033[2J\033[1;1H");
   return(0);
}


/// connects to database
/// @param[in]  cnf      pointer to configuration struct
/// @param[in]  argc     number of arguments passed to command
/// @param[in]  argv     array of arguments passed to command
int odbcshell_cmd_connect(ODBCShell * cnf, int argc, char ** argv)
{
   if (argc == 3)
      return(odbcshell_odbc_connect(cnf, argv[2], argv[1]));
   return(odbcshell_odbc_connect(cnf, argv[1], NULL));
}


/// disconnects from database
/// @param[in]  cnf      pointer to configuration struct
/// @param[in]  argc     number of arguments passed to command
/// @param[in]  argv     array of arguments passed to command
int odbcshell_cmd_disconnect(ODBCShell * cnf, int argc, char ** argv)
{
   if (argc == 2)
      return(odbcshell_odbc_disconnect(cnf, argv[1]));
   return(odbcshell_odbc_disconnect(cnf, NULL));
}


/// displays ODBC data sources
/// @param[in]  cnf      pointer to configuration struct
int odbcshell_cmd_dsn(ODBCShell * cnf)
{
   return(odbcshell_odbc_list_dsn(cnf));
}


/// prints strings to screen
/// @param[in]  cnf      pointer to configuration struct
/// @param[in]  argc     number of arguments passed to command
/// @param[in]  argv     array of arguments passed to command
int odbcshell_cmd_echo(ODBCShell * cnf, int argc, char ** argv)
{
   int i;
   if (!(cnf))
      return(0);
   if (argc < 2)
   {
      printf("\n");
      return(0);
   };
   for(i = 1; i < argc; i++)
      printf("%s\n", argv[i]);
   return(0);
}


/// executes SQL statement
/// @param[in]  cnf      pointer to configuration struct
/// @param[in]  cnf      pointer to configuration struct
int odbcshell_cmd_exec(ODBCShell * cnf, char * sql)
{
   return(odbcshell_odbc_exec(cnf, sql));
}


/// displays usage information
/// @param[in]  cnf      pointer to configuration struct
/// @param[in]  argc     number of arguments passed to command
/// @param[in]  argv     array of arguments passed to command
int odbcshell_cmd_help(ODBCShell * cnf, int argc, char ** argv)
{
   unsigned          u;
   ODBCShellOption * cmd;

   if (!(cnf))
      return(0);
   if (!(argv))
      return(0);

   // prints list of help topics
   if (argc == 1)
   {
      printf("This is %s version %s\n", PROGRAM_NAME, PACKAGE_VERSION);
      printf("Topics:\n");
      for(u = 0; odbcshell_cmd_strings[u].name; u++)
      {
         if ((u%5) == 0)
            printf("   ");
         printf("%-12s", odbcshell_cmd_strings[u].name);
         if ((u%5) == 4)
            printf("\n");
      };
      if ((u%5) != 0)
         printf("\n");

      printf("For more info use \"HELP <topic>\".\n");
      return(0);
   };

   if (!(cmd = odbcshell_lookup_opt_by_name(odbcshell_cmd_strings, argv[1])))
   {
      printf("HELP topic \"%s\" unknown.\n", argv[1]);
      return(0);
   };
   if (!(cmd->name))
   {
      printf("HELP topic \"%s\" unknown.\n", argv[1]);
      return(0);
   };

   printf("Command:\n   %s\n", cmd->name);
   if (cmd->desc)
   {
      printf("Description:\n");
      printf("   %s\n", cmd->desc);
   };
   if (cmd->usage)
   {
      printf("Usage:\n");
      for(u = 0; cmd->usage[u]; u++)
         printf("   %s%s\n", cmd->name, cmd->usage[u]);
   };
   if ( (!(cmd->usage)) && (!(cmd->desc)) )
      printf("   Help information for this topic is unavailable.\n");

   return(0);
}


/// displays information stating the function is incomplete
/// @param[in]  cnf      pointer to configuration struct
/// @param[in]  argc     number of arguments passed to command
/// @param[in]  argv     array of arguments passed to command
/// @param[in]  line     unmodified line sent to ODBC shell
int odbcshell_cmd_incomplete(ODBCShell * cnf, int argc, char ** argv)
{
   printf("WARNING: \"%s\" is not implemented.\n", argv[0]);
   if ( (!(argc)) || (!(cnf)) || (!(argv)) )
      return(0);
   return(0);
}


/// reconnects to a database
/// @param[in]  cnf      pointer to configuration struct
/// @param[in]  argc     number of arguments passed to command
/// @param[in]  argv     array of arguments passed to command
int odbcshell_cmd_reconnect(ODBCShell * cnf, int argc, char ** argv)
{
   if (argc == 2)
      return(odbcshell_odbc_reconnect(cnf, argv[1]));
   return(odbcshell_odbc_reconnect(cnf, NULL));
}


/// resets internal configuration
/// @param[in]  cnf      pointer to configuration struct
int odbcshell_cmd_resetconf(ODBCShell * cnf)
{
   return(odbcshell_set_defaults(cnf));
}


/// sets internal value of configuration parameter
/// @param[in]  cnf      pointer to configuration struct
/// @param[in]  argc     number of arguments passed to command
/// @param[in]  argv     array of arguments passed to command
int odbcshell_cmd_set(ODBCShell * cnf, int argc, char ** argv)
{
   int               i;
   int               ival;
   ODBCShellOption * opt;

   if (argc < 2)
   {
      printf("ODBC Shell Parameters:\n");
      odbcshell_show_option(cnf, ODBCSHELL_OPT_CONTINUE);
      odbcshell_show_option(cnf, ODBCSHELL_OPT_HISTFILE);
      odbcshell_show_option(cnf, ODBCSHELL_OPT_HISTORY);
      odbcshell_show_option(cnf, ODBCSHELL_OPT_NOSHELL);
      odbcshell_show_option(cnf, ODBCSHELL_OPT_PROMPT);
      odbcshell_show_option(cnf, ODBCSHELL_OPT_SILENT);
      odbcshell_show_option(cnf, ODBCSHELL_OPT_VERBOSE);
      return(0);
   };

   if (!(strcasecmp(argv[1], "help")))
   {
      if (argc >= 3)
      {
         if (!(opt = odbcshell_lookup_opt_by_name(odbcshell_opt_strings, argv[2])))
         {
            printf("%s: set %s: unknown option\n", PROGRAM_NAME, argv[2]);
            return(-1);
         };
         printf("%-15s", opt->name);
         if (opt->desc)
            printf(" %s", opt->desc);
         printf("\n");
         return(0);
      };
      printf("ODBC Shell Parameter Descriptions:\n");
      for(i = 0; odbcshell_opt_strings[i].name; i++)
      {
         printf("%-15s", odbcshell_opt_strings[i].name);
         if (odbcshell_opt_strings[i].desc)
            printf(" %s", odbcshell_opt_strings[i].desc);
         printf("\n");
      };
      return(0);
   };

   if (!(opt = odbcshell_lookup_opt_by_name(odbcshell_opt_strings, argv[1])))
   {
      printf("%s: set %s: unknown option\n", PROGRAM_NAME, argv[1]);
      return(-1);
   };

   if (argc < 3)
   {
      odbcshell_show_option(cnf, (int)opt->val);
      return(0);
   };

   switch(opt->val & ODBSHELL_OTYPE_MASK)
   {
      case ODBSHELL_OTYPE_BOOL:
         ival = odbcshell_strtob(argv[2]);
         return(odbcshell_set_option(cnf, (int)opt->val, &ival));

      case ODBSHELL_OTYPE_CHAR:
         return(odbcshell_set_option(cnf, (int)opt->val, argv[2]));

      case ODBSHELL_OTYPE_INT:
         ival = (int)strtol(argv[2], NULL, 0);
         return(odbcshell_set_option(cnf, (int)opt->val, &ival));

      default:
         return(0);
   };

   return(0);
}


// unsets internal value of configuration parameter
int odbcshell_cmd_unset(ODBCShell * cnf, char ** argv)
{
   ODBCShellOption * opt;

   if (!(opt = odbcshell_lookup_opt_by_name(odbcshell_opt_strings, argv[1])))
      return(-1);

   return(odbcshell_set_option(cnf, (int)opt->val, NULL));
}


/// switches active database connection
/// @param[in]  cnf      pointer to configuration struct
/// @param[in]  argc     number of arguments passed to command
/// @param[in]  argv     array of arguments passed to command
int odbcshell_cmd_use(ODBCShell * cnf, int argc, char ** argv)
{
   if (argc == 2)
      odbcshell_odbc_use(cnf, argv[1]);
   else
      odbcshell_odbc_use(cnf, NULL);
   return(0);
}


/// exits from shell
/// @param[in]  cnf      pointer to configuration struct
int odbcshell_cmd_quit(ODBCShell * cnf)
{
   if (!(cnf))
      return(-1);

   odbcshell_odbc_close(cnf);

   odbcshell_printf(cnf, "bye.\n");

   return(1);
}


/// displays version information
/// @param[in]  cnf      pointer to configuration struct
int odbcshell_cmd_version(ODBCShell * cnf)
{
   if (!(cnf))
      return(0);

   printf("%s (%s) %s\n", PROGRAM_NAME, PACKAGE_NAME, PACKAGE_VERSION);
   odbcshell_odbc_version(cnf);

   return(0);
}

/* end of source */

