#include <mysql.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "LibRoo.h"
#include "DbUtils.h"
#include "DbConnection.h"

static char* BuildWhereClause (const char* where, int* length);
static char* BuildOrderClause (const char* orderBy, int* length);
static char* BuildLimitClause (int size, int page, int* length);
static MYSQL_RES* DbQuery (MYSQL* conn, const char* queryStr);

// DbUtils.h Implementation
void DbDelete (const char* tableName, const char* where)
{

}

void DbInsert (const char* tableName, const char* columns, const char* values)
{
  
}

int DbGetCount (const char* tableName, const char* where)
{
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
}

MYSQL_RES* DbSelect (MYSQL* conn, const char* contents,
  const char* tableName, const char* where, const char* orderby,
  int size, int page)
{
  char* whereClause, *orderClause, *limitClause, *selectCmd;
  int selectLen, contentsLen, whereLen, orderLen, limitLen, tableLen, formatLen;

  MYSQL_RES* result;

  const char* selectFmt = "SELECT %s FROM %s%s%s%s"; formatLen = 10;

  tableLen = strlen (tableName);
  contentsLen = strlen (contents);
  whereClause = BuildWhereClause (where, &whereLen);
  orderClause = BuildOrderClause (orderby, &orderLen);
  limitClause = BuildLimitClause (size, page, &limitLen);

  selectLen = strlen (selectFmt);
  selectLen += tableLen;
  selectLen += contentsLen;
  selectLen += whereLen;
  selectLen += orderLen;
  selectLen -= formatLen;

  selectCmd = malloc (selectLen + 1);
  sprintf (selectCmd, selectFmt, contents, tableName, whereClause,
    orderClause, limitClause);

  if (whereLen > 0) free (whereClause);
  if (orderLen > 0) free (orderClause);
  if (limitLen > 0) free (limitClause);

  result = DbQuery (conn, selectCmd);
  free (selectCmd);

  return result;
}

// Static Functions
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

static char* BuildLimitClause (int size, int page, int* length)
{
  int skip, limitLen, formats;
  char *limitClause, sizeStr[8], skipStr[16];
  const char* limitFormat = " LIMIT %s, %s";

  // Return empty if not paging.
  *length = 0;
  if (size <= 0) return "";

  skip = size * page;
  sprintf (sizeStr, "%d", size);
  sprintf (skipStr, "%d", skip);

  formats = 2;
  *length += strlen (limitFormat);
  *length += strlen (sizeStr);
  *length += strlen (skipStr);
  *length -= (formats * 2);

  limitClause = malloc (*length + 1);
  sprintf (limitClause, limitFormat, sizeStr, skipStr);
  
  return limitClause;
}

