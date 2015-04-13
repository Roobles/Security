#ifndef DB_UTILS_H
#define DB_UTILS_H

void DbDelete (const char* tableName, const char* where);
void DbInsert (const char* tableName, const char* columns, const char* values);

MYSQL_RES* DbSelect (MYSQL* conn, const char* contents,
  const char* tableName, const char* where, const char* orderby,
  int size, int page);

int DbGetCount (const char* tableName, const char* where);
#endif
