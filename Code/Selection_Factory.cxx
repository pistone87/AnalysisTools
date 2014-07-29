#include "Selection_Factory.h"

#include "Example.h"
#include "TauSpinExample.h"
#ifdef USE_cherepanov
#include "cherepanov/Validation.h"
#include "cherepanov/Ztotautau_hadmu_ControlSample.h"
#include "cherepanov/Tau_momentum_calculation.h"
#endif
#ifdef USE_inugent
#include "inugent/Ztotautau_ControlSample.h"
#include "inugent/ChargedHiggs_dilepontic.h"
#include "inugent/ChargedHiggs_tauplusjet.h"
#include "inugent/ZDouble3prong.h"
#include "inugent/Ztomumu_ControlSample.h"
#include "inugent/TriggerStudy.h"
#include "inugent/TriggerStudyMC.h"
#include "inugent/TauLifeTime.h"
#endif
#ifdef USE_pistone
#include "pistone/LightChargedHiggs.h"
#endif
#ifdef USE_nehrkorn
#include "nehrkorn/ZtoEMu_Wjets.h"
#include "nehrkorn/ZtoEMu_QCD.h"
#include "nehrkorn/ZtoEMu_Fakerate.h"
#include "nehrkorn/ZtoEMu_Skim.h"
#include "nehrkorn/ZtoEMu.h"
#endif
#ifdef USE_kargoll
#include "kargoll/HToTaumuTauh.h"
#include "kargoll/HToTaumuTauhSkim.h"
#include "kargoll/HToTaumuTauhBackgrounds.h"
#include "kargoll/MuTauSync.h"
#include "kargoll/OneJetBoost.h"
#include "kargoll/OneJetHigh.h"
#include "kargoll/OneJetLow.h"
#include "kargoll/VBFTight.h"
#include "kargoll/VBFLoose.h"
#include "kargoll/ZeroJetHigh.h"
#include "kargoll/ZeroJetLow.h"
#include "kargoll/Inclusive.h"
#endif
#ifdef USE_pistone

#endif

Selection_Factory::Selection_Factory(){
}

Selection_Factory::~Selection_Factory(){
}

Selection_Base* Selection_Factory::Factory(TString Analysis, TString UncertType,int mode, int runtype, double lumi){
  Selection_Base* s;
  Analysis.ToLower();

  // ensuring code will compile independently of user code
  // WARNING: be aware of the consequences of "Contains". Make sure that Class "foo" is put after "foobar".
  if(Analysis.Contains("example"))s=new Example(Analysis,UncertType);
  else if(Analysis.Contains("tauspinexample"))s=new TauSpinExample(Analysis,UncertType);
#ifdef USE_cherepanov
  else if(Analysis.Contains("validation"))s=new Validation(Analysis,UncertType);
  else if(Analysis.Contains("ztotautau_hadmu_controlsample"))s=new Ztotautau_hadmu_ControlSample(Analysis,UncertType);
  else if(Analysis.Contains("tau_momentum_calculation"))s=new Tau_momentum_calculation(Analysis,UncertType);
#endif
#ifdef USE_inugent
  else if(Analysis.Contains("ztotautau_controlsample"))s=new Ztotautau_ControlSample(Analysis,UncertType);
  else if(Analysis.Contains("chargedhiggs_dilepton"))s=new ChargedHiggs_dilepontic(Analysis,UncertType);
  else if(Analysis.Contains("chargedhiggs_tauplusjet"))s=new ChargedHiggs_tauplusjet(Analysis,UncertType);
  else if(Analysis.Contains("zdouble3prong"))s=new ZDouble3prong(Analysis,UncertType);
  else if(Analysis.Contains("ztomumu_controlsample"))s=new Ztomumu_ControlSample(Analysis,UncertType);
  else if(Analysis.Contains("triggerstudymc"))s=new TriggerStudyMC(Analysis,UncertType);
  else if(Analysis.Contains("triggerstudy"))s=new TriggerStudy(Analysis,UncertType);
  else if(Analysis.Contains("taulifetime"))s=new TauLifeTime(Analysis,UncertType);
#endif
#ifdef USE_pistone
  else if(Analysis.Contains("lightchargedhiggs"))s=new LightChargedHiggs(Analysis,UncertType);
#endif
#ifdef USE_nehrkorn
  else if(Analysis.Contains("ztoemu_wjets")) s=new ZtoEMu_Wjets(Analysis,UncertType);
  else if(Analysis.Contains("ztoemu_qcd")) s=new ZtoEMu_QCD(Analysis,UncertType);
  else if(Analysis.Contains("ztoemu_fakerate"))s=new ZtoEMu_Fakerate(Analysis,UncertType);
  else if(Analysis.Contains("ztoemu_skim"))s=new ZtoEMu_Skim(Analysis,UncertType);
  else if(Analysis.Contains("ztoemu"))s=new ZtoEMu(Analysis,UncertType);
#endif
#ifdef USE_kargoll
  else if(Analysis.Contains("htotaumutauhskim")) s=new HToTaumuTauhSkim(Analysis,UncertType);
  else if(Analysis.Contains("htotaumutauhbackgrounds")) s=new HToTaumuTauhBackgrounds(Analysis,UncertType);
  else if(Analysis.Contains("htotaumutauh")) s=new HToTaumuTauh(Analysis,UncertType);
  else if(Analysis.Contains("mutausync")) s=new MuTauSync(Analysis,UncertType);
  else if(Analysis.Contains("onejetboost")) s=new OneJetBoost(Analysis,UncertType);
  else if(Analysis.Contains("onejethigh")) s=new OneJetHigh(Analysis,UncertType);
  else if(Analysis.Contains("onejetlow")) s=new OneJetLow(Analysis,UncertType);
  else if(Analysis.Contains("vbftight")) s=new VBFTight(Analysis,UncertType);
  else if(Analysis.Contains("vbfloose")) s=new VBFLoose(Analysis,UncertType);
  else if(Analysis.Contains("zerojethigh")) s=new ZeroJetHigh(Analysis,UncertType);
  else if(Analysis.Contains("zerojetlow")) s=new ZeroJetLow(Analysis,UncertType);
  else if(Analysis.Contains("inclusive")) s=new Inclusive(Analysis,UncertType);
#endif
#ifdef USE_pistone

#endif
  else{
    std::cout << "WARNING: Selection_Factory::Factory INVALID ANALYSIS TYPE.... USING DEFAULT <Example.h> " << std::endl;
    s=new Example(Analysis,UncertType);
  }
  s->SetMode(mode);
  s->SetRunType(runtype);
  s->SetLumi(lumi);
  s->Configure();
  return s;
}

