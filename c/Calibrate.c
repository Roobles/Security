#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "StockMomentum.h"
#include "DbConnection.h"
#include "SecurityDefinitions.h"
#include "Stock.h"
#include "DbPlot.h"
#include "LibRoo.h"

static MomentumAttributes* GetMomentumAttributes ();
static StockMomentumAttributes* GetStockMomentumAttributes ();
static DbGraphParams* GetDbGraphParams ();

static void SetDb (char* user, char* pass);
static void ProcessStock (Stock* stock, StockMomentumAttributes* stockAttr, MomentumAttributes* system, DbGraphParams* graphParams);
static MomentumHistory* GetStockMomentum (Stock* stock, StockHistory* history, StockMomentumAttributes* stockAttr, MomentumAttributes* system);
static void GraphStock (Stock* stock, StockHistory* stockHistory, MomentumHistory* momentumHistory, DbGraphParams* params);

int main (int argc, char** argv)
{
  int i, stockId;
  MomentumAttributes* system;
  StockMomentumAttributes* stockAttr;
  DbGraphParams* graphParams;
  StockCollection* stocks;
  StockHistory* history;
  Stock* stock;
  char *user, *password;

  user = argv[1];
  password = argv[2];
  stockId = atoi (argv[3]);

  SetDb (user, password);
  system = GetMomentumAttributes ();
  stockAttr = GetStockMomentumAttributes ();
  graphParams = GetDbGraphParams ();

  history = GetStockHistoryById (stockId);
  stocks = GetStockById (stockId); //GetStocks ("IsAlive = 1");
  for (i=0; i<stocks->Count; i++)
    ProcessStock (&stocks->Data[i], stockAttr, system, graphParams);

  CleanseStocks (stocks);
  CleanseGraphParams (graphParams);
  CleanseStockMomentumAttributes (stockAttr);
  CleanseMomentumAttributes (system);
}

static MomentumAttributes* GetMomentumAttributes ()
{
  double degreeOfAttack;
  double tCoefficient, lCoefficient, gravity, airDensity, 
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
  double mCoefficient, baseWeight, initialDirection, initialMagnitude, price;
  
  mCoefficient = 70;
  baseWeight = 1500;
  initialDirection = 0.0;
  initialMagnitude = 5.0;
  price = Close;

  return NewStockMomentumAttributes (mCoefficient, baseWeight, initialDirection, initialMagnitude, price);
}

#define SET_COLOR(ordinal, red, green, blue) colors[ordinal] = PLOT_COLOR (red, green, blue)
static DbGraphParams* GetDbGraphParams ()
{
  int colorCount;
  DbPlotPalette* lineColors;
  DbPlotColor* colors;
  DbPlotColor boxColor;
  double width, height, padding, margin, dpi; 
  double lineWidth, boxWidth;
  const char* type = "svg";

  width = 1400;
  height = 800;
  padding = 10;
  margin = 20;
  dpi = 150;
  lineWidth = 0.5;
  boxWidth = 2;
  boxColor = PLOT_COLOR (0x08, 0x17, 0x29); // 081729

  lineColors = malloc (sizeof (DbPlotPalette));

  colorCount = 4;
  lineColors->Count = colorCount;
  lineColors->Colors = malloc (sizeof (DbPlotColor) * colorCount);
  colors = lineColors->Colors;

  SET_COLOR (0, 0x85, 0x3A, 0x59); // #853A59
  SET_COLOR (1, 0xF5, 0x79, 0x62); // #F57962
  SET_COLOR (2, 0xBC, 0x9A, 0xD7); // #BC9AD7
  SET_COLOR (3, 0xC4, 0x43, 0x5A); // #C4435A

  return NewGraphParams (type, width, height, padding,
    margin, dpi, lineWidth, lineColors, boxWidth, boxColor);
}


static void SetDb (char* user, char* pass)
{
  const char* host = "localhost";
  const char* dbName = "security";
  
  SetDbConnectionSettings (host, user, pass, dbName, 0, NULL, 0);
}

static void ProcessStock (Stock* stock, StockMomentumAttributes* stockAttr, MomentumAttributes* system, DbGraphParams *graphParams)
{
  int stockId;
  StockHistory* stockHistory;
  MomentumHistory* momentumHistory;

  stockId = stock->StockId;
  stockHistory = GetStockHistoryById (stockId);
  momentumHistory = NewStockMomentumHistory (stockHistory, system, stockAttr);

  GraphStock (stock, stockHistory, momentumHistory, graphParams);

  CleanseStockHistory (stockHistory);
  CleanseMomentumHistory (momentumHistory);
}

static void GraphStock (Stock* stock, StockHistory* stockHistory, MomentumHistory* momentumHistory, DbGraphParams* params)
{
  char* ticker, *fileName;
  const char* fileFormat = "%s.%s";
  DbPlotSet *stockPlot, *momentumPlot; 
  DbGraph* stockGraph;

  // Set the File Name
  ticker = stock->Ticker;
  fileName = malloc (strlen (ticker) + strlen (fileFormat) + strlen (params->Type) -4 +1);
  sprintf (fileName, fileFormat, ticker, params->Type);

  stockPlot = PLOT_SET (stockHistory, Close, StockData);
  momentumPlot = PLOT_SET (momentumHistory, 0, Momentum);
  
  stockGraph = GRAPH (stockPlot, momentumPlot);

  CreateGraphFile (stockGraph, fileName, params);

  CleanseGraph (stockGraph);
}
