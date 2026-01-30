#include "generator.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4IonTable.hh"
#include "G4ParticleDefinition.hh"

#include "G4Geantino.hh"
#include "G4Gamma.hh"
#include "G4Neutron.hh"
#include "G4Alpha.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"

#include "G4GenericMessenger.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include <cmath>

MyPrimaryGenerator::MyPrimaryGenerator()
{
  fParticleGun = new G4ParticleGun(1);

  // Safe default
  fParticleGun->SetParticleDefinition(G4Geantino::Geantino());

  // Default macro-controlled values (set ONCE, not per event)
  fSourceMode    = "electron";
  fKineticEnergy = 1500.0 * keV;

  // Default beam controls (you can also expose these later)
  fConeAxis       = G4ThreeVector(0., 0., -1.);
  fConeAngle      = 180.*deg;
  fConeApexRadius = 0.*cm;

  DefineCommands();
}

MyPrimaryGenerator::~MyPrimaryGenerator()
{
  delete fMessenger;
  delete fParticleGun;
}

void MyPrimaryGenerator::DefineCommands()
{
  fMessenger = new G4GenericMessenger(this, "/mygen/", "Primary generator control");

  // /mygen/mode gamma|neutron|alpha|electron|positron|Ba133
  fMessenger->DeclareProperty("mode", fSourceMode,
                              "Source mode: Ba133, gamma, neutron, alpha, electron, positron");

  auto& eCmd = fMessenger->DeclarePropertyWithUnit(
    "energy", "keV", fKineticEnergy,
    "Kinetic energy (ignored for Ba133 which is at rest)");

  eCmd.SetParameterName("E", false);  // <-- IMPORTANT
  eCmd.SetRange("E > 0.");            // <-- use E, not energy

}

void MyPrimaryGenerator::GeneratePrimaries(G4Event* anEvent)
{
  // IMPORTANT: do NOT call SetSourceMode/SetKineticEnergy here anymore.
  // The macro will set fSourceMode and fKineticEnergy between beamOn calls.

  if (fSourceMode == "Ba133")
  {
    G4int Z = 56, A = 133;
    G4double ionExcitationE = 0.*keV;
    auto* ion = G4IonTable::GetIonTable()->GetIon(Z, A, ionExcitationE);

    fParticleGun->SetParticleDefinition(ion);
    fParticleGun->SetParticleCharge(0.*eplus);
    fParticleGun->SetParticleEnergy(0.*keV);
  }
  else if (fSourceMode == "gamma")
  {
    fParticleGun->SetParticleDefinition(G4Gamma::Gamma());
    fParticleGun->SetParticleCharge(0.*eplus);
    fParticleGun->SetParticleEnergy(fKineticEnergy);
  }
  else if (fSourceMode == "neutron")
  {
    fParticleGun->SetParticleDefinition(G4Neutron::Neutron());
    fParticleGun->SetParticleCharge(0.*eplus);
    fParticleGun->SetParticleEnergy(fKineticEnergy);
  }
  else if (fSourceMode == "alpha")
  {
    fParticleGun->SetParticleDefinition(G4Alpha::Alpha());
    fParticleGun->SetParticleCharge(+2.*eplus);
    fParticleGun->SetParticleEnergy(fKineticEnergy);
  }
  else if (fSourceMode == "electron")
  {
    fParticleGun->SetParticleDefinition(G4Electron::Electron());
    fParticleGun->SetParticleCharge(-1.*eplus);
    fParticleGun->SetParticleEnergy(fKineticEnergy);
  }
  else if (fSourceMode == "positron")
  {
    fParticleGun->SetParticleDefinition(G4Positron::Positron());
    fParticleGun->SetParticleCharge(+1.*eplus);
    fParticleGun->SetParticleEnergy(fKineticEnergy);
  }
  else
  {
    fParticleGun->SetParticleDefinition(G4Geantino::Geantino());
    fParticleGun->SetParticleCharge(0.*eplus);
    fParticleGun->SetParticleEnergy(0.*keV);
  }

  // --- your cone direction sampling (unchanged) ---
  G4double cosTheta = std::cos(fConeAngle);
  G4double z = G4UniformRand() * (1. - cosTheta) + cosTheta;
  G4double phi = G4UniformRand() * 2. * M_PI;
  G4double r = std::sqrt(1. - z * z);
  G4double x = r * std::cos(phi);
  G4double y = r * std::sin(phi);

  G4ThreeVector randomDirection = G4ThreeVector(x,y,z).rotateUz(fConeAxis);

  G4ThreeVector sourcePosition(0.,0.,0.*cm);
  if (fConeApexRadius > 0.) {
    G4double radius = fConeApexRadius * std::sqrt(G4UniformRand());
    G4double theta = G4UniformRand() * 2. * M_PI;
    sourcePosition = G4ThreeVector(radius * std::cos(theta), radius * std::sin(theta), 0.0*m);
  }

  fParticleGun->SetParticlePosition(sourcePosition);
  fParticleGun->SetParticleMomentumDirection(randomDirection);

  fParticleGun->GeneratePrimaryVertex(anEvent);
}
