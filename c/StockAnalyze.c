#include "StockData.h"
#include "StockMomentum.h"

int main (int argc, char** argv)
{
  int stockId;
  StockHistory* history;
  StockMomentumAttributes* stockAttr;

  // Setting values.
  float mCoefficient, baseWeight, initialDir, initialMag;
  StockAttribute price;

  mCoefficient = 10;
  baseWeight = 800;
  initialDir = 0;
  initialMag = 2.0;
  price = Low;

  stockId = 15;
  history = GetStockHistory (stockId);
  stockAttr = NewStockMomentumAttributes (mCoefficient, baseWeight, initialDir, initialMag, price);

  PrintStockHistory (history);
  CleanStockHistory (history);
}
