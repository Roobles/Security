#include <mysql.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "LibRoo.h"
#include "DbUtils.h"
#include "DbConnection.h"

static char* BuildSelectCommand (const char* contents, const char* tableName,
  const char* where, const char* orderby);

static char* BuildWhereClause (const char* where, int* length);
static char* BuildOrderClause (const char* orderBy, int* length);
static MYSQL_RES* DbQuery (MYSQL* conn, const char* queryStr);

// DbUtils.h Implementation
int DbGetCount (const char* tableName, const char* where)
{
  /*
  int count;
  MYSQL_RES* result;
  MYSQL_ROW row;
  MYSQL* conn;

  conn = NewDbConnection ();
  if ((result = DbSelect (conn, "COUNT(*)", tableName, where, NULL, 0, 0)) == NULL)
    return 0;

  while (row = mysql_fetch_row(result))
    count = atoi (row[0]);

  mysql_free_result (result);
  CleanseDbConnection (conn);

  return count;
  */
}

void DbSelect (const char* contents, const char* tableName, 
  const char* where, const char* orderby, DbCollection* collection, 
  DataTranslator translator, unsigned int collectionSize)
{
  int i, resultCount;
  char* selectCmd;
  MYSQL_RES* result;
  MYSQL_ROW row;
  MYSQL* conn;

  conn = NewDbConnection ();

  selectCmd = BuildSelectCommand (contents, tableName, where, orderby);
  result = DbQuery (conn, selectCmd);

  collection->Count = resultCount = mysql_num_rows (result);
  collection->Data = malloc (collectionSize * resultCount);

  for (i=0; row = mysql_fetch_row (result); i++)
    translator (&((char*)collection->Data)[i*collectionSize], (MYSQL_ROW*) row);

  mysql_free_result (result);
  CleanseDbConnection (conn);

  free (selectCmd);
}

// Static Functions
static char* BuildSelectCommand (const char* contents, const char* tableName,
  const char* where, const char* orderby)
{
  char* whereClause, *orderClause, *selectCmd;
  int selectLen, contentsLen, whereLen, orderLen, tableLen, formatLen;
  const char* selectFmt = "SELECT %s FROM %s%s%s"; formatLen = 8;

  tableLen = strlen (tableName);
  contentsLen = strlen (contents);
  whereClause = BuildWhereClause (where, &whereLen);
  orderClause = BuildOrderClause (orderby, &orderLen);

  selectLen = strlen (selectFmt);
  selectLen += tableLen;
  selectLen += contentsLen;
  selectLen += whereLen;
  selectLen += orderLen;
  selectLen -= formatLen;

  selectCmd = malloc (selectLen + 1);
  sprintf (selectCmd, selectFmt, contents, tableName, whereClause,
    orderClause);

  if (whereLen > 0) free (whereClause);
  if (orderLen > 0) free (orderClause);

  return selectCmd;
}

static MYSQL_RES* DbQuery (MYSQL* conn, const char* queryStr)
{
  MYSQL_RES* result;
  int resultCount;
  
  debug (queryStr);
  if (mysql_query (conn, queryStr))
    error ("The SQL command failed.");

  if ((result = mysql_store_result (conn)) == NULL)
    error ("Could not retrieve a result.");
  
  return result;
}

static char* BuildWhereClause (const char* where, int* length)
{
  const char* whereFormat = " WHERE %s";
  return BuildClause (whereFormat, where, length);
}

static char* BuildOrderClause (const char* orderBy, int* length)
{
  const char* orderFormat = " ORDER BY %s";
  return BuildClause (orderFormat, orderBy, length);
}
