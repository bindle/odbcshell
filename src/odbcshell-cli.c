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
 *  @file src/odbcshell-cli.c ODBC Shell command line interface
 */
#include "odbcshell-cli.h"

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
#include <readline/readline.h>
#include <readline/history.h>

#include "odbcshell-parse.h"
#include "odbcshell-variables.h"


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark -
#pragma mark Prototypes


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark -
#pragma mark Functions

/// master loop for interactive shell
/// @param[in]  cnf      pointer to configuration struct
int odbcshell_cli_loop(ODBCShellConfig * cnf)
{
   char        * ptr;
   char        * input;
   char        * buffer;
   char       ** argv;
   int           argc;
   size_t        offset;
   size_t        bufflen;
   ODBCShellOption * cmd;

   buffer = NULL;

   if (!(cnf))
      return(1);

   using_history();
   if (cnf->histfile)
      read_history(cnf->histfile);

   if (!(cnf->silent))
   {
      printf("Welcome to ODBC Shell v%s. Commands end with ';'.\n\n", PACKAGE_VERSION);
      printf("Type \"help;\" for usage information.\n\n");
   };

   argv   = NULL;
   argc   = 0;
   buffer = strdup("");

   while((input = readline((!(buffer[0])) ? cnf->prompt : "> ")))
   {
      if (strlen(input))
      {
         ptr = realloc(buffer, (strlen(buffer) + strlen(input) + 2));
         buffer = ptr;
         if ((bufflen = strlen(buffer)))
            if ( (buffer[bufflen-1] != ' ') && (buffer[bufflen-1] != '\t') )
            strcat(buffer, " ");
         strcat(buffer, input);
         free(input);
      } else {
         free(input);
         continue;
      };

      if (odbcshell_parse_line(buffer, &argc, &argv, &offset))
      {
         free(buffer);
         return(1);
      };

      if (!(offset))
         continue;

      if (!(argc))
      {
         buffer[0] = '\0';
         continue;
      };

      add_history(buffer);

      if (!(cmd = odbcshell_lookup_opt_by_name(odbcshell_cmd_strings, argv[0])))
      {
         buffer[0] = '\0';;
         continue;
      };

      if (cmd->min_arg > argc)
      {
         printf("error: invalid syntax for %s\n", argv[0]);
         buffer[0] = '\0';
         continue;
      };
      if ( (cmd->max_arg < argc) && (cmd->max_arg != -1) )
      {
         printf("error: invalid syntax for %s\n", argv[0]);
         buffer[0] = '\0';
         continue;
      };

      switch(cmd->val)
      {
         case ODBCSHELL_CMD_QUIT:
            if (cnf->histfile)
               write_history(cnf->histfile);
            if (!(cnf->silent))
               printf("bye.\n");
            return(0);
         default:
            printf(">>>>: %s\n", buffer);
            break;
      };

      buffer[0] = '\0';;
   };

   return(0);
}

/* end of source */
