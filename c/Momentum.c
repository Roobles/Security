#include <stdlib.h>
#include <math.h>
#include <mysql.h>
#include "Momentum.h"

static float GetTraction (Momentum* inertial, MomentumAttributes* system);
static float GetDownforceMass (float mass, float magnitude, MomentumAttributes* system);
static float GetWeight (float mass, MomentumAttributes* system);

static float GetTurnRatio (float direction, float targetDirection);
static float GetRequiredSpeed (float intialSpeed, float mass, float turnRatio, float traction);
static float GetBreakForce (float initialSpeed, float mass, float adjustedSpeed, MomentumAttributes* system);
static float GetAdjustedSpeed (float initialSpeed, float requiredSpeed, float breakForce, float traction, MomentumAttributes* system);

static Momentum* Blossom (Momentum* inertial, float mass, float direction, MomentumAttributes* system);


// Implementation of Momentum.h
void HaltMomentum (Momentum* green)
{
  free (green);
}

Momentum* NewMomentum (float mass, float magnitude, float direction)
{
  Momentum* prodigy;
  prodigy = malloc (sizeof (Momentum));

  prodigy->Mass = mass;
  prodigy->Velocity.Magnitude = magnitude;
  prodigy->Velocity.Direction = direction;

  return prodigy;
}

Momentum* ApplyMomentum (Momentum* inertial, float mass, float direction, MomentumAttributes* system)
{
  Momentum* prodigy;

  prodigy = Blossom (inertial, mass, direction, system);

  HaltMomentum (inertial);
  return prodigy;
}

void CleanseMomentumAttributes (MomentumAttributes* system)
{
  free (system);
}


MomentumAttributes* NewMomentumAttributes (float tCoefficient, float lCoefficient, float gravity, 
  float airDensity, float angleOfAttack, float vehicleAspectRatio, float vehicleDensity, float acceleration)
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
  system->Acceleration = acceleration;

  return system;
}

// Static Functions
static Momentum* Blossom (Momentum* inertial, float mass, float direction, MomentumAttributes* system)
{
  float requiredTurn, turnRatio, breakForce, requiredSpeed;
  float traction, initialSpeed, speed;

  traction = GetTraction (inertial, system);
  initialSpeed = inertial->Velocity.Magnitude;

  turnRatio = GetTurnRatio (inertial->Velocity.Direction, direction);
  requiredSpeed = GetRequiredSpeed (initialSpeed, mass, turnRatio, traction);
  breakForce = GetBreakForce (initialSpeed, mass, requiredSpeed, system);
  speed = GetAdjustedSpeed (initialSpeed, requiredSpeed, breakForce, traction, system);

  return NewMomentum (mass, speed, direction);
}

static float GetTraction (Momentum* inertial, MomentumAttributes* system)
{
  float traction;
  float mass, magnitude, tCoefficient;
  float totalMass, downforceMass, weight;

  // F(N) = L x Cf
  // F  *is* Traction Force
  // L  *is* Load Weight
  // Cf *is* Traction Coefficient 
  // L(N) = Mass * Gravity Coefficient

  mass = inertial->Mass;
  magnitude = inertial->Velocity.Magnitude;

  downforceMass = GetDownforceMass (mass, magnitude, system);
  totalMass = mass + downforceMass;
  weight = GetWeight (totalMass, system);
  tCoefficient = system->TractionCoefficient;
  
  traction = totalMass * tCoefficient;

  return traction;
}

static float GetDownforceMass (float mass, float magnitude, MomentumAttributes* system)
{
  float vehicleDensity, aspectRatio; 
  float vehicleVolume, vehicleLength, vehicleHeight, vehicleWidth, wingHeight;
  float angleOfAttack, lCoefficient, gCoefficient, airDensity;
  float downforce;

  // Retrieve system variables.
  airDensity = system->AirDensity;
  vehicleDensity = system->VehicleDensity;
  aspectRatio = system->VehicleAspectRatio;
  angleOfAttack = system->AngleOfAttack;
  lCoefficient = system->LiftCoefficient;
  gCoefficient = system->Gravity;
  
  // Get height and length of vehicle.
  // Assume width = height, and width = wingspan.
  // Assume wing height = vehicle height.

  vehicleVolume = mass / vehicleDensity;
  vehicleLength = powf ((vehicleVolume / aspectRatio), (1.0/3.0));
  vehicleHeight = vehicleLength / aspectRatio;
  vehicleWidth = vehicleHeight;
  wingHeight = vehicleHeight / 10.0; // TODO: Make this variable.

  // D(N) = 1/2 WingSpan * Height * alpha * L * r * V^2
  // alpha = AngleOfAttack (radians)
  // L = Lift Coefficient
  // r = Air Resistance
  // V = Velocity

  downforce = (vehicleWidth * wingHeight * angleOfAttack * lCoefficient * airDensity * powf (magnitude, 2)) / 2;
  return downforce / gCoefficient;
}

static float GetWeight (float mass, MomentumAttributes* system)
{
  float gCoefficient;
  
  gCoefficient = system->Gravity;
  return mass * gCoefficient;
}

static float GetTurnRatio (float direction, float targetDirection)
{
  float inside, outside, target, maxDegree, pi;

  // angle = ^tan (theta)
  // theta = (m1 - m2) / (1 + m1*m2)
  float m1, m2, theta;
  m1 = direction;
  m2 = targetDirection;

  theta = (m1 - m2) / (1.0 + m1*m2);
  
  maxDegree = 180.0;
  pi = 3.14;

  inside = fabs ((atanf (theta) * maxDegree) / pi);
  outside = maxDegree - inside;

  target = ((m1 >= 0 && m2 >= 0) || (m1 <=0 && m2 <= 0))
    ? inside
    : outside;

  return target / maxDegree;
}

static float GetRequiredSpeed (float initialSpeed, float mass, float turnRatio, float traction)
{
  // momentum = speed * mass
  // turnForce = momentum * turnRatio
  // turnForce = traction (Make it so.)

  // traction <= momentum * turnRatio
  // traction <= speed * mass * turnRatio
  // speed = traction / (mass * turnRatio);

  float turnForce, speed, momentum;
  float breakAmmount, acceleration;

  speed = initialSpeed;
  momentum = speed * mass;
  turnForce = momentum * turnRatio;

  // If turn force exceeds traction, you need to slow down.
  if (turnForce > traction)
    speed = traction / (mass * turnRatio);

  return speed;
}

static float GetBreakForce (float initialSpeed, float mass, float adjustedSpeed, MomentumAttributes* system)
{
  // TODO: Implement this.
  return 0;
}

static float GetAdjustedSpeed (float initialSpeed, float requiredSpeed, float breakForce, float traction, MomentumAttributes* system)
{
  // TODO: Implement this.
  float accelleration;
  accelleration = 10;
  
  // TODO: No.  Seriously.  Do this.
  return requiredSpeed + (((traction - breakForce) / traction) * accelleration);
}
