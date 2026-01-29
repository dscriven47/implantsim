#include "generator.hh"

#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"
#include "G4ParticleDefinition.hh"

#include "G4Geantino.hh"
#include "G4Gamma.hh"
#include "G4Neutron.hh"
#include "G4Alpha.hh"

#include "G4SystemOfUnits.hh"

#include "Randomize.hh" // G4UniformRand, G4RandGauss::shoot
#include <cmath>

MyPrimaryGenerator::MyPrimaryGenerator()
{
  fParticleGun = new G4ParticleGun(1);

  // Set a safe default (will get overwritten in GeneratePrimaries based on mode)
  G4ParticleDefinition* particle = G4Geantino::Geantino();
  fParticleGun->SetParticleDefinition(particle);

}

MyPrimaryGenerator::~MyPrimaryGenerator()
{
  delete fParticleGun;
}

void MyPrimaryGenerator::GeneratePrimaries(G4Event* anEvent)
{
  // You can switch behavior with:
  //   SetSourceMode("Ba133");
     SetSourceMode("gamma");   SetKineticEnergy(1500*keV);  // example line
  //   SetSourceMode("neutron"); SetKineticEnergy(1.*MeV);
  //   SetSourceMode("alpha");   SetKineticEnergy(5.*MeV);

  if (fSourceMode == "Ba133")
  {
    // Ba-133 ion (Z=56, A=133)
    G4int Z = 56;
    G4int A = 133;

    G4double ionExcitationE = 0.*keV;  // excitation energy state of ion nucleus
    G4ParticleDefinition* ion = G4IonTable::GetIonTable()->GetIon(Z, A, ionExcitationE);

    fParticleGun->SetParticleDefinition(ion);
    fParticleGun->SetParticleCharge(0.*eplus);
    fParticleGun->SetParticleEnergy(0.*keV); // ions at rest (you can change if desired)
  }
  else if (fSourceMode == "gamma")
  {
    fParticleGun->SetParticleDefinition(G4Gamma::Gamma());
    fParticleGun->SetParticleCharge(0.*eplus);
    fParticleGun->SetParticleEnergy(fKineticEnergy); // e.g. 356*keV
  }
  else if (fSourceMode == "neutron")
  {
    fParticleGun->SetParticleDefinition(G4Neutron::Neutron());
    fParticleGun->SetParticleCharge(0.*eplus);
    fParticleGun->SetParticleEnergy(fKineticEnergy); // e.g. 1*MeV
  }
  else if (fSourceMode == "alpha")
  {
    fParticleGun->SetParticleDefinition(G4Alpha::Alpha());
    fParticleGun->SetParticleCharge(+2.*eplus);
    fParticleGun->SetParticleEnergy(fKineticEnergy); // e.g. 5*MeV
  }
  else
  {
    // Fallback so you immediately see you set an invalid mode
    fParticleGun->SetParticleDefinition(G4Geantino::Geantino());
    fParticleGun->SetParticleCharge(0.*eplus);
    fParticleGun->SetParticleEnergy(0.*keV);
  }


  
  //G4ThreeVector pos(0., 0., 1.*cm);
  //G4ThreeVector mom(0., 0., 1.);
  //fParticleGun->SetParticlePosition(pos);
  //fParticleGun->SetParticleMomentumDirection(mom);

  // set beam control
  fConeAxis = G4ThreeVector(0., 0., -1.);
  fConeAngle = 180.*deg;
  fConeApexRadius = 0.*cm;

  // Generate random direction within the cone
  G4double cosTheta = std::cos(fConeAngle);
  G4double z = G4UniformRand() * (1. - cosTheta) + cosTheta;  // Random z within cone
  G4double phi = G4UniformRand() * 2. * M_PI;                   // Random azimuthal angle
  G4double r = std::sqrt(1. - z * z);
  G4double x = r * std::cos(phi);
  G4double y = r * std::sin(phi);

  // Rotate the direction vector to align with fConeAxis
  G4ThreeVector randomDirection = G4ThreeVector(x,y,z).rotateUz(fConeAxis);

  // Generate random position within the apex radius (if non-zero)
  G4ThreeVector sourcePosition = G4ThreeVector(0.,0.,0.*cm);

  if (fConeApexRadius > 0.) {
    G4double radius = fConeApexRadius * std::sqrt(G4UniformRand());
    G4double theta = G4UniformRand() * 2. * M_PI;
    sourcePosition = G4ThreeVector(radius * std::cos(theta), radius * std::sin(theta), 0.1*m);
  }

  fParticleGun->SetParticlePosition(sourcePosition);
  fParticleGun->SetParticleMomentumDirection(randomDirection);
  
  // NOTE: for gamma/neutron/alpha, prefer SetParticleEnergy(ekin) rather than SetParticleMomentum(...)
  //fParticleGun->SetParticleEnergy(0.*keV);
  fParticleGun->GeneratePrimaryVertex(anEvent);

  
}