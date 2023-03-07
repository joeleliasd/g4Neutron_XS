
#ifndef HadPhysicsListMessenger_h
#define HadPhysicsListMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class HadPhysicsList;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAString;
class G4UIcmdWithoutParameter;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class HadPhysicsListMessenger: public G4UImessenger
{
public:
  
  HadPhysicsListMessenger(HadPhysicsList* );
  virtual ~HadPhysicsListMessenger();
    
  void SetNewValue(G4UIcommand*, G4String);
    
private:
  
  HadPhysicsList* pPhysicsList;
    
  G4UIcmdWithADoubleAndUnit* gammaCutCmd;
  G4UIcmdWithADoubleAndUnit* electCutCmd;
  G4UIcmdWithADoubleAndUnit* posCutCmd;
  G4UIcmdWithADoubleAndUnit* allCutCmd;
  G4UIcmdWithAString*        pListCmd;
  G4UIcmdWithoutParameter*   listCmd;  
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

