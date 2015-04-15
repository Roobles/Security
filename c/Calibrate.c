#include <stdio.h>
#include "StockMomentum.h"
#include "DbConnection.h"
#include "SecurityDefinitions.h"
#include "Stock.h"
#include "LibRoo.h"

static MomentumAttributes* GetMomentumAttributes ();
static StockMomentumAttributes* GetStockMomentumAttributes ();
static void SetDb (char* user, char* pass);
static void GetStockMomentum (Stock* stock, StockMomentumAttributes* stockAttr, MomentumAttributes* system);

int main (int argc, char** argv)
{
  int i;
  MomentumAttributes* system;
  StockMomentumAttributes* stockAttr;
  StockCollection* stocks;
  char *user, *password;

  user = argv[1];
  password = argv[2];

  SetDb (user, password);
  system = GetMomentumAttributes ();
  stockAttr = GetStockMomentumAttributes ();
  stocks = GetStocks ("IsAlive = 1");

  for (i=0; i<stocks->Count; i++)
    GetStockMomentum (&stocks->Data[i], stockAttr, system);
    

  CleanseStocks (stocks);
  CleanseStockMomentumAttributes (stockAttr);
  CleanseMomentumAttributes (system);
}

static MomentumAttributes* GetMomentumAttributes ()
{
  float degreeOfAttack;
  float tCoefficient, lCoefficient, gravity, airDensity, 
    angleOfAttack, vehicleAspectRatio, vehicleDensity, acceleration;

  degreeOfAttack = 30;

  tCoefficient = 0.7;
  lCoefficient = 1;
  gravity = 9.8;
  airDensity = 1.2;
  angleOfAttack = (degreeOfAttack / 3.14) * 180.0;
  vehicleAspectRatio = 2.5;
  vehicleDensity = 120;
  acceleration = 5;
  
  return NewMomentumAttributes (tCoefficient, lCoefficient, gravity, airDensity,
    angleOfAttack, vehicleAspectRatio, vehicleDensity, acceleration);
}


static StockMomentumAttributes* GetStockMomentumAttributes ()
{
  float mCoefficient, baseWeight, initialDirection, initialMagnitude, price;
  
  mCoefficient = 70;
  baseWeight = 1500;
  initialDirection = 0.0;
  initialMagnitude = 5.0;
  price = Close;

  return NewStockMomentumAttributes (mCoefficient, baseWeight, initialDirection, initialMagnitude, price);
}

static void SetDb (char* user, char* pass)
{
  const char* host = "localhost";
  const char* dbName = "security";
  
  SetDbConnectionSettings (host, user, pass, dbName, 0, NULL, 0);
}

static void GetStockMomentum (Stock* stock, StockMomentumAttributes* stockAttr, MomentumAttributes* system)
{
  int i, stockId;
  double summation;
  StockHistory* history;
  StockData *child, *parent;
  Momentum* currMomentum = NULL;

  summation = 0;
  stockId = stock->StockId;
  history = GetStockHistoryById (stockId);
  for (i=0; i<history->Count; i++)
  {
    child = &history->Data[i];
    currMomentum = (currMomentum == NULL)
      ? NewStockMomentum (child, stockAttr)
      : ApplyStockMomentum (currMomentum, parent, child, stockAttr, system);

    summation += (currMomentum->Velocity.Magnitude * currMomentum->Mass);
    parent = child;
  }

  printf ("Stock: %-10s Avg.Momentum: %.2f\n", stock->Ticker, (summation / history->Count));
  CleanseStockHistory (history);
  //sleep (1);
}
