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
 *  @file src/odbcshell-print.c ODBC shell print functions
 */
#include "odbcshell-print.h"

///////////////
//           //
//  Headers  //
//           //
///////////////
#ifdef PMARK
#pragma mark Headers
#endif

#include "odbcshell.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>

/////////////////
//             //
//  Functions  //
//             //
/////////////////
#ifdef PMARK
#pragma mark -
#pragma mark Functions
#endif

/// @brief displays error messages
/// @param cnf      pointer to configuration struct
/// @param format   format string for message
/// @param ...      variable arguments for string format
void odbcshell_error(ODBCShell * cnf, const char * format, ...)
{
   va_list ap;

   if ((cnf->silent))
      return;

   fprintf(stderr, "%s: ", PROGRAM_NAME);
   if (cnf->active_cmd)
      fprintf(stderr, "%s: ", cnf->active_cmd->name);
   va_start(ap, format);
      vfprintf(stderr, format, ap);
   va_end(ap);

   return;
}


/// @brief displays fatal error messages
/// @param cnf      pointer to configuration struct
/// @param format   format string for message
/// @param ...      variable arguments for string format
void odbcshell_fatal(ODBCShell * cnf, const char * format, ...)
{
   va_list ap;

   fprintf(stderr, "%s: ", PROGRAM_NAME);
   if (cnf->active_cmd)
      fprintf(stderr, "%s: ", cnf->active_cmd->name);
   va_start(ap, format);
      vfprintf(stderr, format, ap);
   va_end(ap);

   return;
}


/// @brief close file
/// @param cnf      pointer to configuration struct
int odbcshell_fclose(ODBCShell * cnf)
{
   if (cnf->outputfile)
      free(cnf->outputfile);
   cnf->outputfile = NULL;
   if (!(cnf->output))
      return(0);
   fclose(cnf->output);
   cnf->output = NULL;
   return(0);
}


/// @brief open file for writing
/// @param cnf      pointer to configuration struct
/// @param path     file to open for writing
int odbcshell_fopen(ODBCShell * cnf, const char * path)
{
   odbcshell_fclose(cnf);
   if (!(cnf->outputfile = strdup(path)))
   {
      odbcshell_fatal(cnf, "out of virtual memory\n");
      return(-1);
   };
   if (!(cnf->output = fopen(cnf->outputfile, "w")))
   {
      odbcshell_error(cnf, "%s: %s\n", cnf->outputfile, strerror(errno));
      return(-1);
   };
   return(0);
}


/// @brief prints message to file
/// @param cnf      pointer to configuration struct
/// @param format   format string for message
/// @param ...      variable arguments for string format
void odbcshell_fprintf(ODBCShell * cnf, const char * format, ...)
{
   FILE    * fs;
   va_list   ap;

   fs = cnf->output ? cnf->output : stdout;

   va_start(ap, format);
      vfprintf(fs, format, ap);
   va_end(ap);

   return;
}


/// @brief prints message to stdout
/// @param cnf      pointer to configuration struct
/// @param format   format string for message
/// @param ...      variable arguments for string format
void odbcshell_printf(ODBCShell * cnf, const char * format, ...)
{
   va_list ap;

   if ((cnf->silent))
      return;

   va_start(ap, format);
      vprintf(format, ap);
   va_end(ap);

   return;
}


/// @brief displays verbose messages
/// @param cnf      pointer to configuration struct
/// @param format   format string for message
/// @param ...      variable arguments for string format
void odbcshell_verbose(ODBCShell * cnf, const char * format, ...)
{
   va_list ap;

   if (!(cnf->verbose))
      return;

   va_start(ap, format);
      vprintf(format, ap);
   va_end(ap);

   return;
}

/* end of source */
