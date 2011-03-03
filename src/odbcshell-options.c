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
 *  @file src/odbcshell-options.c ODBC Shell options management
 */
#include "odbcshell-options.h"

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

#include "odbcshell-odbc.h"
#include "odbcshell-signal.h"


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark -
#pragma mark Functions

/// frees resources
/// @param[in]  cnf      pointer to configuration struct
void odbcshell_free(ODBCShell * cnf)
{
   if (!(cnf))
      return;

   odbcshell_odbc_close(cnf);

   if (cnf->conffile)
      free(cnf->conffile);
   cnf->conffile = NULL;

   if (cnf->histfile)
      free(cnf->histfile);
   cnf->histfile = NULL;

   if (cnf->prompt)
      free(cnf->prompt);
   cnf->prompt = NULL;

   if (cnf->exec_strs)
      free(cnf->exec_strs);
   cnf->exec_strs = NULL;

   free(cnf);

   return;
}


/// retrieves configuration option
/// @param[in]  cnf      pointer to configuration struct
/// @param[in]  opt      numeric ID of option to retrieve
/// @param[out] ptr      pointer buffer to store value of option
int odbcshell_get_option(ODBCShell * cnf, int opt, void * ptr)
{
   switch(opt)
   {
      case ODBCSHELL_OPT_CONFFILE:
         *((char **)ptr) = NULL;
         if (!(cnf->conffile))
            return(0);
         *((char **)ptr) = cnf->conffile;
         break;
      case ODBCSHELL_OPT_HISTFILE:
         *((char **)ptr) = NULL;
         if (!(cnf->histfile))
            return(0);
         *((char **)ptr) = cnf->histfile;
         break;
      case ODBCSHELL_OPT_PROMPT:
         *((char **)ptr) = NULL;
         if (!(cnf->prompt))
            return(0);
         *((char **)ptr) = cnf->prompt;
         break;
      case ODBCSHELL_OPT_SILENT:
         *((int *)ptr) = (int)cnf->silent;
         break;
      case ODBCSHELL_OPT_VERBOSE:
         *((int *)ptr) = (int)cnf->verbose;
         break;
      default:
         fprintf(stderr, "%s: unknown option\n", PROGRAM_NAME);
         return(-1);
   };
   return(0);
}


/// initializes ODBC Shell options
/// @param[in]  cnfp     pointer to configuration struct
int odbcshell_initialize(ODBCShell ** cnfp)
{
   ODBCShell       * cnf;

   if (!(cnfp))
      return(-1);

   if (!(cnf = malloc(sizeof(ODBCShell))))
   {
      fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
      return(-1);
   };
   memset(cnf, 0, sizeof(ODBCShell));

   odbcshell_signal_init();

   if (odbcshell_set_defaults(cnf))
   {
      odbcshell_free(cnf);
      return(1);
   };

   *cnfp = cnf;

   return(0);
}


// sets default values for configuration options
int odbcshell_set_defaults(ODBCShell * cnf)
{
   odbcshell_odbc_close(cnf);
   if (odbcshell_set_option(cnf, ODBCSHELL_OPT_CONFFILE, NULL)) return(-1);
   if (odbcshell_set_option(cnf, ODBCSHELL_OPT_CONTINUE, NULL)) return(-1);
   if (odbcshell_set_option(cnf, ODBCSHELL_OPT_HISTFILE, NULL)) return(-1);
   if (odbcshell_set_option(cnf, ODBCSHELL_OPT_HISTORY,  NULL)) return(-1);
   if (odbcshell_set_option(cnf, ODBCSHELL_OPT_NOSHELL,  NULL)) return(-1);
   if (odbcshell_set_option(cnf, ODBCSHELL_OPT_PROMPT,   NULL)) return(-1);
   if (odbcshell_set_option(cnf, ODBCSHELL_OPT_SILENT,   NULL)) return(-1);
   if (odbcshell_set_option(cnf, ODBCSHELL_OPT_VERBOSE,  NULL)) return(-1);
   return(0);
}


/// sets configuration option
/// @param[in]  cnf      pointer to configuration struct
/// @param[in]  opt      numeric ID of option to retrieve
/// @param[in]  ptr      pointer buffer containing new value of option
int odbcshell_set_option(ODBCShell * cnf, int opt, const void * ptr)
{
   char   buff[2048];
   switch(opt)
   {
      case ODBCSHELL_OPT_CONFFILE:
         if (cnf->conffile)
            free(cnf->conffile);
         cnf->conffile = NULL;
         if (!(ptr))
            return(0);
         if (!(cnf->conffile = strdup((const char *)ptr)))
         {
            fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
            return(-1);
         };
         break;

      case ODBCSHELL_OPT_CONTINUE:
         if (!(ptr))
            cnf->continues = 0;
         else
            cnf->continues = *((const int *)ptr);
         break;

      case ODBCSHELL_OPT_HISTFILE:
         if (cnf->histfile)
            free(cnf->histfile);
         cnf->histfile = NULL;
         if (!(ptr))
         {
            if (!(getenv("HOME")))
               return(0);
            snprintf(buff, 2048L, "%s/.odbcshell_history", getenv("HOME"));
            ptr = buff;
         };
         if (!(cnf->histfile = strdup((const char *)ptr)))
         {
            fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
            return(-1);
         };
         break;

      case ODBCSHELL_OPT_HISTORY:
         if (!(ptr))
            cnf->history = 1;
         else
            cnf->history = *((const int *)ptr);
         return(0);

      case ODBCSHELL_OPT_NOSHELL:
         if (!(ptr))
            return(0);
         if ( *((const int *)ptr) )
            cnf->noshell = 1;
         return(0);

      case ODBCSHELL_OPT_PROMPT:
         if (cnf->prompt)
            free(cnf->prompt);
         if (!(ptr))
            ptr = "odbcshell> ";
         if (!(cnf->prompt = strdup((const char *)ptr)))
         {
            fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
            return(-1);
         };
         break;

      case ODBCSHELL_OPT_SILENT:
         if (!(ptr))
            cnf->silent = 0;
         else
            cnf->silent = *((const int *)ptr);
         break;

      case ODBCSHELL_OPT_VERBOSE:
         if (!(ptr))
            cnf->verbose = 0;
         else
            cnf->verbose = *((const int *)ptr);
         break;

      default:
         fprintf(stderr, "%s: unknown option\n", PROGRAM_NAME);
         return(-1);
   };
   return(0);
}


/// displays configuration option value
/// @param[in]  cnf      pointer to configuration struct
/// @param[in]  opt      numeric ID of option to retrieve
int odbcshell_show_option(ODBCShell * cnf, int opt)
{
   switch(opt)
   {
      case ODBCSHELL_OPT_CONFFILE:
         printf("%-15s %s\n", "conffile", cnf->conffile  ? cnf->conffile : "");
         break;

      case ODBCSHELL_OPT_CONTINUE:
         printf("%-15s %s\n", "continue", cnf->continues ? "yes" : "no");
         break;

      case ODBCSHELL_OPT_HISTFILE:
         printf("%-15s \"%s\"\n", "histfile", cnf->histfile ? cnf->histfile : "");
         break;

      case ODBCSHELL_OPT_HISTORY:
         printf("%-15s %s\n", "history", cnf->history ? "yes" : "no");
         break;

      case ODBCSHELL_OPT_NOSHELL:
         printf("%-15s %s\n", "noshell", cnf->noshell ? "yes" : "no");
         break;

      case ODBCSHELL_OPT_PROMPT:
         printf("%-15s \"%s\"\n", "prompt", cnf->prompt);
         break;

      case ODBCSHELL_OPT_SILENT:
         printf("%-15s %s\n", "silent", cnf->silent ? "yes" : "no");
         break;

      case ODBCSHELL_OPT_VERBOSE:
         printf("%-15s %s\n", "verbose", cnf->verbose ? "yes" : "no");
         break;

      default:
         break;
   };
   return(0);
}


/// converts a string to a boolean value
/// @param[in]  str      string to convert
int odbcshell_strtob(const char * str)
{
   if (!(strcasecmp(str, "TRUE")))
      return(1);
   if (!(strcasecmp(str, "YES")))
      return(1);
   if (!(strcasecmp(str, "ON")))
      return(1);
   if (!(strcasecmp(str, "1")))
      return(1);
   if (!(strcasecmp(str, "FALSE")))
      return(0);
   if (!(strcasecmp(str, "NO")))
      return(0);
   if (!(strcasecmp(str, "OFF")))
      return(0);
   if (!(strcasecmp(str, "0")))
      return(0);
   return(-1);
}

/* end of source */
