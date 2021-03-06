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
StockHistory* GetStockHistory (char* where)
{
  StockHistory* history;
  const char* contents = "*";

  history = malloc (sizeof (StockHistory));
  SELECT (contents, TABLE_STOCK_DATA, where,
    NULL, history, SetStockData, StockData);

  return history;
}

StockHistory* GetStockHistoryById (int stockId)
{
  StockHistory* history;

  int whereLen;
  char *where, stockIdStr[8];
  const char* whereFmt =  "StockId = %s";

  sprintf (stockIdStr, "%d", stockId);
  where = BuildClause (whereFmt, stockIdStr, &whereLen);

  history = GetStockHistory (where);

  tryfree (where);
  return history;
}

void CleanseStockHistory (StockHistory* history)
{
  history->Count = 0;

  tryfree (history->Data);
  tryfree (history->Filter);
  tryfree (history);
}

void PrintStockHistory (StockHistory* history)
{
  int i,count;

  count = history->Count;
  for (i=0; i<count; i++)
    PrintStockData (&history->Data[i]);
}

double GetStockAttribute (StockData* stock, StockAttribute attribute)
{
  double* stockAttributes;

  stockAttributes = (double*) stock;
  return stockAttributes[attribute];
}

// Statics
static void SetStockData (StockData* data, MYSQL_ROW* row)
{
  data->High = rtof (F_DATA_STOCK_HIGH);
  data->Low = rtof (F_DATA_STOCK_LOW);
  data->Close = rtof (F_DATA_STOCK_CLOSE);

  data->StockId = rtoi (F_DATA_STOCK_ID);
  data->StockDataId = rtoi (F_DATA_STOCK_DATA_ID);

  strcpy (data->Date, (char*) row[F_DATA_STOCK_DATE]);
}

static void PrintStockData (StockData* data)
{
  printf ("StockId:%06d High:%09.2f Low:%09.2f Close:%09.2f StockDataId:%06d\n", data->StockId, data->High, data->Low, data->Close, data->StockDataId); 
}
