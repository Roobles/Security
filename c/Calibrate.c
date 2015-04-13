#include <stdio.h>
#include "StockMomentum.h"
#include "DbConnection.h"
#include "SecurityDefinitions.h"
#include "LibRoo.h"

static MomentumAttributes* GetMomentumAttributes ();
static StockMomentumAttributes* GetStockMomentumAttributes ();
static void SetDb (char* user, char* pass);

int main (int argc, char** argv)
{
  MomentumAttributes* system;
  StockMomentumAttributes* stockAttr;
  StockHistory* history;
  StockData *child, *parent;
  MYSQL* conn;
  char *user, *password;
  int i, stockId;

  Momentum* currMomentum = NULL;

  user = argv[1];
  password = argv[2];
  stockId = atoi (argv[3]);

  SetDb (user, password);
  SetMessageLevel (Debug);

  system = GetMomentumAttributes ();
  stockAttr = GetStockMomentumAttributes ();

  history = GetStockHistory (stockId);
  parent = NULL;
  for (i=0; i<history->Count; i++)
  {
    child = &history->Data[i];
    currMomentum = (currMomentum == NULL)
      ? NewStockMomentum (child, stockAttr)
      : ApplyStockMomentum (currMomentum, parent, child, stockAttr, system);

    printf ("Close: $%-10.2fSpeed: %.2f mph\n", child->Close, currMomentum->Velocity.Magnitude * 2.236);
    parent = child;
  }

  CleanseStockHistory (history);
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
