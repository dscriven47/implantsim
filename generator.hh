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


class G4ParticleGun;
class G4GenericMessenger;
class MyPrimaryGenerator : public G4VUserPrimaryGeneratorAction
{
public:
  MyPrimaryGenerator();
  ~MyPrimaryGenerator() override;

  void GeneratePrimaries(G4Event* anEvent) override;

  // setters used by messenger
  void SetSourceMode(const G4String& mode) { fSourceMode = mode; }
  void SetKineticEnergy(G4double e) { fKineticEnergy = e; }

  // optional getters (useful for RunAction filenames)
  const G4String& GetSourceMode() const { return fSourceMode; }
  G4double GetKineticEnergy() const { return fKineticEnergy; }

private:
  void DefineCommands();

  G4ParticleGun* fParticleGun = nullptr;
  G4GenericMessenger* fMessenger = nullptr;

  // Macro-controlled parameters:
  G4String fSourceMode = "electron";
  G4double fKineticEnergy = 1500.0; // will set to MeV units in ctor

  //
  G4String fGunType = "point";

  // Your beam controls (optional to macro-control later)
  G4ThreeVector fConeAxis;
  G4double fConeAngle = 180.0;
  G4double fConeApexRadius = 3.0;
};


#endif
