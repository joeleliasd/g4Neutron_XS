#include "globals.hh"

#include "ND280HadPhysicsListMessenger.hh"
#include "ND280HadPhysicsList.hh"
#include "ND280ExtraPhysics.hh"

#include <G4UIdirectory.hh>
#include <G4UIcommand.hh>
#include <G4UIparameter.hh>
#include <G4UIcmdWithAString.hh>
#include <G4UIcmdWithoutParameter.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>
#include <G4UIcmdWithABool.hh>

#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"

ND280HadPhysicsListMessenger::ND280HadPhysicsListMessenger(ND280HadPhysicsList* pPhys)
: fPhysicsList(pPhys) {
    fDirectory = new G4UIdirectory("/t2k/phys/");
    fDirectory->SetGuidance("Control the physics lists");

    fGammaCutCMD = new G4UIcmdWithADoubleAndUnit("/t2k/phys/gammaCut",this);  
    fGammaCutCMD->SetGuidance("Set gamma cut");
    fGammaCutCMD->SetParameterName("Gcut",false);
    fGammaCutCMD->SetUnitCategory("Length");
    fGammaCutCMD->SetRange("Gcut>0.0");
    fGammaCutCMD->SetDefaultUnit("mm");
    fGammaCutCMD->AvailableForStates(G4State_PreInit,G4State_Idle);
    
    fElectCutCMD = new G4UIcmdWithADoubleAndUnit("/t2k/phys/electronCut",
                                                 this);
    fElectCutCMD->SetGuidance("Set electron cut");
    fElectCutCMD->SetParameterName("Ecut",false);
    fElectCutCMD->SetUnitCategory("Length");
    fElectCutCMD->SetRange("Ecut>0.0");
    fElectCutCMD->SetDefaultUnit("mm");
    fElectCutCMD->AvailableForStates(G4State_PreInit,G4State_Idle);

    fPosCutCMD = new G4UIcmdWithADoubleAndUnit("/t2k/phys/positronCut",
                                               this);
    fPosCutCMD->SetGuidance("Set positron cut");
    fPosCutCMD->SetParameterName("Pcut",false);
    fPosCutCMD->SetUnitCategory("Length");
    fPosCutCMD->SetRange("Pcut>0.0");
    fPosCutCMD->SetDefaultUnit("mm");
    fPosCutCMD->AvailableForStates(G4State_PreInit,G4State_Idle);

    fAllCutCMD = new G4UIcmdWithADoubleAndUnit("/t2k/phys/allCuts",this);
    fAllCutCMD->SetGuidance("Set cut for all");
    fAllCutCMD->SetParameterName("cut",false);
    fAllCutCMD->SetUnitCategory("Length");
    fAllCutCMD->SetRange("cut>0.0");
    fAllCutCMD->SetDefaultUnit("mm");
    fAllCutCMD->AvailableForStates(G4State_PreInit,G4State_Idle);

    fAddProcCMD = new G4UIcommand("/t2k/phys/addProcess",this);
    fAddProcCMD->SetGuidance("Add an extra process [allowed values "
                               "PAI" "]");
    fAddProcCMD->AvailableForStates(G4State_PreInit,G4State_Idle);
    G4UIparameter* param = new G4UIparameter("process",'s',false);
    param->SetGuidance("Process to be added");
    fAddProcCMD->SetParameter(param);
    param = new G4UIparameter("particle",'s',true);
    param->SetGuidance("Particle to be modified");
    param->SetDefaultValue("all");
    fAddProcCMD->SetParameter(param);
    param = new G4UIparameter("region",'s',true);
    param->SetGuidance("Applicable region");
    param->SetDefaultValue("none");
    fAddProcCMD->SetParameter(param);

    //added by Joel
    pListCmd = new G4UIcmdWithAString("/testhadr/Physics",this);
    pListCmd->SetGuidance("Add modula physics list.");
    pListCmd->SetParameterName("PList",false);
    pListCmd->AvailableForStates(G4State_PreInit);

    listCmd = new G4UIcmdWithoutParameter("/testhadr/ListPhysics",this);
    listCmd->SetGuidance("Available Physics Lists");
    listCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
    
    fClearProcCMD = new G4UIcmdWithoutParameter("/t2k/phys/clearProcess",this);
    fClearProcCMD->SetGuidance("Clear the extra process list.");

}

ND280HadPhysicsListMessenger::~ND280HadPhysicsListMessenger() {
    delete fGammaCutCMD;
    delete fElectCutCMD;
    delete fPosCutCMD;
    delete fAllCutCMD;
    delete fAddProcCMD;
    delete fClearProcCMD;
}

void ND280HadPhysicsListMessenger::SetNewValue(G4UIcommand* command,
                                          G4String newValue) {
    if (command == fGammaCutCMD) {
        fPhysicsList->SetCutForGamma(fGammaCutCMD
                                     ->GetNewDoubleValue(newValue));
    }
    else if (command == fElectCutCMD) {
        fPhysicsList->SetCutForElectron(fElectCutCMD
                                        ->GetNewDoubleValue(newValue));
    }
    else if (command == fPosCutCMD) {
        fPhysicsList->SetCutForPositron(fPosCutCMD
                                        ->GetNewDoubleValue(newValue));
    }
    else if (command == fAllCutCMD) {
        G4double cut = fAllCutCMD->GetNewDoubleValue(newValue);
        fPhysicsList->SetCutForGamma(cut);
        fPhysicsList->SetCutForElectron(cut);
        fPhysicsList->SetCutForPositron(cut);
    }
    else if (command == fAddProcCMD) {
        G4String process;
        G4String particle;
        G4String region;
        std::istringstream val(newValue);
        val >> process >> particle >> region;
        if (region == "none") region = "";
        fPhysicsList->AddExtraProcess(process, particle, region);
    }
    else if (command == fClearProcCMD) {
        fPhysicsList->ClearExtraProcesses();
    }
    //added begin
    if( command == pListCmd ) {
      G4String name = newValue;
      if(name == "PHYSLIST") {
        char* path = getenv(name);
        if (path) name = G4String(path);
        else {
          G4cout << "### PhysicsListMessenger WARNING: "
           << " environment variable PHYSLIST is not defined"
           << G4endl;
    return;
        }
      }
      fPhysicsList->SetPhysicsList(name);
    }

    //if( command == listCmd )
    // fPhysicsList->List();
    //added end
}
