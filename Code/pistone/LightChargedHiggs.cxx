#include "LightChargedHiggs.h"

#include <iostream>
#include <cstdlib>

#include "HistoConfig.h"
#include "TLorentzVector.h"


//******* LightChargedHiggs::LightChargedHiggs START
LightChargedHiggs::LightChargedHiggs(TString Name_, TString id_):
  Selection(Name_,id_)
  ,mu_pt(25.)
  ,mu_eta(2.1)
  ,mu_relIso(0.12)
  ,jet_pt(20.)  //very loose pt cut; pt=35. loose, pt=45 tight
  ,jet_eta(2.5)
  ,tau_pt(20.)
  ,tau_eta(2.7)
  ,tau_Iso(1.5)
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
    if(i==TriggerOk)    cut.at(TriggerOk)=1;
    if(i==PrimeVtx)     cut.at(PrimeVtx)=1;
    if(i==NMu)          cut.at(NMu)=1;
    if(i==NJets)        cut.at(NJets)=2;
    if(i==NTau)         cut.at(NTau)=1;
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


    // NJets
    else if(i==NJets){
      title.at(i)="Number of jets $>=$";
      title.at(i)+=cut.at(NJets);
      htitle=title.at(i);
      htitle.ReplaceAll("$","");
      htitle.ReplaceAll("\\","#");
      hlabel="Number of jets";
      Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_NJets_",htitle,6,-0.5,5.5,hlabel,"Events"));
      Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_NJets_",htitle,6,-0.5,5.5,hlabel,"Events"));
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
  goodmuons=HConfig.GetTH1D(Name+"_goodmuons","goodmuons",6,-0.5,5.5,"Number of tight muons");
  muonPt=HConfig.GetTH1D(Name+"_muonPt","muonPt",40,0.,250.,"p_{T}^{#mu} / GeV");
  muonEta=HConfig.GetTH1D(Name+"_muonEta","muonEta",20,-3.0,3.0,"#eta_{#mu}");


  // jets
  goodjets=HConfig.GetTH1D(Name+"_goodjets","goodjets",6,-0.5,5.5,"Number of loose jets");
  jetPt=HConfig.GetTH1D(Name+"_jetPt","jetPt",40,0.,250.,"p_{T}^{jet} / GeV");
  jetEta=HConfig.GetTH1D(Name+"_jetEta","jetEta",20,-3.0,3.0,"#eta_{jet}");
  jetMass=HConfig.GetTH1D(Name+"_jetMass","jetMass",40,0.,120.,"m_{jet} / GeV");


  // taus
  goodtaus=HConfig.GetTH1D(Name+"_goodtaus","goodtaus",6,-0.5,5.5,"Number of taus (3-prong)");
  tauPt=HConfig.GetTH1D(Name+"_tauPt","tauPt",40,0.,250.,"p_{T}^{#tau} / GeV");
  tauEta=HConfig.GetTH1D(Name+"_tauEta","tauEta",20,-3.0,3.0,"#eta_{#tau}");
  tauPhi=HConfig.GetTH1D(Name+"_tauPhi","tauPhi",20,-3.0,3.0,"#phi_{#tau}");


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
  Extradist1d.push_back(&goodmuons);
  Extradist1d.push_back(&muonPt);
  Extradist1d.push_back(&muonEta);
  
  
  // jet observables
  Extradist1d.push_back(&goodjets);
  Extradist1d.push_back(&jetPt);
  Extradist1d.push_back(&jetEta);
  Extradist1d.push_back(&jetMass);


  // tau observables
  Extradist1d.push_back(&goodtaus);
  Extradist1d.push_back(&tauPt);
  Extradist1d.push_back(&tauEta);
  Extradist1d.push_back(&tauPhi);


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
  
  if(verbose) std::cout << " muon cuts " << std::endl;
  std::vector<unsigned int> NGoodMuons;
  TLorentzVector GoodMuons;

  // number of good muons, i.e. pass the tight muon selection
  for(unsigned int i=0; i<Ntp->NMuons(); i++){
    if(Ntp->isTightMuon(i, vertex)
       && Ntp->Muon_p4(i).Pt()>mu_pt
       && fabs(Ntp->Muon_p4(i).Eta())<mu_eta
       && Ntp->Muon_RelIso(i)<mu_relIso
       ){
      NGoodMuons.push_back(i);
      GoodMuons = Ntp->Muon_p4(i);
    } //if
  } //for

  value.at(NMu)=NGoodMuons.size();
  pass.at(NMu)=(value.at(NMu)>=cut.at(NMu));



  //
  // jet cuts
  //
  
  if(verbose) std::cout << " jet cuts " << std::endl;
  std::vector<unsigned int> NGoodJets;
  TLorentzVector GoodJets;

  // number of good jets, i.e. that pass the loose jet selection
  for(unsigned int i=0; i<Ntp->NPFJets(); i++){
    if(Ntp->isJetID(i)
       && Ntp->PFJet_p4(i).Pt()>jet_pt
       && fabs(Ntp->PFJet_p4(i).Eta())<mu_eta
       ){
      NGoodJets.push_back(i);
      GoodJets = Ntp->PFJet_p4(i);
    } //if
  } //for

  value.at(NJets)=NGoodJets.size();
  pass.at(NJets)=(value.at(NJets)>=cut.at(NJets));



  //
  // tau cuts
  //

  if(verbose) std::cout << " tau cuts " << std::endl;
  std::vector<unsigned int> NGoodTaus;
  TLorentzVector GoodTaus;

  // number of good taus, i.e. pass the tau selection + 3-prong requirement
  for(unsigned int i=0; i<Ntp->NPFTaus(); i++){
    if(Ntp->PFTau_p4(i).Pt()>tau_pt
       && fabs(Ntp->PFTau_p4(i).Eta())<tau_eta
       && Ntp->PFTau_hpsDecayMode(i)==10   // Decay Mode = 10 => 3-prong decay
       && Ntp->PFTau_isHPSByDecayModeFinding(i)
       && Ntp->PFTau_isHPSAgainstMuonTight(i)
       && Ntp->PFTau_isHPSAgainstElectronsTight(i)
       && Ntp->PFTau_HPSPFTauDiscriminationByMediumCombinedIsolationDBSumPtCorr3Hits(i)<tau_Iso
       ){
      NGoodTaus.push_back(i);
      GoodTaus = Ntp->PFTau_p4(i);
    } //if
  } //for

  value.at(NTau)=NGoodTaus.size();
  pass.at(NTau)=(value.at(NTau)>=cut.at(NTau));



  // weights
  if(verbose) std::cout << " do weights " << std::endl;
  double wobs=1;
  double w=1;




  ///////////////////////////////////////////////////////////
  bool status=AnalysisCuts(t,w,wobs);
  if(verbose) std::cout << " status: " << status << std::endl;

  // Add plots
  if(verbose) std::cout << " add plots " << std::endl;


  //boolean variable for the muon+jet selection
  bool passSelection = 0;
  
  passSelection = 
    pass.at(PrimeVtx)            //primary vertex
    && pass.at(TriggerOk)        //trigger
    && pass.at(NMu)              //tight muon selection
    && pass.at(NJets)            //loose jet selection
    && pass.at(NTau);           //tau selection (3-prong)

  if(status){

    // vertices
    NVtx.at(t).Fill(Ntp->NVtx(),w); //number of vertices
    unsigned int nGoodVtx=0;
    for(unsigned int i=0; i<Ntp->NVtx(); i++){
      NTrackperVtx.at(t).Fill(Ntp->Vtx_Track_idx(i).size(),w); //number of tracks per vertex
      if(Ntp->isVtxGood(i)) nGoodVtx++;
    }
    NGoodVtx.at(t).Fill(nGoodVtx,w); //number of good vertices

    

    if(passSelection){

      //muons    
      goodmuons.at(t).Fill(NGoodMuons.size());
      muonPt.at(t).Fill(GoodMuons.Pt());
      muonEta.at(t).Fill(GoodMuons.Eta());

      //jets
      goodjets.at(t).Fill(NGoodJets.size());
      jetPt.at(t).Fill(GoodJets.Pt());
      jetEta.at(t).Fill(GoodJets.Eta());
      jetMass.at(t).Fill(GoodJets.M());

      //taus
      goodtaus.at(t).Fill(NGoodTaus.size());
      tauPt.at(t).Fill(GoodTaus.Pt());
      tauEta.at(t).Fill(GoodTaus.Eta());
      tauPhi.at(t).Fill(GoodTaus.Phi());

    } // if passSelection
    

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


  Selection::Finish();
}
//******* LightChargedHiggs::Finish END





