#ifndef DB_UTILS_H
#define DB_UTILS_H 1

#include <mysql.h>

typedef (*DataTranslator)(void* datum, MYSQL_ROW* row);

typedef struct
{
  unsigned int Count;
  void* Data;
} DbCollection;

// SQL Macros
#define SELECT(contents, tableName, where, orderby, collection, translator, dbType) \
    DbSelect (contents, tableName, where, orderby, (DbCollection*) collection,      \
    (DataTranslator) translator, sizeof (dbType));

// Translation Macros
#define rtoc(tfunc,ordinal) tfunc ((char*)row[ordinal])
#define rtof(ordinal) rtoc (atof,ordinal)
#define rtoi(ordinal) rtoc (atoi,ordinal)

void DbSelect (const char* contents, const char* tableName, 
  const char* where, const char* orderby, DbCollection* collection, 
  DataTranslator translator, unsigned int collectionSize);
#endif
