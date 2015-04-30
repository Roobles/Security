#ifndef LIB_ROO_H
#define LIB_ROO_H 1

// Only works for integer values.
#define NUM_ARGS(...) ((sizeof ((void*[]){0, ##__VA_ARGS__}) / sizeof (void*)) -1)

typedef enum
{
  Message = 0x00,
  Error = 0x01,
  Warning = 0x02,
  Debug = 0x03,
  Trace = 0x04
} MessageLevel;

void SetMessageLevel (MessageLevel level);
void message (const char* message);
void error (const char* message);
void warn (const char* message);
void debug (const char* message);
void trace (const char* message);

char* strclone (const char* source);
char* BuildClause (const char* format, const char* arg, int* length);

void tryfree (void* str);
#endif
