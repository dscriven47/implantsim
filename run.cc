#include "run.hh"

#include <fstream>
#include <sstream>
#include <string>

MyRunAction::MyRunAction()
{

  G4AnalysisManager *man = G4AnalysisManager::Instance();

  man->CreateNtuple("Photons","Photons");
  man->CreateNtupleIColumn("fEvent");
  man->CreateNtupleDColumn("fX");
  man->CreateNtupleDColumn("fY");
  man->CreateNtupleDColumn("fZ");
  man->CreateNtupleDColumn("fWlen");
  man->CreateNtupleDColumn("fTime");
  man->FinishNtuple(0);

  man->CreateNtuple("Hits","Hits");
  man->CreateNtupleIColumn("fEvent");
  man->CreateNtupleDColumn("fX");
  man->CreateNtupleDColumn("fY");
  man->CreateNtupleDColumn("fZ");
  man->FinishNtuple(1);

  man->CreateNtuple("Scoring","Scoring");
  man->CreateNtupleDColumn("fEdep");
  man->CreateNtupleDColumn("fE");
  man->FinishNtuple(2);

}

MyRunAction::~MyRunAction()
{}

void MyRunAction::BeginOfRunAction(const G4Run* run)
{
  G4AnalysisManager* man = G4AnalysisManager::Instance();
  
  G4int runID = run->GetRunID();
  G4int tryRunID = runID;
  
  std::string filename;
  
  while (true) {
  std::stringstream ss;
  ss << "implantsim-output_run" << tryRunID << ".root";
  
  std::ifstream f(ss.str().c_str());
  if (!f.good()) {
  filename = ss.str();
  break;
  }
  
  ++tryRunID; // <-- bump run number
  }
  
  man->OpenFile(filename);
  
  G4cout << "Writing output to: " << filename << G4endl;
}


void MyRunAction::EndOfRunAction(const G4Run*)
{
  G4AnalysisManager *man = G4AnalysisManager::Instance();

  man->Write();
  man->CloseFile();

}