
#ifndef HadPhysicsList_h
#define HadPhysicsList_h 1

#include "G4VModularPhysicsList.hh"
#include "globals.hh"

class G4VPhysicsConstructor;
class HadPhysicsListMessenger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class HadPhysicsList: public G4VModularPhysicsList
{
public:

  HadPhysicsList();
  virtual ~HadPhysicsList();

  void ConstructParticle();
    
  void SetCuts();
  void SetCutForGamma(G4double);
  void SetCutForElectron(G4double);
  void SetCutForPositron(G4double);
        
  void AddPhysicsList(const G4String& name);
  void ConstructProcess();
  void List();
  
private:

  void SetBuilderList0(G4bool flagHP = false);
  void SetBuilderList1(G4bool flagHP = false);
  void SetBuilderList2(G4bool flagHP = false);
  void SetBuilderList3(G4bool flagHP = false);
  void SetBuilderList4(G4bool flagHP = false);
  void SetBuilderList5(G4bool flagHP = false);
  void SetBuilderList6(G4bool flagHP = false);

  G4double cutForGamma;
  G4double cutForElectron;
  G4double cutForPositron;

  G4VPhysicsConstructor*  emPhysicsList;
  G4VPhysicsConstructor*  particleList;
  std::vector<G4VPhysicsConstructor*>  hadronPhys;
    
  HadPhysicsListMessenger* pMessenger;
  G4bool dump;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

