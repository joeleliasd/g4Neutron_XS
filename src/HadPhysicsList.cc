

#include "HadPhysicsList.hh"
#include "HadPhysicsListMessenger.hh"

#include "G4DecayPhysics.hh"
#include "G4EmStandardPhysics_option2.hh"
#include "G4EmStandardPhysics_option1.hh"
#include "G4EmStandardPhysics.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4HadronDElasticPhysics.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4HadronHElasticPhysics.hh"
#include "G4ChargeExchangePhysics.hh"
#include "G4NeutronTrackingCut.hh"
#include "G4StoppingPhysics.hh"
//#include "G4LHEPStoppingPhysics.hh"
#include "G4IonBinaryCascadePhysics.hh"
#include "G4IonPhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4EmProcessOptions.hh"

//#include "G4HadronPhysicsFTFC.hh"
//#include "G4HadronPhysicsFTFP.hh"
#include "G4HadronPhysicsFTFP_BERT.hh"
#include "G4HadronPhysicsFTF_BIC.hh"
//#include "G4HadronPhysicsLHEP.hh"
//#include "G4HadronPhysicsLHEP_BERT.hh"
//#include "G4HadronPhysicsLHEP_EMV.hh"
//#include "G4HadronPhysicsLHEP_PRECO_HP.hh"
#include "G4HadronInelasticQBBC.hh"
//#include "G4HadronPhysicsQGSC.hh"
//#include "G4HadronPhysicsQGSC_BERT.hh"
//#include "G4HadronPhysicsQGSC_EFLOW.hh"
//#include "G4HadronPhysicsQGSP.hh"
#include "G4HadronPhysicsQGSP_BERT.hh"
#include "G4HadronPhysicsQGSP_BERT_HP.hh"
#include "G4HadronPhysicsQGSP_BIC.hh"
#include "G4HadronPhysicsQGSP_BIC_HP.hh"

//#include "G4HadronInelasticQLHEP.hh"
#include "G4IonPhysics.hh"

#include "G4LossTableManager.hh"

#include "G4ProcessManager.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

HadPhysicsList::HadPhysicsList() : G4VModularPhysicsList()
{
  G4LossTableManager::Instance();
  defaultCutValue = 1.*mm;
  cutForGamma     = defaultCutValue;
  cutForElectron  = defaultCutValue;
  cutForPositron  = defaultCutValue;
  dump            = false;
  verboseLevel    = 1;

  pMessenger = new HadPhysicsListMessenger(this);

  // Particles
  particleList = new G4DecayPhysics("decays");

  // EM physics
  emPhysicsList = new G4EmStandardPhysics();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

HadPhysicsList::~HadPhysicsList()
{
  delete pMessenger;
  delete particleList;
  delete emPhysicsList;
  for(size_t i=0; i<hadronPhys.size(); i++) {
    delete hadronPhys[i];
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void HadPhysicsList::ConstructParticle()
{
  particleList->ConstructParticle();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void HadPhysicsList::ConstructProcess()
{
  AddTransportation();
  emPhysicsList->ConstructProcess();
  particleList->ConstructProcess();
  for(size_t i=0; i<hadronPhys.size(); i++) {
    hadronPhys[i]->ConstructProcess();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void HadPhysicsList::AddPhysicsList(const G4String& name)
{
  if (verboseLevel>0)
    G4cout << "PhysicsList::AddPhysicsList: <" << name << ">" << G4endl;

  if (name == "emstandard_opt2") {

    delete emPhysicsList;
    emPhysicsList = new G4EmStandardPhysics_option2();

  } else if (name == "emstandard_opt1") {

    delete emPhysicsList;
    emPhysicsList = new G4EmStandardPhysics_option1();

  } /*else if (name == "FTFC") {

    SetBuilderList1();
    hadronPhys.push_back( new HadronPhysicsFTFC("hadron",true));
    dump = true;

  } else if (name == "FTFP") {

    SetBuilderList1();
    hadronPhys.push_back( new HadronPhysicsFTFP("hadron",true));
    dump = true;

  }*/ else if (name == "FTFP_BERT") {

    SetBuilderList1();
    hadronPhys.push_back( new HadronPhysicsFTFP_BERT("hadron",true));
    dump = true;

  } else if (name == "FTFP_EMV") {

    AddPhysicsList("emstandard_opt1");
    AddPhysicsList("FTFP");

  } else if (name == "FTF_BIC") {

    SetBuilderList0();
    hadronPhys.push_back( new HadronPhysicsFTF_BIC("hadron",true));
    dump = true;

  } else if (name == "LHEP") {

    SetBuilderList2();
    hadronPhys.push_back( new HadronPhysicsLHEP("hadron"));
    dump = true;

  }/* else if (name == "LHEP_BERT") {

    SetBuilderList3();
    hadronPhys.push_back( new HadronPhysicsLHEP_BERT("hadron"));
    dump = true;

  }*/ else if (name == "LHEP_EMV") {

    AddPhysicsList("emstandard_opt1");
    SetBuilderList3();
    hadronPhys.push_back( new HadronPhysicsLHEP_EMV("hadron"));
    dump = true;

  }/* else if (name == "LHEP_PRECO_HP") {

    SetBuilderList3(true);
    hadronPhys.push_back( new HadronPhysicsLHEP_PRECO_HP("hadron"));
    dump = true;

  }*/ else if (name == "QBBC") {

    SetBuilderList0();
    hadronPhys.push_back( new G4HadronInelasticQBBC("QBBC",verboseLevel,
						    false,true,false,false,false));

  } else if (name == "QBBCG") {

    SetBuilderList0();
    hadronPhys.push_back( new G4ChargeExchangePhysics(verboseLevel,false));
    hadronPhys.push_back( new G4HadronInelasticQBBC("QBBC",verboseLevel,
						    false,true,false,false,true));
  } else if (name == "QBBCF") {

    SetBuilderList0();
    hadronPhys.push_back( new G4ChargeExchangePhysics(verboseLevel,false));
    hadronPhys.push_back( new G4HadronInelasticQBBC("QBBC",verboseLevel,
						    false,true,false,false,false));

  } else if (name == "QBBC_DEL") {

    SetBuilderList5();
    hadronPhys.push_back( new G4HadronInelasticQBBC("QBBC",verboseLevel,
						    false,false,false,false,true));

  } else if (name == "QBBC_HEL") {

    SetBuilderList6();
    hadronPhys.push_back( new G4HadronInelasticQBBC("QBBC",verboseLevel,
						    false,false,false,false,true));

  } else if (name == "QBBC_HP") {

    SetBuilderList0(true);
    hadronPhys.push_back( new G4HadronInelasticQBBC("QBBC",verboseLevel,
						    false,false,false,true,true));
  }/* else if (name == "QGSC") {

    SetBuilderList4();
    hadronPhys.push_back( new HadronPhysicsQGSC("hadron",true));
    dump = true;

  }*/ else if (name == "QGSC_BERT") {

    SetBuilderList4();
    hadronPhys.push_back( new HadronPhysicsQGSC_BERT("hadron",true));
    dump = true;

  }/* else if (name == "QGSC_EFLOW") {

    SetBuilderList4();
    hadronPhys.push_back( new HadronPhysicsQGSC_EFLOW("hadron",true));
    dump = true;

  }*/ else if (name == "QGSC_EMV") {

    AddPhysicsList("emstandard_opt1");
    AddPhysicsList("QGSC");

  } else if (name == "QGSP") {

    SetBuilderList1();
    hadronPhys.push_back( new HadronPhysicsQGSP("hadron",true));
    dump = true;

  } else if (name == "QGSP_BERT") {

    SetBuilderList1();
    hadronPhys.push_back( new HadronPhysicsQGSP_BERT("hadron",true));
    dump = true;

  } else if (name == "QGSP_BERT_EMV") {

    AddPhysicsList("emstandard_opt1");
    AddPhysicsList("QGSP_BERT");

  } else if (name == "QGSP_BERT_HP") {

    SetBuilderList1(true);
    hadronPhys.push_back( new HadronPhysicsQGSP_BERT_HP("hadron",true));

  } else if (name == "QGSP_BIC") {

    SetBuilderList0();
    hadronPhys.push_back( new HadronPhysicsQGSP_BIC("hadron",true));
    dump = true;

  } else if (name == "QGSP_BIC_HP") {

    SetBuilderList0(true);
    hadronPhys.push_back( new HadronPhysicsQGSP_BIC_HP("hadron",true));
    dump = true;

  } else if (name == "QGSP_DIF") {

    SetBuilderList1();
    HadronPhysicsQGSP * hp = new HadronPhysicsQGSP("hadron");
    hp->SetQuasiElastic(true);
    hp->SetProjectileDiffraction(true);
    hadronPhys.push_back(hp);
    dump = true;

  } else if (name == "QGSP_EMV") {

    AddPhysicsList("emstandard_opt1");
    AddPhysicsList("QGSP");
    dump = true;

  } else if (name == "QGSP_EMX") {

    AddPhysicsList("emstandard_opt2");
    AddPhysicsList("QGSP");
    dump = true;

  } else if (name == "QGSP_NQE") {

    SetBuilderList1();
    hadronPhys.push_back( new HadronPhysicsQGSP("hadron",false));
    dump = true;

  } else if (name == "QGSP_QEL") {

    SetBuilderList4();
    hadronPhys.push_back( new HadronPhysicsQGSP("hadron",true));
    dump = true;

  } else {

    G4cout << "PhysicsList::AddPhysicsList: <" << name << ">"
           << " is not defined"
           << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void HadPhysicsList::SetBuilderList0(G4bool flagHP)
{
  hadronPhys.push_back( new G4EmExtraPhysics("extra EM"));
  hadronPhys.push_back( new G4HadronElasticPhysics("elastic",verboseLevel,flagHP));
  hadronPhys.push_back( new G4StoppingPhysics("stopping",verboseLevel));
  hadronPhys.push_back( new G4IonBinaryCascadePhysics("ionBIC"));
  hadronPhys.push_back( new G4NeutronTrackingCut("nTackingCut",verboseLevel));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void HadPhysicsList::SetBuilderList1(G4bool flagHP)
{
  hadronPhys.push_back( new G4EmExtraPhysics("extra EM"));
  hadronPhys.push_back( new G4HadronElasticPhysics("elastic",verboseLevel,flagHP));
  hadronPhys.push_back( new G4StoppingPhysics("stopping",verboseLevel));
  hadronPhys.push_back( new G4IonPhysics("ion"));
  hadronPhys.push_back( new G4NeutronTrackingCut("nTackingCut",verboseLevel));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void HadPhysicsList::SetBuilderList2(G4bool flagHP)
{
  hadronPhys.push_back( new G4EmExtraPhysics("extra EM"));
  hadronPhys.push_back( new G4HadronElasticPhysics("LElastic",verboseLevel,flagHP));
  hadronPhys.push_back( new G4IonPhysics("ion"));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void HadPhysicsList::SetBuilderList3(G4bool flagHP)
{
  hadronPhys.push_back( new G4EmExtraPhysics("extra EM"));
  //hadronPhys.push_back( new G4HadronElasticPhysics("LElastic",verboseLevel,
						    //flagHP));
  hadronPhys.push_back( new G4StoppingPhysics("stopping",verboseLevel));
  hadronPhys.push_back( new G4IonPhysics("ion"));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void HadPhysicsList::SetBuilderList4(G4bool)
{
  hadronPhys.push_back( new G4EmExtraPhysics("extra EM"));
//  hadronPhys.push_back( new G4HadronQElasticPhysics("elastic",verboseLevel));
  hadronPhys.push_back( new G4StoppingPhysics("stopping",verboseLevel));
  hadronPhys.push_back( new G4IonPhysics("ion"));
  hadronPhys.push_back( new G4NeutronTrackingCut("nTackingCut",verboseLevel));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void HadPhysicsList::SetBuilderList5(G4bool flagHP)
{
  hadronPhys.push_back( new G4EmExtraPhysics("extra EM"));
  //hadronPhys.push_back( new G4HadronDElasticPhysics(verboseLevel,flagHP));
  hadronPhys.push_back( new G4StoppingPhysics("stopping",verboseLevel));
  hadronPhys.push_back( new G4IonBinaryCascadePhysics("ionBIC"));
  hadronPhys.push_back( new G4NeutronTrackingCut("nTackingCut",verboseLevel));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void HadPhysicsList::SetBuilderList6(G4bool flagHP)
{
  hadronPhys.push_back( new G4EmExtraPhysics("extra EM"));
  hadronPhys.push_back( new G4HadronHElasticPhysics(verboseLevel,flagHP));
  hadronPhys.push_back( new G4StoppingPhysics("stopping",verboseLevel));
  hadronPhys.push_back( new G4IonBinaryCascadePhysics("ionBIC"));
  hadronPhys.push_back( new G4NeutronTrackingCut("nTackingCut",verboseLevel));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void HadPhysicsList::SetCuts()
{

  if (verboseLevel >0){
    G4cout << "PhysicsList::SetCuts:";
    G4cout << "CutLength : " << G4BestUnit(defaultCutValue,"Length") << G4endl;
  }

  // set cut values for gamma at first and for e- second and next for e+,
  // because some processes for e+/e- need cut values for gamma
  SetCutValue(cutForGamma, "gamma");
  SetCutValue(cutForElectron, "e-");
  SetCutValue(cutForPositron, "e+");

  if (verboseLevel>0) DumpCutValuesTable();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void HadPhysicsList::SetCutForGamma(G4double cut)
{
  cutForGamma = cut;
  SetParticleCuts(cutForGamma, G4Gamma::Gamma());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HadPhysicsList::SetCutForElectron(G4double cut)
{
  cutForElectron = cut;
  SetParticleCuts(cutForElectron, G4Electron::Electron());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HadPhysicsList::SetCutForPositron(G4double cut)
{
  cutForPositron = cut;
  SetParticleCuts(cutForPositron, G4Positron::Positron());
}

void HadPhysicsList::List()
{
  G4cout << "### PhysicsLists available: FTFC FTFP FTFP_BERT FTFP_EMV FTF_BIC LHEP LHEP_BERT LHEP_EMV"
	 << G4endl;
  G4cout << "                            LHEP_PRECO_HP QBBC QBBC_DEL QBBC_HEL QBBC_HP QGSC "
	 << G4endl;
  G4cout << "                            QGSC_BERT QGSC_EFLOW QGSC_EMV QGSP QGSP_BERT QGSP_BER_EMV "
	 << G4endl;
  G4cout << "                            QGSP_BERT_HP QGSP_BIC QGSP_BIC_HP QGSP_DIF "
	 << G4endl;
  G4cout << "                            QGSP_EMV QGSP_EMX QGSP_NQE QGSP_QEL  "
	 << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

