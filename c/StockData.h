#ifndef STOCK_DATA_H
#define STOCK_DATA_H 1

typedef enum
{
  High = 0x00,
  Low = 0x01,
  Close = 0x02
} StockAttribute;

typedef struct 
{
  float High;
  float Low;
  float Close;

  unsigned int StockId;
  unsigned int StockDataId;

  char Date[11];
} StockData;

typedef struct
{
  unsigned int Count;
  StockData* Data;
} StockHistory;

StockHistory* GetStockHistory (int stockId);
void CleanseStockHistory (StockHistory* history);
void PrintStockHistory (StockHistory* history);
float GetStockAttribute (StockData* stock, StockAttribute attribute);

#endif
