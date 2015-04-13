#include <stdlib.h>
#include <mysql.h>

#include "DbConnection.h"
#include "LibRoo.h"

int dbSet = 0;
DbConnectionSettings _dbConnection;

static void CleanseDbConnectionSettings (DbConnectionSettings* settings);

// Implementation of DbConnection.h
DbConnectionSettings* GetDbConnectionSettings ()
{
  return &_dbConnection;
}

#define DBCLONE(dbset,dbval) settings->dbset = strclone (dbval);
void SetDbConnectionSettings (const char* host, const char* user, const char* password,
  const char* dbName, unsigned int port, const char* socket, unsigned long flag)
{
  trace ("Setting db connection settings.");
  DbConnectionSettings* settings;
  settings = GetDbConnectionSettings ();
  CleanseDbConnectionSettings (settings); 

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
  MYSQL* conn;
  DbConnectionSettings* settings;
  
  ASSERT_DB_ASSET (dbSet, "The database connection settings were never set.");
  settings = GetDbConnectionSettings ();

  trace ("Opening a new db connection.");
  conn = mysql_init (NULL);
  
  ASSERT_DB_ASSET (conn, "Error initializing database connection object.");

  conn = mysql_real_connect (conn,
    settings->Hostname,
    settings->User,
    settings->Password,
    settings->Database,
    settings->Port,
    settings->Socket,
    settings->Flags);

  ASSERT_DB_ASSET (conn, "Error connection to the database.");

  return conn;
}

void CleanseDbConnection (MYSQL* conn)
{
  if (mysql_errno (conn))
    error (mysql_error (conn));

  trace ("Closing db connection object.");
  mysql_close (conn);
}

// Static functions
#define FDBSET(dbset) if (settings->dbset != NULL) free (settings->dbset)
static void CleanseDbConnectionSettings (DbConnectionSettings* settings)
{
  if (dbSet)
  {
    trace ("Cleansing old db connection settings.");
    FDBSET (Hostname);
    FDBSET (User);
    FDBSET (Password);
    FDBSET (Database);
    FDBSET (Socket);
  }
}
