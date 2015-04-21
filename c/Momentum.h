#ifndef MOMENTUM_H
#define MOMENTUM_H 1

#include "DbUtils.h"


typedef struct
{
  double Magnitude;
  double Direction;
} Vector;

typedef struct
{
  Vector Velocity;
  double Mass;
} Momentum;

typedef struct
{
  unsigned int Count;
  Momentum* Momentums;
} MomentumHistory;

typedef struct 
{
  double TractionCoefficient;
  double LiftCoefficient;

  double Gravity;
  double AirDensity;
  double AngleOfAttack;

  // Height = Width. Length = (Height * AspectRatio)
  double VehicleAspectRatio;
  double VehicleDensity;

  double Acceleration;

} MomentumAttributes;

typedef Momentum* (*MomentumTranslator)(void* datum, void* attributes);

void HaltMomentum (Momentum* green);
Momentum* NewMomentum (double mass, double magnitude, double direction);
Momentum* ApplyMomentum (Momentum* inertial, double mass, double direction, MomentumAttributes* system);

void CleanseMomentumAttributes (MomentumAttributes* system);
MomentumAttributes* NewMomentumAttributes (double tCoefficient, double lCoefficient, double gravity, 
  double airDensity, double angleOfAttack, double vehicleAspectRatio, double vehicleDensity, double acceleration);

#define MOMENTUM_HISTORY(collection, ordinal, type) \
  NewPlotSet(sizeof (type), ordinal * sizeof (double), (DbCollection*) collection);

MomentumHistory* NewMomentumHistory (DbCollection* collection, MomentumTranslator translator);

void CleanseMomentumHistory (MomentumHistory* history);

#endif
