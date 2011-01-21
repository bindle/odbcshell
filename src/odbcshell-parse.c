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
 *  @file src/odbcshell-parse.c ODBC Shell command parser
 */
#include "odbcshell-parse.h"

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
//  Variables  //
//             //
/////////////////
#pragma mark -
#pragma mark Variables


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark -
#pragma mark Functions

/// splits a line into multiple arguments
/// @param[in]  line
/// @param[out] argcp
/// @param[out] argvp
/// @param[out] eolp
int odbcshell_parse_line(const char * line, int * argcp, char *** argvp,
   size_t * eolp)
{
   int       i;
   char    * arg;
   char    * str;    // mutable buffer
   void    * ptr;
   size_t    len;    // line length
   size_t    pos;    // position within line
   size_t    start;  // start of argument within line
   size_t    arglen; // length of argument

   if ( (!(line)) || (!(argcp)) || (!(argvp)) || (!(eolp)) )
   {
      fprintf(stderr, "%s: internal error\n", PROGRAM_NAME);
      return(-1);
   };

   if (!(len = strlen(line)))
      return(0);

   if (!(str = strdup(line)))
   {
      fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
      return(-1);
   };


   // frees old argc/argv data
   for(i = 0; i < *argcp; i++)
      free((*argvp)[i]);
   *argcp = 0;
   *eolp  = 0;

   for(pos = 0; pos < len; pos++)
   {
      arg = NULL;
      switch(str[pos])
      {
         // exit if end of command is found
         case ';':
            *eolp = pos;
            free(str);
            return(0);

         // skip white space between arguments
         case ' ':
         case '\t':
            break;

         case '\r':
         case '\n':
            str[pos] = ' ';
            break;

         // skip comments
         case '#':
            while((str[pos] != '\n') && (str[pos] != '\r') && (pos < len))
               pos++;
            if (pos >= len)
            {
               *eolp = pos;
               free(str);
               return(0);
            };
            break;

         // processes arguments contained within single quotes
         case '\'':
            start = pos + 1;
            pos += 2;
            while((str[pos] != '\'') && (pos < len))
               pos++;
            if (pos >= len)
            {
               free(str);
               return(0);
            };
            arglen = pos - start;
            if (!(arg = malloc(arglen)))
            {
               fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
               return(1);
            };
            memset(arg, 0, arglen);
            strncat(arg, &line[start], arglen);
            break;

         // processes arguments contained within double quotes
         case '"':
            start = pos + 1;
            pos += 2;
            while((str[pos] != '"') && (pos < len))
               pos++;
            if (pos >= len)
            {
               free(str);
               return(0);
            };
            arglen = pos - start;
            if (!(arg = malloc(arglen)))
            {
               fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
               return(1);
            };
            memset(arg, 0, arglen);
            strncat(arg, &line[start], arglen);
            break;

         // processes unquoted arguments
         default:
            start = pos;
            while ( ((pos+1) < len) &&
                    (str[pos+1] != ';') &&
                    (str[pos+1] != '#') &&
                    (str[pos+1] != ' ') &&
                    (str[pos+1] != '\t') &&
                    (str[pos+1] != '\n') &&
                    (str[pos+1] != '\r') )
               pos++;
            arglen = 1 + pos - start;
            if (!(arg = malloc(arglen)))
            {
               fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
               return(1);
            };
            memset(arg, 0, arglen);
            strncat(arg, &line[start], arglen);
            break;
      };

      // if an argument is not defined, skip to next character in string.
      if (!(arg))
      {
         continue;
      };

      if (!(ptr = realloc(*argvp, sizeof(char *) * ((*argcp)+1))))
      {
         fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
         return(1);
      };
      *argvp = ptr;
      (*argvp)[(*argcp)] = arg;
      (*argcp)++;
   };

   return(0);
}


/* end of source */
