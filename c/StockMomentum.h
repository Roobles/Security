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

Momentum* NewStockMomentum (StockData* stock, StockMomentumAttributes* stockAttr);
Momentum* ApplyStockMomentum (Momentum* inertial, StockData* parent, StockData* child, StockMomentumAttributes* stockAttr, MomentumAttributes* system);

StockMomentumAttributes* NewStockMomentumAttributes (double mCoefficient, double baseWeight, double initialDirection, double initialMagnitude, StockAttribute price);
void CleanseStockMomentumAttributes (StockMomentumAttributes* stockAttr);

#endif
