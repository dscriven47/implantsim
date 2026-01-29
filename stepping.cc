#include "stepping.hh"

MySteppingAction::MySteppingAction(MyEventAction *eventAction)
{
  fEventAction = eventAction;
}

MySteppingAction::~MySteppingAction()
{}

void MySteppingAction::UserSteppingAction(const G4Step *step)
{
  G4LogicalVolume *volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();

  const MyDetectorConstruction *detectorConstruction = static_cast<const MyDetectorConstruction*> (G4RunManager::GetRunManager()->GetUserDetectorConstruction());

  G4LogicalVolume *fScoringVolume = detectorConstruction->GetScoringVolume();


  if(volume != fScoringVolume)
    return;

  G4double edep = step->GetTotalEnergyDeposit();
  fEventAction->AddEdep(edep);



  


  // here we can add the smearing... need to get profile for CeBr resolution as function of E....
  // the energy resolution at 511 is 5.29% +/- 0.003%
  // the energy resolution at 662 is 4.34% +/- 0.005%
  // the energy resoltion at 1275 is 3.42% +/- 0.004%
}