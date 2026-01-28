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


  

  const auto track = step->GetTrack();
  if (track->GetDefinition() != G4OpticalPhoton::OpticalPhoton()) return; 
  

  const auto prePV = step->GetPreStepPoint()->GetPhysicalVolume();
  const auto postPV = step->GetPostStepPoint()->GetPhysicalVolume();
  if (!prePV || !postPV) return;  
  

  // Only care about "going into air"
  if (postPV->GetName() != "G4Air") return; // <-- better: compare logical/physical pointer, see note below 
  

  // Ensure it's a boundary crossing
  if (step->GetPostStepPoint()->GetStepStatus() != fGeomBoundary) return; 
  

  // Position to mark: boundary location (post-step point is usually fine)
  const G4ThreeVector pos = step->GetPostStepPoint()->GetPosition();  
  

  auto vis = G4VVisManager::GetConcreteInstance();
  if (!vis) return; // running in batch / no vis  
  

  G4Circle circle(pos);
  circle.SetScreenSize(40.0); // pixels-ish (screen size)
  circle.SetFillStyle(G4Circle::filled);
  G4Colour col(1.0, 0.0, 1.0); // magenta
  circle.SetVisAttributes(G4VisAttributes(col));  
  

  vis->Draw(circle);







  if(volume != fScoringVolume)
    return;

  G4double edep = step->GetTotalEnergyDeposit();
  fEventAction->AddEdep(edep);


  // here we can add the smearing... need to get profile for CeBr resolution as function of E....
}