#include "ND280HadPhysicsList.hh"
#include "ND280HadPhysicsListMessenger.hh"
#include "ND280Exception.hh"
#include "ND280ExtraPhysics.hh"

#include <G4Version.hh>
#include <G4SystemOfUnits.hh>

#include <G4LossTableManager.hh>
#include <G4ParticleTable.hh>
#include <G4ProcessManager.hh>
#include <G4ProcessType.hh>
#include <G4HadronicProcessType.hh>
#include <G4ProcessVector.hh>
#include <G4EmConfigurator.hh>
#include <G4Gamma.hh>
#include <G4Electron.hh>
#include <G4Positron.hh>
#include <G4PAIModel.hh>
#include <G4RegionStore.hh>


#if G4VERSION_NUMBER < 1000
#include <G4PhysListFactory.hh>
#include <G4EmStandardPhysics_option3.hh>
#else
#include <G4PhysListFactoryAlt.hh>
using namespace g4alt;
#endif

#include "neutGeant4CascadeInterfacePionPhysics.hh"
#include "neutGeant4CascadeInterfaceHadronPhysicsQGSP_BERT.hh"

#include <TND280Log.hxx>

#include <unistd.h>
#include <stdexcept>

ND280HadPhysicsList::ND280HadPhysicsList(G4String physName) 
: G4VModularPhysicsList() {
    G4LossTableManager::Instance();
    
    defaultCutValue  = 1.*mm;
    fCutForGamma     = defaultCutValue;
    fCutForElectron  = defaultCutValue;
    fCutForPositron  = defaultCutValue;
    
    fMessenger = new ND280HadPhysicsListMessenger(this);
    
    SetVerboseLevel(1);
    
    G4PhysListFactory factory;
    
    // Add our specific lists.  This is mainly the step limiter.
    fExtra = new ND280ExtraPhysics();
    RegisterPhysics(fExtra);
    
    // Add our default list of extra processes.  This can be overridden in the
    // macro file by using 
    // /t2k/phys/addProcess 
    // AddExtraProcess("PAI","all","driftRegion");
    
    // neutGeant4CascadeInterface off by default
    useneutGeant4CascadeInterface = false;
    
    // Handle neutGeant4CascadeInterface case passed with ND280GEANT4SIM.exe "-p" option
    if(physName == "neutGeant4CascadeInterface_QGSP_BERT"){
    	useneutGeant4CascadeInterface = true;
    	physName = "QGSP_BERT";
    }
    
    // Check to see if the physics list has been over ridden from the
    // environment variable PHYSLIST
    char* list = std::getenv("PHYSLIST");
    if (list) {
        SetPhysicsList(factory.ReferencePhysList());
    }
    else if (physName.size() > 1 && factory.IsReferencePhysList(physName)) {
        SetPhysicsList(physName);
    }
    else {
    	#if G4VERSION_NUMBER < 1000
        SetPhysicsList("QGSP_BERT");
        #else
        SetPhysicsList("QGSP_BERT_EMZ");
        #endif
    }
}

ND280HadPhysicsList::~ND280HadPhysicsList() {
    delete fMessenger;
}

void ND280HadPhysicsList::SetCuts() {
    if (verboseLevel >0) {
        ND280Log("ND280HadPhysicsList::SetCuts: "
            << "CutLength : " << G4BestUnit(defaultCutValue,"Length"));
    }
    
    // set cut values for gamma at first and for e- second and next for e+,
    // because some processes for e+/e- need cut values for gamma
    SetCutValue(fCutForGamma, "gamma");
    SetCutValue(fCutForElectron, "e-");
    SetCutValue(fCutForPositron, "e+");
    
    if (verboseLevel>0) DumpCutValuesTable();
}

void ND280HadPhysicsList::AddPAIModel(G4String particle, G4String region) {
    ND280Log("Add the PAI model to " << region);
    // Loop over all the particles.
    G4EmConfigurator* emConfig
    = G4LossTableManager::Instance()->EmConfigurator();
    G4ParticleTable::G4PTblDicIterator* particleIterator
    = theParticleTable->GetIterator();
    particleIterator->reset();
    while ((*particleIterator)()) {
        G4ParticleDefinition* particleDef = particleIterator->value();
        G4String particleName = particleDef->GetParticleName();
        if (particle != "all" && particle != particleName) continue;
        if(particleName == "e-" || particleName == "e+") {
            G4PAIModel* pai = new G4PAIModel(particleDef,"PAIModel");
            emConfig->SetExtraEmModel(particleName,"eIoni",pai,region,
            	0.0,100.*GeV,pai);
        }
        else if (particleName == "mu-" || particleName == "mu+") {
            G4PAIModel* pai = new G4PAIModel(particleDef,"PAIModel");
            emConfig->SetExtraEmModel(particleName,"muIoni",pai,region,
            	0.0,100.*GeV,pai);
        }
        else if (particleName == "proton" ||
            particleName == "pi+" ||
        particleName == "pi-" ) {
        G4PAIModel* pai = new G4PAIModel(particleDef,"PAIModel");
        emConfig->SetExtraEmModel(particleName,"hIoni",pai,region,
            0.0,100.*GeV,pai);
        }
    }
    /* Define a set of ProductionCuts for this region, a copy of the default ones. */
    G4RegionStore* Grs = G4RegionStore::GetInstance();
    G4Region* thisRegion = Grs->GetRegion(region,false);
    if(thisRegion)
    {
    	G4ProductionCuts* defaultRegionCuts = 
    	G4ProductionCutsTable::GetProductionCutsTable()->GetDefaultProductionCuts();
    	G4ProductionCuts* thisRegionCuts = new G4ProductionCuts();
    	*thisRegionCuts = *defaultRegionCuts;
    	thisRegion->SetProductionCuts(thisRegionCuts);
    }
    else
    {
    	ND280Error(region<<" does not exist");
    	G4String message("ND280PhysicsList::AddPAIModel() found no region called:");
    	message=message+region;
    	
    	throw std::runtime_error(message.data());
    }
    
}

void ND280HadPhysicsList::ConstructProcess() {
    
    G4VModularPhysicsList::ConstructProcess();
    
    bool modelAdded = false;
    
    for (std::vector< ExtraProcess_t >::iterator p
    	= fExtraProcesses.begin(); p != fExtraProcesses.end(); ++p) 
    {
    	G4String name = p->fProcess;
    	G4String particle = p->fParticle;
    	G4String region = p->fRegion;
    	bool localModelAdded = false;
    	
    	if (name == "PAI") {
    	    AddPAIModel(particle,region);        
    	    modelAdded = true;
    	    localModelAdded = true;
    	}
    	else 
    	{
    	    ND280Error("Unknown extra process requested: " << name);
    	}
    	if(localModelAdded)ND280Log("Extra process <"<<name<<"> added  for particle type:"<<particle);   
    }
    
    if (modelAdded) {
    	
    	G4LossTableManager::Instance()->EmConfigurator()->AddModels();
    }
}

void ND280HadPhysicsList::SetCutForGamma(G4double cut) {
    fCutForGamma = cut;
    SetParticleCuts(fCutForGamma, G4Gamma::Gamma());
}

void ND280HadPhysicsList::SetCutForElectron(G4double cut) {
    fCutForElectron = cut;
    SetParticleCuts(fCutForElectron, G4Electron::Electron());
}

void ND280HadPhysicsList::SetCutForPositron(G4double cut) {
    fCutForPositron = cut;
    SetParticleCuts(fCutForPositron, G4Positron::Positron());
}

void ND280HadPhysicsList::SetPhysicsList(G4String physName) {
    G4PhysListFactory factory;
    G4VModularPhysicsList* phys = NULL;
    
    ND280Log("Set physics list name: " << physName);
    
    phys =factory.GetReferencePhysList(physName);
    if (!phys) {
        ND280Error("Unknown physics list");
        std::runtime_error("Missing physics list");
    }
    
    SetPhysicsList(phys);
}

void ND280HadPhysicsList::SetPhysicsList(G4VModularPhysicsList* list) {
    // Transfer the physics list from the input list to this one.
    for (G4int i = 0; ; ++i) {
        G4VPhysicsConstructor* elem =
        const_cast<G4VPhysicsConstructor*> (list->GetPhysics(i));
        if (elem == NULL) break;
        
	if( elem->GetPhysicsName() == "hInelastic QGSP_BERT" && useneutGeant4CascadeInterface){
	    ND280Log("Will Replace default hInelastic QGSP_BERT with neutGeant4CascadeInterface");
	    continue;
	SetBuilderList4();
	hadronPhys.push_back( new neutGeant4CascadeInterfaceHadronPhysicsQGSP_BERT("hadron",true));
	dump = true;
	    continue;
	}
	
        ND280Log("RegisterPhysics: " << elem->GetPhysicsName());
        #if G4VERSION_NUMBER < 1000
        RegisterPhysics(elem);
        #else
        ReplacePhysics(elem);
        #endif
    }
    
    if(useneutGeant4CascadeInterface){
    	ND280Log("Adding neutGeant4CascadeInterfacePionPhysics and neutGeant4CascadeInterfaceHadronPhysicsQGSP_BERT");
    	#if G4VERSION_NUMBER < 1000
    	RegisterPhysics( new neutGeant4CascadeInterfacePionPhysics() );
    	RegisterPhysics( new neutGeant4CascadeInterfaceHadronPhysicsQGSP_BERT("hadron",true) );
    	#else
    	ReplacePhysics( new neutGeant4CascadeInterfacePionPhysics() );
    	ReplacePhysics( new neutGeant4CascadeInterfaceHadronPhysicsQGSP_BERT("hadron",true) );
    	#endif
    }
    
}
