#ifndef ND280HadPhysicsListMessenger_h
#define ND280HadPhysicsListMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class ND280HadPhysicsList;

class G4UIdirectory;
class G4UIcommand;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithABool;
class G4UIcmdWithAString;
class G4UIcmdWithoutParameter;

/// Provide control of the physics list and cut parameters
class ND280HadPhysicsListMessenger 
    : public G4UImessenger {
public:
    
    ND280HadPhysicsListMessenger(ND280HadPhysicsList* );
    virtual ~ND280HadPhysicsListMessenger();
 
    virtual void SetNewValue(G4UIcommand*, G4String);

private:

    ND280HadPhysicsList* fPhysicsList;
 
    G4UIdirectory* fDirectory;

    G4UIcmdWithADoubleAndUnit* fGammaCutCMD;
    G4UIcmdWithADoubleAndUnit* fElectCutCMD;
    G4UIcmdWithADoubleAndUnit* fPosCutCMD;
    G4UIcmdWithADoubleAndUnit* fAllCutCMD;
    G4UIcommand*               fAddProcCMD;
    G4UIcmdWithAString*        pListCmd;
    G4UIcmdWithoutParameter*   listCmd;  
    G4UIcmdWithoutParameter*   fClearProcCMD;
    
};
#endif
