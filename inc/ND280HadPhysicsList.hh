#ifndef ND280HadPhysicsList_h
#define ND280HadPhysicsList_h 1

#include "globals.hh"
#include "G4VModularPhysicsList.hh"
#include <vector>
#include <utility>

class G4VPhysicsConstructor;
class ND280HadPhysicsListMessenger;
class ND280ExtraPhysics;

/// Use the G4PhysListFactory to select a physics list for this run.  The
/// physics list can be set from the PHYSLIST environment variable, or a macro
/// file.  All of the physics lists need to be defined before using the
/// /run/initialize command.
class ND280HadPhysicsList: public G4VModularPhysicsList {
public:
    /// A vector of extra processes and regions.
    struct ExtraProcess_t {
        ExtraProcess_t(G4String proc, G4String part, G4String reg)
            : fProcess(proc), fParticle(part), fRegion(reg) {}
        G4String fProcess;      // The process name
        G4String fParticle;     // The particle name ("all" for everything).
        G4String fRegion;       // The detector region.
    };

    /// Construct the default physics list.  If physName is a valid list, then
    /// it will be used.  Otherwise, the PHYSLIST environment variable will be
    /// checked to provide a default.  If that doesn't exist, then a default
    /// is provided.  The macro file can be used to override the default
    /// physics list.
    explicit ND280HadPhysicsList(G4String physName = "");
    virtual ~ND280HadPhysicsList();

    /// Used by ND280GEANT4SIM to set the physics list by name.  The physics list is
    /// generated using the G4PhysListFactory class.
    void SetPhysicsList(G4String pName);

    /// Use by ND280GEANT4SIM to set the physics list from an existing pointer.  This
    /// copies the information in the input list to the local list.
    void SetPhysicsList(G4VModularPhysicsList* list);
    
    /// Set the range cut for photons.
    void SetCutForGamma(G4double);
    
    /// Set the range cut for electrons.
    void SetCutForElectron(G4double);

    /// Set the range cut for positrons.
    void SetCutForPositron(G4double);

    /// Clear the extra processes list.
    void ClearExtraProcesses() {fExtraProcesses.clear();}

    //add begin
    //void AddPhysicsList(const G4String& name);
    //void List();
    //add end

    /// Add an extra process.
    void AddExtraProcess(G4String name, G4String particle, G4String region) {
        fExtraProcesses.push_back(ExtraProcess_t(name,particle,region));
    }
    
    /// Used by GEANT4 to set the cuts defined below.  This must be define.
    virtual void SetCuts();

    /// Used by GEANT4 to construct the particles.  Uncomment this if we need
    // to augment the base method.
    // virtual void ConstructParticle();

    /// Used by GEANT4 to construct the particles.  Declared to let us augment
    /// the base method.  In particular this is where we add any extra EM
    /// models (like the G4PAIModel.
    virtual void ConstructProcess();
    
private:
    void SetBuilderList0(G4bool flagHP = false);
    void SetBuilderList1(G4bool flagHP = false);
    void SetBuilderList2(G4bool flagHP = false);
    void SetBuilderList3(G4bool flagHP = false);
    void SetBuilderList4(G4bool flagHP = false);
    void SetBuilderList5(G4bool flagHP = false);
    void SetBuilderList6(G4bool flagHP = false);

    /// The gamma-ray range cut.
    G4double fCutForGamma;

    /// The electron range cut.
    G4double fCutForElectron;

    /// The positron range cut.
    G4double fCutForPositron;

    /// The extra physics list
    ND280ExtraPhysics* fExtra;
    
    std::vector<G4VPhysicsConstructor*>  hadronPhys;

    G4bool dump;

    std::vector< struct ExtraProcess_t > fExtraProcesses;

    /// Boolean to control usage of neutGeant4CascadeInterface
    G4bool useneutGeant4CascadeInterface;
    
    /// The messenger to control this class.
    ND280HadPhysicsListMessenger* fMessenger;

    /// A method to add the PAI model to the requested region.  This does not
    /// call the AddModels method.
    void AddPAIModel(G4String particle, G4String region);

};
#endif
