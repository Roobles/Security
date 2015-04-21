#ifndef STOCK_MOMENTUM_H
#define STOCK_MOMENTUM_H 1

#include "StockData.h"
#include "Momentum.h"

typedef struct
{
  double MassCoefficient;
  double BaseWeight;

  double InitialDirection;
  double InitialMagnitude;

  StockAttribute Price;

} StockMomentumAttributes;

MomentumHistory* NewStockMomentumHistory (StockHistory* stockHistory, MomentumAttributes* system, StockMomentumAttributes* stockAttr);

StockMomentumAttributes* NewStockMomentumAttributes (double mCoefficient, double baseWeight, double initialDirection, double initialMagnitude, StockAttribute price);
void CleanseStockMomentumAttributes (StockMomentumAttributes* stockAttr);

#endif
