#ifndef MOMENTUM_H
#define MOMENTUM_H 1

#include "DbUtils.h"

typedef struct
{
  double Mass;
  double Direction;
} MomentumTangents;

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
  double VehicleThrustForce;
  double VehicleDragCoefficient;

  double SprintLength;

} MomentumAttributes;

typedef void (*MomentumInitializor)(Momentum* momentum, void* datum, void* attributes);
typedef MomentumTangents (*TangentalOperator)(void* parentDatum, void* childDatum, void* attributes);

void SetMomentum (Momentum* prodigy, MomentumTangents tangents, double magnitude);
void ApplyMomentum (Momentum* inertial, Momentum* prodigy, MomentumTangents tangents, MomentumAttributes* system);

void CleanseMomentumAttributes (MomentumAttributes* system);
MomentumAttributes* NewMomentumAttributes (double tCoefficient, double lCoefficient, double gravity, 
  double airDensity, double angleOfAttack, double vehicleAspectRatio, double vehicleDensity,
  double vehicleThrust, double vehicleDrag, double sprintLength);

#define MOMENTUM_HISTORY(collection, type, init, tangental, attributes, system)               \
  NewMomentumHistory ((DbCollection*) collection, sizeof (type), (MomentumInitializor) init,  \
  (TangentalOperator) tangental, (void*) attributes, system);

MomentumHistory* NewMomentumHistory (DbCollection* collection, int structSize, MomentumInitializor init,
  TangentalOperator tangental, void* attributes, MomentumAttributes* system);

void CleanseMomentumHistory (MomentumHistory* history);

#endif
