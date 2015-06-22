#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <regex.h>

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

#define DEFAULT_OUTPUT Console
typedef enum
{
  Unknown = 0x0,
  Console = 0x1,
  Graph = 0x2
} OutputType;

typedef struct
{
  char* UserName;
  char* Password;
  int StockId;
  int Detailed;

  OutputType Output;
  MomentumAttributes* System;
  StockMomentumAttributes* StockAttributes;
  DbGraphParams* GraphAttributes;
} CalibrationArgs;

typedef void (*DisplayFunction)(Stock* stock, StockHistory* stockHistory, MomentumHistory* momentumHistory, CalibrationArgs* args);

static void InitCalibrationArgs (CalibrationArgs* args);
static void ReadInput (int argc, char** argv, CalibrationArgs* args);
static void WipeInput (CalibrationArgs* args);
static int ValidateInput (CalibrationArgs* args);
static OutputType ParseOutput (const char* outArg);
static int Calibrate (CalibrationArgs* args);

static StockCollection* GetTargetStocks (CalibrationArgs* args);
static MomentumHistory* GetStockMomentum (Stock* stock, StockHistory* history, StockMomentumAttributes* stockAttr, MomentumAttributes* system);

static void ProcessStock (Stock* stock, CalibrationArgs* args);
static void DisplayStock (Stock* stock, StockHistory* stockHistory, MomentumHistory* momentumHistory, CalibrationArgs* args);
static void PrintStock (Stock* stock, StockHistory* stockHistory, MomentumHistory* momentumHistory, CalibrationArgs* args);
static void PrintStockDetailed (Stock* stock, StockHistory* stockHistory, MomentumHistory* momentumHistory, CalibrationArgs* args);
static void GraphStock (Stock* stock, StockHistory* stockHistory, MomentumHistory* momentumHistory, CalibrationArgs* params);

int main (int argc, char** argv)
{
  int result; 
  CalibrationArgs args;
  ReadInput (argc, argv, &args);

  if (!(result = ValidateInput (&args)))
    result = Calibrate (&args);

  WipeInput (&args);
  return result;
}

static void ReadInput (int argc, char** argv, CalibrationArgs* args)
{
  int c;
  char* output = NULL;
  const char* optstr = "du:p:s:o:h:w:l:";

  InitCalibrationArgs (args);
  while ((c = getopt (argc, argv, optstr)) > 0)
  {
    switch (c)
    {
      case 'u':
        args->UserName = optarg;
        break;

      case 'p':
        args->Password = optarg;  
        break;

      case 's':
        args->StockId = atoi (optarg);
        break;

      case 'o':
        output = optarg;
        break;

      case 'h':
        args->GraphAttributes->Height = atoi (optarg);
        break;

      case 'w':
        args->GraphAttributes->Width = atoi (optarg);
        break;

      case 'd':
        args->Detailed = 1;
        break;

      case 'l':
        args->System->SprintLength = atoi (optarg);
    }
  }

  args->Output = ParseOutput (output);
}

static void InitCalibrationArgs (CalibrationArgs* args)
{
  args->System = NULL;
  args->StockAttributes = NULL;
  args->GraphAttributes = NULL;
  args->Output = Unknown;
  WipeInput (args);

  args->System = GetMomentumAttributes ();
  args->StockAttributes = GetStockMomentumAttributes ();
  args->GraphAttributes = GetDbGraphParams ();
}

static void WipeInput (CalibrationArgs* args)
{
  args->UserName = NULL;
  args->Password = NULL;
  args->StockId = -1;
  args->Detailed = 0;
  
  CleanseMomentumAttributes (args->System);
  CleanseStockMomentumAttributes (args->StockAttributes);
  CleanseGraphParams (args->GraphAttributes);
  CleanseDbConnectionSettings ();
}


#define VALIDATE_CODE(expr, message, rtn, quiet) if (!(expr)) { if (!quiet) error (message); return rtn; }
#define VALIDATE(expr, message) VALIDATE_CODE(expr, message, 1, 0)
#define VALIDATE_STR(attr, attrName) VALIDATE (args->attr != NULL, "Must provide input for " #attrName ".")
#define VALIDATE_QUIET(expr) VALIDATE_CODE(expr, NULL, 1, 1)
static int ValidateInput (CalibrationArgs* args)
{
  VALIDATE_STR (UserName, user name);
  VALIDATE_STR (Password, password);

  SetDb (args->UserName, args->Password);
  VALIDATE_QUIET (!ValidateConnection());
  return 0;
}

static OutputType ParseOutput (const char* outArg)
{
  int cflags;
  regex_t graphExpr, consoleExpr;
  const char* graphExprStr = "^ *g(raph|rph|ph)? *$";
  const char* consoleExprStr = "^ *c(onsole|ons?)? *$";
  OutputType type;

  type = DEFAULT_OUTPUT;
  if (outArg != NULL)
  {
    cflags = 0 | REG_NOSUB | REG_EXTENDED | REG_ICASE;

    regcomp (&graphExpr, graphExprStr, cflags);
    regcomp (&consoleExpr, consoleExprStr, cflags);

    if (!regexec (&graphExpr, outArg, 0, NULL, 0))
      type = Graph;

    if (!regexec (&consoleExpr, outArg, 0, NULL, 0))
      type = Console;

    regfree (&graphExpr);
    regfree (&consoleExpr);
  }
  return type;
}

static int Calibrate (CalibrationArgs* args)
{
  int i;
  StockCollection* stocks;
  StockHistory* history;

  stocks = GetTargetStocks (args);
  for (i=0; i<stocks->Count; i++)
    ProcessStock (&stocks->Data[i], args);
  

  CleanseStocks (stocks);
  return 0;
}

static StockCollection* GetTargetStocks (CalibrationArgs* args)
{
  const char* targetStocks = "IsAlive = 1";
  return (args->StockId > 0)
    ? GetStockById (args->StockId)
    : GetStocks (targetStocks);
}

static MomentumAttributes* GetMomentumAttributes ()
{
  double degreeOfAttack;
  double tCoefficient, lCoefficient, gravity, airDensity, 
    angleOfAttack, vehicleAspectRatio, vehicleDensity,
    vehicleThrust, vehicleDrag, sprintLength;

  degreeOfAttack = 15;

  tCoefficient = 0.7;
  lCoefficient = 1;
  gravity = 9.8;
  airDensity = 1.2;
  angleOfAttack = (degreeOfAttack / 3.14) * 180.0;
  vehicleAspectRatio = .3;
  vehicleDensity = 151;
  vehicleThrust = 8500; 
  vehicleDrag = 0.914;
  sprintLength = 75;
  
  return NewMomentumAttributes (tCoefficient, lCoefficient, gravity,
    airDensity, angleOfAttack, vehicleAspectRatio, vehicleDensity,
    vehicleThrust, vehicleDrag, sprintLength);
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

#define SET_COLOR(ordinal, red, green, blue, weight) colors[ordinal] = NewColor (red, green, blue, weight)
#define SET_COLOR_D(ordinal, red, green, blue) SET_COLOR (ordinal, red, green, blue, lineWidth)
static DbGraphParams* GetDbGraphParams ()
{
  int colorCount;
  DbPlotPalette* lineColors;
  DbPlotColor* colors;
  DbPlotColor boxColor, fillColor;
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

  boxColor = NewColor (0x08, 0x17, 0x29, boxWidth); // 081729
  fillColor = NewColor (0xFF, 0xFF, 0xFD, 0); // F4F0EB

  lineColors = malloc (sizeof (DbPlotPalette));

  colorCount = 4;
  lineColors->Count = colorCount;
  lineColors->Colors = malloc (sizeof (DbPlotColor) * colorCount);
  colors = lineColors->Colors;

  SET_COLOR (0, 0x85, 0x3A, 0x59, 1); // #853A59
  SET_COLOR_D (1, 0xF5, 0x79, 0x62); // #F57962
  SET_COLOR_D (2, 0xBC, 0x9A, 0xD7); // #BC9AD7
  SET_COLOR_D (3, 0xC4, 0x43, 0x5A); // #C4435A

  return NewGraphParams (type, width, height, padding,
    margin, dpi, lineColors, boxColor, fillColor);
}


static void SetDb (char* user, char* pass)
{
  const char* host = "localhost";
  const char* dbName = "security";
  
  SetDbConnectionSettings (host, user, pass, dbName, 0, NULL, 0);
}

static void ProcessStock (Stock* stock, CalibrationArgs* args)
{
  int stockId;
  StockHistory* stockHistory;
  MomentumHistory* momentumHistory;

  stockId = stock->StockId;
  stockHistory = GetStockHistoryById (stockId);
  momentumHistory = NewStockMomentumHistory (stockHistory, args->System, args->StockAttributes);

  DisplayStock (stock, stockHistory, momentumHistory, args);

  CleanseStockHistory (stockHistory);
  CleanseMomentumHistory (momentumHistory);
}

static void DisplayStock (Stock* stock, StockHistory* stockHistory, MomentumHistory* momentumHistory, CalibrationArgs* args)
{

  DisplayFunction displayFunc = NULL;
  
  switch (args->Output)
  {
    case Console:
      displayFunc = args->Detailed 
        ? PrintStockDetailed
        : PrintStock;
      break;

    case Graph:
      displayFunc = GraphStock;
      break;

    default:
      displayFunc = NULL;
      break;
  }

  if (displayFunc != NULL)
    displayFunc (stock, stockHistory, momentumHistory, args);
}

static void PrintStock (Stock* stock, StockHistory* stockHistory, MomentumHistory* momentumHistory, CalibrationArgs* args)
{
  int i, count;
  double momentumSummation, avgMomentum;
  const char* outputFmt = "Stock: %-12dMomentum: %0.2f\n";

  count = stockHistory->Count;
  momentumSummation = 0;
  for (i=0; i<count; i++)
  {
    Momentum* momentum = &momentumHistory->Momentums[i];
    momentumSummation += (momentum->Mass * momentum->Velocity.Magnitude);
  }

  avgMomentum = (momentumSummation / (double) count);

  printf (outputFmt, stock->StockId, avgMomentum);
}

static void PrintStockDetailed (Stock* stock, StockHistory* stockHistory, MomentumHistory* momentumHistory, CalibrationArgs* args)
{
  int i, count;
  double speed, price, momentumVal;
  const double mphConv = 2.23694;
  const char* stockOutputFmt = "Stock: %s\n";
  const char* momentumFmt = "Speed: %.2f\tPrice: $%.2f\tMomentum: %.2f\n";

  printf (stockOutputFmt, stock->Ticker);
  count = stockHistory->Count;
  for (i = 0; i<count; i++)
  {
    Momentum* momentum = &momentumHistory->Momentums[i];
    StockData* data = &stockHistory->Data[i];
    speed = momentum->Velocity.Magnitude * mphConv;
    price = data->Close;
    momentumVal = momentum->Mass * momentum->Velocity.Magnitude;
    printf (momentumFmt, speed, price, momentumVal);
    //sleep (1);
  }
}

static void GraphStock (Stock* stock, StockHistory* stockHistory, MomentumHistory* momentumHistory, CalibrationArgs* args)
{
  char* ticker, *fileName;
  const char* fileFormat = "%s.%s";
  DbPlotSet *stockPlot, *momentumPlot; 
  DbGraph* stockGraph;
  DbGraphParams* params;

  // Set the File Name
  params = args->GraphAttributes;
  ticker = stock->Ticker;
  fileName = malloc (strlen (ticker) + strlen (fileFormat) + strlen (params->Type) -4 +1);
  sprintf (fileName, fileFormat, ticker, params->Type);

  stockPlot = PLOT_SET (stockHistory, Close, StockData);
  momentumPlot = PLOT_SET (momentumHistory, 0, Momentum);
  
  stockGraph = GRAPH (stockPlot, momentumPlot);

  CreateGraphFile (stockGraph, fileName, params);

  CleanseGraph (stockGraph);
}
