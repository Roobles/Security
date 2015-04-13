#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "LibRoo.h"

// Definitions
#define DEFAULT_MESSAGE_LEVEL Error

#define MESS_COLOR "0;32"
#define ERROR_COLOR "1;31"
#define DEBUG_COLOR "1;33"
#define WARN_COLOR ERROR_COLOR
#define TRACE_COLOR DEBUG_COLOR

#define MESS_PREFIX "MESSAGE"
#define ERROR_PREFIX "ERROR"
#define WARN_PREFIX "WARN"
#define DEBUG_PREFIX "DEBUG"
#define TRACE_PREFIX "TRACE"

// Global Variables
MessageLevel _messageLevel = DEFAULT_MESSAGE_LEVEL;

// Function Definitions
static FILE* get_std_stream ();
static FILE* get_err_stream ();
static void print_message (MessageLevel level, const char* prefix, const char* color, const char* message, FILE* stream);

// Messaging
void SetMessageLevel (MessageLevel level)
{
  _messageLevel = level;
}

void message (const char* message)
{
  FILE* stream = get_std_stream ();
  print_message (Message, MESS_COLOR, MESS_PREFIX, message, stream);
}

void error (const char* message)
{
  FILE* stream = get_err_stream ();
  print_message (Error, ERROR_COLOR, ERROR_PREFIX, message, stream);
}

void warn (const char* message)
{
  FILE* stream = get_err_stream ();
  print_message (Warning, WARN_COLOR, WARN_PREFIX, message, stream);
}

void debug (const char* message)
{
  FILE* stream = get_std_stream ();
  print_message (Debug, DEBUG_COLOR, DEBUG_PREFIX, message, stream);
}

void trace (const char* message)
{
  FILE* stream = get_std_stream ();
  print_message (Trace, TRACE_COLOR, TRACE_PREFIX, message, stream);
}

// String utilities
char* strclone (const char* source)
{
  int length;
  char* clone; 

  if (source == NULL) return NULL;

  length = strlen (source) + 1;
  clone = malloc (length);

  strcpy (clone, source);
}

char* BuildClause (const char* format, const char* arg, int* length)
{
  int argLength, formatLength, formats;
  char* clause;

  *length = 0;
  if (arg == NULL) return "";

  formats = 1;
  argLength = strlen (arg);
  formatLength = strlen (format);
  *length = formatLength + argLength - (formats * 2);

  clause = malloc (*length + 1);
  sprintf (clause, format, arg);

  return clause;
}


// Static functions
static FILE* get_std_stream ()
{
  return stdout;
}

static FILE* get_err_stream ()
{
  return stderr;
}

static void print_message (MessageLevel level, const char* prefix, const char* color, const char* message, FILE* stream)
{
  if (level <= _messageLevel)
    printf ("\e[%sm%s\e[0m: %s\n", color, prefix, message);
}
