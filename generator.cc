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
#include "G4ios.hh"
#include <cmath>

MyPrimaryGenerator::MyPrimaryGenerator()
{
  fParticleGun = new G4ParticleGun(1);

  // Safe default
  fParticleGun->SetParticleDefinition(G4Geantino::Geantino());

  // Default macro-controlled values (set ONCE, not per event)
  fSourceMode    = "gamma";
  fKineticEnergy = 40 * keV;
  fGunType = "gaussian";



  
  std::ifstream in("../generate_zpos/data_norm.txt");

  std::vector<G4double> p;
  G4double x, pdf;

  while (in >> x >> pdf) {
    fX.push_back(x * mm);  // apply units ONCE
    p.push_back(pdf);      // pdf in 1/mm
  }
  in.close();

  // Allocate CDF
  fCDF.resize(p.size());
  fCDF[0] = 0.0;

  // 👉 THIS IS WHERE YOUR LOOP GOES 👇
  for (size_t i = 1; i < p.size(); ++i) {
    G4double dx = fX[i] - fX[i-1];
    fCDF[i] = fCDF[i-1] + 0.5*(p[i] + p[i-1]) * dx;
  }

  // Normalize to exactly 1
  for (auto& v : fCDF) v /= fCDF.back();


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
  
  
  // point source
  if(fGunType == "point")
  {
    SetSourceDirection(G4ThreeVector(0.,0.,-1.));
    SetSourcePosition(G4ThreeVector(0.,0.,1.*cm));
  }


  // conical source
  else if(fGunType == "cone"){
    G4double cosTheta = std::cos(fConeAngle);
    G4double z = G4UniformRand() * (1. - cosTheta) + cosTheta;
    G4double phi = G4UniformRand() * 2. * M_PI;
    G4double r = std::sqrt(1. - z * z);
    G4double x = r * std::cos(phi);
    G4double y = r * std::sin(phi);
    
    G4ThreeVector sourcePosition = G4ThreeVector(0.,0.,1.*cm);

    if(fConeApexRadius > 0.) 
    {
      G4double radius = fConeApexRadius * std::sqrt(G4UniformRand());
      G4double theta = G4UniformRand() * 2. * M_PI;
      sourcePosition = G4ThreeVector(radius * std::cos(theta), radius * std::sin(theta), 0.0*m);
    }

    SetSourceDirection(G4ThreeVector(x,y,z).rotateUz(fConeAxis));
    SetSourcePosition(sourcePosition);
  }

  // gaussian distributed source
  else if(fGunType == "gaussian"){

    // convert px to mm
    double px_mm = 3.0; // 3 mm per pixel

    // a gaussian implant beam profile from 69Mn
    //G4double beamMeanX = 6.69276*px_mm; // in mm
    //G4double beamMeanY = 7.94655*px_mm; // in mm
    //G4double beamSigmaX = 1.711274*px_mm; // in mm
    //G4double beamSigmaY = 2.15191*px_mm; // in mm
    
    // a gaussian beta decay electron profile from 69Mn bdecay
    G4double beamMeanX = 8.09627/16*px_mm; // in mm
    G4double beamMeanY = 8.96997/16*px_mm; // in mm
    G4double beamSigmaX = 2.07117*px_mm; // in mm
    G4double beamSigmaY = 3.32483*px_mm; // in mm


    // Gaussian beam profile in XY plane with pure z direction
    G4double x0 = G4RandGauss::shoot(beamMeanX, beamSigmaX);
    G4double y0 = G4RandGauss::shoot(beamMeanY, beamSigmaY);
    
    G4double sample_depth = SampleImplantDepth(); // 1.5mm is half the detector thickness
    G4double z0 = 1.5-sample_depth;
    //G4cout << "Event " << anEvent->GetEventID() << "  z = " << 1.5 << " mm" << " sample_depth = " << sample_depth << " mm" << G4endl;




    SetSourcePosition(G4ThreeVector(x0*mm, y0*mm, z0*mm));
    //SetSourceDirection(G4ThreeVector(0.,0.,-1.)); // gaussian beam

    // gaussian source with random momentum direction
    G4double cosT = 2.0*G4UniformRand() - 1.0;
    G4double sinT = std::sqrt(1.0 - cosT*cosT);
    G4double phi  = 2.0*M_PI*G4UniformRand();

    G4ThreeVector dir(sinT*std::cos(phi),sinT*std::sin(phi),cosT);
    //G4ThreeVector dir = G4RandomDirection();
    //particleGun->SetParticleMomentumDirection(dir);
    SetSourceDirection(dir); 


  }

  

  // fall backs
  else{
    SetSourcePosition(G4ThreeVector(0.,0.,1.*cm));
    SetSourceDirection(G4ThreeVector(0.,0.,-1.));
  }

  fParticleGun->SetParticlePosition(fSourcePosition);
  fParticleGun->SetParticleMomentumDirection(fSourceDirection);

  fParticleGun->GeneratePrimaryVertex(anEvent);
}


G4double MyPrimaryGenerator::SampleImplantDepth()
{
  G4double u = G4UniformRand();

  auto it = std::lower_bound(fCDF.begin(), fCDF.end(), u);

  if (it == fCDF.begin())
    return fX.front();

  if (it == fCDF.end())
    return fX.back();   // should basically never happen if CDF is correct

  size_t i = it - fCDF.begin();

  return fX[i-1]
       + (u - fCDF[i-1]) * (fX[i] - fX[i-1])
         / (fCDF[i] - fCDF[i-1]);
}

