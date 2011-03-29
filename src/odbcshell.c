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
 *  @file src/odbcshell.c ODBC command line client
 */
/*
 *  Simple Build:
 *     gcc -W -Wall -O2 -c odbcshell.c
 *     gcc -W -Wall -O2 -lodbc -o odbcshell odbcshell.o
 *
 *  GNU Libtool Build:
 *     libtool --mode=compile gcc -W -Wall -g -O2 -c odbcshell.c
 *     libtool --mode=link    gcc -W -Wall -g -O2 -o odbcshell odbcshell.lo
 *
 *  GNU Libtool Install:
 *     libtool --mode=install install -c odbcshell /usr/local/bin/odbcshell
 *
 *  GNU Libtool Clean:
 *     libtool --mode=clean rm -f odbcshell.lo odbcshell
 */

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
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "odbcshell-cli.h"
#include "odbcshell-commands.h"
#include "odbcshell-exec.h"
#include "odbcshell-options.h"
#include "odbcshell-odbc.h"
#include "odbcshell-print.h"
#include "odbcshell-profile.h"
#include "odbcshell-script.h"


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#ifdef PMARK
#pragma mark -
#pragma mark Prototypes
#endif

// displays usage
void odbcshell_usage(void);

// main statement
int main(int argc, char * argv[]);


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#ifdef PMARK
#pragma mark -
#pragma mark Functions
#endif


/// @brief displays usage
void odbcshell_usage(void)
{
   printf(("Usage: %s [OPTIONS]\n"
         "  -c                        continue if error is encoutered\n"
         "  -D dsn                    connect to DSN\n"
         "  -e sql                    execute SQL statement\n"
         "  -h, --help                print this help and exit\n"
         "  -l                        print list of DSN\n"
         "  -N, --noprofile           disables reading .odbcshellrc\n"
         "  -n                        disables prompting by driver\n"
         "  -o file                   file to write output\n"
         "  -q, --quiet, --silent     do not print messages\n"
         "  -s data                   print database information\n"
         "  -V, --version             print version number and exit\n"
         "  -v, --verbose             print verbose messages\n"
         "\n"
         "Report bugs to <%s>.\n"
      ), PROGRAM_NAME, PACKAGE_BUGREPORT
   );
   return;
}


/// @brief displays version information
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


/// @brief main statement
/// @param argc  number of arguments passed to program
/// @param argv  array of arguments passed to program
/// @return exit code of program
int main(int argc, char * argv[])
{
   int           c;
   int           ival;
   int           sts;
   int           opt_index;
   ODBCShell   * cnf;

   static char   short_opt[] = "cD:e:hlNno:qs:Vv";
   static struct option long_opt[] =
   {
      {"help",          no_argument, 0, 'h'},
      {"noprofile",     no_argument, 0, 'N'},
      {"silent",        no_argument, 0, 'q'},
      {"quiet",         no_argument, 0, 'q'},
      {"verbose",       no_argument, 0, 'v'},
      {"version",       no_argument, 0, 'V'},
      {NULL,            0,           0, 0  }
   };

   if (ODBCVER < 0x0300)
   {
      fprintf(stderr, "%s: unsupported version of iODBC\n", PROGRAM_NAME);
      return(1);
   };

   if ((odbcshell_initialize(&cnf)))
      return(1);

   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:	/* no more arguments */
         case 0:	/* long options toggles */
            break;
         case 'c':
            ival = 1;
            odbcshell_set_option(cnf, ODBCSHELL_OPT_CONTINUE, &ival);
            break;
         case 'D':
            cnf->dflt_dsn = optarg;
            break;
         case 'e':
            if (((cnf->mode)) && (cnf->mode != ODBCSHELL_MODE_EXEC))
            {
               fprintf(stderr, "%s: incompatible options `-e', `-l' and `-s'\n", PROGRAM_NAME);
               fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
               return(1);
            };
            cnf->mode = ODBCSHELL_MODE_EXEC;
            if (odbcshell_exec_append_str(cnf, optarg))
               return(1);
            break;
         case 'h':
            odbcshell_usage();
            return(0);
         case 'l':
            if (((cnf->mode)) && (cnf->mode != ODBCSHELL_MODE_LISTDSN))
            {
               fprintf(stderr, "%s: incompatible options `-e', `-l' and `-s'\n", PROGRAM_NAME);
               fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
               return(1);
            };
            cnf->mode = ODBCSHELL_MODE_LISTDSN;
            break;
         case 'N':
            cnf->noprofile = 1;
            break;
         case 'n':
            ival = 0;
            odbcshell_set_option(cnf, ODBCSHELL_OPT_ODBCPROMPT, &ival);
            break;
         case 'o':
            cnf->dflt_output = optarg;
            break;
         case 'q':
            ival = 1;
            odbcshell_set_option(cnf, ODBCSHELL_OPT_SILENT, &ival);
            break;
         case 's':
            if (((cnf->mode)) && (cnf->mode != ODBCSHELL_MODE_SHOW))
            {
               fprintf(stderr, "%s: incompatible options `-e', `-l' and `-s'\n", PROGRAM_NAME);
               fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
               return(1);
            };
            cnf->mode = ODBCSHELL_MODE_SHOW;
            cnf->dflt_show = optarg;
            break;
         case 'V':
            odbcshell_version();
            return(0);
         case 'v':
            ival = 1;
            odbcshell_set_option(cnf, ODBCSHELL_OPT_VERBOSE, &ival);
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

   if (!(cnf->mode))
      cnf->mode = ODBCSHELL_MODE_SHELL;
   if (cnf->mode == ODBCSHELL_MODE_SHELL)
      if (optind < argc)
         cnf->mode = ODBCSHELL_MODE_SCRIPT;

   if ((cnf->dflt_output))
   {
      if ((odbcshell_fopen(cnf, cnf->dflt_output)))
      {
         odbcshell_free(cnf);
         return(1);
      };
   };

   if ((odbcshell_odbc_initialize(cnf)))
   {
      odbcshell_free(cnf);
      return(1);
   };

   if ((odbcshell_profile(cnf)))
   {
      odbcshell_free(cnf);
      return(1);
   };

   sts = 0;
   switch(cnf->mode)
   {
      case ODBCSHELL_MODE_SCRIPT:
         if (cnf->dflt_dsn)
            if (odbcshell_odbc_connect(cnf, cnf->dflt_dsn, NULL))
               return(1);
         for(c = optind; c < argc; c++)
            if (odbcshell_script_loop(cnf, argv[c]))
               return(1);
         break;

      case ODBCSHELL_MODE_EXEC:
         sts = odbcshell_exec_loop(cnf);
         break;

      case ODBCSHELL_MODE_LISTDSN:
         sts = odbcshell_odbc_show_dsn(cnf);
         break;

      case ODBCSHELL_MODE_SHOW:
         if (cnf->dflt_dsn)
            if (odbcshell_odbc_connect(cnf, cnf->dflt_dsn, NULL))
               return(1);
         sts = odbcshell_cmd_show(cnf, cnf->dflt_show);
         break;

      case ODBCSHELL_MODE_SHELL:
      default:
         c = 1;
         odbcshell_set_option(cnf, ODBCSHELL_OPT_CONTINUE, &c);
         sts = odbcshell_cli_loop(cnf);
         break;
   };

   odbcshell_free(cnf);

   return(sts);
}

/* end of source */
