#include <stdlib.h>
#include <math.h>
#include <mysql.h>
#include "Momentum.h"

typedef struct
{
  double Width;
  double Height;
  double Length;
  double Mass;
  double Volume;
} VehicleAttributes;

static void GetVehicleAttributes (VehicleAttributes* specs, MomentumTangents* tangents, MomentumAttributes* system);
static double GetTraction (VehicleAttributes* specs, double speed, MomentumAttributes* system);
static double GetDownforceMass (VehicleAttributes* specs, double speed, MomentumAttributes* system);
static double GetWeight (double mass, MomentumAttributes* system);

static double GetTurnRatio (double direction, double targetDirection);
static double GetRequiredSpeed (double intialSpeed, double mass, double turnRatio, double traction);
static double GetBreakForce (double initialSpeed, double mass, double requiredSpeed, MomentumAttributes* system);

static double GetMaxAcceleration (VehicleAttributes* specs, double initialSpeed, MomentumAttributes* system);

static double GetAdjustedSpeed (VehicleAttributes* specs, double initialSpeed, double requiredSpeed,
  double breakForce, double traction, MomentumAttributes* system);

// Implementation of Momentum.h
void SetMomentum (Momentum* prodigy, MomentumTangents tangents, double magnitude)
{
  prodigy->Mass = tangents.Mass;
  prodigy->Velocity.Direction = tangents.Direction;

  prodigy->Velocity.Magnitude = magnitude;
}

void ApplyMomentum (Momentum* inertial, Momentum* prodigy, MomentumTangents tangents, MomentumAttributes* system)
{
  VehicleAttributes specs;
  double requiredTurn, turnRatio, breakForce, requiredSpeed;
  double traction, initialSpeed, speed;

  GetVehicleAttributes (&specs, &tangents, system);

  initialSpeed = inertial->Velocity.Magnitude;

  traction = GetTraction (&specs, initialSpeed, system);
  turnRatio = GetTurnRatio (inertial->Velocity.Direction, tangents.Direction);
  requiredSpeed = GetRequiredSpeed (initialSpeed, specs.Mass, turnRatio, traction);
  breakForce = GetBreakForce (initialSpeed, specs.Mass, requiredSpeed, system);
  speed = GetAdjustedSpeed (&specs, initialSpeed, requiredSpeed, breakForce, traction, system);

  SetMomentum (prodigy, tangents, speed);
}

void CleanseMomentumAttributes (MomentumAttributes* system)
{
  tryfree (system);
}

MomentumAttributes* NewMomentumAttributes (double tCoefficient, double lCoefficient, double gravity, 
  double airDensity, double angleOfAttack, double vehicleAspectRatio, double vehicleDensity,
  double vehicleThrust, double vehicleDrag, double sprintLength)
{
  MomentumAttributes* system;

  system = malloc (sizeof (MomentumAttributes));
  system->TractionCoefficient = tCoefficient;
  system->LiftCoefficient = lCoefficient;
  system->Gravity = gravity;
  system->AirDensity = airDensity;
  system->AngleOfAttack = angleOfAttack;
  system->VehicleAspectRatio = vehicleAspectRatio;
  system->VehicleDensity = vehicleDensity;
  system->VehicleThrustForce = vehicleThrust;
  system->VehicleDragCoefficient = vehicleDrag;
  system->SprintLength = sprintLength;

  return system;
}

MomentumHistory* NewMomentumHistory (DbCollection* collection, int structSize, MomentumInitializor init,
  TangentalOperator tangental, void* attributes, MomentumAttributes* system)
{
  int i, count;
  MomentumHistory* history;

  count = collection->Count;
  history = malloc (sizeof (MomentumHistory));
  history->Count = count;
  history->Momentums = malloc (sizeof (Momentum) * count);
  
  init (history->Momentums, collection->Data, attributes);
  for (i=1; i<count; i++)
  {
    void *childDatum, *parentDatum;
    Momentum *inertial, *prodigy;
    MomentumTangents tangents;
  
    parentDatum = collection->Data + (structSize * (i-1));
    childDatum = collection->Data + (structSize * i);

    inertial = &history->Momentums[i-1];
    prodigy = &history->Momentums[i];

    tangents = tangental (parentDatum, childDatum, attributes);
    ApplyMomentum (inertial, prodigy, tangents, system);
  }

  return history;
}

void CleanseMomentumHistory (MomentumHistory* history)
{
  history->Count = 0;
  
  tryfree (history->Momentums);
  tryfree (history);
}

// Static Functions
static void GetVehicleAttributes (VehicleAttributes* specs, MomentumTangents* tangents, MomentumAttributes* system)
{
  double mass, volume;
  double width, length, height;
  double density, aspectRatio;

  mass = tangents->Mass;
  aspectRatio = system->VehicleAspectRatio;
  density = system->VehicleDensity;
  volume = mass / density;

  length = powf (volume / powf(aspectRatio, 2.0), (1.0/3.0));
  width = length * aspectRatio;
  height = width;

  specs->Width = width;
  specs->Height = height;
  specs->Length = length;
  specs->Mass = mass;
  specs->Volume = volume;
}

static double GetTraction (VehicleAttributes* specs, double speed, MomentumAttributes* system)
{
  double traction;
  double mass, magnitude, tCoefficient;
  double totalMass, downforceMass, weight;

  // F(N) = L x Cf
  // F  *is* Traction Force
  // L  *is* Load Weight
  // Cf *is* Traction Coefficient 
  // L(N) = Mass * Gravity Coefficient

  mass = specs->Mass;
  downforceMass = GetDownforceMass (specs, speed, system);
  totalMass = mass + downforceMass;
  weight = GetWeight (totalMass, system);
  tCoefficient = system->TractionCoefficient;
  
  traction = totalMass * tCoefficient;

  return traction;
}

static double GetDownforceMass (VehicleAttributes* specs, double speed, MomentumAttributes* system)
{
  double wingWidthRatio, wingHeightRatio;
  double vehicleHeight, vehicleWidth, wingHeight, wingWidth;
  double angleOfAttack, lCoefficient, gCoefficient, airDensity;
  double downforce;

  // Retrieve system variables.
  airDensity = system->AirDensity;
  angleOfAttack = system->AngleOfAttack;
  lCoefficient = system->LiftCoefficient;
  gCoefficient = system->Gravity;

  // Assume width = height, and width = wingspan.
  vehicleHeight = specs->Height;
  vehicleWidth = specs->Width;

  // TODO: Make these variable.
  wingWidthRatio = 0.95;
  wingHeightRatio = 0.10;

  wingHeight = vehicleHeight * wingHeightRatio; 
  wingWidth = vehicleWidth * wingWidthRatio;

  // D(N) = 1/2 WingSpan * Height * alpha * L * r * V^2
  // alpha = AngleOfAttack (radians)
  // L = Lift Coefficient
  // r = Air Resistance
  // V = Velocity

  downforce = (vehicleWidth * wingHeight * angleOfAttack * lCoefficient * airDensity * powf (speed, 2.0)) / 2;
  return downforce / gCoefficient;
}

static double GetWeight (double mass, MomentumAttributes* system)
{
  double gCoefficient;
  
  gCoefficient = system->Gravity;
  return mass * gCoefficient;
}

static double GetTurnRatio (double direction, double targetDirection)
{
  double inside, outside, target, maxDegree, pi;

  // angle = ^tan (theta)
  // theta = (m1 - m2) / (1 + m1*m2)
  double m1, m2, theta;
  m1 = direction;
  m2 = targetDirection;

  theta = (m1 - m2) / (1.0 + m1*m2);
  
  maxDegree = 180.0;
  inside = fabs ((atanf (theta) * maxDegree) / M_PI);
  outside = maxDegree - inside;

  target = ((m1 >= 0 && m2 >= 0) || (m1 <=0 && m2 <= 0))
    ? inside
    : outside;

  return target / maxDegree;
}

static double GetRequiredSpeed (double initialSpeed, double mass, double turnRatio, double traction)
{
  // momentum = speed * mass
  // turnForce = momentum * turnRatio
  // turnForce = traction (Make it so.)

  // traction <= momentum * turnRatio
  // traction <= speed * mass * turnRatio
  // speed = traction / (mass * turnRatio);

  double turnForce, speed, momentum;
  double breakAmmount;

  speed = initialSpeed;
  momentum = speed * mass;
  turnForce = momentum * turnRatio;

  // If turn force exceeds traction, you need to slow down.
  if (turnForce > traction)
    speed = traction / (mass * turnRatio);

  return speed;
}

static double GetBreakForce (double initialSpeed, double mass, double requiredSpeed, MomentumAttributes* system)
{
  double deltaSpeed, sprintLength, time;
  double decelleration;

  // time = t
  // acceleration = A m/s^2
  // speed = A * t m/s
  // acceleration = dS / t
  // speed = dD / t
  // dD/t = dS/t * t | dD/t = dS | t = dD/dS
  // A = speed/t | dD/t * dS/dD | dS/t | dS * dS/dD
  // A = dS^2 / dD

  deltaSpeed = initialSpeed - requiredSpeed;
  decelleration = (deltaSpeed * deltaSpeed) / sprintLength;
  return decelleration * mass;
}

static double GetMaxAcceleration (VehicleAttributes* specs, double initialSpeed, MomentumAttributes* system)
{
  // TODO: Consider using calculus.
  double dragCoefficient, area, airDensity;
  double dragForce, thrustForce, netForce;

  area = specs->Width * specs->Height;
  dragCoefficient = 0.914; 
  airDensity = system->AirDensity;
  dragForce = (dragCoefficient * area * airDensity * powf (initialSpeed, 2.0)) / 2; // TODO: Implement this.

  thrustForce = system->VehicleThrustForce;
  netForce = thrustForce > dragForce 
    ? thrustForce - dragForce
    : 0.01;

  return netForce / specs->Mass;
}

static double GetAdjustedSpeed (VehicleAttributes* specs, double initialSpeed, double requiredSpeed,
  double breakForce, double traction, MomentumAttributes* system)
{
  double acceleration, sprintLength;
  double initialDistance, finalDistance;
  double initialTime, finalTime, deltaTime;
  double maxAcceleration;

  double breakRatio, accelerationRatio, deltaSpeed;
  double adjustedSpeed;

  acceleration = GetMaxAcceleration (specs, initialSpeed, system);
  sprintLength = system->SprintLength;
  breakRatio = breakForce / traction;
  accelerationRatio = (1 - breakRatio);

  // time = t
  // acceleration = A m/s^2
  // speed = A * t m/s
  // distance = A/2 * t^2 m

  // initialDistance = s/(2time) * t^2 | 1/2 * s * t | 1/2 * s * s/a | s^2/(2a)
  // initialDistance = s^2/(2a)
  // finalDistance = initialDistance + sprintLength
  // time = (2*distance2 / a)^(1/2)
  // initialTime = ((2 * initialDistance) / a)^(1/2)
  // finalTime = ((2 * finalDistance) / a )^(1/2)

  // maxSpeed = (a * deltaTime) +  initialSpeed

  initialDistance = (initialSpeed * initialSpeed) / (2 * acceleration);
  finalDistance = initialDistance + sprintLength;
  initialTime = sqrt ((2 * initialDistance) / acceleration);
  finalTime = sqrt ((2 * finalDistance) / acceleration);
  deltaTime = finalTime - initialTime;
  maxAcceleration = (acceleration * deltaTime);

  deltaSpeed = maxAcceleration * accelerationRatio;
  adjustedSpeed = requiredSpeed + deltaSpeed;

  return adjustedSpeed;
}
