#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>

#include "LibRoo.h"
#include "SecurityDefinitions.h"
#include "StockData.h"
#include "DbConnection.h"
#include "DbUtils.h"

// Function Definitions
static void SetStockData (StockData* data, MYSQL_ROW* row);
static void PrintStockData (StockData* history);


// StockData.h Implementation
StockHistory* GetStockHistory (int stockId)
{
  // TODO: Implement this for real.
  MYSQL* conn;
  MYSQL_ROW row;
  MYSQL_RES* result;
  StockHistory* history;

  int stockCount, whereLen, i;
  char *where, stockIdStr[8];
  const char* whereFmt =  "StockId = %s";
  const char* contents = "*";

  history = malloc (sizeof (StockHistory));
  sprintf (stockIdStr, "%d", stockId);
  where = BuildClause (whereFmt, stockIdStr, &whereLen);

  conn = NewDbConnection ();
  result = DbSelect (conn, contents, TABLE_STOCK_DATA, where, NULL, 0, 0);

  history->Count = stockCount = mysql_num_rows (result);
  history->Data = malloc (sizeof (StockData) * stockCount);

  for (i=0; row = mysql_fetch_row (result); i++)
    SetStockData (&history->Data[i], (MYSQL_ROW*) row);

  mysql_free_result (result);
  CleanseDbConnection (conn);

  if (whereLen > 0) free (where);
  return history;
}

void CleanseStockHistory (StockHistory* history)
{
  free (history->Data);
  free (history);
}

void PrintStockHistory (StockHistory* history)
{
  int i,count;

  count = history->Count;
  for (i=0; i<count; i++)
    PrintStockData (&history->Data[i]);
}

float GetStockAttribute (StockData* stock, StockAttribute attribute)
{
  float* stockAttributes;

  stockAttributes = (float*) stock;
  return stockAttributes[attribute];
}

// Statics
#define rtoc(tfunc,ordinal) tfunc ((char*)row[ordinal])
#define rtof(ordinal) rtoc (atof,ordinal)
#define rtoi(ordinal) rtoc (atoi,ordinal)
static void SetStockData (StockData* data, MYSQL_ROW* row)
{
  const char* floatFormat = "%f";
  const char* intFormat = "%d";
  
  data->High = rtof (F_STOCK_HIGH);
  data->Low = rtof (F_STOCK_LOW);
  data->Close = rtof (F_STOCK_CLOSE);

  data->StockId = rtoi (F_STOCK_ID);
  data->StockDataId = rtoi (F_STOCK_DATA_ID);

  strcpy (data->Date, (char*) row[F_STOCK_DATE]);
}

static void PrintStockData (StockData* data)
{
  printf ("StockId:%06d High:%09.2f Low:%09.2f Close:%09.2f StockDataId:%06d\n", data->StockId, data->High, data->Low, data->Close, data->StockDataId); 
}
