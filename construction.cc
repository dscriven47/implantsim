
#include "construction.hh"

MyDetectorConstruction::MyDetectorConstruction()
{
  fMessenger = new G4GenericMessenger(this, "/detector/", "Detector Construction");

  fMessenger->DeclareProperty("nCols", nCols, "Number of columns");
  fMessenger->DeclareProperty("nRows", nRows, "Number of rows");
  fMessenger->DeclareProperty("isCherenkov", isCherenkov, "Toggle Cherenkov Detector");
  fMessenger->DeclareProperty("isScintillator", isScintillator, "Toggle Scintillator Detector");
  fMessenger->DeclareProperty("isTOF", isTOF, "Toggle TOF Detector");

  nCols = 10;
  nRows = 10;

  DefineMaterials();

  isCherenkov=false;
  isScintillator=true;
  isTOF=false;

  // size of the world volume
  xWorld = 0.08*m;
  yWorld = 0.08*m;
  zWorld = 0.3*m;
}

MyDetectorConstruction::~MyDetectorConstruction()
{

}

void MyDetectorConstruction::DefineMaterials()
{ 

  ////////////////////////////// Define Materials /////////////////////////////
  // this will call a nist database which can be used to source materials
  G4NistManager *nist = G4NistManager::Instance(); 

  // make our first material, fused silica
  SiO2 = new G4Material("SiO2", 2.201*g/cm3, 2);
  SiO2->AddElement(nist->FindOrBuildElement("Si"), 1);
  SiO2->AddElement(nist->FindOrBuildElement("O"), 2);

  // another definition for water
  H2O = new G4Material("H2O", 1.000*g/cm3, 2);
  H2O->AddElement(nist->FindOrBuildElement("H"), 2);
  H2O->AddElement(nist->FindOrBuildElement("O"), 1);

  B2O3 = new G4Material("B2O3",2.46*g/cm3,2);
  B2O3->AddElement(nist->FindOrBuildElement("B"),2);
  B2O3->AddElement(nist->FindOrBuildElement("O"),3);

  Na2O = new G4Material("Na2O",2.27,2);
  Na2O->AddElement(nist->FindOrBuildElement("Na"),2);
  Na2O->AddElement(nist->FindOrBuildElement("O"),1);

  Al2O3 = new G4Material("Al2O3",3.99*g/cm3,2);
  Al2O3->AddElement(nist->FindOrBuildElement("Al"),2);
  Al2O3->AddElement(nist->FindOrBuildElement("O"),3);

  CaO = new G4Material("CaO",3.34*g/cm3,2);
  CaO->AddElement(nist->FindOrBuildElement("O"),1);
  CaO->AddElement(nist->FindOrBuildElement("O"),1);

  HeavyMet = new G4Material("HeavyMet",17.0*g/cm3,3);
  HeavyMet->AddElement(nist->FindOrBuildElement("W"), 90.0*perCent);
  HeavyMet->AddElement(nist->FindOrBuildElement("Ni"), 6.0*perCent);
  HeavyMet->AddElement(nist->FindOrBuildElement("Cu"), 4.0*perCent);

  PbMat = new G4Material("PbMat",11.34*g/cm3,1);
  PbMat->AddElement(nist->FindOrBuildElement("Pb"),1);

  // definition for carbon
  C = nist->FindOrBuildElement("C");

  // definition for silicon
  Si = new G4Material("Si", 2.329*g/cm3, 1);
  Si->AddElement(nist->FindOrBuildElement("Si"),1);

  // create a compount material made of elements and compounds
  Aerogel = new G4Material("Aerogel", 0.200*g/cm3, 3);
  Aerogel->AddMaterial(SiO2, 62.5*perCent);
  Aerogel->AddMaterial(H2O, 37.4*perCent);
  Aerogel->AddElement(C, 0.1*perCent);

  // NaI
  Na = nist->FindOrBuildElement("Na");
  I = nist->FindOrBuildElement("I");
  NaI = new G4Material("NaI",3.67*g/cm3,2);
  NaI->AddElement(Na,1);
  NaI->AddElement(I,1);

  // CeBr3
  CeBr3 = new G4Material("CeBr3",5.23*g/cm3,2);
  CeBr3->AddElement(nist->FindOrBuildElement("Ce"),1); 
  CeBr3->AddElement(nist->FindOrBuildElement("Br"),3);

  // quartz window
  Quartz = new G4Material("Quartz",2.65*g/cm3,1);
  Quartz->AddMaterial(SiO2,1);

  // world material
  worldMat = nist->FindOrBuildMaterial("G4_AIR");

  // Borosilicate glass window
  Borosilicate = new G4Material("Borosilicate",2.23*g/cm3,5);
  Borosilicate->AddMaterial(SiO2,75.00*perCent);
  Borosilicate->AddMaterial(B2O3,10.50*perCent);
  Borosilicate->AddMaterial(Na2O, 5.00*perCent);
  Borosilicate->AddMaterial(Al2O3,7.00*perCent);
  Borosilicate->AddMaterial(CaO,  1.50*perCent);

  // Aluminum
  Aluminum = new G4Material("Aluminum",2.7*g/cm3,1);
  Aluminum->AddElement(nist->FindOrBuildElement("Al"),1);



  // Bialkali photocathode
  //Bialkali = new G4Material("Bialkali",)



  ////////////////////////// Define Optical Properties ///////////////////////
  


  // setup the refractive properties of the aerogel
  G4double rindexAerogel[2] = {1.1,1.1};
  G4double energyAerogel[2] = {1.239841939*eV/0.2, 1.239841939*eV/0.9};

  G4MaterialPropertiesTable *mptAerogel = new G4MaterialPropertiesTable(); // make a material property for the aerogel and add the above properties to the table
  mptAerogel->AddProperty("RINDEX",energyAerogel,rindexAerogel,2);
  Aerogel->SetMaterialPropertiesTable(mptAerogel); // now add the material properties table to the material



  // setup optical properties of CeBr3
  G4double energyCeBr3[2] = {1.239841939*eV/0.2, 1.239841939*eV/0.9};
  G4double rindexCeBr3[2] = {2.09,2.09};
  //G4double reflectivityCeBr3[2] = {1.0,1.0};
  G4double fractionCeBr3[2] = {1.0,1.0}; // fast component spectrum

  G4MaterialPropertiesTable *mptCeBr3 = new G4MaterialPropertiesTable();
  mptCeBr3->AddProperty("RINDEX",energyCeBr3,rindexCeBr3,2);
  mptCeBr3->AddProperty("FASTCOMPONENT",energyCeBr3,fractionCeBr3,2);
  //mptCeBr3->AddProperty("ABSLENGTH",energy,fraction,2);
  mptCeBr3->AddConstProperty("SCINTILLATIONYIELD",66000./MeV);
  mptCeBr3->AddConstProperty("RESOLUTIONSCALE",1.0);
  mptCeBr3->AddConstProperty("FASTTIMECONSTANT",20*ns);
  mptCeBr3->AddConstProperty("YIELDRATIO",1.);
  CeBr3->SetMaterialPropertiesTable(mptCeBr3);
  // we also need to define the surface properties
  // also quenching



  //setup optical properties of quartz
  G4double energyQuartz[2] = {1.239841939*eV/0.2, 1.239841939*eV/0.9};
  G4double rindexQuartz[2] = {1.5,1.5};
  G4double reflectivityQuartz[2] = {0.08,0.08};

  G4MaterialPropertiesTable *mptQuartz = new G4MaterialPropertiesTable();
  mptQuartz->AddProperty("RINDEX",energyQuartz,rindexQuartz,2);
  mptQuartz->AddProperty("REFLECTIVITY",energyQuartz,reflectivityQuartz,2);
  //mptQuartz->AddProperty("FASTCOMPONENT",energy,fraction,2);
  //mptQuartz->AddProperty("ABSLENGTH",energy,fraction,2);
  //mptQuartz->AddConstProperty("FASTCOMPONENT",38000./MeV);
  //mptQuartz->AddConstProperty("SCINTILLATIONYIELD",66000./MeV);
  //mptQuartz->AddConstProperty("RESOLUTIONSCALE",1.0);
  //mptQuartz->AddConstProperty("FASTTIMECONSTANT",250*ns);
  //mptQuartz->AddConstProperty("YIELDRATIO",1.);
  Quartz->SetMaterialPropertiesTable(mptQuartz);
  //also need to define the surface properties

  

  // World optical properties
  G4double energyWorld[2] = {1.239841939*eV/0.2, 1.239841939*eV/0.9};
  G4double rindexWorld[2] = {1.,1.};

  G4MaterialPropertiesTable *mptWorld = new G4MaterialPropertiesTable();
  mptWorld->AddProperty("RINDEX",energyWorld,rindexWorld,2);
  worldMat->SetMaterialPropertiesTable(mptWorld);
  


  // NaI Scintillator optical properties
  G4double energyNaI[2] = {1.239841939*eV/0.2, 1.239841939*eV/0.9};
  G4double rindexNaI[2] = {1.78,1.78};
  G4double fractionNaI[2] = {1.0,1.0}; // fast component spectrum

  G4MaterialPropertiesTable *mptNaI = new G4MaterialPropertiesTable();
  mptNaI->AddProperty("RINDEX",energyNaI,rindexNaI,2);
  mptNaI->AddProperty("FASTCOMPONENT",energyNaI,fractionNaI,2);
  mptNaI->AddConstProperty("SCINTILLATIONYIELD",38000./MeV);
  mptNaI->AddConstProperty("RESOLUTIONSCALE",1.0);
  mptNaI->AddConstProperty("FASTTIMECONSTANT",250*ns);
  mptNaI->AddConstProperty("YIELDRATIO",1.);
  NaI->SetMaterialPropertiesTable(mptNaI);



  // Borosilicate glass optical properties
  G4double energyBorosilicate[2] = {1.*eV,10.*eV};
  G4double rindexBorosilicate[2] = {1.49,1.537};
  G4MaterialPropertiesTable *mptBorosilicate = new G4MaterialPropertiesTable();
  mptBorosilicate->AddProperty("RINDEX",energyBorosilicate,rindexBorosilicate,2);
  Borosilicate->SetMaterialPropertiesTable(mptNaI);


  // Mirror Surface optical properties
  G4double energyMirror[2] = {1.239841939*eV/0.2, 1.239841939*eV/0.9};
  G4double reflectivityMirror[2] = {1.0,1.0};

  mirrorSurface = new G4OpticalSurface("mirrorSurface");
  mirrorSurface->SetType(dielectric_metal);
  mirrorSurface->SetFinish(ground);
  mirrorSurface->SetModel(unified);
  G4MaterialPropertiesTable *mptMirror = new G4MaterialPropertiesTable();
  mptMirror->AddProperty("REFLECTIVITY",energyMirror,reflectivityMirror,2);
  mirrorSurface->SetMaterialPropertiesTable(mptMirror);
}



void MyDetectorConstruction::ConstructScintillator()
{

  G4double scinthousing_xsize = 66.5*mm;
  G4double scinthousing_ysize = 66.5*mm;
  G4double scinthousing_zsize = 0.5*cm;
  G4double scinthousing_xpos = 0.*cm;
  G4double scinthousing_ypos = 0.*cm;
  G4double scinthousing_zpos = 0.*cm;

  solidScintHousing = new G4Box("solidScintHousing",scinthousing_xsize/2.,scinthousing_ysize/2.,scinthousing_zsize/2.);
  logicScintHousing = new G4LogicalVolume(solidScintHousing,Aluminum,"logicalScintHousing");
  physScintHousing  = new G4PVPlacement(0,G4ThreeVector(scinthousing_xpos,scinthousing_ypos,scinthousing_zpos),
                                       logicScintHousing,"physScintHousing",logicWorld,false,0,true);


  G4double quartzWindow_xsize = 5.6*cm;
  G4double quartzWindow_ysize = 5.6*cm;
  G4double quartzWindow_zsize = 1.0*mm;
  G4double quartzWindow_xpos = 0.*cm;
  G4double quartzWindow_ypos = 0.*cm;
  G4double quartzWindow_zpos = -scinthousing_zsize/2 + quartzWindow_zsize/2;

  solidQuartzWindow = new G4Box("solidQuartzWindow",quartzWindow_xsize/2.,quartzWindow_ysize/2.,quartzWindow_zsize/2.);
  logicQuartzWindow = new G4LogicalVolume(solidQuartzWindow,Quartz,"logicQuartzWindow");
  physQuartzWindow  = new G4PVPlacement(0,G4ThreeVector(quartzWindow_xpos,quartzWindow_ypos,quartzWindow_zpos),
                                      logicQuartzWindow,"physQuartzWindow",logicScintHousing,false,0,true);


  G4double cebr_xpos = 0.*cm;
  G4double cebr_ypos = 0.*cm;
  G4double cebr_zpos = 0.*cm;
  G4double cebr_xsize = 5.1*cm;
  G4double cebr_ysize = 5.1*cm;
  G4double cebr_zsize = 0.3*cm;

  G4double pspmt_cath_xsize = 0.3*cm;
  G4double pspmt_cath_ysize = 0.3*cm;
  G4double pspmt_cath_zsize = 0.1*cm;


  solidScintillator = new G4Box("solidScintillator",cebr_xsize/2,cebr_ysize/2,cebr_zsize/2);
  logicScintillator = new G4LogicalVolume(solidScintillator,CeBr3,"logicalScintillator");
  //G4LogicalSkinSurface *skin = new G4LogicalSkinSurface("skin",
  //                                    logicWorld,mirrorSurface);
  physScintillator = new G4PVPlacement(0,G4ThreeVector(cebr_xpos,cebr_ypos,cebr_zpos),
                                       logicScintillator,"physScintillator",logicScintHousing, false, 0, true);

  // PMT glass window dimensions
  G4double pmt_winX1 = 51.8*mm;
  G4double pmt_winX2 = 48.5*mm;
  G4double pmt_winY1 = 51.8*mm;
  G4double pmt_winY2 = 48.5*mm;
  G4double pmt_winZ  =  2.0*mm;
  solidPMTWindow = new G4Trd("solidPMTWindow",pmt_winX1/2.,pmt_winX2/2.,pmt_winY1/2.,pmt_winY2/2.,pmt_winZ/2.);
  logicPMTWindow = new G4LogicalVolume(solidPMTWindow,Borosilicate,"logicPMTWindow");
  physPMTWindow = new G4PVPlacement(0,G4ThreeVector(0,0,-pmt_winZ/2.-scinthousing_zsize/2.),
                                    logicPMTWindow,"physPMTWindow",logicWorld,false,0,true);


  // place the photocathodes
  G4int pspmt_xn=16;
  G4int pspmt_yn=16;
  solidDetector = new G4Box("solidDetector",pspmt_cath_xsize/2,pspmt_cath_ysize/2,pspmt_cath_zsize/2);
  logicDetector = new G4LogicalVolume(solidDetector,worldMat,
                                      "logicDetector");  
  for(G4int j=0; j<pspmt_yn; j++){
    for(G4int i=0; i<pspmt_xn; i++){
      physDetector = new G4PVPlacement(0,G4ThreeVector(
                                        -pspmt_xn*pspmt_cath_xsize/2+pspmt_cath_xsize/2+i*pspmt_cath_xsize,            
                                        -pspmt_yn*pspmt_cath_ysize/2+pspmt_cath_ysize/2+j*pspmt_cath_ysize,
                                        -pmt_winZ/2.+pspmt_cath_zsize/2.),logicDetector,"physDetector",
                                        logicPMTWindow,false,0,true);
    }
  }

  fScoringVolume = logicScintillator;

  // build Pb Collimator
  G4double pbColRMin = 0.2*cm;
  G4double pbColRMax = 5.0*cm;
  G4double pbColZ    = 7.0*cm;
  G4double pbX = 0.*cm;
  G4double pbY = 0.*cm;
  G4double pbZ = 10.*cm;

  //G4Tubs *pbTub = new G4Tubs("pbTub",pbColRMin,pbColRMax,pbColZ,0.*deg,360.*deg);
  //G4LogicalVolume *pbLogic = new G4LogicalVolume(pbTub,PbMat,"pbLogic");
  //G4PVPlacement *pbPhys = new G4PVPlacement(0,G4ThreeVector(pbX,pbY,pbZ),pbLogic,"pbPhys",logicWorld,false,0,true);


  // place the scintillator frame
  auto* mountLV = BuildScintillatorMountLV();
  G4RotationMatrix* scintFrameRot = new G4RotationMatrix();
  scintFrameRot->rotateX(0*deg);
  scintFrameRot->rotateY(180*deg);
  scintFrameRot->rotateZ(0*deg);	
  new G4PVPlacement(scintFrameRot, G4ThreeVector(0,-2.8*mm,0.25*cm), mountLV, "ScintillatorMountPV", logicWorld, false, 0);
  
 
  VisAttributes();
}




void MyDetectorConstruction::ConstructCherenkov()
{
  // here we can construction our aerogel
  solidRadiator = new G4Box("solidRadiator",0.4*m, 0.4*m, 0.01*m);
  logicRadiator = new G4LogicalVolume(solidRadiator,Aerogel,"logicRadiator");
  physRadiator = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.25*m),logicRadiator,"physRadiator",logicWorld, 
                                   false,0,true);

  fScoringVolume = logicRadiator;

  // now we will make a sensitive detector
  solidDetector = new G4Box("solidDetector",xWorld/nRows,yWorld/nCols,0.01*m);
  logicDetector = new G4LogicalVolume(solidDetector,worldMat,"logicDetector");

  // here we generate many detectors at the same time. The value of rows and
  // columns is initialized in the header file but can be changed at runtime
  // by using the commands of the detector class
  for(G4int i=0; i<nRows; i++){// for loop to produce many detectors
    for(G4int j=0; j<nCols; j++){
      physDetector = new G4PVPlacement(0,
                        G4ThreeVector(-0.5*m+(i+0.5)*m/nRows,-0.5*m+(j+0.5)*m/nCols,0.49*m),logicDetector,
                        "physDetector",logicWorld,false,j+i*nCols,true);

    }
  }
}






void MyDetectorConstruction::ConstructTOF()
{
  solidDetector = new G4Box("solidDetector",1.*m,1.*m,0.1*m);
  logicDetector = new G4LogicalVolume(solidDetector, worldMat,"logicDetector");
  physDetector = new G4PVPlacement(0,G4ThreeVector(0.*m,0.*m,-4.*m),logicDetector,"physDetector",logicWorld,
                                   false,0,true);
  physDetector = new G4PVPlacement(0,G4ThreeVector(0.*m,0.*m, 3.*m),logicDetector,"physDetector",logicWorld,
                                   false,1,true);
}


G4VPhysicalVolume *MyDetectorConstruction::Construct()
{


  // Every material in geant4 has 3 parts
  //  The "solid", defines the size
  //  The "logical volume" which includes the material
  //  The "physical volume" places the volume in the geant4 simulation with coordinates and rotations, etc, and interacts with particles
  // The physical volume inherits the shape and logical volume from "logicWorld"
  solidWorld = new G4Box("solidWorld",xWorld,yWorld,zWorld); // creates a volume with half length x=0.5,y=0.5,z=0.5, giving a volumes 1x1x1 m^3
  logicWorld = new G4LogicalVolume(solidWorld,worldMat,"locigWorld");
  physWorld = new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),logicWorld,"physWorld",0,false,0,true);

  if(isCherenkov) ConstructCherenkov();
  if(isScintillator) ConstructScintillator();
  if(isTOF) ConstructTOF();


  return physWorld;
}

void MyDetectorConstruction::ConstructSDandField()
{


  // here we make a sensative detector and we tell the logical detector that it is this sensitive detector
  MySensitiveDetector *sensDet = new MySensitiveDetector("SensitiveDetector");

  if(logicDetector != NULL) logicDetector->SetSensitiveDetector(sensDet);
  
}


void MyDetectorConstruction::VisAttributes()
{
  G4VisAttributes *scint_va = new G4VisAttributes(G4Color(0.8,0.1,0.5,0.5));
  scint_va->SetForceSolid(true);
  scint_va->SetForceWireframe(true);
  logicScintillator->SetVisAttributes(scint_va);

  G4VisAttributes *photocath_va = new G4VisAttributes(G4Color(1.0,0.6,0.4,1));
  //photocath_va->SetForceSolid(true);
  photocath_va->SetForceWireframe(true);
  logicDetector->SetVisAttributes(photocath_va);

  G4VisAttributes *PMTglass_va = new G4VisAttributes(G4Color(1.0,1.0,1.0,0.5));
  PMTglass_va->SetForceSolid(false);
  PMTglass_va->SetForceWireframe(true);
  logicPMTWindow->SetVisAttributes(PMTglass_va);


  G4VisAttributes *quartzWindow_va = new G4VisAttributes(G4Color(0.0,0.0,0.5,0.5));
  quartzWindow_va->SetForceSolid(true);
  quartzWindow_va->SetForceWireframe(true);
  logicQuartzWindow->SetVisAttributes(quartzWindow_va);

}

G4LogicalVolume* MyDetectorConstruction::BuildScintillatorMountLV()
{
const G4double inch = 25.4*mm;


// Material (approx): 6061-T6 -> use pure Al for geometry
auto* nist = G4NistManager::Instance();
G4Material* mat = nist->FindOrBuildMaterial("G4_Al");


// ---- Overall stock ---- (inches)
const G4double W = 2.900*inch;
const G4double H = 3.380*inch;
const G4double T = 0.125*inch;


// ---- Through window (rectangular cutout) ----
// Using x = 0.378 -> 2.523 and y = 0.600 -> 3.000 from the print.
const G4double win_x0 = 0.25*inch;
const G4double win_x1 = 2.65*inch;
const G4double win_y0 = 0.600*inch;
const G4double win_y1 = 3.000*inch;


const G4double winW = (win_x1 - win_x0);
const G4double winH = (win_y1 - win_y0);
const G4double winCx = 0.5*(win_x0 + win_x1) - 0.5*W;
const G4double winCy = 0.5*(win_y0 + win_y1) - 0.5*H;


// ---- Front-side pocket (recess) ----
// Depth = 0.063 into the face (from section view).
// Using pocket footprint x = 0.250 -> 2.650 and y = 0.490 -> 3.110.
const G4double pocketDepth = 0.063*inch;


const G4double p_x0 = 0.140*inch;
const G4double p_x1 = 2.760*inch;
const G4double p_y0 = 0.490*inch;
const G4double p_y1 = 3.110*inch;


const G4double pW = (p_x1 - p_x0);
const G4double pH = (p_y1 - p_y0);
const G4double pCx = 0.5*(p_x0 + p_x1) - 0.5*W;
const G4double pCy = 0.5*(p_y0 + p_y1) - 0.5*H;


// ---- 4X Ø0.116 THRU holes ----
const G4double holeD = 0.116*inch;
const G4double holeR = 0.5*holeD;


// Using hole center coords from the print:
// x = 0.378 and 2.523 ; y = 0.345 and 3.255
const G4double hxL = 0.378*inch - 0.5*W;
const G4double hxR = 2.523*inch - 0.5*W;
const G4double hyB = 0.345*inch - 0.5*H;
const G4double hyT = 3.255*inch - 0.5*H;


// ---- Base solid: simple rectangular stock ----
// NOTE: This ignores the outer chamfers/radii (R2.115 TYP) on the drawing.
auto* plate = new G4Box("CeBrMountPlate", 0.5*W, 0.5*H, 0.5*T);


// ---- Subtract the front-side pocket ----
auto* pocketCut = new G4Box("CeBrPocketCut", 0.5*pW, 0.5*pH, 0.5*pocketDepth);


// Put pocket on +Z face (flip sign if your “front” is -Z)
const G4double zPocket = 0.5*T - 0.5*pocketDepth;


auto* s1 = new G4SubtractionSolid("CeBrMinusPocket",
plate, pocketCut,
nullptr,
G4ThreeVector(pCx, pCy, zPocket));


// ---- Subtract through window (full thickness) ----
auto* windowCut = new G4Box("CeBrWindowCut",
                                            0.5*winW, 0.5*winH, 0.5*(T + 1.0*mm)); // safety margin


auto* s2 = new G4SubtractionSolid("CeBrMinusPocketMinusWindow",
                                                                s1, windowCut,
                                                                nullptr,
                                                                G4ThreeVector(winCx, winCy, 0.0));


// ---- Subtract the 4 holes ----
auto* holeCut = new G4Tubs("CeBrHoleCut",
                            0.0, holeR, 0.5*(T + 1.0*mm),
                            0.0, 360.0*deg);


auto* s3 = new G4SubtractionSolid("CeBrMinusHole1", s2, holeCut, nullptr, G4ThreeVector(hxL, hyB, 0.0));
auto* s4 = new G4SubtractionSolid("CeBrMinusHole2", s3, holeCut, nullptr, G4ThreeVector(hxR, hyB, 0.0));
auto* s5 = new G4SubtractionSolid("CeBrMinusHole3", s4, holeCut, nullptr, G4ThreeVector(hxL, hyT, 0.0));
auto* s6 = new G4SubtractionSolid("CeBrMinusHole4", s5, holeCut, nullptr, G4ThreeVector(hxR, hyT, 0.0));


auto* lv = new G4LogicalVolume(s6, mat, "CeBrDetectorMountLV");


// Optional vis
auto* vis = new G4VisAttributes(G4Colour(0.7,0,0));
vis->SetForceSolid(false);
lv->SetVisAttributes(vis);


return lv;
}