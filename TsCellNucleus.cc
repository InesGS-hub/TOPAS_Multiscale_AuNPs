// Component for TsCellNucleus

#include "TsCellNucleus.hh"
#include "G4SubtractionSolid.hh"
#include "TsParameterManager.hh"
#include "G4ThreeVector.hh"
#include "G4Material.hh"
#include "TsParameterizationDistributed.hh"
#include <sstream>
#include <iostream>
#include <vector> 
// #include <memory>
#include "G4NistManager.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include <fstream> 
#include "G4Orb.hh"

TsCellNucleus::TsCellNucleus(TsParameterManager* pM, TsExtensionManager* eM, TsMaterialManager* mM, TsGeometryManager* gM,
                             TsVGeometryComponent* parentComponent, G4VPhysicalVolume* parentVolume, G4String& name) :
    TsVGeometryComponent(pM, eM, mM, gM, parentComponent, parentVolume, name),
    fNucleusRadius(0.0),
    fNucleusMaterial(""),
    fCellRadius(0.0),
    fCellMaterialName(""),
    fElementRadius(0.0),
    fElementMaterialName(""),
    fPositionFileNameIn("")

{

    // Parâmetros específicos da célula sem núcleo
    fNucleusRadius = fPm->GetDoubleParameter(GetFullParmName("NucleusRadius"), "Length");
    fCellRadius = fPm->GetDoubleParameter(GetFullParmName("CellRadius"), "Length");
    fNucleusMaterial = fPm->GetStringParameter(GetFullParmName("NucleusMaterial"));
    fCellMaterialName = fPm->GetStringParameter(GetFullParmName("CellMaterial"));
    fElementRadius = fPm->GetDoubleParameter(GetFullParmName("ElementRadius"), "Length");
    fElementMaterialName = fPm->GetStringParameter(GetFullParmName("ElementMaterial"));
    fPositionFileNameIn = fPm->GetStringParameter(GetFullParmName("PositionsFile"));
}

TsCellNucleus::~TsCellNucleus() {}

G4VPhysicalVolume* TsCellNucleus::Construct() 
{  
    std::vector<G4ThreeVector> positionList = ReadPositionsFile();
    G4int nSubcomponents = positionList.size();
    BeginConstruction();
    
    // Origin of Nucleus
    G4ThreeVector nucleusCenter(0 * CLHEP::um, 0 * CLHEP::um, 3.9 * CLHEP::um);

    G4Material* nucleusMaterial = G4Material::GetMaterial(fNucleusMaterial);
    G4Material* cellMaterial = G4Material::GetMaterial(fCellMaterialName);
    G4Material* ElementMaterial = G4Material::GetMaterial(fElementMaterialName);

    // Component Cell

    G4Sphere* cellSolid = new G4Sphere("cellSolid", 0.0, fCellRadius, 0*CLHEP::deg, 360*CLHEP::deg, 0*CLHEP::deg, 90*CLHEP::deg);
    G4LogicalVolume* cellSolidLogic = new G4LogicalVolume(cellSolid, cellMaterial, "cellSolidLogic");
    //G4VPhysicalVolume* cellSolidPhys = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), cellSolidLogic, "cellSolidPhys", nullptr, false, 0);
    
    // Component Nucleus
    G4Sphere* nucleusSolid = new G4Sphere("NucleusSolid", 0.0, fNucleusRadius, 0 * CLHEP::deg, 360 * CLHEP::deg, 0 * CLHEP::deg, 360 * CLHEP::deg);
	G4LogicalVolume* nucleusLogic = new G4LogicalVolume(nucleusSolid, nucleusMaterial, "nucleusLogic");
    G4VPhysicalVolume* nucleusPhys = new G4PVPlacement(0, nucleusCenter, nucleusLogic, "nucleusPhys", cellSolidLogic, false, 0);
    

    // Subtraction of Volumes (Cell - Nucleus)
    // Calcula a transformação para a posição desejada do núcleo
    G4ThreeVector translation = nucleusCenter; // Núcleo centrado em nucleusCenter
    G4RotationMatrix rot; // Sem rotação
    G4Transform3D transform(rot, translation);

    G4SubtractionSolid* cellWithoutNucleusSolid = new G4SubtractionSolid("CellWithoutNucleus", cellSolid, nucleusSolid, transform);
    G4LogicalVolume* cellWithoutNucleusLogic = new G4LogicalVolume(cellWithoutNucleusSolid, cellMaterial, "cellWithoutNucleusLogic");
     //G4VPhysicalVolume* cellWithoutNucleusPhys = CreatePhysicalVolume(cellWithoutNucleusLogic);
  
    //fEnvelopeLog = CreateLogicalVolume(cellWithoutNucleusSolid);
	fEnvelopePhys = CreatePhysicalVolume(cellWithoutNucleusLogic);

    delete cellSolid;
    delete cellSolidLogic;	
    delete nucleusSolid;
    delete nucleusLogic;
    delete nucleusPhys;

    // Parameterization
    fParam = new TsParameterizationDistributed(fElementRadius, positionList);

    // Subelements
    G4String name = "SubcompSolid";
    G4VSolid* subcomponentSolid = new G4Orb(name, fElementRadius);
    G4LogicalVolume* subLogic = new G4LogicalVolume(subcomponentSolid, ElementMaterial, "subcomponentSolid");

    if (nSubcomponents > 1)
        CreatePhysicalVolume("Subcomponents", subLogic, fEnvelopePhys, kUndefined, nSubcomponents, fParam);
    if (nSubcomponents == 1)
        CreatePhysicalVolume("Subcomponent", subLogic, new G4RotationMatrix(), &positionList[0], fEnvelopePhys);

    if (fParentVolume)
        InstantiateChildren();

    return fEnvelopePhys; // Assegure-se de que este é o retorno correto para o seu método
}

std::vector<G4ThreeVector> TsCellNucleus::ReadPositionsFile()
{
    std::vector<G4ThreeVector> positions;

      // Open the file for reading.
      std::ifstream file(fPositionFileNameIn);
      if (!file)
      {
        std::cerr << "Error: Unable to open file '" << fPositionFileNameIn << "'." << std::endl;
        return positions;
      }

      // Read each line of the file, which should contain a 3D position in the
      // format "x y z".
      std::string line;
      while (std::getline(file, line))
      {
        std::istringstream lineStream(line);
        double x, y, z;
        if (lineStream >> x >> y >> z)
        {
          // If the line could be parsed as a 3D position, add it to the vector.
          positions.push_back(G4ThreeVector(x*nm, y*nm, z*nm));
        }
        else
        {
          // Otherwise, print an error message.
          std::cerr << "Error: Invalid position '" << line << "'." << std::endl;
        }
      }
      return positions;
}
;
