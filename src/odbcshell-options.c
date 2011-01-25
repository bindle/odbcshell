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

/// frees resources
/// @param[in]  cnf      pointer to configuration struct
void odbcshell_free(ODBCShell * cnf)
{
   if (!(cnf))
      return;

   if (cnf->conffile)
      free(cnf->conffile);
   cnf->conffile = NULL;

   if (cnf->histfile)
      free(cnf->histfile);
   cnf->histfile = NULL;

   if (cnf->prompt)
      free(cnf->prompt);
   cnf->prompt = NULL;

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
         *((int *)ptr) = cnf->silent;
         break;
      case ODBCSHELL_OPT_VERBOSE:
         *((int *)ptr) = cnf->verbose;
         break;
      default:
         fprintf(stderr, "%s: unknown option\n", PROGRAM_NAME);
         return(1);
   };
   return(0);
}


/// initializes ODBC Shell options
/// @param[in]  cnfp     pointer to configuration struct
int odbcshell_initialize(ODBCShell ** cnfp)
{
   //int               ival;
   char              ibuff[2048];
   //size_t            len;
   ODBCShell       * cnf;

   if (!(cnfp))
      return(1);

   if (!(cnf = malloc(sizeof(ODBCShell))))
   {
      fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
      return(1);
   };
   memset(cnf, 0, sizeof(ODBCShell));

   if (getenv("HOME"))
   {
      snprintf(ibuff, 2048L, "%s/.odbcshell_history", getenv("HOME"));
      if (odbcshell_set_option(cnf, ODBCSHELL_OPT_HISTFILE, ibuff))
         return(1);
   };

   *cnfp = cnf;

   return(0);
}


/// sets configuration option
/// @param[in]  cnf      pointer to configuration struct
/// @param[in]  opt      numeric ID of option to retrieve
/// @param[in]  ptr      pointer buffer containing new value of option
int odbcshell_set_option(ODBCShell * cnf, int opt, void * ptr)
{
   switch(opt)
   {
      case ODBCSHELL_OPT_CONFFILE:
         if (cnf->conffile)
            free(cnf->conffile);
         if (!(cnf->conffile = strdup((char *)ptr)))
         {
            fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
            return(1);
         };
         break;
      case ODBCSHELL_OPT_CONTINUE:
         cnf->continues = *((int *)ptr);
         break;
      case ODBCSHELL_OPT_HISTFILE:
         if (cnf->histfile)
            free(cnf->histfile);
         if (!(cnf->histfile = strdup((char *)ptr)))
         {
            fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
            return(1);
         };
         break;
      case ODBCSHELL_OPT_HISTORY:
         cnf->history = *((int *)ptr);
         break;
      case ODBCSHELL_OPT_NOSHELL:
         cnf->noshell = 1;
         break;
      case ODBCSHELL_OPT_PROMPT:
         if (cnf->prompt)
            free(cnf->prompt);
         if (!(cnf->prompt = strdup((char *)ptr)))
         {
            fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
            return(1);
         };
         break;
      case ODBCSHELL_OPT_SILENT:
         cnf->silent = *((int *)ptr);
         break;
      case ODBCSHELL_OPT_VERBOSE:
         cnf->verbose = *((int *)ptr);
         break;
      default:
         fprintf(stderr, "%s: unknown option\n", PROGRAM_NAME);
         return(1);
   };
   return(0);
}

/* end of source */
