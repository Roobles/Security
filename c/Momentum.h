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
  float LiftCoefficient;

  float Gravity;
  float AirDensity;
  float AngleOfAttack;

  // Height = Width. Length = (Height * AspectRatio)
  float VehicleAspectRatio;
  float VehicleDensity;

} MomentumAttributes;

void HaltMomentum (Momentum* green);
Momentum* NewMomentum (float mass, float magnitude, float direction);
Momentum* ApplyMomentum (Momentum* inertial, float mass, float direction, MomentumAttributes* system);

void CleanseMomentumAttributes (MomentumAttributes* system);
MomentumAttributes* NewMomentumAttributes ();

#endif
