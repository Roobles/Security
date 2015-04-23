#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <plot.h>
#include <float.h>
#include <string.h>

#include "DbPlot.h"
#include "DbUtils.h"

static void SetPage (FILE* file, DbGraphParams* params, plPlotter** pplotter, plPlotterParams** pplparams);
static void DrawBox (plPlotter* plotter, DbGraphParams* params);
static void DrawGraph (plPlotter* plotter, DbPlotSet* plots, DbPlotColor color, DbGraphParams* params);

static DbPlotCoordinate DrawGraphLine (plPlotter* plotter, DbPlotCoordinate last, DbPlotSet* points,
  int itter, double xUnitSize, double yUnitSize, double padding);

DbGraphParams* NewGraphParams (const char* type, double width, double height, double padding,
  double margin, double dpi, DbPlotPalette* lineColors, DbPlotColor boxColor, DbPlotColor fillColor)
{
  DbGraphParams* params;

  params = malloc (sizeof (DbGraphParams));

  params->Width = width;
  params->Height = height;
  params->Padding = padding;
  params->Margin = margin;
  params->DPI = dpi;
  params->LineColors = lineColors;
  params->BoxColor = boxColor;
  params->FillColor = fillColor;

  strcpy (params->Type, type);

  return params;
}

DbPlotSet* NewPlotSet (int structSize, int offset, DbCollection* collection)
{
  int i, count; 
  DbPlotSet* plotSet;
  double maxInit, minInit;

  maxInit = DBL_MIN;
  minInit = DBL_MAX;

  count = collection->Count;
  plotSet = malloc (sizeof (DbPlotSet));

  plotSet->Count = count;
  plotSet->Data = malloc (sizeof (double) * count);

  plotSet->Highest = &maxInit;
  plotSet->Lowest = &minInit;

  for (i=0; i<count; i++)
  {
    void* targetDatum;
    double* targetValue;

    targetDatum = collection->Data + (i * structSize);
    targetValue = targetDatum + offset;   

    plotSet->Data[i] = *targetValue;

    if (*plotSet->Highest < *targetValue) plotSet->Highest = &plotSet->Data[i];
    if (*plotSet->Lowest > *targetValue) plotSet->Lowest = &plotSet->Data[i];
  }
  
  return plotSet;
}

DbGraph* NewGraph (int plots, ...)
{
  int i;
  va_list ap;
  DbGraph* graph;
  double maxInit, minInit;
  
  if (plots < 1)
    return NULL;

  maxInit = DBL_MIN;
  minInit = DBL_MAX;

  graph = malloc (sizeof (DbGraph));

  graph->Count = plots;
  graph->Sets = malloc (sizeof (DbPlotSet*) * plots);

  graph->Highest = &maxInit;
  graph->Lowest = &minInit;

  va_start (ap, plots);
  for (i=0; i<plots; i++)
  {
    DbPlotSet* plots = va_arg (ap, DbPlotSet*);
    graph->Sets[i] = plots;

    if (*graph->Highest < *graph->Sets[i]->Highest) graph->Highest = graph->Sets[i]->Highest;
    if (*graph->Lowest > *graph->Sets[i]->Lowest) graph->Lowest = graph->Sets[i]->Lowest;
  }

  va_end (ap);
  return graph;
}

DbPlotColor NewColor (int red, int green, int blue, double weight)
{
  DbPlotColor color;
  const char* colorFmt = "#%02x%02x%02x";

  color.Red = red;
  color.Green = green;
  color.Blue = blue;
  color.Weight = weight;

  sprintf (color.Color, colorFmt, red, green, blue);

  return color;
}

void CreateGraphFile (DbGraph* graph, const char* file, DbGraphParams* params)
{
  int i, j;
  plPlotter* plotter;
  plPlotterParams* plparams;
  FILE* outputFile;
  DbPlotColor* colors = params->LineColors->Colors;
  int colorCount = params->LineColors->Count;
   
  // Initialize plotter.
  outputFile = fopen (file, "w+");
  SetPage (outputFile, params, &plotter, &plparams);

  // Create Box.
  DrawBox (plotter, params);


  // Draw Graphs.
  for (i=graph->Count -1; i>=0; i--)
    DrawGraph (plotter, graph->Sets[i], colors[i%colorCount], params);
    
  // Close up shop.
  pl_closepl_r (plotter);
  pl_deletepl_r (plotter);
  pl_deleteplparams (plparams);
  fclose (outputFile);
}

void CleansePlotSet (DbPlotSet* plots)
{
  plots->Count = 0;
  plots->Highest = 0;
  plots->Lowest = 0;

  tryfree (plots->Data);
  tryfree (plots);
}

void CleanseGraph (DbGraph* graph)
{
  int i;
  for (i=0; i<graph->Count; i++)
    CleansePlotSet (graph->Sets[i]);

  graph->Count = 0;
  graph->Highest = 0;
  graph->Lowest = 0;

  tryfree (graph->Sets);
  tryfree (graph);
}

void CleanseGraphParams (DbGraphParams* params)
{
  CleansePlotPalette (params->LineColors);
  tryfree (params);
}

void CleansePlotPalette (DbPlotPalette* palette)
{
  tryfree (palette->Colors);
  tryfree (palette);
}

// Static Functions
static void SetPage (FILE* file, DbGraphParams* params, plPlotter** pplotter, plPlotterParams** pplparams)
{
  char pagesize[64];
  double height, width, dpi;
  const char* pageFmt = "letter,xsize=%.1fin,ysize=%.1fin";
  plPlotter* plotter;
  plPlotterParams* plparams;

  height = params->Height;
  width = params->Width;
  dpi = params->DPI;
  sprintf (pagesize, pageFmt, width / dpi, height / dpi);

  // Set Parameters
  *pplparams = pl_newplparams ();
  plparams = *pplparams;
  pl_setplparam (plparams, "PAGESIZE", pagesize);

  // Open Plotter
  *pplotter = pl_newpl_r (params->Type, NULL, file, NULL, plparams);
  plotter = *pplotter;
  pl_openpl_r (plotter);
  pl_fspace_r (plotter, 0.0, 0.0, width, height);
  pl_erase_r (plotter);
}

static void DrawBox (plPlotter* plotter, DbGraphParams* params)
{
  double height, width;
  double margin, limit;
  char colorStr[8];
  DbPlotColor boxColor, fillColor;

  margin = params->Margin;
  height = params->Height - margin;
  width = params->Width - margin;
  boxColor = params->BoxColor;
  fillColor = params->FillColor;

  pl_flinewidth_r (plotter, boxColor.Weight);
  pl_pencolorname_r (plotter, boxColor.Color);
  pl_fillcolorname_r (plotter, fillColor.Color);
  pl_filltype_r (plotter, 1);

  pl_fline_r (plotter, margin, margin, margin, height);
  pl_fline_r (plotter, margin, height, width, height);
  pl_fline_r (plotter, width, height, width, margin);
  pl_fline_r (plotter, width, margin, margin, margin);

  pl_filltype_r (plotter, 0);
}

static void DrawGraph (plPlotter* plotter, DbPlotSet* plots, DbPlotColor color, DbGraphParams* params)
{
  int i;
  double padding;
  double xSpace, ySpace;
  double xDelta, yDelta;
  double xUnitSize, yUnitSize;
  DbPlotCoordinate last;

  // Derrive variables.
  padding = params->Padding + params->Margin;

  xSpace = params->Width - (2 * padding);
  ySpace = params->Height - (2 * padding);

  xDelta = plots->Count;
  yDelta = *plots->Highest - *plots->Lowest;
  
  xUnitSize = xSpace / xDelta;
  yUnitSize = ySpace / yDelta;

  // Set parameters.
  pl_flinewidth_r (plotter, color.Weight);
  pl_pencolorname_r (plotter, color.Color);

  // Plot points.
  for (i=0; i<plots->Count; i++)
    last = DrawGraphLine (plotter, last, plots, i, xUnitSize, yUnitSize, padding);
}

static DbPlotCoordinate DrawGraphLine (plPlotter* plotter, DbPlotCoordinate last, DbPlotSet* points,
  int itter, double xUnitSize, double yUnitSize, double padding)
{
  DbPlotCoordinate current;

  // Derrive coordinates
  current.X = (xUnitSize * (itter+1)) + padding;
  current.Y = (yUnitSize * (points->Data[itter] - *points->Lowest)) + padding;

  // Draw
  if (itter > 0) 
    pl_fline_r (plotter, last.X, last.Y, current.X, current.Y);

  return current;
}
