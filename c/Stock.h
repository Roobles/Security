#ifndef STOCK_H
#define STOCK_H 1

typedef struct
{
  // Stock
  unsigned int StockId;
  char Ticker[11]; 
  char StockName[51];

  // Exchange
  char ExchangeName[4];

  // Category
  char CategoryName[51];
  
  // MetaData
  char Founded[11];
  int IsAlive;
  int HasDividends;
} Stock;

typedef struct
{
  unsigned int Count;
  Stock* Data;
  char* Filter;
} StockCollection;

StockCollection* GetStockById (int id);
StockCollection* GetStocks (const char* where);

void CleanseStocks (StockCollection* stocks);
void CleanseStock (Stock* stock);
#endif 
