#include "LightChargedHiggs.h"

#include <iostream>
#include <cmath>
#include <cstdlib>

#include "HistoConfig.h"
#include "TLorentzVector.h"

double pi = M_PI;

int ptNbins = 25;
int etaNbins = 20;

//******* LightChargedHiggs::LightChargedHiggs START
LightChargedHiggs::LightChargedHiggs(TString Name_, TString id_):
  Selection(Name_,id_)
  ,mu_pt(25.)
  ,mu_eta(2.1)
  ,mu_relIso(0.12)
  ,tau_pt(20.)
  ,tau_eta(2.4)
  ,jet_pt(20.)  //very loose pt cut; pt=35. loose, pt=45 tight
  ,jet_eta(2.5)
  ,jetClean_dR(0.5)
  ,bjetDiscr(0.679) //medium WP, https://twiki.cern.ch/twiki/bin/viewauth/CMS/BTagPerformanceOP#B_tagging_Operating_Points_for_5
{
}
//******* LightChargedHiggs::LightChargedHiggs END



//******* LightChargedHiggs::~LightChargedHiggs START
LightChargedHiggs::~LightChargedHiggs(){
  for(int j=0; j<Npassed.size(); j++){
    std::cout << "LightChargedHiggs::~LightChargedHiggs Selection Summary before: " 
      << Npassed.at(j).GetBinContent(1)     << " +/- " << Npassed.at(j).GetBinError(1)     << " after: "
      << Npassed.at(j).GetBinContent(NCuts) << " +/- " << Npassed.at(j).GetBinError(NCuts) << std::endl;
  }
  std::cout << "LightChargedHiggs::~LightChargedHiggs()" << std::endl;
}
//******* LightChargedHiggs::~LightChargedHiggs END



//******* LightChargedHiggs::Configure START
void  LightChargedHiggs::Configure(){
  // Setup Cut Values
  for(int i=0; i<NCuts; i++){
    cut.push_back(0);
    value.push_back(0);
    pass.push_back(false);

    //trigger
    if(i==TriggerOk)                cut.at(TriggerOk)=1;

    //primary vertex
    if(i==PrimeVtx)                 cut.at(PrimeVtx)=1;

    //muons
    //if(i==NMu)                    cut.at(NMu)=1;
    if(i==NTightMu)                 cut.at(NTightMu)=1;
    if(i==NTightMuIso)              cut.at(NTightMuIso)=1;

    //taus
    if(i==NTau)                     cut.at(NTau)=1;
    if(i==NTau3Prong)               cut.at(NTau3Prong)=1;
    if(i==NTauClean)                cut.at(NTauClean)=1;
    if(i==NTauMediumIso)            cut.at(NTauMediumIso)=1;

    //jets
    if(i==NJetsClean)               cut.at(NJetsClean)=2;
    if(i==NJetsID)                  cut.at(NJetsID)=2;
    if(i==NJets)                    cut.at(NJets)=2;
    if(i==NBJets)                   cut.at(NBJets)=2;
  }



  TString hlabel;
  TString htitle;
  //for i<NCuts start
  for(unsigned int i=0; i<NCuts; i++){
    title.push_back("");
    distindx.push_back(false);
    dist.push_back(std::vector<float>());
    TString c="_Cut_";
    c+=i;
  
    // PrimeVtx
    if(i==PrimeVtx){
      title.at(i)="Number of Prime Vertices $(N>$";
      title.at(i)+=cut.at(PrimeVtx);
      title.at(i)+=")";
      htitle=title.at(i);
      htitle.ReplaceAll("$","");
      htitle.ReplaceAll("\\","#");
      hlabel="Number of Prime Vertices";
      Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_PrimeVtx_",htitle,31,-0.5,30.5,hlabel,"Events"));
      Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_PrimeVtx_",htitle,31,-0.5,30.5,hlabel,"Events"));
    } //if(i==PrimeVtx)


    // TriggerOk
    else if(i==TriggerOk){
      title.at(i)="Trigger ";
      hlabel="Trigger ";
      Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_TriggerOk_",htitle,2,-0.5,1.5,hlabel,"Events"));
      Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_TriggerOk_",htitle,2,-0.5,1.5,hlabel,"Events"));
    } //else if(i==TriggerOk)


    // NMu
/*
    else if(i==NMu){
      title.at(i)="Number of $\\mu >=$";
      title.at(i)+=cut.at(NMu);
      htitle=title.at(i);
      htitle.ReplaceAll("$","");
      htitle.ReplaceAll("\\","#");
      hlabel="Number of #mu";
      Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_NMu_",htitle,6,-0.5,5.5,hlabel,"Events"));
      Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_NMu_",htitle,6,-0.5,5.5,hlabel,"Events"));
      }
*/

    // NTightMu
    else if(i==NTightMu){
      title.at(i)="Number of tight $\\mu >=$";
      title.at(i)+=cut.at(NTightMu);
      htitle=title.at(i);
      htitle.ReplaceAll("$","");
      htitle.ReplaceAll("\\","#");
      hlabel="Number of tight #mu (no iso)";
      Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_NTightMu_",htitle,6,-0.5,5.5,hlabel,"Events"));
      Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_NTightMu_",htitle,6,-0.5,5.5,hlabel,"Events"));
      }


    // NTightMuIso
    else if(i==NTightMuIso){
      title.at(i)="Number of tight isolated $\\mu >=$";
      title.at(i)+=cut.at(NTightMuIso);
      htitle=title.at(i);
      htitle.ReplaceAll("$","");
      htitle.ReplaceAll("\\","#");
      hlabel="Number of tight isolated #mu";
      Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_NTightMuIso_",htitle,6,-0.5,5.5,hlabel,"Events"));
      Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_NTightMuIso_",htitle,6,-0.5,5.5,hlabel,"Events"));
      }


    // NTau
    else if(i==NTau){
      title.at(i)="Number of $\\tau >=$";
      title.at(i)+=cut.at(NTau);
      htitle=title.at(i);
      htitle.ReplaceAll("$","");
      htitle.ReplaceAll("\\","#");
      hlabel="Number of #tau";
      Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_NTau_",htitle,6,-0.5,5.5,hlabel,"Events"));
      Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_NTau_",htitle,6,-0.5,5.5,hlabel,"Events"));
      }

    // NTau3Prong
    else if(i==NTau3Prong){
      title.at(i)="Number of $\\tau$-3-prong $>=$";
      title.at(i)+=cut.at(NTau3Prong);
      htitle=title.at(i);
      htitle.ReplaceAll("$","");
      htitle.ReplaceAll("\\","#");
      hlabel="Number of #tau-3-prong";
      Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_NTau3Prong_",htitle,6,-0.5,5.5,hlabel,"Events"));
      Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_NTau3Prong_",htitle,6,-0.5,5.5,hlabel,"Events"));
      }

    // NTauClean
    else if(i==NTauClean){
      title.at(i)="Number of $\\tau$ cleaned against $\\mu$ and $e$ $>=$";
      title.at(i)+=cut.at(NTauClean);
      htitle=title.at(i);
      htitle.ReplaceAll("$","");
      htitle.ReplaceAll("\\","#");
      hlabel="Number of #tau (against #mu and e)";
      Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_NTauClean_",htitle,6,-0.5,5.5,hlabel,"Events"));
      Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_NTauClean_",htitle,6,-0.5,5.5,hlabel,"Events"));
      }

    // NTauMediumIso
    else if(i==NTauMediumIso){
      title.at(i)="Number of medium isolated $\\tau >=$";
      title.at(i)+=cut.at(NTauMediumIso);
      htitle=title.at(i);
      htitle.ReplaceAll("$","");
      htitle.ReplaceAll("\\","#");
      hlabel="Number of isolated #tau";
      Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_NTauMediumIso_",htitle,6,-0.5,5.5,hlabel,"Events"));
      Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_NTauMediumIso_",htitle,6,-0.5,5.5,hlabel,"Events"));
      }


    // NJetsClean 
    else if(i==NJetsClean){
      title.at(i)="Number of jets cleaned against $\\mu$ and $\\tau$ $>=$";
      title.at(i)+=cut.at(NJetsClean);
      htitle=title.at(i);
      htitle.ReplaceAll("$","");
      htitle.ReplaceAll("\\","#");
      hlabel="Number of jets cleaned against #mu and #tau";
      Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_NJetsClean_",htitle,11,-0.5,10.5,hlabel,"Events"));
      Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_NJetsClean_",htitle,11,-0.5,10.5,hlabel,"Events"));
      }

    // NJetsID
    else if(i==NJetsID){
      title.at(i)="Number of jets (passed ID) $>=$";
      title.at(i)+=cut.at(NJetsID);
      htitle=title.at(i);
      htitle.ReplaceAll("$","");
      htitle.ReplaceAll("\\","#");
      hlabel="Number of jets (passed ID)";
      Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_NJetsID_",htitle,11,-0.5,10.5,hlabel,"Events"));
      Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_NJetsID_",htitle,11,-0.5,10.5,hlabel,"Events"));
      }

    // NJets
    else if(i==NJets){
      title.at(i)="Number of jets $>=$";
      title.at(i)+=cut.at(NJets);
      htitle=title.at(i);
      htitle.ReplaceAll("$","");
      htitle.ReplaceAll("\\","#");
      hlabel="Number of jets";
      Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_NJets_",htitle,11,-0.5,10.5,hlabel,"Events"));
      Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_NJets_",htitle,11,-0.5,10.5,hlabel,"Events"));
      }

    // NBJets
    else if(i==NBJets){
      title.at(i)="Number of b-jets $>=$";
      title.at(i)+=cut.at(NBJets);
      htitle=title.at(i);
      htitle.ReplaceAll("$","");
      htitle.ReplaceAll("\\","#");
      hlabel="Number of b-jets";
      Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_NBJets_",htitle,11,-0.5,10.5,hlabel,"Events"));
      Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_NBJets_",htitle,11,-0.5,10.5,hlabel,"Events"));
      }
  } //for i<NCuts end

 

  // Setup NPassed Histogams
  Npassed=HConfig.GetTH1D(Name+"_NPass","Cut Flow",NCuts+1,-1,NCuts,"Number of Accumulative Cuts Passed","Events");

  //
  // Setup Extra Histograms
  //

  // vertices
  NVtx=HConfig.GetTH1D(Name+"_NVtx","NVtx",26,-0.5,25.5,"Number of Accumulative Cuts Passed","Events");
  NGoodVtx=HConfig.GetTH1D(Name+"_NGoodVtx","NGoodVtx",26,-0.05,25.5,"Number of Vertex","Events");
  NTrackperVtx=HConfig.GetTH1D(Name+"_NTracksperVtx","NTracksperVtx",151,-0.5,150.5,"Number of Track per Vertex","Events");


  // muons
  // before cuts
  //muonPt=HConfig.GetTH1D(Name+"_muonPt","muonPt",40,0.,250.,"p_{T}^{#mu} / GeV");
  //muonEta=HConfig.GetTH1D(Name+"_muonEta","muonEta",20,-3.0,3.0,"#eta_{#mu}");

  // after selection
  goodMuons=HConfig.GetTH1D(Name+"_goodMuons_","goodMuons",6,-0.5,5.5,"Number of tight muons");
  goodMuonPt=HConfig.GetTH1D(Name+"_goodMuonPt_","goodMuonPt",25,0.,250.,"p_{T}^{#mu} / GeV");
  goodMuonEta=HConfig.GetTH1D(Name+"_goodMuonEta_","goodMuonEta",20,-3.0,3.0,"#eta_{#mu}");

  // transverse mass mt of muon and missing transverse energy
  transMass=HConfig.GetTH1D(Name+"_transMass_","transMass",40,0.,250.,"m_{T}(#mu, MET) / GeV");


  // taus
  // before cuts
  //tauPt=HConfig.GetTH1D(Name+"_tauPt","tauPt",40,0.,250.,"p_{T}^{#tau} / GeV");
  //tauEta=HConfig.GetTH1D(Name+"_tauEta","tauEta",20,-3.0,3.0,"#eta_{#tau}");
  //tauPhi=HConfig.GetTH1D(Name+"_tauPhi","tauPhi",20,-pi,pi,"#phi_{#tau}"); 

  // after selection
  goodTaus=HConfig.GetTH1D(Name+"_goodTaus_","goodTaus",6,-0.5,5.5,"Number of taus (3-prong)");
  goodTauPt=HConfig.GetTH1D(Name+"_goodTauPt_","goodTauPt",25,0.,250.,"p_{T}^{#tau} / GeV");
  goodTauEta=HConfig.GetTH1D(Name+"_goodTauEta_","goodTauEta",20,-3.0,3.0,"#eta_{#tau}");
  goodTauPhi=HConfig.GetTH1D(Name+"_goodTauPhi_","goodTauPhi",20,-pi,pi,"#phi_{#tau}");


  // jets
  // before cuts
  // jet1st
  //jet1stPt=HConfig.GetTH1D(Name+"_jet1stPt","jet1stPt",40,0.,250.,"p_{T}^{1st,jet} / GeV");
  //jet1stEta=HConfig.GetTH1D(Name+"_jet1stEta","jet1stEta",20,-3.0,3.0,"#eta_{1st,jet}");
  //jet1stMass=HConfig.GetTH1D(Name+"_jet1stMass","jet1stMass",40,0.,120.,"m_{1st,jet} / GeV");

  // jet2nd
  //jet2ndPt=HConfig.GetTH1D(Name+"_jet2ndPt","jet2ndPt",40,0.,250.,"p_{T}^{2nd,jet} / GeV");
  //jet2ndEta=HConfig.GetTH1D(Name+"_jet2ndEta","jet2ndEta",20,-3.0,3.0,"#eta_{2nd,jet}");
  //jet2ndMass=HConfig.GetTH1D(Name+"_jet2ndMass","jet2ndMass",40,0.,120.,"m_{2nd,jet} / GeV");

  // after selection
  goodJets=HConfig.GetTH1D(Name+"_goodJets_","goodJets",11,-0.5,10.5,"Number of loose jets");

  // jet1st
  goodJet1stPt=HConfig.GetTH1D(Name+"_goodJet1stPt_","goodJet1stPt",25,0.,250.,"p_{T}^{1st,jet} / GeV");
  goodJet1stEta=HConfig.GetTH1D(Name+"_goodJet1stEta_","goodJet1stEta",20,-3.0,3.0,"#eta_{1st,jet}");
  goodJet1stMass=HConfig.GetTH1D(Name+"_goodJet1stMass_","goodJet1stMass",40,0.,120.,"m_{1st,jet} / GeV");

  // jet2nd
  goodJet2ndPt=HConfig.GetTH1D(Name+"_goodJet2ndPt_","goodJet2ndPt",25,0.,250.,"p_{T}^{2nd,jet} / GeV");
  goodJet2ndEta=HConfig.GetTH1D(Name+"_goodJet2ndEta_","goodJet2ndEta",20,-3.0,3.0,"#eta_{2nd,jet}");
  goodJet2ndMass=HConfig.GetTH1D(Name+"_goodJet2ndMass_","goodJet2ndMass",40,0.,120.,"m_{2nd,jet} / GeV");


  // MET
  METEt=HConfig.GetTH1D(Name+"_METEt","METEt_",40,0.,250.,"MET / GeV");
  METPhi=HConfig.GetTH1D(Name+"_METPhi","METPhi_",20,-pi,pi,"#phi_{MET} / GeV");

  

  Selection::ConfigureHistograms();
  HConfig.GetHistoInfo(types,CrossSectionandAcceptance,legend,colour);
}
//******* LightChargedHiggs::Configure END



//******* LightChargedHiggs::Store_ExtraDist START
void LightChargedHiggs::Store_ExtraDist(){

  // vertices
  Extradist1d.push_back(&NVtx);
  Extradist1d.push_back(&NGoodVtx);
  Extradist1d.push_back(&NTrackperVtx);


  // muon observables
  // before cuts
  //Extradist1d.push_back(&muonPt);  
  //Extradist1d.push_back(&muonEta);

  // after selection
  Extradist1d.push_back(&goodMuons);
  Extradist1d.push_back(&goodMuonPt);
  Extradist1d.push_back(&goodMuonEta);
  
  //transverse mass(mu, MET)
  Extradist1d.push_back(&transMass);
 
 
  // tau observables
  // before cuts
  //Extradist1d.push_back(&tauPt);
  //Extradist1d.push_back(&tauEta);
  //Extradist1d.push_back(&tauPhi);

  // after selection
  Extradist1d.push_back(&goodTaus);
  Extradist1d.push_back(&goodTauPt);
  Extradist1d.push_back(&goodTauEta);
  Extradist1d.push_back(&goodTauPhi);


  // jet observables
  // before cuts
  // jet1st
  //Extradist1d.push_back(&jet1stPt);
  //Extradist1d.push_back(&jet1stEta);
  //Extradist1d.push_back(&jet1stMass);

  // jet2nd
  //Extradist1d.push_back(&jet2ndPt);
  //Extradist1d.push_back(&jet2ndEta);
  //Extradist1d.push_back(&jet2ndMass);

  // after selection
  Extradist1d.push_back(&goodJets);

  // jet1st
  Extradist1d.push_back(&goodJet1stPt);
  Extradist1d.push_back(&goodJet1stEta);
  Extradist1d.push_back(&goodJet1stMass);

  //jet2nd
  Extradist1d.push_back(&goodJet2ndPt);
  Extradist1d.push_back(&goodJet2ndEta);
  Extradist1d.push_back(&goodJet2ndMass);


  // MET observables
  Extradist1d.push_back(&METEt);
  Extradist1d.push_back(&METPhi);


}
//******* LightChargedHiggs::Store_ExtraDist END



//******* LightChargedHiggs::doEvent START
void  LightChargedHiggs::doEvent(){
  if(verbose) std::cout << " LightChargedHiggs::doEvent() START " << std::endl;

  unsigned int t;
  int id(Ntp->GetMCID());
  if(!HConfig.GetHisto(Ntp->isData(), id, t)){ std::cout << " failed to find id " <<std::endl; return; }
  


  //////////////////////////
  //                      //
  //    Apply Selection   //
  //                      //
  //////////////////////////


  //
  // number of good vertices
  //

  if(verbose) std::cout << " vertex selection " << std::endl;
  unsigned int nGoodVtx=0;
  int vertex = -1;
  for(unsigned int i=0; i<Ntp->NVtx(); i++){
    if(Ntp->isGoodVtx(i)){
      if(vertex==-1) vertex=i;
      nGoodVtx++;
    }
  }
  if(verbose) std::cout << " selected vertex: " << vertex << std::endl;
  value.at(PrimeVtx)=nGoodVtx;
  pass.at(PrimeVtx)=(value.at(PrimeVtx)>=cut.at(PrimeVtx));
  

  //
  // trigger
  //

  // cross trigger muon+tau:
  // HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v
  // HLT_IsoMu18_eta2p1_LooseIsoPFTau20_v 

  if(verbose) std::cout << " trigger " << std::endl;

  value.at(TriggerOk)=0;
  if(Ntp->TriggerAccept("eta2p1_LooseIsoPFTau")){  
    value.at(TriggerOk)=1;
  }
  pass.at(TriggerOk)= (value.at(TriggerOk)==cut.at(TriggerOk));


  //
  // muon cuts
  //
  
  // GoodMuonsIdx: vector of indices of muons that pass the tight muon selection
  // muon1st: index of the muon with the highest pt
  // muonCandidate: TLorentzVector of the muon1st

  if(verbose) std::cout << " muon cuts " << std::endl;
  std::vector<unsigned int> TightMuonsIdx;
  std::vector<unsigned int> GoodMuonsIdx;

  // number of good muons, i.e. muons that pass the tight muon selection
/*
  for(unsigned int i=0; i<Ntp->NMuons(); i++){
    if(Ntp->isTightMuon(i, vertex)
       && Ntp->Muon_p4(i).Pt()>mu_pt
       && fabs(Ntp->Muon_p4(i).Eta())<mu_eta
       && Ntp->Muon_RelIso(i)<mu_relIso
       ){
      GoodMuonsIdx.push_back(i);
    } //if
  } //for

  value.at(NMu)=GoodMuonsIdx.size();
  pass.at(NMu)=(value.at(NMu)>=cut.at(NMu));
*/

  // number of tight muons without requirements on the isolation
  for(unsigned int i=0; i<Ntp->NMuons(); i++){
    if(Ntp->isTightMuon(i, vertex)
       && Ntp->Muon_p4(i).Pt()>mu_pt
       && fabs(Ntp->Muon_p4(i).Eta())<mu_eta
       ){
      TightMuonsIdx.push_back(i);
    } //if
  } //for

  value.at(NTightMu)=TightMuonsIdx.size();
  pass.at(NTightMu)=(value.at(NTightMu)>=cut.at(NTightMu));

  // number of tight isolated muons
  for(unsigned int i=0; i<TightMuonsIdx.size(); i++){
    unsigned int j = TightMuonsIdx.at(i);
    if(Ntp->Muon_RelIso(j)<mu_relIso){
      GoodMuonsIdx.push_back(j);
    } //if
  } //for

  value.at(NTightMuIso)=GoodMuonsIdx.size();
  pass.at(NTightMuIso)=(value.at(NTightMuIso)>=cut.at(NTightMuIso));


  // take the muon1st
  unsigned int muon1st(999);
  TLorentzVector muonCandidate;
  double tempMuPt(0);
  double tempMu1stPt(0);
  for(unsigned int i=0; i<GoodMuonsIdx.size(); i++){
    unsigned int j = GoodMuonsIdx.at(i);
    tempMuPt = Ntp->Muon_p4(j).Pt();
    if(tempMuPt > tempMu1stPt){
      muon1st = j;
      tempMu1stPt = tempMuPt;
    } //if
  } //for

  if(muon1st!=999){
    muonCandidate = Ntp->Muon_p4(muon1st);
  } //if


  //
  // tau cuts
  //

  // GoodTauIdx: vector of indices of taus that pass the tau selection + 3-prong requirement
  // tau1st: index of the tau with the highest pt
  // tauCandidate: TLorentzVector of the tau1st

  if(verbose) std::cout << " tau cuts " << std::endl;
  std::vector<unsigned int> TausIdx;
  std::vector<unsigned int> Taus3ProngIdx;
  std::vector<unsigned int> TausCleanIdx;
  std::vector<unsigned int> GoodTausIdx;

  // number of good taus, i.e. taus that pass the tau selection + 3-prong requirement
/*
  for(unsigned int i=0; i<Ntp->NPFTaus(); i++){
    if(Ntp->PFTau_p4(i).Pt()>tau_pt
       && fabs(Ntp->PFTau_p4(i).Eta())<tau_eta
       && Ntp->PFTau_hpsDecayMode(i)==10   // DecayMode = 10 => 3-prong decay
       && Ntp->PFTau_isHPSByDecayModeFinding(i)
       && Ntp->PFTau_isHPSAgainstMuonTight(i)
       && Ntp->PFTau_isHPSAgainstElectronsTight(i) 
       && Ntp->PFTau_HPSPFTauDiscriminationByMediumCombinedIsolationDBSumPtCorr3Hits(i) // it's a boolean variable
       ){
      GoodTausIdx.push_back(i);
    } //if
  } //for

  value.at(NTau)=GoodTausIdx.size();
  pass.at(NTau)=(value.at(NTau)>=cut.at(NTau));
*/

  // number of taus with pt > 20.GeV and |eta| < 2.4
  for(unsigned int i=0; i<Ntp->NPFTaus(); i++){
    if(Ntp->PFTau_p4(i).Pt()>tau_pt
       && fabs(Ntp->PFTau_p4(i).Eta())<tau_eta
       ){
      TausIdx.push_back(i);
    } //if
  } //for

  value.at(NTau)=TausIdx.size();
  pass.at(NTau)=(value.at(NTau)>=cut.at(NTau));

  // number of tau-3-prong candidates
  for(unsigned int i=0; i<TausIdx.size(); i++){
    unsigned int j = TausIdx.at(i);
    if(Ntp->PFTau_hpsDecayMode(j)==10   // DecayMode = 10 => 3-prong decay
       && Ntp->PFTau_isHPSByDecayModeFinding(j)
       ){
      Taus3ProngIdx.push_back(j);
    } //if
  } //for

  value.at(NTau3Prong)=Taus3ProngIdx.size();
  pass.at(NTau3Prong)=(value.at(NTau3Prong)>=cut.at(NTau3Prong));

  // number of tau cleaned against muons and electrons
  for(unsigned int i=0; i<Taus3ProngIdx.size(); i++){
    unsigned int j = Taus3ProngIdx.at(i);
    if(Ntp->PFTau_isHPSAgainstMuonTight(j)
       && Ntp->PFTau_isHPSAgainstElectronsTight(j) 
       ){
      TausCleanIdx.push_back(j);
    } //if
  } //for

  value.at(NTauClean)=TausCleanIdx.size();
  pass.at(NTauClean)=(value.at(NTauClean)>=cut.at(NTauClean));

  // number of good taus
  for(unsigned int i=0; i<TausCleanIdx.size(); i++){
    unsigned int j = TausCleanIdx.at(i);
    if(Ntp->PFTau_HPSPFTauDiscriminationByMediumCombinedIsolationDBSumPtCorr3Hits(j) // it's a boolean variable
       ){
      GoodTausIdx.push_back(j);
    } //if
  } //for

  value.at(NTauMediumIso)=GoodTausIdx.size();
  pass.at(NTauMediumIso)=(value.at(NTauMediumIso)>=cut.at(NTauMediumIso));

  // take the tau1st
  unsigned int tau1st(999);
  TLorentzVector tauCandidate;
  double tempTauPt(0);
  double tempTau1stPt(0);
  for(unsigned int i=0; i<GoodTausIdx.size(); i++){
    unsigned int j = GoodTausIdx.at(i);
    tempTauPt = Ntp->PFTau_p4(j).Pt();
    if(tempTauPt > tempTau1stPt){
      tau1st = j;
      tempTau1stPt = tempTauPt;
    } //if
  } //for

  if(tau1st!=999){
    tauCandidate = Ntp->PFTau_p4(tau1st);
  } //if


  //
  // jet cuts
  // 
 
  // GoodJetsIdx: vector of indices of jets that pass the loose jet selection
  // jet1st: index of the jet with the highest pt
  // jet2nd: index of the jet with the second highest pt
  // jet1stCandidate: TLorentzVector for the jet1st
  // jet2ndCandidate: TLorentzVector for the jet2nd 

  if(verbose) std::cout << " jet cuts " << std::endl;
  std::vector<unsigned int> JetsCleanIdx;
  std::vector<unsigned int> JetsIDIdx;
  std::vector<unsigned int> JetsIdx;
  std::vector<unsigned int> GoodJetsIdx;

  // number of jets cleaned against muonCandidate and tauCandidate
  for(unsigned int i=0; i<Ntp->NPFJets(); i++){
    if(JetCleaning(i, muon1st, tau1st)){
      JetsCleanIdx.push_back(i);
    } //if
  } //for

  value.at(NJetsClean)=JetsCleanIdx.size();
  pass.at(NJetsClean)=(value.at(NJetsClean)>=cut.at(NJetsClean));

  // number of jets that pass the jet id selection
  for(unsigned int i=0; i<JetsCleanIdx.size(); i++){
    unsigned int j = JetsCleanIdx.at(i);
    if(Ntp->isJetID(j)){
      JetsIDIdx.push_back(j);
    } //if
  } //for

  value.at(NJetsID)=JetsIDIdx.size();
  pass.at(NJetsID)=(value.at(NJetsID)>=cut.at(NJetsID));

  // number of jets that pass the cuts on pt and eta (loose jet selection)
  for(unsigned int i=0; i<JetsIDIdx.size(); i++){
    unsigned int j = JetsIDIdx.at(i);
    if(Ntp->PFJet_p4(j).Pt()>jet_pt
       && fabs(Ntp->PFJet_p4(j).Eta())<jet_eta
       ){
      JetsIdx.push_back(j);
    } //if
  } //for

  value.at(NJets)=JetsIdx.size();
  pass.at(NJets)=(value.at(NJets)>=cut.at(NJets));

  // number of good jets, i.e. jets that pass the loose jet selection AND the b-tagging
  for(unsigned int i=0; i<JetsIdx.size(); i++){
    unsigned int j = JetsIdx.at(i);
    if(Ntp->PFJet_bDiscriminator(j) > bjetDiscr){
      GoodJetsIdx.push_back(j);
    } //if
  } //for

  value.at(NBJets)=GoodJetsIdx.size();
  pass.at(NBJets)=(value.at(NBJets)>=cut.at(NBJets));



  // take the jet1st and jet2nd
  unsigned int jet1st(999);
  unsigned int jet2nd(999);
  TLorentzVector jet1stCandidate;
  TLorentzVector jet2ndCandidate;
  double tempJetPt(0);
  double tempJet1stPt(0);
  double tempJet2ndPt(0);
  for(unsigned int i=0; i<GoodJetsIdx.size(); i++){
    unsigned int j = GoodJetsIdx.at(i);
    tempJetPt = Ntp->PFJet_p4(j).Pt();
    if(tempJetPt > tempJet1stPt){
      jet1st = j;
      tempJet1stPt = tempJetPt;
    } //if jet1st

    else if(tempJetPt < tempJet1stPt && tempJetPt > tempJet2ndPt){
      jet2nd = GoodJetsIdx.at(i);
      tempJet2ndPt = tempJetPt;
    } //if jet2nd
  } //for

  if(jet1st!=999){
    jet1stCandidate = Ntp->PFJet_p4(jet1st);
  } //if jet1st!=999

  if(jet2nd!=999){
    jet2ndCandidate = Ntp->PFJet_p4(jet2nd);
  } //if jet2nd!=999



  // weight
  if(verbose) std::cout << " do weights " << std::endl;
  double wobs=1;
  double w=1;
  if(!Ntp->isData()){w = Ntp->PUWeight();}
  else{w=1;}



  ///////////////////////////////////////////////////////////

  // *** Add plots ***
  if(verbose) std::cout << " add plots " << std::endl;

/*
  // before cuts
  if(verbose) std::cout << " add plots before cuts " << std::endl;

  // Muons
  if(pass.at(PrimeVtx)
    && pass.at(TriggerOk)
    && pass.at(NTau)
    && pass.at(NJets)){
    
      muonPt.at(t).Fill(muonCandidate.Pt(),w);
      muonEta.at(t).Fill(muonCandidate.Eta(),w);

  } //if PrimeVtx, TriggerOk, NTau, NJets


  // Taus
  if(pass.at(PrimeVtx)
    && pass.at(TriggerOk)
    && pass.at(NMu)
    && pass.at(NJets)){
    
      tauPt.at(t).Fill(tauCandidate.Pt(),w);
      tauEta.at(t).Fill(tauCandidate.Eta(),w);
      tauPhi.at(t).Fill(tauCandidate.Phi(),w);
      //tauDR.at(t).Fill(tauCandidate.DeltaR(),w);  // TLorentzVector.DeltaR(TLorenzVector); FIX ME!

  } //if PrimeVtx, TriggerOk, NMu, NJets


  // Jets
  if(pass.at(PrimeVtx)
    && pass.at(TriggerOk)
    && pass.at(NMu)
    && pass.at(NTau)){
    
      jet1stPt.at(t).Fill(jet1stCandidate.Pt(),w);
      jet1stEta.at(t).Fill(jet1stCandidate.Eta(),w);
      jet1stMass.at(t).Fill(jet1stCandidate.M(),w);

      jet2ndPt.at(t).Fill(jet2ndCandidate.Pt(),w);
      jet2ndEta.at(t).Fill(jet2ndCandidate.Eta(),w);
      jet2ndMass.at(t).Fill(jet2ndCandidate.M(),w);

  } //if PrimeVtx, TriggerOk, NMu, NTau
*/


  // after the FULL selection
  if(verbose) std::cout << " add plots after the full selection " << std::endl;

  bool status=AnalysisCuts(t,w,wobs); //true only if FULL selection passed
  if(verbose) std::cout << " status: " << status << std::endl;

  if(status){

    // vertices
    NVtx.at(t).Fill(Ntp->NVtx(),w); //number of vertices
    unsigned int nGoodVtx=0;
    for(unsigned int i=0; i<Ntp->NVtx(); i++){
      NTrackperVtx.at(t).Fill(Ntp->Vtx_Track_idx(i).size(),w); //number of tracks per vertex
      if(Ntp->isVtxGood(i)) nGoodVtx++;
    }
    NGoodVtx.at(t).Fill(nGoodVtx,w); //number of good vertices

    
    // Muons
    // goodMuons: plot of number of muons that pass the tight muon selection in the events that pass the full selection
    // goodMuonPt/Eta: pt/eta distribution of the muon1st

    goodMuons.at(t).Fill(GoodMuonsIdx.size(),w);
    goodMuonPt.at(t).Fill(muonCandidate.Pt(),w);
    goodMuonEta.at(t).Fill(muonCandidate.Eta(),w);

    // Transverse mass of muon and MET
    double met(0.), deltaPhiMuMet(0.);
    double Mt(0.);
    met = Ntp->MET_CorrT0pcTxy_et();
    deltaPhiMuMet = muonCandidate.Phi() - Ntp->MET_CorrT0pcTxy_phi();
    Mt = sqrt( 2 * muonCandidate.Pt() * met * (1 - cos(deltaPhiMuMet)) );
    
    transMass.at(t).Fill(Mt,w);

    // Taus
    // goodTaus: plot of number of taus that pass the tau selection (3-prong candidates) in the events that pass the full selection
    // goodTauPt/Eta/Phi/DR: pt/eta/phi/deltaR distribution of the tau1st

    goodTaus.at(t).Fill(GoodTausIdx.size(),w);
    goodTauPt.at(t).Fill(tauCandidate.Pt(),w);
    goodTauEta.at(t).Fill(tauCandidate.Eta(),w);
    goodTauPhi.at(t).Fill(tauCandidate.Phi(),w);
    //goodTauDR.at(t).Fill(tauCandidate.DeltaR(),w); // TLorentzVector.DeltaR(TLorenzVector); FIX ME!

    // Jets
    //  goodJets: plot of number of jets that pass the loose jets selection in the events that pass the full selection
    goodJets.at(t).Fill(GoodJetsIdx.size(),w);

    // jet1st
    // goodJet1stPt/Eta/Mass: pt/eta/mass distribution of the jet1st
    goodJet1stPt.at(t).Fill(jet1stCandidate.Pt(),w);
    goodJet1stEta.at(t).Fill(jet1stCandidate.Eta(),w);
    goodJet1stMass.at(t).Fill(jet1stCandidate.M(),w);

    // jet2nd
    goodJet2ndPt.at(t).Fill(jet2ndCandidate.Pt(),w);
    goodJet2ndEta.at(t).Fill(jet2ndCandidate.Eta(),w);
    goodJet2ndMass.at(t).Fill(jet2ndCandidate.M(),w);

    
    // MET
    double metPhi(0.);
    metPhi = Ntp->MET_CorrT0pcTxy_phi();
    METEt.at(t).Fill(met,w);
    METPhi.at(t).Fill(metPhi,w);


  } //if(status)

  if(verbose) std::cout << " LightChargedHiggs::doEvent() END " << std::endl;
}
//******* LightChargedHiggs::doEvent END



//******* LightChargedHiggs::Finish START
void  LightChargedHiggs::Finish(){

  //loop over histograms
  //  for(unsigned int i=0; i<Nminus1.size(); i++){
  //    for(unsigned int j=0; j<Nminus1.at(i).size(); j++){
      
  //      std::cout << "***************" << Nminus1.at(i).at(j).GetName() << "  " << Nminus1.at(i).at(j).GetEntries() << " " << Nminus1.at(i).at(j).GetLineColor() << " " << Nminus1.at(i).at(j).GetMarkerColor() << std::endl;

  //    }
  //  }

  std::cout << " LightCahrgedHiggs is finished " << std::endl;
  Selection::Finish();
}
//******* LightChargedHiggs::Finish END



/////////////////////////////
//                         //
// definition of functions //
//                         //
/////////////////////////////

//******* LightChargedHiggs::JetCleaning START
// function of jet cleaning: in this way, the muon and tau candidates don't enter the jet selection
bool LightChargedHiggs::JetCleaning(unsigned int i, int muon1st, int tau1st){
  if(muon1st!=999){
    if(Ntp->PFJet_p4(i).DeltaR(Ntp->Muon_p4(muon1st)) < jetClean_dR) return false;
  } //if muon1st

  if(tau1st!=999){  
    if(Ntp->PFJet_p4(i).DeltaR(Ntp->PFTau_p4(tau1st)) < jetClean_dR) return false;
  } //if tau1st
  return true;
}
//******* LightChargedHiggs::JetCleaning END

