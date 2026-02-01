#include "event.hh"

#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include <cmath>

MyEventAction::MyEventAction(MyRunAction*)
{
  fEdep = 0.;
  fE = 0.;
}

MyEventAction::~MyEventAction()
{}

void MyEventAction::BeginOfEventAction(const G4Event*)
{
  fEdep = 0.;
  fE = 0.;
}

void MyEventAction::EndOfEventAction(const G4Event*)
{
  //G4cout << "energy deposition: " << fEdep << G4endl;

  G4AnalysisManager *man = G4AnalysisManager::Instance();

  man->FillNtupleDColumn(2,0,fEdep);
  man->FillNtupleDColumn(2,1,SmearEnergy(fEdep));
  man->AddNtupleRow(2);
}


// Example: HPGe-like parametrization in keV
// FWHM(E)^2 = a^2 + b*E + c*E^2
double MyEventAction::SmearEnergy(double E_MeV)
{
  // more complicated parametrization could be implemented here
  //const double a = 1.0;      // keV  (electronics/noise term)
  //const double b = 0.002;    // keV   (Fano/statistics-ish term)
  //const double c = 0.0;      // 1     (optional high-E term)
  //double fwhm2 = a*a + b*E_keV + c*E_keV*E_keV;


  //Uses source data and 23.6 from Mn69
  // all data was position corrected for energy except Mn69
  //Energy		% Resolution
  //23.6 	 0.342689898
  //30.85	 0.293496564
  //81.0	 0.165960016
  //276.4	 0.066382
  //356.02 0.078308
  //383.8	 0.071285
  //511		 0.0529
  //661		 0.0434
  //1274	 0.0342
  // the data was characterized and well with a power law 2.2007x^-0.591
  // let's say it's flat and that the resolution at low energy is due to less light collection

  // calculate resolution at this energy
  
  double resolution = 0.0371 * pow(E_MeV,-0.591);
  double sigma = resolution / 2.355;
  //std::cout << "Energy: " << E_keV << " keV, Resolution: " << resolution*100 << " %, Sigma: " << sigma << " keV" << std::endl;

  // Gaussian smear
  double Es = G4RandGauss::shoot(E_MeV, sigma);

  // avoid negative energy from tails
  if (Es < 0) Es = 0;

  return Es;
}