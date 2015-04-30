#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mysql.h>

#include "Stock.h"
#include "LibRoo.h"
#include "DbUtils.h"
#include "SecurityDefinitions.h"

static void SetStock (Stock* stock, MYSQL_ROW* row);

// Stock.h Implementation
StockCollection* GetStockById (int id)
{
  char where[32];
  const char* whereFmt = "stk.StockId = %d";
  
  sprintf (where, whereFmt, id);
  return GetStocks (where);
}

StockCollection* GetStocks (const char* where)
{
  StockCollection* stocks;
  char* contents, *from, *orderBy;

  from = TABLE_STOCK;
  contents = "*";
  orderBy = NULL;
  
  stocks = malloc (sizeof (StockCollection));
  SELECT (contents, from, where, orderBy, stocks, SetStock, Stock);

  return stocks;
}

void CleanseStocks (StockCollection* stocks)
{
  stocks->Count = 0;

  tryfree (stocks->Filter);
  tryfree (stocks->Data);
  tryfree (stocks);
}


// Static Functions
static void SetStock (Stock* stock, MYSQL_ROW* row)
{
  stock->StockId = rtoi (F_STOCK_ID);
  stock->IsAlive = rtoi (F_STOCK_IS_ALIVE);
  stock->HasDividends = rtoi (F_STOCK_HAS_DIVIDENDS);
  
  strcpy (stock->Ticker, (char*) row[F_STOCK_TICKER]);
  strcpy (stock->StockName, (char*) row[F_STOCK_NAME]);
  strcpy (stock->ExchangeName, (char*) row[F_STOCK_EXCHANGE_NAME]);
  strcpy (stock->CategoryName, (char*) row[F_STOCK_CATEGORY_NAME]);
  strcpy (stock->Founded, (char*) row[F_STOCK_FOUNDED]);
}
