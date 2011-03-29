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
 *  @file src/odbcshell-profile.c loads RC files for ODBC Shell
 */
#include "odbcshell-profile.h"

///////////////
//           //
//  Headers  //
//           //
///////////////
#ifdef PMARK
#pragma mark Headers
#endif

#include "odbcshell.h"

#include <sys/stat.h>
#include <stdlib.h>

#include "odbcshell-script.h"


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#ifdef PMARK
#pragma mark -
#pragma mark Definitions & Macros
#endif

#ifndef SYSCONFDIR
#define SYSCONFDIR "/usr/local/etc"
#endif


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#ifdef PMARK
#pragma mark -
#pragma mark Functions
#endif

/// @brief loads system, user, and local profiles for ODBC Shell
/// @param cnf      pointer to configuration struct
int odbcshell_profile(ODBCShell * cnf)
{
   int           sts;
   struct stat   sb;
   char          filename[1024];
   const char  * home;

   if ((cnf->noprofile))
      return(0);

   if (cnf->mode == ODBCSHELL_MODE_SCRIPT)
      return(0);

   home = getenv("HOME");
   home = home ? home : "/dev/null";

   // process /etc/odbcshellrc
   if (!(stat(SYSCONFDIR "/odbcshell_profile", &sb)))
   {
      if ((sts = odbcshell_script_loop(cnf, (SYSCONFDIR "/odbcshellrc"))))
         return(sts);
   };

   // process ~/odbcshellrc
   filename[1023] = '\0';
   snprintf(filename, 1023, "%s/%s", home, ".odbcshell_profile");
   if (!(stat(filename, &sb)))
   {
      if ((sts = odbcshell_script_loop(cnf, filename)))
         return(sts);
      return(0);
   };

   // process ./.odbcshellrc
   if (!(stat(".odbcshellrc", &sb)))
   {
      if ((sts = odbcshell_script_loop(cnf, ".odbcshell_profile")))
         return(sts);
      return(0);
   };

   return(0);
}

/* end of source */
