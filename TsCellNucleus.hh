//TsCellNucleus.hh
#ifndef TsCellNucleus_h
#define TsCellNucleus_h

#include "TsVGeometryComponent.hh"
#include"G4PVParameterised.hh"
#include "TsParameterizationDistributed.hh"

//#include "G4NistManager.hh"
#include "G4ThreeVector.hh"
#include "G4Orb.hh"
#include "G4Material.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include <vector> 

class TsCellNucleus : public TsVGeometryComponent {
public:
    TsCellNucleus(TsParameterManager* pM, TsExtensionManager* eM, TsMaterialManager* mM, TsGeometryManager* gM,
                  TsVGeometryComponent* parentComponent, G4VPhysicalVolume* parentVolume, G4String& name);
    virtual ~TsCellNucleus();
    //virtual G4VPhysicalVolume* Construct() override;
    G4VPhysicalVolume* Construct();
    std::vector<G4ThreeVector> ReadPositionsFile();
    //void ProcessPositions(); // Este método pode ser usado para lógica adicional se necessário

private:
    TsParameterizationDistributed* fParam;

    // Parâmetros específicos da célula sem núcleo
    G4double fNucleusRadius;
    G4String fNucleusMaterial;
   
    G4double fCellRadius;
    G4String fCellMaterialName;
   
    G4double fElementRadius; // Raio das nanopartículas
    G4String fElementMaterialName; // Material das nanopartículas
   
    G4String fPositionFileNameIn; // Nome do arquivo contendo as posições das nanopartículas

    // Removido: m_BatchSize e m_Positions, a menos que ainda seja necessário para outra lógica
    // Se ProcessPositions não for mais necessário, você pode removê-lo ou comentá-lo
};

#endif // TsCellNucleus_h

