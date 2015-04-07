#include <stdlib.h>
#include <math.h>
#include "Momentum.h"

static float Supplication (Momentum* inertial, float mass);
static float GetMomentum (Momentum* inertial);
static float GetTraction (Momentum* inertial, MomentumAttributes* system);
static float GetDownforceMass (float mass, float magnitude, MomentumAttributes* system);
static float GetWeight (float mass, MomentumAttributes* system);
static float AboutFace (float momentum, float traction, float requestFacing, MomentumAttributes* system);
static float KineticLoss (float momentum, float requestFacing, float deltaFacing, MomentumAttributes* system);
static float Hunger (float magnitude, float kineticLoss, MomentumAttributes* system);

static Momentum* Blossom (Momentum* inertial, float mass, float supplication, MomentumAttributes* system);


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

Momentum* ApplyMomentum (Momentum* inertial, float mass, MomentumAttributes* system)
{
  float supplication;
  Momentum* prodigy;

  supplication = Supplication (inertial, mass);
  prodigy = Blossom (inertial, mass, supplication, system);

  HaltMomentum (inertial);
  return prodigy;
}


MomentumAttributes* NewMomentumAttributes ()
{
  MomentumAttributes* system;

  system = malloc (sizeof (MomentumAttributes));
  return system;
}

void CleanseMomentumAttributes (MomentumAttributes* system)
{
  free (system);
}

// Static Functions
static float GetMomentum (Momentum* inertial)
{
  return inertial->Mass * inertial->Velocity.Magnitude;
}

static float Supplication (Momentum* inertial, float mass)
{
  float direction, supplicant; 

  supplicant = inertial->Mass;
  direction = inertial->Velocity.Direction;

  return ((mass - supplicant) / (2 * mass)) * 100;
}

static Momentum* Blossom (Momentum* inertial, float mass, float supplication, MomentumAttributes* system)
{
  float requestFacing, facing, deltaFacing; 
  float momentum, traction, deltaKinetic, speed, magnitude;

  momentum = GetMomentum (inertial);
  traction = GetTraction (inertial, system);
  requestFacing = inertial->Velocity.Direction - supplication;
  magnitude = inertial->Velocity.Magnitude;

  facing = AboutFace (momentum, traction, requestFacing, system);
  deltaFacing = requestFacing - facing;
  deltaKinetic = KineticLoss (momentum, requestFacing, deltaFacing, system);
  speed = Hunger (magnitude, deltaKinetic, system);

  return NewMomentum (mass, speed, facing);
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
  float vehicleVolume, vehicleLength, vehicleHeight;
  float angleOfAttack, lCoefficient, gCoefficient, airDensity;
  float downforce;

  // Get height and length of vehicle.
  // Assume width = height, and width = wingspan.
  // Assume wing height = vehicle height.

  vehicleVolume = mass / vehicleDensity;
  vehicleHeight = powf ((vehicleVolume / aspectRatio), (1.0/3.0));
  vehicleLength = vehicleHeight / aspectRatio;

  // D(N) = 1/2 WingSpan * Height * alpha * L * r * V^2
  // alpha = AngleOfAttack (radians)
  // L = Lift Coefficient
  // r = Air Resistance
  // V = Velocity

  airDensity = system->AirDensity;
  vehicleDensity = system->VehicleDensity;
  aspectRatio = system->VehicleAspectRatio;
  angleOfAttack = system->AngleOfAttack;
  lCoefficient = system->LiftCoefficient;
  gCoefficient = system->GravityCoefficient;
  
  downforce = (vehicleLength * vehicleHeight * angleOfAttack * lCoefficient * airDensity * powf (magnitude, 2)) / 2;
  return downforce / gCoefficient;
}

static float GetWeight (float mass, MomentumAttributes* system)
{
  float gCoefficient;
  
  gCoefficient = system->GravityCoefficient;
  return mass * gCoefficient;
}

static float AboutFace (float momentum, float traction, float requestFacing, MomentumAttributes* system)
{
  float deltaFacing;

  // TODO: Write this!
  deltaFacing = 0;

  return deltaFacing;
}

static float KineticLoss (float momentum, float requestFacing, float deltaFacing, MomentumAttributes* system)
{
  float deltaKinetic;

  // TODO: Write this!
  deltaKinetic = 0;

  return deltaKinetic;
}

static float Hunger (float magnitude, float kineticLoss, MomentumAttributes* system)
{
  float speed;

  // TODO: Write this!
  speed = 0;

  return speed;
}
