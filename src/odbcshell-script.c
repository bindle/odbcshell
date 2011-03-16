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
 *  @file src/odbcshell-script.c ODBC Shell script interface
 */
#include "odbcshell-script.h"

///////////////
//           //
//  Headers  //
//           //
///////////////
#ifdef PMARK
#pragma mark Headers
#endif

#include "odbcshell.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "odbcshell-commands.h"
#include "odbcshell-odbc.h"
#include "odbcshell-parse.h"
#include "odbcshell-print.h"
#include "odbcshell-variables.h"


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#ifdef PMARK
#pragma mark -
#pragma mark Functions
#endif

/// master loop for interactive shell
/// @param[in]  cnf      pointer to configuration struct
int odbcshell_script_loop(ODBCShell * cnf, const char * script)
{
   int       fd;
   int       code;
   char      buff[4096];
   ssize_t    offset;
   ssize_t    len;
   ssize_t    pos;

   if (!(cnf))
      return(-1);

   offset = 0;

   if ((fd = open(script, O_RDONLY)) == -1)
   {
      odbcshell_error(cnf, "%s: open(%s)\n", script, strerror(errno));
      return(-1);
   };

   while((len = read(fd, &buff[offset], 4095-offset)) > 0)
   {
      buff[len] = '\0';

      switch((code = odbcshell_interpret_buffer(cnf, buff, 0L, &offset)))
      {
         case 1:
            code = 0;
         case -1:
            if ( (!(code)) || (!(cnf->continues)) )
            {
               close(fd);
               return(code);
            };
         case 2:
            continue;
         default:
            break;
      };

      for(pos = 0; pos < (1 + len - offset); pos++)
         buff[pos] = buff[1+offset+pos];
      buff[pos] = '\0';
      offset = len - offset;
   };

   if (len == -1)
   {
      odbcshell_error(cnf, "%s: read(): %s\n", script, strerror(errno));
      close(fd);
      return(-1);
   };

   close(fd);

   return(0);
}

/* end of source */
