#ifndef DB_PLOT_H
#define DB_PLOT_H 1

#include "DbUtils.h"

typedef struct
{
  unsigned int Count;
  float* Data;
} DbPlotSet;

typedef struct 
{
  unsigned int Count;
  DbPlotSet** Sets;
} DbGraph;

#define GRAPH(...) NewGraph (NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

#define PLOT_SET(collection, ordinal, type) \
  NewPlotSet(sizeof (type), ordinal * sizeof (float), (DbCollection*) collection);

DbPlotSet* NewPlotSet (int structSize, int offset, DbCollection* collection);
DbGraph* NewGraph (int plots, ...);
void CreateGraphFile (DbGraph* graph, const char* file);

void CleansePlotSet (DbPlotSet* plots);
void CleanseGraph (DbGraph* graph);
#endif 
