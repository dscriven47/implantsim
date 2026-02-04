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
  void SetSourcePosition(G4ThreeVector sourcePosition) { fSourcePosition=sourcePosition; }
  void SetSourceDirection(G4ThreeVector sourceDirection) { fSourceDirection=sourceDirection; }

  // optional getters (useful for RunAction filenames)
  const G4String& GetSourceMode() const { return fSourceMode; }
  G4double GetKineticEnergy() const { return fKineticEnergy; }
  G4ThreeVector GetSourcePosition() const { return fSourcePosition; }
  G4ThreeVector GetSourceDirection() const { return fSourceDirection; }

private:
  void DefineCommands();

  G4ParticleGun* fParticleGun = nullptr;
  G4GenericMessenger* fMessenger = nullptr;

  // Macro-controlled parameters
  G4String fSourceMode = "electron";
  G4double fKineticEnergy = 50.0;
  G4ThreeVector fSourcePosition = G4ThreeVector(0.,0.,1.*cm);
  G4ThreeVector fSourceDirection = G4ThreeVector(0.,0.,1.);

  // definition for gun type
  G4String fGunType = "point";

  // beam controls
  G4ThreeVector fConeAxis = G4ThreeVector(0., 0., -1.);
  G4double fConeAngle = 180.0*deg;
  G4double fConeApexRadius = 3.0*cm;

  // stuff to sample position
  G4double SampleImplantDepth();
  std::vector<G4double> fX;
  std::vector<G4double> fCDF;
};



#endif
