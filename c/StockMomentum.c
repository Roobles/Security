#include <math.h>
#include <stdlib.h>

#include "StockData.h"
#include "Momentum.h"
#include "StockMomentum.h"

#define STOCK_PRICE(stockVar) GetStockAttribute (stockVar, stockAttr->Price)

static double GetMassFromStock (StockData* stock, StockMomentumAttributes* stockAttr);
static void InitializeMomentum (Momentum* momentum, StockData* datum, StockMomentumAttributes* stockAttr);
static MomentumTangents StockMomentumTangents (StockData* parentStock, StockData* childStock, StockMomentumAttributes* stockAttr);

// StockMomentum.h implementation
MomentumHistory* NewStockMomentumHistory (StockHistory* stockHistory, MomentumAttributes* system, StockMomentumAttributes* stockAttr)
{
  return MOMENTUM_HISTORY (stockHistory, StockData, InitializeMomentum, StockMomentumTangents, stockAttr, system);
}

StockMomentumAttributes* NewStockMomentumAttributes (double mCoefficient, double baseWeight, double initialDirection, double initialMagnitude, StockAttribute price)
{
  StockMomentumAttributes* stockAttr;
  
  stockAttr = malloc (sizeof (StockMomentumAttributes));

  stockAttr->MassCoefficient = mCoefficient;
  stockAttr->BaseWeight = baseWeight;
  stockAttr->InitialDirection = initialDirection;
  stockAttr->InitialMagnitude = initialMagnitude;
  stockAttr->Price = price;
}

void CleanseStockMomentumAttributes (StockMomentumAttributes* stockAttr)
{
  tryfree (stockAttr);
}

// Static Functions
static double GetMassFromStock (StockData* stock, StockMomentumAttributes* stockAttr)
{
  double mass, price;
  double baseWeight, mCoefficient;

  baseWeight = stockAttr->BaseWeight;
  mCoefficient = stockAttr->MassCoefficient;
  price = STOCK_PRICE (stock);
  if (price <= 0) price = 0.001;
  
  // TODO: Maybe add a pointer to a logarithmic function for determining mass.
  mass = (log (price) * mCoefficient) + baseWeight;

  return mass;
}

static void InitializeMomentum (Momentum* momentum, StockData* datum, StockMomentumAttributes* stockAttr)
{
  double mass, magnitude, direction;

  direction = stockAttr->InitialDirection;
  magnitude = stockAttr->InitialMagnitude;
  mass = GetMassFromStock (datum, stockAttr);

  momentum->Velocity.Direction = direction;
  momentum->Velocity.Magnitude = magnitude;
  momentum->Mass = mass;
}

static MomentumTangents StockMomentumTangents (StockData* parentStock, StockData* childStock, StockMomentumAttributes* stockAttr)
{
  MomentumTangents tangents;

  tangents.Mass = GetMassFromStock (childStock, stockAttr);
  tangents.Direction =  STOCK_PRICE (childStock) - STOCK_PRICE (parentStock);

  return tangents;
}
