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
 *  @file src/odbc-shell.c ODBC command line client
 */
/*
 *  Simple Build:
 *     gcc -W -Wall -O2 -c odbc-shell.c
 *     gcc -W -Wall -O2 -lodbc -o odbcshell odbc-shell.o
 *
 *  GNU Libtool Build:
 *     libtool --mode=compile gcc -W -Wall -g -O2 -c odbc-shell.c
 *     libtool --mode=link    gcc -W -Wall -g -O2 -o odbc-shell odbc-shell.lo
 *
 *  GNU Libtool Install:
 *     libtool --mode=install install -c odbcshell /usr/local/bin/odbcshell
 *
 *  GNU Libtool Clean:
 *     libtool --mode=clean rm -f odbc-shell.lo odbcshell
 */

///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark Headers

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include <readline/readline.h>
#include <readline/history.h>

#include <sql.h>
#include <sqlext.h>
#include <sqlucode.h>
#include <iodbcext.h>


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#pragma mark -
#pragma mark Definitions & Macros

#ifndef PROGRAM_NAME
#define PROGRAM_NAME "odbcshell"
#endif

#ifndef PACKAGE_BUGREPORT
#define PACKAGE_BUGREPORT "syzdek@bindlebinaries.com"
#endif

#ifndef PACKAGE_NAME
#define PACKAGE_NAME "ODBC Shell"
#endif

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION ""
#endif


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark -
#pragma mark Prototypes

// displays usage
void odbcshell_usage(void);

// displays version information
void odbcshell_version(void);

// main statement
int main(int argc, char * argv[]);


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark -
#pragma mark Functions


/// displays usage
void odbcshell_usage(void)
{
   printf(("Usage: %s [OPTIONS]\n"
         "  -h, --help                print this help and exit\n"
         "  -q, --quiet, --silent     do not print messages\n"
         "  -V, --version             print version number and exit\n"
         "  -v, --verbose             print verbose messages\n"
         "\n"
         "Report bugs to <%s>.\n"
      ), PROGRAM_NAME, PACKAGE_BUGREPORT
   );
   return;
}


/// displays version information
void odbcshell_version(void)
{
   printf(("%s (%s) %s\n"
         "Written by David M. Syzdek.\n"
         "\n"
         "Copyright 2011 Bindle Binaries <syzdek@bindlebinaries.com>.\n"
         "This is free software; see the source for copying conditions.  There is NO\n"
         "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"
      ), PROGRAM_NAME, PACKAGE_NAME, PACKAGE_VERSION
   );
   return;
}


/// main statement
/// @param[in]  argc  number of arguments passed to program
/// @param[in]  argv  array of arguments passed to program
int main(int argc, char * argv[])
{
   int           c;
   int           opt_index;

   static char   short_opt[] = "hqVv";
   static struct option long_opt[] =
   {
      {"help",          no_argument, 0, 'h'},
      {"silent",        no_argument, 0, 'q'},
      {"quiet",         no_argument, 0, 'q'},
      {"verbose",       no_argument, 0, 'v'},
      {"version",       no_argument, 0, 'V'},
      {NULL,            0,           0, 0  }
   };

   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:	/* no more arguments */
         case 0:	/* long options toggles */
            break;
         case 'h':
            odbcshell_usage();
            return(0);
         case 'q':
            break;
         case 'V':
            odbcshell_version();
            return(0);
         case 'v':
            break;
         case '?':
            fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
            return(1);
         default:
            fprintf(stderr, "%s: unrecognized option `--%c'\n", PROGRAM_NAME, c);
            fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
            return(1);
      };
   };

   return(0);
}

/* end of source */
