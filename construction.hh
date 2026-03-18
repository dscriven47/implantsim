#ifndef CONSTRUCTION_HH
#define CONSTRUCTION_HH


#include "G4VUserDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Trd.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4GenericMessenger.hh"
#include "G4VisAttributes.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"

#include "detector.hh"


// Here we define a new detector construction which is inherited from the
// G4UserDetectorConstruction class supplied by Geant4.
class MyDetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    MyDetectorConstruction();
    MyDetectorConstruction(G4LogicalVolume*);
    ~MyDetectorConstruction();

    G4LogicalVolume *GetScoringVolume() const { return fScoringVolume; }

    virtual G4VPhysicalVolume *Construct(); 

  private:
    G4Box *solidWorld, *solidRadiator, *solidDetector,*solidScintillator,
          *solidScintHousing,*solidQuartzWindow;
    G4Trd *solidPMTWindow;
    G4LogicalVolume *logicWorld, *logicRadiator, *logicDetector, 
                    *logicScintillator,*logicPMTWindow,*logicScintHousing,*logicQuartzWindow;
    G4VPhysicalVolume *physWorld, *physRadiator, *physDetector, 
                      *physScintillator,*physPMTWindow,*physScintHousing,*physQuartzWindow;
    G4int nCols,nRows;

    G4OpticalSurface *mirrorSurface;

    // material definitions
    G4Material *SiO2, *H2O, *Aerogel, *worldMat, *NaI, *CeBr3, *Quartz,
      *Si, *Borosilicate, *B2O3, *Na2O, *Al2O3, *CaO, *Bialkali, *Aluminum, *HeavyMet, *PbMat;
    G4Element *C, *Na, *I, *Pb, *W, *Ni, *Cu;
    

    G4GenericMessenger *fMessenger;

    G4LogicalVolume *fScoringVolume;

    G4double xWorld,yWorld,zWorld;

        
    void DefineMaterials();
    
    void ConstructScintillator();
    

    virtual void ConstructSDandField();
    void VisAttributes();
    G4LogicalVolume* BuildCylindricalHousing();
    G4LogicalVolume* BuildScintillatorMountLV();
    
};

#endif
