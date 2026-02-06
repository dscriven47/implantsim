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
  fKineticEnergy = 75.0 * keV;
  fGunType = "point";


  // read the implant profile PDF
  std::ifstream infile("implant_depth_cdf.txt");
  G4double x, cdf;
  while (infile >> x >> cdf)
  {
    fX.push_back(x * mm);   // apply units HERE
    fCDF.push_back(cdf);
  }
  infile.close();


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

    G4double z = SampleImplantDepth();

    G4cout << "Event " << anEvent->GetEventID()
           << "  z = " << z/mm << " mm" << G4endl;




    SetSourcePosition(G4ThreeVector(x0*mm, y0*mm, z*mm));
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

    // find first CDF bin above u
    auto it = std::lower_bound(fCDF.begin(), fCDF.end(), u);

    if (it == fCDF.begin())
        return fX.front();
    if (it == fCDF.end())
        return fX.back();

    size_t i = std::distance(fCDF.begin(), it);

    // linear interpolation
    G4double x1 = fX[i-1];
    G4double x2 = fX[i];
    G4double c1 = fCDF[i-1];
    G4double c2 = fCDF[i];

    return x1 + (u - c1) * (x2 - x1) / (c2 - c1);
}
