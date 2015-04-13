#include <math.h>
#include <stdlib.h>

#include "StockData.h"
#include "Momentum.h"
#include "StockMomentum.h"

#define STOCK_PRICE(stockVar) GetStockAttribute (stockVar, stockAttr->Price)

static float GetMassFromStock (StockData* stock, StockMomentumAttributes* stockAttr);

// StockMomentum.h implementation
Momentum* NewStockMomentum (StockData* stock, StockMomentumAttributes* stockAttr)
{
  float mass, magnitude, direction;
  Momentum* prodigy;

  direction = stockAttr->InitialDirection;
  magnitude = stockAttr->InitialMagnitude;
  mass = GetMassFromStock (stock, stockAttr);

  prodigy = malloc (sizeof (Momentum));
  prodigy->Mass = mass;
  prodigy->Velocity.Direction = direction;
  prodigy->Velocity.Magnitude = magnitude;

  return prodigy;
}

Momentum* ApplyStockMomentum (Momentum* inertial, StockData* parent, StockData* child, StockMomentumAttributes* stockAttr, MomentumAttributes* system)
{
  float mass, direction;

  mass = GetMassFromStock (child, stockAttr);
  direction = STOCK_PRICE (child) - STOCK_PRICE (parent);

  return ApplyMomentum (inertial, mass, direction, system);
}

StockMomentumAttributes* NewStockMomentumAttributes (float mCoefficient, float baseWeight, float initialDirection, float initialMagnitude, StockAttribute price)
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
  free (stockAttr);
}

// Static Functions
static float GetMassFromStock (StockData* stock, StockMomentumAttributes* stockAttr)
{
  float mass, price;
  float baseWeight, mCoefficient;

  baseWeight = stockAttr->BaseWeight;
  mCoefficient = stockAttr->MassCoefficient;
  price = STOCK_PRICE (stock);
  
  // TODO: Maybe add a pointer to a logarithmic function for determining mass.
  mass = (log (price) * mCoefficient) + baseWeight;

  return mass;
}
