#ifndef DB_CONNECTION_H
#define DB_CONNECTION_H

#include <mysql.h>

typedef struct
{
  char* Hostname;
  char* User;
  char* Password;
  char* Database;
  char* Socket;

  unsigned int Port;
  unsigned long Flags;
} DbConnectionSettings;

DbConnectionSettings* GetDbConnectionSettings ();
void SetDbConnectionSettings (const char* host, const char* user, const char* password,
  const char* dbName, unsigned int port, const char* socket, unsigned long flag);

MYSQL* NewDbConnection();
void CleanseDbConnection (MYSQL* conn);
#endif
