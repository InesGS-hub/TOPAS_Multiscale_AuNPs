// Component for TsDistributedComponents
//
// ******************************************************************
// *
// *
// * File included as an extension for TOPAS
// * Authors: Alejandro Bertolet <abertoletreina@mgh.harvard.edu>, 
// * modified by José Antonio López-Valverde <jlvalverde@us.es>
// *
// ******************************************************************

#include "TsDistributedComponents.hh"
#include "TsParameterManager.hh"



TsDistributedComponents::TsDistributedComponents(TsParameterManager* pM, TsExtensionManager* eM, TsMaterialManager* mM, TsGeometryManager* gM,
		TsVGeometryComponent* parentComponent, G4VPhysicalVolume* parentVolume, G4String& name) :
		TsVGeometryComponent(pM, eM, mM, gM, parentComponent, parentVolume, name)
{

	fComponentRadius = fPm->GetDoubleParameter(GetFullParmName("ComponentRadius"), "Length");
	fElementRadius = fPm->GetDoubleParameter(GetFullParmName("ElementRadius"), "Length");
	fElementMaterialName = fPm->GetStringParameter(GetFullParmName("ElementMaterial"));
	fPositionsFileName = fPm->GetStringParameter(GetFullParmName("PositionsFile"));
	fComponentRmin = fPm->GetDoubleParameter(GetFullParmName("ComponentRmin"), "Length");
	fComponentSTheta = fPm->GetDoubleParameter(GetFullParmName("ComponentSTheta"), "Angle");
	fComponentDTheta = fPm->GetDoubleParameter(GetFullParmName("ComponentDTheta"), "Angle");

	
	
}
TsDistributedComponents::~TsDistributedComponents() {}

G4VPhysicalVolume* TsDistributedComponents::Construct()
{
	std::vector<G4ThreeVector> positionList = ReadPositionsFile();
	G4int nSubcomponents = positionList.size();
	BeginConstruction();

	// Component Orb
	G4Sphere* componentSolid = new G4Sphere(fName, fComponentRmin, fComponentRadius, 0*deg, 360*deg, fComponentSTheta, fComponentDTheta);
	fEnvelopeLog = CreateLogicalVolume(componentSolid);
	fEnvelopePhys = CreatePhysicalVolume(fEnvelopeLog);

	// Parameterization
	fParam = new TsParameterizationDistributed(fElementRadius, positionList);
	
	// Subelements
	G4String name = "SubcompSolid";
	G4VSolid* subcomponentSolid = new G4Orb(name, fElementRadius);
	G4LogicalVolume* subLogic = CreateLogicalVolume("LogSubcomponent", fElementMaterialName, subcomponentSolid);
	if (nSubcomponents > 1)
		CreatePhysicalVolume("Subcomponents", subLogic, fEnvelopePhys, kUndefined, nSubcomponents, fParam);
	if (nSubcomponents == 1)
		CreatePhysicalVolume("Subcomponent", subLogic, new G4RotationMatrix(), &positionList[0], fEnvelopePhys);

	if (fParentVolume)
		InstantiateChildren();
	return fEnvelopePhys;
	
}

std::vector<G4ThreeVector> TsDistributedComponents::ReadPositionsFile()
{
	std::vector<G4ThreeVector> positions;

	  // Open the file for reading.
	  std::ifstream file(fPositionsFileName);
	  if (!file)
	  {
	    std::cerr << "Error: Unable to open file '" << fPositionsFileName << "'." << std::endl;
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
