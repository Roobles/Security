#ifndef DB_PLOT_H
#define DB_PLOT_H 1

#include "DbUtils.h"

typedef struct
{
  double X;
  double Y;
} DbPlotCoordinate;

typedef struct
{
  int Red;
  int Green;
  int Blue;
  double Weight;
  char Color[8];
} DbPlotColor;

typedef struct
{
  int Count;
  DbPlotColor* Colors;
} DbPlotPalette;

typedef struct
{
  char Type[16];

  double Width;
  double Height;
  double Padding;
  double Margin;
  double DPI;

  double LineWidth;
  DbPlotPalette* LineColors;

  double BoxWidth;
  DbPlotColor BoxColor;
  DbPlotColor FillColor;
} DbGraphParams;

typedef struct
{
  unsigned int Count;
  double* Data;
  double* Highest;
  double* Lowest;
} DbPlotSet;

typedef struct 
{
  unsigned int Count;
  DbPlotSet** Sets;
  
  double* Highest;
  double* Lowest;
} DbGraph;

#define PLOT_COLOR(red, green, blue) (DbPlotColor) {.Red = red, .Green = green, .Blue = blue}
#define GRAPH(...) NewGraph (NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

#define PLOT_SET(collection, ordinal, type) \
  NewPlotSet(sizeof (type), ordinal * sizeof (double), (DbCollection*) collection);

DbPlotSet* NewPlotSet (int structSize, int offset, DbCollection* collection);
DbGraph* NewGraph (int plots, ...);
DbPlotColor NewColor (int red, int green, int blue, double weight);

DbGraphParams* NewGraphParams (const char* type, double width, double height, double padding,
  double margin, double dpi, DbPlotPalette* lineColors, DbPlotColor boxColor, DbPlotColor fillColor);

void CreateGraphFile (DbGraph* graph, const char* file, DbGraphParams* params);

void CleansePlotSet (DbPlotSet* plots);
void CleanseGraph (DbGraph* graph);
void CleanseGraphParams (DbGraphParams* params);
void CleansePlotPalette (DbPlotPalette* palette);
#endif 
