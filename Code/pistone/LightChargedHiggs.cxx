#include "LightChargedHiggs.h"

#include <iostream>
#include <cstdlib>

#include "HistoConfig.h"
#include "TLorentzVector.h"


//******* LightChargedHiggs::LightChargedHiggs START
LightChargedHiggs::LightChargedHiggs(TString Name_, TString id_):
  Selection(Name_,id_)
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
  muonPt=HConfig.GetTH1D(Name+"_muonPt","muonPt",40,0.,250.,"p_{T}^{#mu}");
  muonEta=HConfig.GetTH1D(Name+"_muonEta","muonEta",20,-2.5,2.5,"#eta_{#mu}");


  Selection::ConfigureHistograms();
  HConfig.GetHistoInfo(types,CrossSectionandAcceptance,legend,colour);
}
//******* LightChargedHiggs::Configure END



//******* LightChargedHiggs::Store_ExtraDist START
void  LightChargedHiggs::Store_ExtraDist(){

  // vertices
  Extradist1d.push_back(&NVtx);
  Extradist1d.push_back(&NGoodVtx);
  Extradist1d.push_back(&NTrackperVtx);

  // muon observables
  Extradist1d.push_back(&goodmuons);
  Extradist1d.push_back(&muonPt);
  Extradist1d.push_back(&muonEta);

}
//******* LightChargedHiggs::Store_ExtraDist END



//******* LightChargedHiggs::doEvent START
void  LightChargedHiggs::doEvent(){
  if(verbose) std::cout << " LightChargedHiggs::doEvent() START " << std::endl;

  unsigned int t;
  int id(Ntp->GetMCID());
  if(!HConfig.GetHisto(Ntp->isData(), id, t)){ std::cout << " failed to find id " <<std::endl; return; }
  

  // Apply Selection

  // number of good vertices
  if(verbose) std::cout << " find primary vertex " << std::endl;
  unsigned int nGoodVtx=0;
  for(unsigned int i=0; i<Ntp->NVtx(); i++){
    if(Ntp->isVtxGood_muJets(i)) nGoodVtx++;
  }

  value.at(PrimeVtx)=nGoodVtx;
  pass.at(PrimeVtx)=(value.at(PrimeVtx)>=cut.at(PrimeVtx));
  

  // trigger
  if(verbose) std::cout << " trigger " << std::endl;

  value.at(TriggerOk)=0;
  if(Ntp->TriggerAccept("HLT_IsoMu24")){
    value.at(TriggerOk)=1;
  }
  pass.at(TriggerOk)= (value.at(TriggerOk)==cut.at(TriggerOk));
  


  //
  // muon cuts
  //
  
  if(verbose) std::cout << " muon cuts " << std::endl;
  std::vector<unsigned int> NGoodMuons;
  TLorentzVector GoodMuons;

  // number of good muons
  for(unsigned i=0; i<Ntp->NMuons(); i++){
    if(Ntp->isGoodMuon_muJets(i)){
      NGoodMuons.push_back(i);
      GoodMuons = Ntp->Muon_p4(i);
    } //if
  } //for

  value.at(NMu)=NGoodMuons.size();
  pass.at(NMu)=(value.at(NMu)>=cut.at(NMu));
  


  // weights
  if(verbose) std::cout << " do weights " << std::endl;
  double wobs=1;
  double w;
  if(!Ntp->isData()){ w = Ntp->EvtWeight3D(); }
  else{ w=1; }




  ///////////////////////////////////////////////////////////
  bool status=AnalysisCuts(t,w,wobs);
  if(verbose) std::cout << " status: " << status << std::endl;

  // Add plots
  if(verbose) std::cout << " add plots " << std::endl;

  if(status){

    // vertices
    NVtx.at(t).Fill(Ntp->NVtx(),w); //number of vertices
    unsigned int nGoodVtx=0;
    for(unsigned int i=0; i<Ntp->NVtx(); i++){
      NTrackperVtx.at(t).Fill(Ntp->Vtx_Track_idx(i).size(),w); //number of tracks per vertex
      if(Ntp->isVtxGood(i)) nGoodVtx++;
    }
    NGoodVtx.at(t).Fill(nGoodVtx,w); //number of good vertices

    
    // muons
    bool passMuonSelection = 0;
    passMuonSelection = 
      pass.at(PrimeVtx)
      && pass.at(TriggerOk)
      && pass.at(NMu);


    goodmuons.at(t).Fill(NGoodMuons.size());

    if(passMuonSelection){
    
      muonPt.at(t).Fill(GoodMuons.Pt());
      muonEta.at(t).Fill(GoodMuons.Eta());

    } // if passMuonSelection
    


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





