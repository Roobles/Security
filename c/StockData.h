#ifndef STOCK_DATA_H
#define STOCK_DATA_H

typedef struct 
{
  unsigned int StockId;
  unsigned int StockDataId;
  float High;
  float Low;
  float Close;
  char Date[11];
} StockData;

typedef struct
{
  unsigned int Count;
  StockData* Data;
} StockHistory;

StockHistory* GetStockHistory (int stockId);
void CleanStockHistory (StockHistory* history);
void PrintStockHistory (StockHistory* history);

#endif
