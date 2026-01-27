#ifndef GENERATOR_HH
#define GENERATOR_HH

#include "G4VUserPrimaryGeneratorAction.hh"

#include "G4ParticleGun.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParticleTable.hh"
#include "G4Geantino.hh"
#include "G4IonTable.hh"
#include "G4String.hh"
#include "globals.hh"

class MyPrimaryGenerator : public G4VUserPrimaryGeneratorAction
{
public:
MyPrimaryGenerator();
~MyPrimaryGenerator() override;


void GeneratePrimaries(G4Event* anEvent) override;


// Choose what to generate: "Ba133", "gamma", "neutron", "alpha"
void SetSourceMode(const G4String& mode) { fSourceMode = mode; }


// Optional: set kinetic energy for particle sources (gamma/neutron/alpha)
void SetKineticEnergy(G4double ekin) { fKineticEnergy = ekin; }


private:
G4ParticleGun* fParticleGun = nullptr;
G4ThreeVector fConeAxis;
G4double fConeAngle;
G4double fConeApexRadius;
G4String fParticleName;


G4String fSourceMode = "gamma"; // default behavior
G4double fKineticEnergy = 100.*keV; // used for gamma/neutron/alpha
};


#endif
