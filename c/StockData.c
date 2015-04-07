#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "StockData.h"

static void InitializeStockData (StockData* data, int stockId, int itteration);
static void PrintStockData (StockData* history);


StockHistory* GetStockHistory (int stockId)
{
  StockHistory* history;
  int i, count = 15;

  history = malloc (sizeof (StockHistory));
  history->Count = count;
  history->Data = malloc (sizeof (StockData) * count);

  for (i=0; i<count; i++)
    InitializeStockData (&history->Data[i], stockId, i);
    
  return history;
}

void CleanStockHistory (StockHistory* history)
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

// Statics
static void InitializeStockData (StockData* data, int stockId, int itteration)
{
  data->StockId = stockId;
  data->StockDataId = itteration + 1; 
  data->High = 19.45;
  data->Low = 15.21;
  data->Close = 17.11;
  
  strcpy (data->Date, "2015-03-15");
}

static void PrintStockData (StockData* data)
{
  printf ("StockId:%06d High:%09.2f Low:%09.2f Close:%09.2f StockDataId:%06d\n", data->StockId, data->High, data->Low, data->Close, data->StockDataId); 
}
