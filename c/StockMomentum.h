#ifndef STOCK_MOMENTUM_H
#define STOCK_MOMENTUM_H 1

#include "StockData.h"
#include "Momentum.h"

typedef struct
{
  float MassCoefficient;
  float BaseWeight;

  float InitialDirection;
  float InitialMagnitude;

  StockAttribute Price;

} StockMomentumAttributes;

Momentum* NewStockMomentum (StockData* stock, StockMomentumAttributes* stockAttr);
Momentum* ApplyStockMomentum (Momentum* inertial, StockData* parent, StockData* child, StockMomentumAttributes* stockAttr, MomentumAttributes* system);

StockMomentumAttributes* NewStockMomentumAttributes (float mCoefficient, float baseWeight, float initialDirection, float initialMagnitude, StockAttribute price);
void CleanseStockMomentumAttributes (StockMomentumAttributes* stockAttr);

#endif
