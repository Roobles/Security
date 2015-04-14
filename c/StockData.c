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
  StockHistory* history;

  int stockCount, whereLen, i;
  char *where, stockIdStr[8];
  const char* whereFmt =  "StockId = %s";
  const char* contents = "*";

  history = malloc (sizeof (StockHistory));
  sprintf (stockIdStr, "%d", stockId);
  where = BuildClause (whereFmt, stockIdStr, &whereLen);

  SELECT (contents, TABLE_STOCK_DATA, where,
    NULL, history, SetStockData, StockData);

  tryfree (where);
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
