#include <stdio.h>
#include "StockData.h"

int main (int argc, char** argv)
{
  int stockId;
  StockHistory* history;

  stockId = 15;
  history = GetStockHistory (stockId);

  PrintStockHistory (history);
  CleanStockHistory (history);
}
