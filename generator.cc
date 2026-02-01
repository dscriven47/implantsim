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
  fSourceMode    = "Ba133";
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
                              "Source mode: Ba133, Na22, gamma, neutron, alpha, electron, positron");

  auto& eCmd = fMessenger->DeclarePropertyWithUnit(
    "energy", "keV", fKineticEnergy,
    "Kinetic energy (ignored for Ba133 which is at rest)");

  eCmd.SetParameterName("E", false);  // <-- IMPORTANT
  eCmd.SetRange("E >= 0.");            // <-- use E, not energy

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
    fParticleGun->SetParticleEnergy(fKineticEnergy);
  }
  else if (fSourceMode == "Na22")
  {
    G4int Z = 11, A = 22;
    G4double ionExcitationE = 0.*keV;
    auto* ion = G4IonTable::GetIonTable()->GetIon(Z, A, ionExcitationE);
    fParticleGun->SetParticleDefinition(ion);
    fParticleGun->SetParticleCharge(0.*eplus);
    fParticleGun->SetParticleEnergy(fKineticEnergy);
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


  if(fGunType == "cone"){
    // --- your cone direction sampling (unchanged) ---
    G4double cosTheta = std::cos(fConeAngle);
    G4double z = G4UniformRand() * (1. - cosTheta) + cosTheta;
    G4double phi = G4UniformRand() * 2. * M_PI;
    G4double r = std::sqrt(1. - z * z);
    G4double x = r * std::cos(phi);
    G4double y = r * std::sin(phi);

    G4ThreeVector randomDirection = G4ThreeVector(x,y,z).rotateUz(fConeAxis);

    G4ThreeVector sourcePosition(0.,0.,1.*cm);
    if (fConeApexRadius > 0.) {
      G4double radius = fConeApexRadius * std::sqrt(G4UniformRand());
      G4double theta = G4UniformRand() * 2. * M_PI;
      sourcePosition = G4ThreeVector(radius * std::cos(theta), radius * std::sin(theta), 0.0*m);
    }
  }
  else if(fGunType == "point")
  {
    G4ThreeVector randomDirection = G4ThreeVector(0.,0.,-1.);
    G4ThreeVector sourcePosition(0.,0.,1.*cm);
  }
  else if(fGunType == "gaussian"){

    // convert px to mm
    double px_mm = 3.0; // 3 mm per pixel

    // a gaussian implant beam profile from 69Mn
    //G4double beamMeanX = 6.69276*px_mm; // in mm
    //G4double beamMeanY = 7.94655*px_mm; // in mm
    //G4double beamSigmaX = 1.711274*px_mm; // in mm
    //G4double beamSigmaY = 2.15191*px_mm; // in mm
    

    // a gaussian beta decay electron profile from 69Mn bdecay
    G4double beamMeanX = 8.09627*px_mm; // in mm
    G4double beamMeanY = 8.96997*px_mm; // in mm
    G4double beamSigmaX = 2.07117*px_mm; // in mm
    G4double beamSigmaY = 3.32483*px_mm; // in mm


    // Gaussian beam profile in XY plane with pure z direction
    G4double x0 = G4RandGauss::shoot(beamMeanX, beamSigmaX);
    G4double y0 = G4RandGauss::shoot(beamMeanY, beamSigmaY);
    G4ThreeVector sourcePosition(x0*mm, y0*mm, 1.*mm);

    G4ThreeVector randomDirection = G4ThreeVector(0.,0.,-1.);
  }
  else{
    G4ThreeVector randomDirection = G4ThreeVector(0.,0.,-1.);
    G4ThreeVector sourcePosition(0.,0.,1.*cm);
  }



  fParticleGun->SetParticlePosition(sourcePosition);
  fParticleGun->SetParticleMomentumDirection(randomDirection);

  fParticleGun->GeneratePrimaryVertex(anEvent);
}
