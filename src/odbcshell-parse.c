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
#ifdef PMARK
#pragma mark Headers
#endif

#include "odbcshell.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "odbcshell-commands.h"
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

/// @brief expands variables within strings
/// @param  cnf      pointer to configuration struct
/// @param[out] strp     pointer string to expand
int odbcshell_expand_string(ODBCShell * cnf, char ** strp)
{
   size_t       old_len;
   size_t       new_len;
   size_t       pos;
   size_t       start;
   size_t       offset;
   void       * ptr;
   char       * str;
   const char * value;

   old_len = strlen(*strp);
   new_len = old_len + 1;

   if (!(str = malloc(new_len)))
   {
      odbcshell_fatal(cnf, "out of virtual memory\n");
      return(-2);
   };

   // loops through string looking for variables to expand
   offset = 0;
   for(pos = 0; pos < old_len; pos++)
   {
      switch((*strp)[pos])
      {
         // inserts variable into string
         case '$':
            // check for false variable start
            pos++;
            if ((pos >= old_len) || ((*strp)[pos] != '{'))
            {
               str[offset+0] = (*strp)[pos-1];
               str[offset+1] = (*strp)[pos-0];
               offset += 2;
               break;
            };
            pos++;

            // NULL terminates end of variable name
            for(start = pos; ((pos < old_len) && ((*strp)[pos] != '}')); pos++);
            (*strp)[pos] = '\0';

            // retrieves variable's value
            value = getenv(&(*strp)[start]);
            value = value ? value : "";

            // increases size of buffer to make space for variable's value
            new_len += strlen(value);
            if (!(ptr = realloc(str, new_len)))
            {
               free(str);
               odbcshell_fatal(cnf, "out of virtual memory\n");
               return(-2);
            };
            str = ptr;

            // copies variable value into buffer
            str[offset] = '\0';
            strcat(str, value);
            offset += strlen(value);
            break;

         // handles escaped character
         case '\\':
            pos++;
            str[offset] = (*strp)[pos];
            offset++;
            break;

         // saves characters to string
         default:
            str[offset] = (*strp)[pos];
            offset++;
            break;
      };
   };

   // stores expanded string
   str[offset] = '\0';
   free(*strp);
   *strp = str;

   return(0);
}


/// @brief interprets the string buffer
/// @param  cnf          pointer to configuration struct
/// @param  buff         string buffer to process
/// @param  len          length of buffer
/// @param[out] offsetp  number of bytes interpreted by function
int odbcshell_interpret_buffer(ODBCShell * cnf, char * buff, size_t len,
   ssize_t * offsetp)
{
   int           code;
   int           argc;
   char       ** argv;
   char          delim;
   size_t        pos;
   ssize_t       offset;

   argc     = 0;
   argv     = NULL;
   len      = strlen(buff);
   pos      = 0L;
   *offsetp = 0L;

   // processes buffer one "line" at a time
   while(pos < len)
   {
      // extracts next line in buffer
      if (odbcshell_parse_line(cnf, &buff[pos], &argc, &argv, &offset))
         return(-1);
      if ((offset == -1))
         return(2);
      if (!(argc))
      {
         pos              += offset+1;
         *offsetp         += offset;
         continue;
      };

      // replaces line delimiter with '\0'
      delim             = buff[offset+pos];
      buff[offset+pos]  = '\0';
      *offsetp         += offset;

      switch((code = odbcshell_interpret_line(cnf, &buff[pos], argc, argv)))
      {
         // command requested ODBC Shell exits (I.E. logout, quit, exit)
         case 1:
            buff[offset+pos] = delim;
            return(code);

         // indicates that an error was encountered
         case -1:
            buff[offset+pos] = delim;
            if ( (!(code)) || (!(cnf->continues)) )
               return(code);

         // indicates that a fatal error was encountered
         case -2:
            return(code);

         // restores line delimiter
         default:
            buff[offset+pos] = delim;
            break;
      };

      pos += offset+1;
   };
   return(0);
}

/// @brief interprets the arguments from a command line
/// @param cnf      pointer to configuration struct
/// @param str      original string from buffer
/// @param argc     number of arguments in string buffer
/// @param argv     array of arguments from string buffer
int odbcshell_interpret_line(ODBCShell * cnf, char * str, int argc,
   char ** argv)
{
   int               code;
   ODBCShellOption * cmd;

   if (!(argc))
      return(0);

   // retrieves command information
   if (!(cmd = odbcshell_lookup_opt_by_name(odbcshell_cmd_strings, argv[0])))
   {
      if ( (argc > 1) && (!(strcmp(argv[1], "="))) )
         cmd = odbcshell_lookup_opt_by_name(odbcshell_cmd_strings, "setenv");
   };
   if (!(cmd))
   {
      odbcshell_error(cnf, "%s: unknown ODBC command.\n", argv[0]);
      odbcshell_error(cnf, "try `help;' for more information.\n");
      return(-1);
   };

   // checks for permitted number of arguments
   if (cmd->min_arg > argc)
   {
      odbcshell_error(cnf, "%s: missing required arguments.\n", argv[0]);
      odbcshell_error(cnf, "try `help %s;' for more information.\n", argv[0]);
      return(-1);
   };
   if ( (cmd->max_arg < argc) && (cmd->max_arg != -1) )
   {
      odbcshell_error(cnf, "%s: unknown arguments\n", argv[0]);
      odbcshell_error(cnf, "try `help %s;' for more information.\n", argv[0]);
      return(-1);
   };

   // executes command
   cnf->active_cmd = cmd;
   switch(cmd->val)
   {
      case ODBCSHELL_CMD_CLEAR:      code = odbcshell_cmd_clear(); break;
      case ODBCSHELL_CMD_CLOSE:      code = odbcshell_cmd_close(cnf); break;
      case ODBCSHELL_CMD_CONNECT:    code = odbcshell_cmd_connect(cnf, argc, argv); break;
      case ODBCSHELL_CMD_DISCONNECT: code = odbcshell_cmd_disconnect(cnf, argc, argv); break;
      case ODBCSHELL_CMD_ECHO:       code = odbcshell_cmd_echo(cnf, argc, argv); break;
      case ODBCSHELL_CMD_HELP:       code = odbcshell_cmd_help(cnf, argc, argv); break;
      case ODBCSHELL_CMD_ODBC:       code = odbcshell_cmd_exec(cnf, str, 0); break;
      case ODBCSHELL_CMD_OPEN:       code = odbcshell_cmd_open(cnf, argc, argv); break;
      case ODBCSHELL_CMD_QUIT:       code = odbcshell_cmd_quit(cnf); break;
      case ODBCSHELL_CMD_RECONNECT:  code = odbcshell_cmd_reconnect(cnf, argc, argv); break;
      case ODBCSHELL_CMD_RESET:      code = odbcshell_cmd_reset(cnf); break;
      case ODBCSHELL_CMD_SEND:       code = odbcshell_cmd_exec(cnf, str, 1); break;
      case ODBCSHELL_CMD_SET:        code = odbcshell_cmd_set(cnf, argc, argv); break;
      case ODBCSHELL_CMD_SETENV:     code = odbcshell_cmd_setenv(cnf, argc, argv); break;
      case ODBCSHELL_CMD_SHOW:       code = odbcshell_cmd_show(cnf, argv[1]); break;
      case ODBCSHELL_CMD_SOURCE:     code = odbcshell_cmd_source(cnf, argv[1]); break;
      case ODBCSHELL_CMD_UNSET:      code = odbcshell_cmd_unset(cnf, argv); break;
      case ODBCSHELL_CMD_UNSETENV:   code = odbcshell_cmd_unsetenv(argc, argv); break;
      case ODBCSHELL_CMD_USE:        code = odbcshell_cmd_use(cnf, argc, argv); break;
      case ODBCSHELL_CMD_VERSION:    code = odbcshell_cmd_version(cnf); break;
      default:                       code = odbcshell_cmd_incomplete(cnf, argc, argv); break;
   };
   cnf->active_cmd = NULL;

   return(code);
}


/// @brief splits a line into multiple arguments
/// @param cnf         pointer to configuration struct
/// @param line        string buffer to process as a command line string
/// @param[out] argcp  returns number of arguments parsed from string
/// @param[out] argvp  returns array of arguments parsed from string
/// @param[out] eolp   returns end of line within the string
int odbcshell_parse_line(ODBCShell * cnf, char * line, int * argcp,
   char *** argvp, ssize_t * eolp)
{
   int       i;
   int       code;
   char    * arg;
   void    * ptr;
   size_t    len;    // line length
   size_t    pos;    // position within line
   size_t    start;  // start of argument within line
   size_t    arglen; // length of argument

   if ( (!(line)) || (!(argcp)) || (!(argvp)) || (!(eolp)) )
   {
      odbcshell_fatal(cnf, "%s: internal error\n", PROGRAM_NAME);
      return(-2);
   };

   // frees old argc/argv data
   for(i = 0; i < *argcp; i++)
      free((*argvp)[i]);
   *argcp = 0;
   *eolp  = -1;

   if (!(len = strlen(line)))
      return(0);

   // processes each character of the line
   for(pos = 0; pos < len+1; pos++)
   {
      arg = NULL;
      switch(line[pos])
      {
         // exit if end of command is found
         case '\n':
         case '\0':
         case ';':
            *eolp = pos;
            return(0);

         case '\\':
            line[pos] = ' ';
            if ((line[pos+1] != '\n') && (line[pos+1] != '\0'))
               continue;
            if (line[pos+1] == '\n')
               line[pos+1] = ' ';
            if (line[pos+1] == '\0')
               return(0);
            break;

         // skip white space between arguments
         case ' ':
         case '\t':
            break;

         case '\r':
            line[pos] = ' ';
            break;

         // skip comments
         case '#':
            while((line[pos] != '\n') && (line[pos] != '\r') && (pos < len))
               pos++;
            if (pos >= len)
            {
               *eolp = pos;
               return(0);
            };
            break;

         // processes variable assignment operator
         case '=':
            arglen = 2;
            if (!(arg = malloc(arglen)))
            {
               odbcshell_fatal(cnf, "%s: out of virtual memory\n", PROGRAM_NAME);
               return(-2);
            };
            memset(arg, 0, arglen);
            strncpy(arg, &line[pos], arglen-1);
            break;

         // processes arguments contained within single quotes
         case '\'':
            start = pos + 1;
            pos += 1;
            while((line[pos] != '\'') && (pos < len))
               pos++;
            if (pos >= len)
               return(0);
            arglen = 1 + pos - start;
            if (!(arg = malloc(arglen)))
            {
               odbcshell_fatal(cnf, "%s: out of virtual memory\n", PROGRAM_NAME);
               return(-2);
            };
            memset(arg, 0, arglen);
            strncpy(arg, &line[start], arglen-1);
            break;

         // processes arguments contained within double quotes
         case '"':
            start = pos + 1;
            pos += 1;
            while( ((line[pos] != '"') || (line[pos-1] == '\\')) && (pos < len))
               pos++;
            if (pos >= len)
               return(0);
            arglen = 1 + pos - start;
            if (!(arg = malloc(arglen)))
            {
               odbcshell_fatal(cnf, "%s: out of virtual memory\n", PROGRAM_NAME);
               return(-2);
            };
            memset(arg, 0, arglen);
            strncpy(arg, &line[start], arglen-1);
            if ((code = odbcshell_expand_string(cnf, &arg)))
            {
               free(arg);
               return(code);
            };
            break;

         // processes unquoted arguments
         default:
            start = pos;
            while ( ((pos+1) < len) &&
                    (line[pos+1] != '=') &&
                    (line[pos+1] != ';') &&
                    (line[pos+1] != '#') &&
                    (line[pos+1] != ' ') &&
                    (line[pos+1] != '\t') &&
                    (line[pos+1] != '\\') &&
                    (line[pos+1] != '\n') &&
                    (line[pos+1] != '\r') )
               pos++;
            arglen = 2 + pos - start;
            if (!(arg = malloc(arglen)))
            {
               odbcshell_fatal(cnf, "%s: out of virtual memory\n", PROGRAM_NAME);
               return(-2);
            };
            memset(arg, 0, arglen);
            strncpy(arg, &line[start], arglen-1);
            if ((code = odbcshell_expand_string(cnf, &arg)))
            {
               free(arg);
               return(code);
            };
            break;
      };

      // if an argument is not defined, skip to next character in string.
      if (!(arg))
         continue;

      // stores argument in array
      if (!(ptr = realloc(*argvp, sizeof(char *) * ((*argcp)+1))))
      {
         odbcshell_fatal(cnf, "%s: out of virtual memory\n", PROGRAM_NAME);
         return(-2);
      };
      *argvp = ptr;
      (*argvp)[(*argcp)] = arg;
      (*argcp)++;
   };

   return(0);
}


/* end of source */
