#include <stdlib.h>
#include <mysql.h>

#include "DbConnection.h"
#include "LibRoo.h"

int dbSet = 0;
DbConnectionSettings _dbConnection;

// Implementation of DbConnection.h
DbConnectionSettings* GetDbConnectionSettings ()
{
  return &_dbConnection;
}

#define DBCLEANSE(dbattr) tryfree (settings->dbattr);
void CleanseDbConnectionSettings ()
{
  DbConnectionSettings* settings;
  settings = GetDbConnectionSettings ();

  if (dbSet)
  {
    trace ("Cleansing db connection settings.");
    DBCLEANSE (Hostname);
    DBCLEANSE (User);
    DBCLEANSE (Password);
    DBCLEANSE (Database);
    DBCLEANSE (Socket);

    settings->Port = 0;
    settings->Flags = 0;

    dbSet = 0;
  }
}

#define DBCLONE(dbset,dbval) settings->dbset = strclone (dbval);
void SetDbConnectionSettings (const char* host, const char* user, const char* password,
  const char* dbName, unsigned int port, const char* socket, unsigned long flag)
{
  DbConnectionSettings* settings;
  settings = GetDbConnectionSettings ();

  trace ("Setting db connection settings.");
  CleanseDbConnectionSettings (); 

  DBCLONE (Hostname, host);
  DBCLONE (User, user);
  DBCLONE (Password, password);
  DBCLONE (Database, dbName);
  DBCLONE (Socket, socket);

  settings->Port = port;
  settings->Flags = flag;

  dbSet = 1;
}

#define ASSERT_DB_ASSET(asset,message) if (!asset) { error (message); CleanseDbConnection (conn); return NULL; }
MYSQL* NewDbConnection()
{
  MYSQL* conn, *connected;
  DbConnectionSettings* settings;
  
  ASSERT_DB_ASSET (dbSet, "The database connection settings were never set.");
  settings = GetDbConnectionSettings ();

  trace ("Opening a new db connection.");
  conn = mysql_init (NULL);
  
  ASSERT_DB_ASSET (conn, "Error initializing database connection object.");

  connected = mysql_real_connect (conn,
    settings->Hostname,
    settings->User,
    settings->Password,
    settings->Database,
    settings->Port,
    settings->Socket,
    settings->Flags);

  ASSERT_DB_ASSET (connected, "Error connection to the database.");

  return connected;
}

void CleanseDbConnection (MYSQL* conn)
{
  if (mysql_errno (conn))
    error (mysql_error (conn));

  trace ("Closing db connection object.");
  mysql_close (conn);
}
