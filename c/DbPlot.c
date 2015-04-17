#include <stdlib.h>
#include <stdarg.h>

#include "DbPlot.h"
#include "DbUtils.h"

DbPlotSet* NewPlotSet (int structSize, int offset, DbCollection* collection)
{
  int i, count; 
  DbPlotSet* plotSet;

  count = collection->Count;
  plotSet = malloc (sizeof (DbPlotSet));

  plotSet->Count = count;
  plotSet->Data = malloc (sizeof (float) * count);

  for (i=0; i<count; i++)
  {
    void* targetDatum;
    float* targetValue;

    targetDatum = collection->Data + (i * structSize);
    targetValue = targetDatum + offset;   

    plotSet->Data[i] = *targetValue;
  }
  
  return plotSet;
}

DbGraph* NewGraph (int plots, ...)
{
  int i;
  va_list ap;
  DbGraph* graph;
  
  if (plots < 1)
    return NULL;

  graph = malloc (sizeof (DbGraph));

  graph->Count = plots;
  graph->Sets = malloc (sizeof (DbPlotSet*) * plots);

  va_start (ap, plots);
  for (i=0; i<plots; i++)
  {
    DbPlotSet* plots = va_arg (ap, DbPlotSet*);
    graph->Sets[i] = plots;
  }

  va_end (ap);
  return graph;
}

void CreateGraphFile (DbGraph* graph, const char* file)
{
  
   
}

void CleansePlotSet (DbPlotSet* plots)
{
  plots->Count = 0;
  free (plots->Data);
  free (plots);
}

void CleanseGraph (DbGraph* graph)
{
  int i;
  for (i=0; i<graph->Count; i++)
    CleansePlotSet (graph->Sets[i]);

  graph->Count = 0;
  free (graph->Sets);
  free (graph);
}

