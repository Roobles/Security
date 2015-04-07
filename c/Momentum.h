#ifndef MOMENTUM_H
#define MOMENTUM_H

typedef struct
{
  float Magnitude;
  float Direction;
} Vector;

typedef struct
{
  Vector Velocity;
  float Mass;
} Momentum;

typedef struct 
{
  float TractionCoefficient;
  float GravityCoefficient;
  float LiftCoefficient;

  float AirDensity;
  float AngleOfAttack;

  float VehicleDensity;

  // Height = Width. Length = (Height * AspectRatio)
  float VehicleAspectRatio;
} MomentumAttributes;

void HaltMomentum (Momentum* green);
Momentum* NewMomentum (float mass, float magnitude, float direction);
Momentum* ApplyMomentum (Momentum* inertial, float mass, MomentumAttributes* system);

void CleanseMomentumAttributes (MomentumAttributes* system);
MomentumAttributes* NewMomentumAttributes ();




#endif
