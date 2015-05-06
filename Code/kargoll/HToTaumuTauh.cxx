#include "HToTaumuTauh.h"
#include "TLorentzVector.h"
#include <cstdlib>
#include "HistoConfig.h"
#include "SkimConfig.h"
#include <iostream>
#include "TauDataFormat/TauNtuple/interface/DataMCType.h"
#include "SVfitProvider.h"
#include "SimpleFits/FitSoftware/interface/Logger.h"

HToTaumuTauh::HToTaumuTauh(TString Name_, TString id_):
  Selection(Name_,id_),
  cMu_dxy(0.045),
  cMu_dz(0.2),
  cMu_relIso(0.1),
  cMu_pt(20.0),
  cMu_eta(2.1),
  cMu_dRHltMatch(0.5),
  cTau_pt(20.0),
  cTau_eta(2.3),
  cTau_rawIso(1.5),
  cMuTau_dR(0.5),
  cTau_dRHltMatch(0.5),
  cMuTriLep_pt(10.0),
  cMuTriLep_eta(2.4),
  cEleTriLep_pt(10.0),
  cEleTriLep_eta(2.5),
  cCat_jetPt(30.0),
  cCat_jetEta(4.7),
  cCat_bjetPt(20.0),
  cCat_bjetEta(2.4),
  cCat_btagDisc(0.679), // medium WP, https://twiki.cern.ch/twiki/bin/viewauth/CMS/BTagPerformanceOP#B_tagging_Operating_Points_for_5
  cCat_splitTauPt(45.0),
  cJetClean_dR(0.5)
{
	Logger(Logger::Verbose) << "Start." << std::endl;
	TString trigNames[] = {"HLT_IsoMu18_eta2p1_LooseIsoPFTau20","HLT_IsoMu17_eta2p1_LooseIsoPFTau20"};
	std::vector<TString> temp (trigNames, trigNames + sizeof(trigNames) / sizeof(TString) );
	cTriggerNames = temp;

	// Set object corrections to use
	correctTaus = "scalecorr"; // "scalecorr" = energy scale correction by decay mode
	correctMuons = ""; // "roch" = Rochester muon ID corrections
	correctElecs = ""; // "run" = run dependent corrections, "JER" = jet energy resolution smearing
	correctJets = "";

	// implemented categories:
	// VBFTight, VBFLoose
	// OneJetHigh, OneJetLow, OneJetBoost
	// ZeroJetHigh, ZeroJetLow
	// Inclusive

	// Set it to "Inclusive" here.
	// For each category, there should be a special class inheriting from HToTaumuTauh
	categoryFlag = "Inclusive";

	// Do you want to use embedding or MC for DY background?
	// set to "false" for background estimation, to "true" for categories
	useEmbedding = false;

	// select which WJets Background source to use
	// chose between:
	// * "MC": use MC as given in Histo.txt
	// * "Data": use data driven method (make sure wJetsYieldScaleMap is filled correctly)
	wJetsBGSource = "MC";

	// this one is used to set the event yield for W+Jet
	wJetsYieldMap.insert(std::pair<TString,double>("ZeroJetLow",  6593.05981966) );
	wJetsYieldMap.insert(std::pair<TString,double>("ZeroJetHigh", 1128.81404765) );
	wJetsYieldMap.insert(std::pair<TString,double>("OneJetLow",   4817.17263439) );
	wJetsYieldMap.insert(std::pair<TString,double>("OneJetHigh",   674.57141930) );
	wJetsYieldMap.insert(std::pair<TString,double>("OneJetBoost",  158.46545425) );
	wJetsYieldMap.insert(std::pair<TString,double>("VBFLoose",      62.95892997) );
	wJetsYieldMap.insert(std::pair<TString,double>("VBFTight",       4.89934663) );
	wJetsYieldMap.insert(std::pair<TString,double>("Inclusive",  13295.55036387) );

	// flag to switch data-driven QCD on/off
	// set to "true" if running analyses (i.e. in categories)
	// set to "false" to estimate QCD yield
	qcdShapeFromData = false;

	// these are used to set the event yield for QCD
	qcdYieldABCDMap.insert(std::pair<TString,double>("ZeroJetLow",  16235.46236044) );
	qcdYieldABCDMap.insert(std::pair<TString,double>("ZeroJetHigh",   465.59995267) );
	qcdYieldABCDMap.insert(std::pair<TString,double>("OneJetLow",    4893.21983995) );
	qcdYieldABCDMap.insert(std::pair<TString,double>("OneJetHigh",    264.47357676) );
	qcdYieldABCDMap.insert(std::pair<TString,double>("OneJetBoost",    56.94622289) );
	qcdYieldABCDMap.insert(std::pair<TString,double>("VBFLoose",       38.40373683) );
	qcdYieldABCDMap.insert(std::pair<TString,double>("VBFTight",        5.74384738) );
	qcdYieldABCDMap.insert(std::pair<TString,double>("Inclusive",   22157.13046410) );

	// flag to enable efficiency method for data-driven QCD yield
	// these numbers will only be used in the categories where they are available,
	// in all other categories the yield from the ABCD method will be used
	// (thus it is safe to set this flag globally here, no need to set it in individual categories)
	// Attention: if flag "qcdShapeFromData" is set to "false", this flag has no effect
	qcdUseEfficiencyMethod = true;

	// these are used to set the event yield for QCD using the efficiency method
	qcdYieldEffMap.insert(std::pair<TString,double>("OneJetBoost", 32.94421105));
	qcdYieldEffMap.insert(std::pair<TString,double>("VBFLoose",    57.69432816));
	qcdYieldEffMap.insert(std::pair<TString,double>("VBFTight",    1.42017411));

	clock = new TBenchmark();
}

HToTaumuTauh::~HToTaumuTauh(){
	Logger(Logger::Verbose) << "start destructing" << std::endl;
	for(unsigned int j=0; j<Npassed.size(); j++){
	Logger(Logger::Info) << "Selection Summary before: "
	 << Npassed.at(j).GetBinContent(1)     << " +/- " << Npassed.at(j).GetBinError(1)     << " after: "
	 << Npassed.at(j).GetBinContent(NCuts+1) << " +/- " << Npassed.at(j).GetBinError(NCuts) << std::endl;
	}

	if (mode == ANALYSIS) {
		delete RSF;
	}

	delete clock;

	Logger(Logger::Info) << "HToTaumuTauh::~HToTaumuTauh() done" << std::endl;
}

void  HToTaumuTauh::Setup(){
  Logger(Logger::Verbose) << "HToTaumuTauh::Setup()" << std::endl;
  // Setup Cut Values
  for(int i=0; i<NCuts;i++){
    cut.push_back(0);
    value.push_back(0);
    pass.push_back(false);
    if(i==TriggerOk)    	cut.at(TriggerOk)=0;
    if(i==PrimeVtx)     	cut.at(PrimeVtx)=1;
    if(i==NMuId)			cut.at(NMuId)=1;
    if(i==NMuKin)			cut.at(NMuKin)=1;
    if(i==DiMuonVeto)		cut.at(DiMuonVeto)=0.15;
    if(i==NTauId)			cut.at(NTauId)=1;
    if(i==NTauIso)			cut.at(NTauIso)=1;
    if(i==NTauKin)			cut.at(NTauKin)=1;
    if(i==TriLeptonVeto)	cut.at(TriLeptonVeto)=0;
    if(i==OppCharge)		cut.at(OppCharge)=0;
    if(i==MT)				cut.at(MT)=30.0; // https://twiki.cern.ch/twiki/bin/viewauth/CMS/HiggsToTauTauWorkingSummer2013#Event_Categories_SM
    if(i==BJetVeto)			cut.at(BJetVeto)=0;
    //category-specific values are set in the corresponding configure function
    // set them to dummy value -10.0 here
    if(i>=CatCut1){
    	cut.at(i)	= -10.0;
    	pass.at(i)	= true;
    }
  }

  TString hlabel;
  TString htitle;
  for(unsigned int i_cut=0; i_cut<NCuts; i_cut++){
    title.push_back("");
    distindx.push_back(false);
    dist.push_back(std::vector<float>());
    TString c="_Cut_";c+=i_cut;
  
    if(i_cut==PrimeVtx){
      title.at(i_cut)="Number of Prime Vertices $(N>$";
      title.at(i_cut)+=cut.at(PrimeVtx);
      title.at(i_cut)+=")";
      htitle=title.at(i_cut);
      htitle.ReplaceAll("$","");
      htitle.ReplaceAll("\\","#");
      hlabel="Number of Prime Vertices";
      Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_PrimeVtx_",htitle,51,-0.5,50.5,hlabel,"Events"));
      Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_PrimeVtx_",htitle,51,-0.5,50.5,hlabel,"Events"));
    }
    else if(i_cut==TriggerOk){
      title.at(i_cut)="Trigger ";
      hlabel="Trigger ";

      std::vector<TH1D> Nm1Temp = HConfig.GetTH1D(Name+c+"_Nminus1_TriggerOk_",htitle,cTriggerNames.size()+2,-1.5,cTriggerNames.size()+0.5,hlabel,"Events");
      std::vector<TH1D> Nm0Temp = HConfig.GetTH1D(Name+c+"_Nminus0_TriggerOk_",htitle,cTriggerNames.size()+2,-1.5,cTriggerNames.size()+0.5,hlabel,"Events");
      for (unsigned i_hist = 0; i_hist < Nm1Temp.size(); i_hist++){
    	  Nm1Temp.at(i_hist).GetXaxis()->SetBinLabel(1,"not fired");
    	  Nm0Temp.at(i_hist).GetXaxis()->SetBinLabel(1,"not fired");
    	  for (unsigned i_bin = 2; i_bin < cTriggerNames.size()+2; i_bin++){
    		  Nm1Temp.at(i_hist).GetXaxis()->SetBinLabel(i_bin,cTriggerNames.at(i_bin-2));
    		  Nm0Temp.at(i_hist).GetXaxis()->SetBinLabel(i_bin,cTriggerNames.at(i_bin-2));
    	  }
    	  Nm1Temp.at(i_hist).GetXaxis()->SetBinLabel(cTriggerNames.size()+2,"multiple fired");
    	  Nm0Temp.at(i_hist).GetXaxis()->SetBinLabel(cTriggerNames.size()+2,"multiple fired");
      }
      Nminus1.push_back(Nm1Temp);
      Nminus0.push_back(Nm0Temp);
    }
    else if(i_cut==NMuId){
    	title.at(i_cut)="Number $\\mu_{ID} >=$";
    	title.at(i_cut)+=cut.at(NMuId);
    	htitle=title.at(i_cut);
    	htitle.ReplaceAll("$","");
    	htitle.ReplaceAll("\\","#");
    	hlabel="Number of #mu_{ID}";
    	Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_NMuId_",htitle,11,-0.5,10.5,hlabel,"Events"));
    	Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_NMuId_",htitle,11,-0.5,10.5,hlabel,"Events"));
    }
    else if(i_cut==NMuKin){
    	title.at(i_cut)="Number $\\mu_{sel} >=$";
    	title.at(i_cut)+=cut.at(NMuKin);
    	htitle=title.at(i_cut);
    	htitle.ReplaceAll("$","");
    	htitle.ReplaceAll("\\","#");
    	hlabel="Number of #mu_{sel}";
    	Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_NMuKin_",htitle,6,-0.5,5.5,hlabel,"Events"));
    	Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_NMuKin_",htitle,6,-0.5,5.5,hlabel,"Events"));
    }
    else if(i_cut==DiMuonVeto){
    	title.at(i_cut)="$\\Delta R(\\mu_{veto}^{+},\\mu_{veto}^{-}) <$";
        char buffer[50];
        sprintf(buffer,"%5.1f",cut.at(DiMuonVeto));
    	title.at(i_cut)+=buffer;
    	htitle=title.at(i_cut);
    	htitle.ReplaceAll("$","");
    	htitle.ReplaceAll("\\","#");
    	hlabel="#DeltaR(#mu_{veto}^{+},#mu_{veto}^{-})";
    	Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_DiMuonVeto_",htitle,100,0.,5.0,hlabel,"Events"));
    	Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_DiMuonVeto_",htitle,100,0.,5.0,hlabel,"Events"));
    }
    else if(i_cut==NTauId){
    	title.at(i_cut)="Number $\\tau_{ID} >=$";
    	title.at(i_cut)+=cut.at(NTauId);
    	htitle=title.at(i_cut);
    	htitle.ReplaceAll("$","");
    	htitle.ReplaceAll("\\","#");
    	hlabel="Number of #tau_{ID}";
    	Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_NTauId_",htitle,26,-0.5,25.5,hlabel,"Events"));
    	Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_NTauId_",htitle,26,-0.5,25.5,hlabel,"Events"));
    }
    else if(i_cut==NTauIso){
    	title.at(i_cut)="Number $\\tau_{Iso} >=$";
    	title.at(i_cut)+=cut.at(NTauIso);
    	htitle=title.at(i_cut);
    	htitle.ReplaceAll("$","");
    	htitle.ReplaceAll("\\","#");
    	hlabel="Number of #tau_{Iso}";
    	Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_NTauIso_",htitle,16,-0.5,15.5,hlabel,"Events"));
    	Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_NTauIso_",htitle,16,-0.5,15.5,hlabel,"Events"));
    }
    else if(i_cut==NTauKin){
    	title.at(i_cut)="Number $\\tau_{sel} >=$";
    	title.at(i_cut)+=cut.at(NTauKin);
    	htitle=title.at(i_cut);
    	htitle.ReplaceAll("$","");
    	htitle.ReplaceAll("\\","#");
    	hlabel="Number of #tau_{sel}";
    	Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_NTauKin_",htitle,11,-0.5,10.5,hlabel,"Events"));
    	Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_NTauKin_",htitle,11,-0.5,10.5,hlabel,"Events"));
    }
    else if(i_cut==TriLeptonVeto){
    	title.at(i_cut)="3 lepton veto: $N(\\mu)+N(e) =$";
    	title.at(i_cut)+=cut.at(TriLeptonVeto);
    	htitle=title.at(i_cut);
    	htitle.ReplaceAll("$","");
    	htitle.ReplaceAll("\\","#");
    	hlabel="Number of tri-lepton veto leptons";
    	Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_TriLeptonVeto_",htitle,5,-0.5,4.5,hlabel,"Events"));
    	Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_TriLeptonVeto_",htitle,5,-0.5,4.5,hlabel,"Events"));
    }
    else if(i_cut==OppCharge){
    	title.at(i_cut)="$q(\\mu)+q(\\tau) =$";
    	title.at(i_cut)+=cut.at(OppCharge);
    	htitle=title.at(i_cut);
    	htitle.ReplaceAll("$","");
    	htitle.ReplaceAll("\\","#");
    	hlabel="q(#mu)+q(#tau)";
    	Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_OppCharge_",htitle,5,-2.5,2.5,hlabel,"Events"));
    	Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_OppCharge_",htitle,5,-2.5,2.5,hlabel,"Events"));
    }
    else if(i_cut==MT){
    	title.at(i_cut)="$m_{T}(\\mu,E_{T}^{miss}) <$";
    	title.at(i_cut)+=cut.at(MT);
    	title.at(i_cut)+=" GeV";
    	htitle=title.at(i_cut);
    	htitle.ReplaceAll("$","");
    	htitle.ReplaceAll("\\","#");
    	hlabel="m_{T}(#mu,E_{T}^{miss})/GeV";
    	Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_MT_",htitle,100,0.,200.,hlabel,"Events"));
    	Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_MT_",htitle,100,0.,200.,hlabel,"Events"));
    }
    else if(i_cut==BJetVeto){
    	title.at(i_cut)="Number b-Jets $<=$";
    	title.at(i_cut)+=cut.at(BJetVeto);
    	htitle=title.at(i_cut);
    	htitle.ReplaceAll("$","");
    	htitle.ReplaceAll("\\","#");
    	hlabel="Number of b-Jets";
    	Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_BJetVeto_",htitle,11,-0.5,10.5,hlabel,"Events"));
    	Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_BJetVeto_",htitle,11,-0.5,10.5,hlabel,"Events"));
    }
    else if(i_cut>=CatCut1){
    	// set histograms to dummy values
    	// will be overwritten in configure_{Category} method
    	title.at(i_cut) = "Category Dummy ";
    	title.at(i_cut)	+=(i_cut-CatCut1);
    	htitle=title.at(i_cut);
    	htitle.ReplaceAll("$","");
    	htitle.ReplaceAll("\\","#");
    	hlabel = title.at(i_cut);
    	TString n = Name+c+"_Nminus1_CatDummy";
    	n += (i_cut-CatCut1);
    	n += "_";
    	Nminus1.push_back(HConfig.GetTH1D(n,htitle,50,-50.,50.,hlabel,"Events"));
    	n.ReplaceAll("Nminus1","Nminus0");
    	Nminus0.push_back(HConfig.GetTH1D(n,htitle,50,-50.,50.,hlabel,"Events"));
    }
  } 
  // Setup NPassed Histogams
  Npassed=HConfig.GetTH1D(Name+"_NPass","Cut Flow",NCuts+1,-1,NCuts,"Number of Accumulative Cuts Passed","Events");

  // Setup Extra Histograms
  h_NVtx=HConfig.GetTH1D(Name+"_NVtx","NVtx",26,-0.5,25.5,"N(Vtx) before selection");
  h_VtxZ=HConfig.GetTH1D(Name+"_VtxZ","VtxZ",50,-50.0,50.0,"z(Vtx)/cm");
  h_VtxRho=HConfig.GetTH1D(Name+"_VtxRho","VtxRho",50,0.0,2.0,"#rho(Vtx)/cm");
  h_VtxPhi=HConfig.GetTH1D(Name+"_VtxPhi","VtxPhi",50,0.0,3.2,"#phi(Vtx)");
  h_VtxNdof=HConfig.GetTH1D(Name+"_VtxNdof","VtxNdof",50,-0.5,49.5,"NDoF(Vtx Fit)");
  h_VtxIsfake=HConfig.GetTH1D(Name+"_VtxIsfake","VtxIsfake",2,-0.5,1.5,"IsFake(Vtx)");

  h_MuDxy=HConfig.GetTH1D(Name+"_MuDxy","MuDxy",60,-0.3,0.3,"d_{xy}(#mu,Vtx)/cm");
  h_MuDz=HConfig.GetTH1D(Name+"_MuDz","MuDz",60,-.6,.6,"d_{z}(#mu,Vtx)/cm");
  h_MuRelIso=HConfig.GetTH1D(Name+"_MuRelIso","MuRelIso",50,0.,1.,"relIso(#mu)");
  h_MuPt=HConfig.GetTH1D(Name+"_MuPt","MuPt",50,0.,200.,"p_{T}(#mu)/GeV");
  h_MuEta=HConfig.GetTH1D(Name+"_MuEta","MuEta",50,-2.5,2.5,"#eta(#mu)");
  h_MuPhi=HConfig.GetTH1D(Name+"_MuPhi","MuPhi",50,-3.14159,3.14159,"#phi(#mu)");

  h_MuSelPt=HConfig.GetTH1D(Name+"_MuSelPt","MuSelPt",50,0.,100.,"p_{T}(#mu_{sel})/GeV");
  h_MuSelEta=HConfig.GetTH1D(Name+"_MuSelEta","MuSelEta",50,-2.5,2.5,"#eta(#mu_{sel})");
  h_MuSelPhi=HConfig.GetTH1D(Name+"_MuSelPhi","MuSelPhi",50,-3.14159,3.14159,"#phi(#mu_{sel})");
  h_MuSelDxy=HConfig.GetTH1D(Name+"_MuSelDxy","MuSelDxy",60,-0.3,0.3,"d_{xy}(#mu_{sel},Vtx)/cm");
  h_MuSelDz=HConfig.GetTH1D(Name+"_MuSelDz","MuSelDz",60,-.6,.6,"d_{z}(#mu_{sel},Vtx)/cm");
  h_MuSelRelIso=HConfig.GetTH1D(Name+"_MuSelRelIso","MuSelRelIso",50,0.,1.,"relIso(#mu_{sel})");
  h_MuSelFakesTauID=HConfig.GetTH1D(Name+"_MuSelFakesTauID","MuSelFakesTauID",2,-0.5,1.5,"#mu_{sel} fakes #tau_{h}");
  h_MuSelDrHlt=HConfig.GetTH1D(Name+"_MuSelDrHlt","MuSelDrHLT",50,0.,1.,"#DeltaR(#mu_{sel},#mu_{HLT})");

  h_TauPt=HConfig.GetTH1D(Name+"_TauPt","TauPt",50,0.,100.,"p_{T}(#tau)/GeV");
  h_TauEta=HConfig.GetTH1D(Name+"_TauEta","TauEta",50,-2.5,2.5,"#eta(#tau)");
  h_TauPhi=HConfig.GetTH1D(Name+"_TauPhi","TauPhi",50,-3.14159,3.14159,"#phi(#tau)");
  h_TauDecayMode=HConfig.GetTH1D(Name+"_TauDecayMode","TauDecayMode",16,-0.5,15.5,"#tau decay mode");
  h_TauIso=HConfig.GetTH1D(Name+"_TauIso","TauIso",50,0.,25.,"Iso(#tau)/GeV");
  h_TauSelMass=HConfig.GetTH1D(Name+"_TauMass","TauMass",100,-1.0,2.0,"m_{vis}(#tau)/GeV");

  h_TauSelPt=HConfig.GetTH1D(Name+"_TauSelPt","TauSelPt",50,0.,100.,"p_{T}(#tau_{sel})/GeV");
  h_TauSelEta=HConfig.GetTH1D(Name+"_TauSelEta","TauSelEta",50,-2.5,2.5,"#eta(#tau_{sel})");
  h_TauSelPhi=HConfig.GetTH1D(Name+"_TauSelPhi","TauSelPhi",50,-3.14159,3.14159,"#phi(#tau_{sel})");
  h_TauSelDrHlt=HConfig.GetTH1D(Name+"_TauSelDrHlt","TauSelDrHLT",50,0.,1.,"#DeltaR(#tau_{sel},#tau_{HLT})");
  h_TauSelDecayMode=HConfig.GetTH1D(Name+"_TauSelDecayMode","TauSelDecayMode",16,-0.5,15.5,"#tau_{sel} decay mode");
  h_TauSelIso=HConfig.GetTH1D(Name+"_TauSelIso","TauSelIso",50,0.,25.,"Iso(#tau_{sel})/GeV");

  h_MuVetoDPtSelMuon=HConfig.GetTH1D(Name+"_MuVetoDPtSelMuon","MuVetoDPtSelMuon",100,-100.,100.,"#Deltap_{T}(#mu_{veto},#mu)/GeV");
  h_MuVetoInvM=HConfig.GetTH1D(Name+"_MuVetoInvM","MuVetoInvM",100,0.,200,"m_{inv}(#mu_{veto}^{1},#mu_{veto}^{2})/GeV");
  h_MuVetoPtPositive=HConfig.GetTH1D(Name+"_MuVetoPtPositive","MuVetoPtPositive",50,0.,100.,"p_{T}(#mu_{veto}^{+})/GeV");
  h_MuVetoPtNegative=HConfig.GetTH1D(Name+"_MuVetoPtNegative","MuVetoPtNegative",50,0.,100.,"p_{T}(#mu_{veto}^{-})/GeV");
  h_MuVetoDRTau=HConfig.GetTH1D(Name+"_MuVetoDRTau","MuVetoDRTau",50,0.,5.,"#DeltaR(#mu_{veto},#tau_{h})");
  h_MuVetoDeltaR=HConfig.GetTH1D(Name+"_MuVetoDeltaR","MuVetoDeltaR",50,0.,5.,"#DeltaR(#mu^{+}_{veto},#mu^{-}_{veto})");

  h_NMuonTriLepVeto=HConfig.GetTH1D(Name+"_NMuonTriLepVeto","NMuonTriLepVeto",5,-0.5,4.5,"N(#mu_{3l veto})");
  h_NElecTriLepVeto=HConfig.GetTH1D(Name+"_NElecTriLepVeto","NElecTriLepVeto",5,-0.5,4.5,"N(e_{3l veto})");

  h_MuCharge=HConfig.GetTH1D(Name+"_MuCharge","MuCharge",3,-1.5,1.5,"q(#mu)/e");
  h_TauCharge=HConfig.GetTH1D(Name+"_TauCharge","TauCharge",7,-3.5,3.5,"q(#tau)/e");

  h_MuTauDR=HConfig.GetTH1D(Name+"_MuTauDR","MuTauDR",50,0.,5.,"#DeltaR(#mu,#tau_{h})");
  h_MuTauDPhi=HConfig.GetTH1D(Name+"_MuTauDPhi","MuTauDPhi",50,0.,3.2,"#Delta#phi(#mu,#tau_{h})");
  h_MuTauDEta=HConfig.GetTH1D(Name+"_MuTauDEta","MuTauDEta",100,-6.,6.,"#Delta#eta(#mu,#tau_{h})");
  h_MuTauDPt=HConfig.GetTH1D(Name+"_MuTauDPt","MuTauDPt",100,-100.,100.,"#Deltap_{T}(#mu,#tau_{h})/GeV");
  h_MuTauRelDPt=HConfig.GetTH1D(Name+"_MuTauRelDPt","MuTauRelDPt",100,-2.,2.,"#Deltap_{T}(#mu,#tau_{h})/p_{T}(#mu)");
  h_MuPtVsTauPt=HConfig.GetTH2D(Name+"_MuPtVsTauPt","MuPtVsTauPt",50,0.,100.,50,0.,100.,"p_{T}(#mu)/GeV","p_{T}(#tau)/GeV");

  h_MetPt  = HConfig.GetTH1D(Name+"_MetPt","MetPt",50,0.,200.,"E_{T}^{miss}/GeV");
  h_MetPhi = HConfig.GetTH1D(Name+"_MetPhi","MetPhi",50,-3.14159,3.14159,"#phi(E_{T}^{miss})");

  h_MetLepMuDr = HConfig.GetTH1D(Name+"_MetLepMuDr","MetLepMuDr",102,-0.1,5.0,"#DeltaR(#mu,#mu^{MET})");
  h_MetLepTauDr = HConfig.GetTH1D(Name+"_MetLepTauDr","MetLepTauDr",102,-0.1,5.0,"#DeltaR(#tau_{h},#tau_{h}^{MET})");
  h_MetLepNMu = HConfig.GetTH1D(Name+"_MetLepNMu","MetLepNMu",11,-0.5,10.5,"N(#mu^{MET})");
  h_MetLepNTau = HConfig.GetTH1D(Name+"_MetLepNTau","MetLepNTau",11,-0.5,10.5,"N(#tau_{h}^{MET})");
  h_MetLepNMuMinusNMu = HConfig.GetTH1D(Name+"_MetLepNMuMinusNMu","MetLepNMuMinusNMu",11,-5.5,5.5,"N(#mu^{MET}) - N(#mu^{sel}");
  h_MetLepNTauMinusNTau = HConfig.GetTH1D(Name+"_MetLepNTauMinusNTau","MetLepNTauMinusNTau",11,-5.5,5.5,"N(#tau_{h}^{MET}) - N(#tau_{h}^{sel}");
  h_MetLepDiffMET  = HConfig.GetTH1D(Name+"_MetLepDiffMET","MetLepDiffMET",50,0.,200.,"#mu^{MET}#neq#mu^{sel}: E_{T}^{miss}/GeV");
  h_MetLepDiffMETPhi = HConfig.GetTH1D(Name+"_MetLepDiffMETPhi","MetLepDiffMETPhi",50,-3.14159,3.14159,"#mu^{MET}#neq#mu^{sel}: #phi(E_{T}^{miss})");
  h_MetLepDiffMt = HConfig.GetTH1D(Name+"_MetLepDiffMt","MetLepDiffMt",100,0.,200.,"#mu^{MET}#neq#mu^{sel}: m_{T}/GeV");

  h_NJetsKin = HConfig.GetTH1D(Name+"_NJetsKin","NJetsKin",11,-0.5,10.5,"N(j_{kin})");
  h_JetKin1Pt = HConfig.GetTH1D(Name+"_JetKin1Pt","JetKin1Pt",50,0.,200.,"p_{T}(j_{kin}^{1})/GeV");
  h_JetKin1Eta = HConfig.GetTH1D(Name+"_JetKin1Eta","JetKin1Eta",100,-5.0,5.0,"#eta(j_{kin}^{1})");
  h_JetKin1Phi = HConfig.GetTH1D(Name+"_JetKin1Phi","JetKin1Phi",50,-3.14159,3.14159,"#phi(j_{kin}^{1})");
  h_JetKin1IsLooseId = HConfig.GetTH1D(Name+"_JetKin1IsLooseId","JetKin1IsLooseId",2,-0.5,1.5,"isLoosePUJetID(j_{kin}^{1}");
  h_JetKin2IsLooseId = HConfig.GetTH1D(Name+"_JetKin2IsLooseId","JetKin2IsLooseId",2,-0.5,1.5,"isLoosePUJetID(j_{kin}^{2}");
  h_JetKin2Pt = HConfig.GetTH1D(Name+"_JetKin2Pt","JetKin2Pt",50,0.,200.,"p_{T}(j_{kin}^{2})/GeV");
  h_JetKin2Eta = HConfig.GetTH1D(Name+"_JetKin2Eta","JetKin2Eta",100,-5.0,5.0,"#eta(j_{kin}^{2})");
  h_JetKin2Phi = HConfig.GetTH1D(Name+"_JetKin2Phi","JetKin2Phi",50,-3.14159,3.14159,"#phi(j_{kin}^{2})");
  h_NJetsId = HConfig.GetTH1D(Name+"_NJetsId","NJetsId",11,-0.5,10.5,"N(jets)");
  h_Jet1Pt = HConfig.GetTH1D(Name+"_Jet1Pt","Jet1Pt",50,0.,200.,"p_{T}(j^{1})/GeV");
  h_Jet1Eta = HConfig.GetTH1D(Name+"_Jet1Eta","Jet1Eta",100,-5.0,5.0,"#eta(j^{1})");
  h_Jet1Phi = HConfig.GetTH1D(Name+"_Jet1Phi","Jet1Phi",50,-3.14159,3.14159,"#phi(j^{1})");
  h_Jet1IsB = HConfig.GetTH1D(Name+"_Jet1IsB","Jet1IsB",2,-0.5,1.5,"isBJet(j^{1})");
  h_Jet2Pt = HConfig.GetTH1D(Name+"_Jet2Pt","Jet2Pt",50,0.,200.,"p_{T}(j^{2})/GeV");
  h_Jet2Eta = HConfig.GetTH1D(Name+"_Jet2Eta","Jet2Eta",100,-5.0,5.0,"#eta(j^{2})");
  h_Jet2Phi = HConfig.GetTH1D(Name+"_Jet2Phi","Jet2Phi",50,-3.14159,3.14159,"#phi(j^{2})");
  h_Jet2IsB = HConfig.GetTH1D(Name+"_Jet2IsB","Jet2IsB",2,-0.5,1.5,"isBJet(j^{2})");

  h_NBJets = HConfig.GetTH1D(Name+"_NBJets","NBJets",6,-0.5,5.5,"N(bjets)");
  h_BJet1Pt = HConfig.GetTH1D(Name+"_BJet1Pt","BJet1Pt",50,0.,200.,"p_{T}(b^{1})/GeV");
  h_BJet1Eta = HConfig.GetTH1D(Name+"_BJet1Eta","BJet1Eta",100,-5.0,5.0,"#eta(b^{1})");
  h_BJet1Phi = HConfig.GetTH1D(Name+"_BJet1Phi","BJet1Phi",50,-3.14159,3.14159,"#phi(b^{1})");

  h_HiggsPt = HConfig.GetTH1D(Name+"_HiggsPt","HiggsPt",50,0.,200.,"p_{T}(H)/GeV");
  h_HiggsPhi = HConfig.GetTH1D(Name+"_HiggsPhi","HiggsPhi",50,-3.14159,3.14159,"#phi(H)");
  h_JetsDEta = HConfig.GetTH1D(Name+"_JetsDEta","JetsDEta",100,-10.,10.,"#Delta#eta(j^{1},j^{2})");
  h_JetsInEtaGap = HConfig.GetTH1D(Name+"_JetsInEtaGap","JetsInEtaGap",6,-0.5,5.5,"N(j in #eta gap)");
  h_JetsInvM = HConfig.GetTH1D(Name+"_JetsInvM","JetsInvM",100,0.,2000.,"m_{inv}(j^{1},j^{2})");

  h_MtMuPlusOnly = HConfig.GetTH1D(Name+"_MtMuPlusOnly","MtMuPlusOnly",100,0.,200.,"m_{T}/GeV");
  h_MtMuMinusOnly = HConfig.GetTH1D(Name+"_MtMuMinusOnly","MtMuMinusOnly",100,0.,200.,"m_{T}/GeV");
  h_Mt1ProngOnly = HConfig.GetTH1D(Name+"_Mt1ProngOnly","Mt1ProngOnly",100,0.,200.,"m_{T}/GeV");
  h_Mt3ProngOnly = HConfig.GetTH1D(Name+"_Mt3ProngOnly","Mt3ProngOnly",100,0.,200.,"m_{T}/GeV");
  h_Mt3ProngSV = HConfig.GetTH1D(Name+"_Mt3ProngSV","Mt3ProngSV",100,0.,200.,"m_{T}/GeV");
  h_Mt3ProngSVFlight = HConfig.GetTH1D(Name+"_Mt3ProngSVFlight","Mt3ProngSVFlight",100,0.,200.,"m_{T}/GeV");

  h_MetPt1ProngOnly  = HConfig.GetTH1D(Name+"_MetPt1ProngOnly","MetPt1ProngOnly",50,0.,200.,"E_{T}^{miss}/GeV");
  h_MetPhi1ProngOnly = HConfig.GetTH1D(Name+"_MetPhi1ProngOnly","MetPhi1ProngOnly",50,-3.14159,3.14159,"#phi(E_{T}^{miss})");
  h_MetPt3ProngOnly  = HConfig.GetTH1D(Name+"_MetPt3ProngOnly","MetPt3ProngOnly",50,0.,200.,"E_{T}^{miss}/GeV");
  h_MetPhi3ProngOnly = HConfig.GetTH1D(Name+"_MetPhi3ProngOnly","MetPhi3ProngOnly",50,-3.14159,3.14159,"#phi(E_{T}^{miss})");
  h_MetPtNoMtCut = HConfig.GetTH1D(Name+"_MetPtNoMtCut","MetPtNoMtCut",50,0.,200.,"E_{T}^{miss}/GeV");
  h_MetPhiNoMtCut = HConfig.GetTH1D(Name+"_MetPhiNoMtCut","MetPhiNoMtCut",50,-3.14159,3.14159,"#phi(E_{T}^{miss})");
  h_MetPtNoMtCut1ProngOnly = HConfig.GetTH1D(Name+"_MetPtNoMtCut1ProngOnly","MetPtNoMtCut1ProngOnly",50,0.,200.,"E_{T}^{miss}/GeV");
  h_MetPhiNoMtCut1ProngOnly = HConfig.GetTH1D(Name+"_MetPhiNoMtCut1ProngOnly","MetPhiNoMtCut1ProngOnly",50,-3.14159,3.14159,"#phi(E_{T}^{miss})");
  h_MetPtNoMtCut3ProngOnly = HConfig.GetTH1D(Name+"_MetPtNoMtCut3ProngOnly","MetPtNoMtCut3ProngOnly",50,0.,200.,"E_{T}^{miss}/GeV");
  h_MetPhiNoMtCut3ProngOnly = HConfig.GetTH1D(Name+"_MetPhiNoMtCut3ProngOnly","MetPhiNoMtCut3ProngOnly",50,-3.14159,3.14159,"#phi(E_{T}^{miss})");

  h_QcdShapeRegion = HConfig.GetTH1D(Name+"_CatInclusiveQcdShapeRegion","CatInclusiveQcdShapeRegion",100,0.,200.,"Incl: m_{inv}^{QCD}/GeV");

  h_embeddingWeight_TauSpinner = HConfig.GetTH1D(Name+"_embeddingWeight_TauSpinner","embeddingWeight_TauSpinner",50,0.,3.,"emb. TauSpinnerWeight");
  h_embeddingWeight_MinVisPtFilter = HConfig.GetTH1D(Name+"_embeddingWeight_MinVisPtFilter","embeddingWeight_MinVisPtFilter",50,0.,3.,"emb. MinVisPtFilter weight");
  h_embeddingWeight_SelEffWeight = HConfig.GetTH1D(Name+"_embeddingWeight_SelEffWeight","embeddingWeight_SelEffWeight",50,0.,3.,"emb. SelEffWeight");
  h_HiggsGenPtWeight = HConfig.GetTH1D(Name+"_higgsPtWeight","higgsPtWeight",50,0.3,1.3,"higgsPtWeight");
  h_HiggsGenPt = HConfig.GetTH1D(Name+"_higgsGenPt","higgsGenPt",50,0.,200.,"p_{T}(H_{gen})/GeV");
  h_HiggsMassFromSampleName  = HConfig.GetTH1D(Name+"_HiggsMassFromMCID","HiggsMassFromMCID",40,82.5,182.5,"m_{MCID}(H)/GeV");

  h_visibleMass = HConfig.GetTH1D(Name+"_visibleMass","visibleMass",100,0.,200.,"m_{vis}(#tau_{h},#mu)/GeV");

  h_shape_VisM = HConfig.GetTH1D(Name+"_shape_VisM","shape_VisM",400,0.,400.,"m_{vis}(#tau_{h},#mu)/GeV");
  h_shape_SVfitM = HConfig.GetTH1D(Name+"_shape_SVfitM","shape_SVfitM",400,0.,400.,"m_{SVfit}(#tau_{h},#mu)/GeV");

  h_shape_VisM_ZLScaleUp 		= HConfig.GetTH1D(Name+"_shape_VisM_ZLScaleUp",		"shape_VisM_ZLScaleUp",		400,0.,400.,"m_{vis}^{ZL up}(#tau_{h},#mu)/GeV");
  h_shape_VisM_ZLScaleDown 	= HConfig.GetTH1D(Name+"_shape_VisM_ZLScaleDown",	"shape_VisM_ZLScaleDown",	400,0.,400.,"m_{vis}^{ZL down}(#tau_{h},#mu)/GeV");
  h_shape_SVfitM_ZLScaleUp 	= HConfig.GetTH1D(Name+"_shape_SVfitM_ZLScaleUp",	"shape_SVfitM_ZLScaleUp",	400,0.,400.,"m_{SVfit}^{ZL up}(#tau_{h},#mu)/GeV");
  h_shape_SVfitM_ZLScaleDown 	= HConfig.GetTH1D(Name+"_shape_SVfitM_ZLScaleDown",	"shape_SVfitM_ZLScaleDown",	400,0.,400.,"m_{SVfit}^{ZL down}(#tau_{h},#mu)/GeV");

  h_shape_VisM_TauESUp     = HConfig.GetTH1D(Name+"_shape_VisM_TauESUp",		"shape_VisM_TauESUp",		400,0.,400.,"m_{vis}^{#tauES up}(#tau_{h},#mu)/GeV");
  h_shape_VisM_TauESDown   = HConfig.GetTH1D(Name+"_shape_VisM_TauESDown",	"shape_VisM_TauESDown",		400,0.,400.,"m_{vis}^{#tauES down}(#tau_{h},#mu)/GeV");
  h_shape_SVfitM_TauESUp   = HConfig.GetTH1D(Name+"_shape_SVfitM_TauESUp",	"shape_SVfitM_TauESUp",		400,0.,400.,"m_{SVfit}^{#tauES up}(#tau_{h},#mu)/GeV");
  h_shape_SVfitM_TauESDown = HConfig.GetTH1D(Name+"_shape_SVfitM_TauESDown",	"shape_SVfitM_TauESDown",	400,0.,400.,"m_{SVfit}^{#tauES down}(#tau_{h},#mu)/GeV");

  h_SVFitTimeReal = HConfig.GetTH1D(Name+"_SVFitTimeReal","SVFitTimeReal",200,0.,60.,"t_{real}(SVFit)/sec");
  h_SVFitTimeCPU =  HConfig.GetTH1D(Name+"_SVFitTimeCPU","SVFitTimeCPU",200,0.,60.,"t_{CPU}(SVFit)/sec");
  h_SVFitStatus = HConfig.GetTH1D(Name+"_SVFitStatus", "SVFitStatus", 5, -0.5, 4.5, "Status of SVFit calculation");

  if (mode == ANALYSIS) { // only apply scale factors on analysis level, not for combine
	  RSF = new ReferenceScaleFactors(runtype, false, false, true);
  }
}

void HToTaumuTauh::Configure(){
  Logger(Logger::Verbose) << "Start." << std::endl;
  Setup();
  Selection::ConfigureHistograms();
  HConfig.GetHistoInfo(types, CrossSectionandAcceptance, legend, colour);
}


void  HToTaumuTauh::Store_ExtraDist(){
 Logger(Logger::Verbose) << "Start." << std::endl;
 Extradist1d.push_back(&h_NVtx);
 Extradist1d.push_back(&h_VtxZ);
 Extradist1d.push_back(&h_VtxRho);
 Extradist1d.push_back(&h_VtxNdof);
 Extradist1d.push_back(&h_VtxIsfake);

 Extradist1d.push_back(&h_MuDxy);
 Extradist1d.push_back(&h_MuDz );
 Extradist1d.push_back(&h_MuRelIso);
 Extradist1d.push_back(&h_MuPt  );
 Extradist1d.push_back(&h_MuEta  );
 Extradist1d.push_back(&h_MuPhi  );

 Extradist1d.push_back(&h_MuSelPt  );
 Extradist1d.push_back(&h_MuSelEta  );
 Extradist1d.push_back(&h_MuSelPhi  );
 Extradist1d.push_back(&h_MuSelDxy  );
 Extradist1d.push_back(&h_MuSelDz   );
 Extradist1d.push_back(&h_MuSelRelIso);
 Extradist1d.push_back(&h_MuSelFakesTauID  );

 Extradist1d.push_back(&h_TauPt  );
 Extradist1d.push_back(&h_TauEta  );
 Extradist1d.push_back(&h_TauPhi  );
 Extradist1d.push_back(&h_TauDecayMode  );
 Extradist1d.push_back(&h_TauIso );
 Extradist1d.push_back(&h_TauSelMass );

 Extradist1d.push_back(&h_TauSelPt  );
 Extradist1d.push_back(&h_TauSelEta  );
 Extradist1d.push_back(&h_TauSelPhi  );
 Extradist1d.push_back(&h_TauSelDecayMode  );
 Extradist1d.push_back(&h_TauSelIso );

 Extradist1d.push_back(&h_MuVetoDPtSelMuon);
 Extradist1d.push_back(&h_MuVetoInvM);
 Extradist1d.push_back(&h_MuVetoPtPositive);
 Extradist1d.push_back(&h_MuVetoPtNegative);
 Extradist1d.push_back(&h_MuVetoDRTau);
 Extradist1d.push_back(&h_MuVetoDeltaR);

 Extradist1d.push_back(&h_NMuonTriLepVeto);
 Extradist1d.push_back(&h_NElecTriLepVeto);

 Extradist1d.push_back(&h_MuCharge  );
 Extradist1d.push_back(&h_TauCharge  );

 Extradist1d.push_back(&h_MuTauDR);
 Extradist1d.push_back(&h_MuTauDPhi);
 Extradist1d.push_back(&h_MuTauDEta);
 Extradist1d.push_back(&h_MuTauDPt);
 Extradist1d.push_back(&h_MuTauRelDPt);
 Extradist2d.push_back(&h_MuPtVsTauPt);

 Extradist1d.push_back(&h_MetPt);
 Extradist1d.push_back(&h_MetPhi);

 Extradist1d.push_back(&h_MetLepMuDr);
 Extradist1d.push_back(&h_MetLepTauDr);
 Extradist1d.push_back(&h_MetLepNMu);
 Extradist1d.push_back(&h_MetLepNTau);
 Extradist1d.push_back(&h_MetLepNMuMinusNMu);
 Extradist1d.push_back(&h_MetLepNTauMinusNTau);
 Extradist1d.push_back(&h_MetLepDiffMET);
 Extradist1d.push_back(&h_MetLepDiffMETPhi);
 Extradist1d.push_back(&h_MetLepDiffMt);

 Extradist1d.push_back(&h_NJetsKin);
 Extradist1d.push_back(&h_JetKin1Pt);
 Extradist1d.push_back(&h_JetKin1Eta);
 Extradist1d.push_back(&h_JetKin1Phi);
 Extradist1d.push_back(&h_JetKin1IsLooseId);
 Extradist1d.push_back(&h_JetKin2IsLooseId);
 Extradist1d.push_back(&h_JetKin2Pt);
 Extradist1d.push_back(&h_JetKin2Eta);
 Extradist1d.push_back(&h_JetKin2Phi);
 Extradist1d.push_back(&h_NJetsId);
 Extradist1d.push_back(&h_Jet1Pt);
 Extradist1d.push_back(&h_Jet1Eta);
 Extradist1d.push_back(&h_Jet1Phi);
 Extradist1d.push_back(&h_Jet1IsB);
 Extradist1d.push_back(&h_Jet2Pt);
 Extradist1d.push_back(&h_Jet2Eta);
 Extradist1d.push_back(&h_Jet2Phi);
 Extradist1d.push_back(&h_Jet2IsB);

 Extradist1d.push_back(&h_NBJets);
 Extradist1d.push_back(&h_BJet1Pt);
 Extradist1d.push_back(&h_BJet1Eta);
 Extradist1d.push_back(&h_BJet1Phi);

 Extradist1d.push_back(&h_HiggsPt);
 Extradist1d.push_back(&h_HiggsPhi);
 Extradist1d.push_back(&h_JetsDEta);
 Extradist1d.push_back(&h_JetsInEtaGap);
 Extradist1d.push_back(&h_JetsInvM);

 Extradist1d.push_back(&h_MtMuPlusOnly);
 Extradist1d.push_back(&h_MtMuMinusOnly);
 Extradist1d.push_back(&h_Mt1ProngOnly);
 Extradist1d.push_back(&h_Mt3ProngOnly);
 Extradist1d.push_back(&h_Mt3ProngSV);
 Extradist1d.push_back(&h_Mt3ProngSVFlight);

 Extradist1d.push_back(&h_MetPt1ProngOnly);
 Extradist1d.push_back(&h_MetPhi1ProngOnly);
 Extradist1d.push_back(&h_MetPt3ProngOnly);
 Extradist1d.push_back(&h_MetPhi3ProngOnly);

 Extradist1d.push_back(&h_MetPtNoMtCut);
 Extradist1d.push_back(&h_MetPhiNoMtCut);
 Extradist1d.push_back(&h_MetPtNoMtCut1ProngOnly);
 Extradist1d.push_back(&h_MetPhiNoMtCut1ProngOnly);
 Extradist1d.push_back(&h_MetPtNoMtCut3ProngOnly);
 Extradist1d.push_back(&h_MetPhiNoMtCut3ProngOnly);

 Extradist1d.push_back(&h_QcdShapeRegion);

 Extradist1d.push_back(&h_embeddingWeight_TauSpinner);
 Extradist1d.push_back(&h_embeddingWeight_MinVisPtFilter);
 Extradist1d.push_back(&h_embeddingWeight_SelEffWeight);
 Extradist1d.push_back(&h_HiggsGenPtWeight);
 Extradist1d.push_back(&h_HiggsGenPt);
 Extradist1d.push_back(&h_HiggsMassFromSampleName);

 Extradist1d.push_back(&h_visibleMass);

 Extradist1d.push_back(&h_shape_VisM);
 Extradist1d.push_back(&h_shape_SVfitM);

 Extradist1d.push_back(&h_shape_VisM_ZLScaleUp);
 Extradist1d.push_back(&h_shape_VisM_ZLScaleDown);
 Extradist1d.push_back(&h_shape_SVfitM_ZLScaleUp);
 Extradist1d.push_back(&h_shape_SVfitM_ZLScaleDown);

 Extradist1d.push_back(&h_shape_VisM_TauESUp    );
 Extradist1d.push_back(&h_shape_VisM_TauESDown  );
 Extradist1d.push_back(&h_shape_SVfitM_TauESUp  );
 Extradist1d.push_back(&h_shape_SVfitM_TauESDown);

 Extradist1d.push_back(&h_SVFitTimeReal);
 Extradist1d.push_back(&h_SVFitTimeCPU);
 Extradist1d.push_back(&h_SVFitStatus);
}

void HToTaumuTauh::doEvent(){
	Logger(Logger::Verbose) << ">>>>>>>>>>>>>>>>" << std::endl;
	Logger(Logger::Verbose) << "\tCategory: " << categoryFlag << std::endl;

	// setup event
	doEventSetup();

	// run selection part of analysis
	doSelection(true);

	// do plotting part of analysis
	doPlotting();
}

void HToTaumuTauh::doEventSetup(){
	Logger(Logger::Verbose) << std::endl;

	clock->Reset(); // reset all benchmark clocks

	// set variables to default values
	selVertex = -1;
	selMuon = -1;
	selTau = -1;
	higgs_GenPtWeight = -999;
	higgs_GenPt = -999;
	higgsPhi = -999;
	diMuonNeg = -1;
	diMuonPos = -1;
	// set categorization variables to default values
	nJets_ = 999;
	tauPt_ = -999;
	higgsPt_ = -999;
	jetdEta_ = -999;
	nJetsInGap_ = -999;
	mjj_ = -999;
	passedVBFTight_ = false;
	passedVBF_ = false;
	// set all analysis status booleans to false
	setStatusBooleans(true);
	status = false;
	isWJetMC = false;
	isQCDShapeEvent = false;
	isSignal = false;
	// clear all vectors
	selectedMuonsId.clear();
	selectedMuons.clear();
	antiIsoMuons.clear();
	selectedTausId.clear();
	selectedTausIso.clear();
	selectedTaus.clear();
	diMuonVetoMuonsPositive.clear();
	diMuonVetoMuonsNegative.clear();
	relaxedIsoTaus.clear();
	triLepVetoMuons.clear();
	triLepVetoElecs.clear();
	selectedJetsClean.clear();
	selectedJetsKin.clear();
	selectedJets.clear();
	selectedBJets.clear();

	// determine event type
	id = Ntp->GetMCID();
	idStripped = Ntp->GetStrippedMCID();
	if(!HConfig.GetHisto(Ntp->isData(),id,t)){ Logger(Logger::Warning) << "failed to find id" <<std::endl; return;}

	// set event weights
	wobs = 1;
	if(!Ntp->isData() && Ntp->GetMCID() != DataMCType::DY_mutau_embedded){
		  w = Ntp->PUWeightFineBins();}
	else{w=1;}

	// set object corrections at beginning of each event to avoid segfaults
	// and to allow for using different corrections in different analyses
	isSignal = ((idStripped >= 10 && idStripped <= 13) || (idStripped >= 30 && idStripped <= 35)) ? true : false;
	if (isSignal) Ntp->SetTauCorrections(correctTaus);
	else			Ntp->SetTauCorrections("");
	Ntp->SetMuonCorrections(correctMuons);
	Ntp->SetElecCorrections(correctElecs);
	Ntp->SetJetCorrections(correctJets);
}

void HToTaumuTauh::doSelection(bool runAnalysisCuts){
	Logger(Logger::Verbose) << std::endl;
	// Vertex
	Logger(Logger::Debug) << "Cut: Vertex" << std::endl;
	unsigned int nGoodVtx=0;
	for(unsigned int i_vtx=0;i_vtx<Ntp->NVtx();i_vtx++){
	  if(Ntp->isGoodVtx(i_vtx)){
	  	if(selVertex == -1) selVertex = i_vtx; // selected vertex = first vertex (highest sum[pT^2]) to fulfill vertex requirements
	  	nGoodVtx++;
	  }
	}

	value.at(PrimeVtx)=nGoodVtx;
	pass.at(PrimeVtx)=(value.at(PrimeVtx)>=cut.at(PrimeVtx));

	// Trigger
	Logger(Logger::Debug) << "Cut: Trigger" << std::endl;
	value.at(TriggerOk) = -1;
	for (std::vector<TString>::iterator it_trig = cTriggerNames.begin(); it_trig != cTriggerNames.end(); ++it_trig){
	  if(Ntp->TriggerAccept(*it_trig)){
		  if ( value.at(TriggerOk) == -1 )
			  value.at(TriggerOk) = it_trig - cTriggerNames.begin();
		  else // more than 1 trigger fired, save this separately
			  value.at(TriggerOk) = cTriggerNames.size();
	  }
	}
	pass.at(TriggerOk) = (value.at(TriggerOk) >= cut.at(TriggerOk));
	// disable trigger for embedding
	if (idStripped == DataMCType::DY_mutau_embedded) pass.at(TriggerOk) = true;

	// Muon cuts
	Logger(Logger::Debug) << "Cut: Muon ID" << std::endl;
	for(unsigned i_mu=0;i_mu<Ntp->NMuons();i_mu++){
	  if( selectMuon_Id(i_mu,selVertex) ) {
		  selectedMuonsId.push_back(i_mu);
	  }
	}
	value.at(NMuId)=selectedMuonsId.size();
	pass.at(NMuId)=(value.at(NMuId)>=cut.at(NMuId));

	Logger(Logger::Debug) << "Cut: Muon Kinematics" << std::endl;
	for(std::vector<int>::iterator it_mu = selectedMuonsId.begin(); it_mu != selectedMuonsId.end(); ++it_mu){
	  if( selectMuon_Kinematics(*it_mu)) {
		  selectedMuons.push_back(*it_mu);
	  }
	}
	value.at(NMuKin)=selectedMuons.size();
	pass.at(NMuKin)=(value.at(NMuKin)>=cut.at(NMuKin));

	// muons for QCD background method
	Logger(Logger::Debug) << "QCD Muons" << std::endl;
	for(unsigned i_mu=0;i_mu<Ntp->NMuons();i_mu++){
	  if( selectMuon_antiIso(i_mu,selVertex) ) {
		  antiIsoMuons.push_back(i_mu);
	  }
	}
	hasAntiIsoMuon = (antiIsoMuons.size() > 0);

	Logger(Logger::Debug) << "select Muon" << std::endl;
	if (selectedMuons.size() > 0)
	  selMuon = selectedMuons.at(0); // use signal muon
	if (selectedMuons.size() == 0 && hasAntiIsoMuon)
	  selMuon = antiIsoMuons.at(0); // for background methods: use anti-iso muon

	Logger(Logger::Debug) << "Cut: Di-muon Veto" << std::endl;
	for(unsigned i=0;i<Ntp->NMuons();i++){
	  if( selectMuon_diMuonVeto(i, selVertex) ) {
		  if (Ntp->Muon_Charge(i) == 1) {
			  diMuonVetoMuonsPositive.push_back(i); // be aware that there are -999 cases for charge track matching
		  }
		  else if (Ntp->Muon_Charge(i) == -1) {
			  diMuonVetoMuonsNegative.push_back(i);
		  }
	  }
	}
	if (diMuonVetoMuonsPositive.size() == 0 || diMuonVetoMuonsNegative.size() == 0){
	  value.at(DiMuonVeto) = 0.0;
	}
	else{
	  double dRmax(0.0);
	  for (std::vector<int>::iterator it_mup = diMuonVetoMuonsPositive.begin(); it_mup != diMuonVetoMuonsPositive.end(); ++it_mup){
		  for (std::vector<int>::iterator it_mun = diMuonVetoMuonsNegative.begin(); it_mun != diMuonVetoMuonsNegative.end(); ++it_mun){
			  double dr = Ntp->Muon_p4(*it_mup).DeltaR( Ntp->Muon_p4(*it_mun) );
			  if (dr > dRmax){
				  dRmax = dr;
				  diMuonPos = *it_mup;
				  diMuonNeg = *it_mun;
			  }
		  }
	  }
	  value.at(DiMuonVeto) = dRmax;
	}
	pass.at(DiMuonVeto) = (value.at(DiMuonVeto) < cut.at(DiMuonVeto));

	// Tau cuts
	Logger(Logger::Debug) << "Cut: Tau ID" << std::endl;
	for(unsigned i_tau=0; i_tau < Ntp->NPFTaus(); i_tau++){
	  if ( selectPFTau_Id(i_tau,selectedMuonsId) ){
		  selectedTausId.push_back(i_tau);
	  }
	}
	value.at(NTauId)=selectedTausId.size();
	pass.at(NTauId)=(value.at(NTauId)>=cut.at(NTauId));

	Logger(Logger::Debug) << "Cut: Tau Iso" << std::endl;
	for(std::vector<int>::iterator it_tau = selectedTausId.begin(); it_tau != selectedTausId.end(); ++it_tau){
	  if ( selectPFTau_Iso(*it_tau) ){
		  selectedTausIso.push_back(*it_tau);
	  }
	}
	value.at(NTauIso)=selectedTausIso.size();
	pass.at(NTauIso)=(value.at(NTauIso)>=cut.at(NTauIso));

	Logger(Logger::Debug) << "Cut: Tau Kinematics" << std::endl;
	for(std::vector<int>::iterator it_tau = selectedTausIso.begin(); it_tau != selectedTausIso.end(); ++it_tau){
	  if ( selectPFTau_Kinematics(*it_tau) ){
		  selectedTaus.push_back(*it_tau);
	  }
	}
	value.at(NTauKin)=selectedTaus.size();
	pass.at(NTauKin)=(value.at(NTauKin)>=cut.at(NTauKin));

	// taus for QCD background method
	Logger(Logger::Debug) << "QCD Taus" << std::endl;
	for(unsigned i_tau=0; i_tau < Ntp->NPFTaus(); i_tau++){
	  if ( selectPFTau_relaxedIso(i_tau,selectedMuonsId) ){
		  relaxedIsoTaus.push_back(i_tau);
	  }
	}
	hasRelaxedIsoTau = (relaxedIsoTaus.size() > 0);

	Logger(Logger::Debug) << "select Tau" << std::endl;
	if(selectedTaus.size() > 0)
	  selTau = selectedTaus.at(0); // use signal tau
	if(selectedTaus.size() == 0 && hasRelaxedIsoTau)
	  selTau = relaxedIsoTaus.at(0); // relaxed isolation tau

	// Tri-lepton veto
	Logger(Logger::Debug) << "Cut: Tri-lepton veto" << std::endl;
	for(unsigned i_mu=0;i_mu<Ntp->NMuons();i_mu++){
	  if( selectMuon_triLeptonVeto(i_mu,selMuon,selVertex) ) {
		  triLepVetoMuons.push_back(i_mu);
	  }
	}
	for(unsigned i_el=0;i_el<Ntp->NElectrons();i_el++){
	  if( selectElectron_triLeptonVeto(i_el,selVertex,selectedMuonsId) ) {
		  triLepVetoElecs.push_back(i_el);
	  }
	}
	value.at(TriLeptonVeto) = triLepVetoMuons.size() + triLepVetoElecs.size();
	pass.at(TriLeptonVeto) = (value.at(TriLeptonVeto) <= cut.at(TriLeptonVeto));

	// Opposite charge
	Logger(Logger::Debug) << "Cut: Opposite Charge" << std::endl;
	if (selMuon != -1 && selTau != -1){
	  value.at(OppCharge) = Ntp->Muon_Charge(selMuon) + Ntp->PFTau_Charge(selTau);
	}
	else {
	  value.at(OppCharge) = -9;
	  pass.at(OppCharge) = true;
	}
	if (cut.at(OppCharge) == 999) // set to 999 to disable oppcharge cut
	  pass.at(OppCharge) = true;
	else
	  pass.at(OppCharge) = (value.at(OppCharge) == cut.at(OppCharge));

	// Transverse mass
	Logger(Logger::Debug) << "Cut: transverse mass" << std::endl;
	if(selMuon == -1){ // no good muon in event: set MT to small dummy value -10 -> pass cut
	  value.at(MT) = -10.0;
	}
	else{
	  double pT 	= Ntp->Muon_p4(selMuon).Pt();
	  double phi	= Ntp->Muon_p4(selMuon).Phi();
	  double eTmiss = Ntp->MET_CorrMVAMuTau_et();
	  double eTmPhi = Ntp->MET_CorrMVAMuTau_phi();
	  value.at(MT)	= Ntp->transverseMass(pT,phi,eTmiss,eTmPhi);
	}
	if (cut.at(MT) == 999) // set to 999 to disable mt cut
	  pass.at(MT) = true;
	else
	  pass.at(MT) = (value.at(MT) < cut.at(MT));

	// sort jets by corrected pt
	Logger(Logger::Debug) << "select Jets" << std::endl;
	std::vector<int> sortedPFJets = Ntp->sortDefaultObjectsByPt("Jets");
	// select jets for categories
	// PFJet and bjet collections can have mutual elements!
	for (unsigned i_jet = 0; i_jet < Ntp->NPFJets(); i_jet++){
	  if ( selectPFJet_Cleaning(sortedPFJets.at(i_jet), selMuon, selTau)){
		  selectedJetsClean.push_back(sortedPFJets.at(i_jet));
		  if ( selectPFJet_Kinematics(sortedPFJets.at(i_jet)) ) {
			  selectedJetsKin.push_back(sortedPFJets.at(i_jet));
			  if ( selectPFJet_Id(sortedPFJets.at(i_jet)) ){
				  selectedJets.push_back(sortedPFJets.at(i_jet));
			  }
		  }
	  }
	  if ( selectBJet(sortedPFJets.at(i_jet), selMuon, selTau) ) {
		  selectedBJets.push_back(sortedPFJets.at(i_jet));
	  }
	}

	// looser jet selection (for QCD background method)
	std::vector<int> selectedLooseJets;
	for (unsigned i_jet = 0; i_jet < Ntp->NPFJets(); i_jet++){
	  if ( selectPFJet_Relaxed(sortedPFJets.at(i_jet), selMuon, selTau) ){
			selectedLooseJets.push_back(sortedPFJets.at(i_jet));
	  }
	}

	// QCD background method
	isQCDShapeEvent = false;
	if (qcdShapeFromData && Ntp->isData() && idStripped!=DataMCType::DY_mutau_embedded){
	  Logger(Logger::Debug) << "QCD shape" << std::endl;
	  // use anti-iso muons and SS for QCD shape
	  if( !passedMu && hasAntiIsoMuon && !pass.at(OppCharge)){
		 // use this data event for QCD shape
		 isQCDShapeEvent = true;
		 if (!HConfig.GetHisto(false, DataMCType::QCD, t)) {Logger(Logger::Error) << "failed to find id " << DataMCType::QCD << std::endl; return;}
	  	 pass.at(OppCharge) = true;
	  	 pass.at(NMuId) = true;
	  	 pass.at(NMuKin) = true;

	     // loosen jet requirement for VBF categories
	     if (categoryFlag == "VBFLoose" || categoryFlag == "VBFTight"){
	    	 selectedJets = selectedLooseJets;
		 }
	     // loosen tau isolation for one jet boost category
	     if (categoryFlag == "OneJetBoost"){
			 pass.at(NTauIso) = hasRelaxedIsoTau;
			 pass.at(NTauKin) = hasRelaxedIsoTau;
	     }
	 }
	}

	// b-Jet veto
	Logger(Logger::Debug) << "Cut: b-jet veto" << std::endl;
	value.at(BJetVeto) = selectedBJets.size();
	pass.at(BJetVeto) = (value.at(BJetVeto) <= cut.at(BJetVeto));

	// store pt of selected tau for categories
	if (selTau != -1){
	  tauPt_ = Ntp->PFTau_p4(selTau).Pt();
	}

	// calculate pt of higgs candidate
	Logger(Logger::Debug) << "calculate Higgs pT" << std::endl;
	if (selMuon != -1 && selTau != -1){
	  TVector3 muon3Vec = Ntp->Muon_p4(selMuon).Vect();
	  TVector3 tau3Vec = Ntp->PFTau_p4(selTau).Vect();
	  TVector3 met3Vec = TVector3(Ntp->MET_CorrMVAMuTau_ex(),Ntp->MET_CorrMVAMuTau_ey(),0);

	  higgsPt_ = (muon3Vec + tau3Vec + met3Vec).Pt();
	  higgsPhi = (muon3Vec + tau3Vec + met3Vec).Phi();
	}

	// calculate jet-related variables used by categories
	Logger(Logger::Debug) << "calculate VBF Jet variables" << std::endl;
	nJets_ = selectedJets.size();

	if (nJets_ >= 2){
	  double vbfJetEta1 = Ntp->PFJet_p4(selectedJets.at(0)).Eta();
	  double vbfJetEta2 = Ntp->PFJet_p4(selectedJets.at(1)).Eta();
	  jetdEta_ = vbfJetEta1 - vbfJetEta2;

	  int jetsInRapidityGap = 0;
	  for(std::vector<int>::iterator it_jet = selectedJets.begin()+2; it_jet != selectedJets.end(); ++it_jet){
		  double etaPos = ( jetdEta_ >= 0) ? vbfJetEta1 : vbfJetEta2;
		  double etaNeg = ( jetdEta_ >= 0) ? vbfJetEta2 : vbfJetEta1;
		  if (	Ntp->PFJet_p4(*it_jet).Eta() > etaNeg &&
				Ntp->PFJet_p4(*it_jet).Eta() < etaPos){
			  jetsInRapidityGap++;
		  }
	  }
	  nJetsInGap_ = jetsInRapidityGap;

	  double invM = (Ntp->PFJet_p4(selectedJets.at(0)) + Ntp->PFJet_p4(selectedJets.at(1))).M();
	  mjj_ = invM;
	}
	else{
	  jetdEta_ = -100;
	  nJetsInGap_ = -1;
	  mjj_ = -1;
	}

	// correction factors
	if( !Ntp->isData() ){
	  // apply trigger efficiencies
	  if (selMuon != -1) w *= RSF->HiggsTauTau_MuTau_Trigger_Mu_ScaleMCtoData(Ntp->Muon_p4(selMuon));
	  if (selTau != -1)  w *= RSF->HiggsTauTau_MuTau_Trigger_Tau_ScaleMCtoData(Ntp->PFTau_p4(selTau, "")); // no Tau energy scale here
	  // apply muon ID & iso scale factors
	  if (selMuon != -1){
		  w *= RSF->HiggsTauTau_MuTau_Id_Mu(Ntp->Muon_p4(selMuon));
		  w *= RSF->HiggsTauTau_MuTau_Iso_Mu(Ntp->Muon_p4(selMuon));
	  }
	  // tau decay mode scale factors
	  // https://twiki.cern.ch/twiki/bin/viewauth/CMS/HiggsToTauTauWorkingSummer2013#TauES_and_decay_mode_scale_facto
	  if (selTau != -1){
		  if(isSignal && Ntp->PFTau_hpsDecayMode(selTau) == 0) w *= 0.88;
	  }
	  // todo: b-tag scale factors
	  // https://twiki.cern.ch/twiki/bin/viewauth/CMS/HiggsToTauTauWorkingSummer2013#B_tag_scale_factors
	}
	// embedding weights (see Tau Meeting 05.01.2015, slide 29)
	if(idStripped == DataMCType::DY_mutau_embedded){
	  // embedding weights
	  w *= Ntp->Embedding_TauSpinnerWeight();
	  w *= Ntp->Embedding_MinVisPtFilter();
	  //w *= Ntp->Embedding_SelEffWeight(); // do NOT apply this weight for now, as it might not be modelled well enough
	  // muon weights (apply muon ID weight, but not muon Iso weight)
	  if (selMuon != -1) w *= RSF->HiggsTauTau_MuTau_Id_Mu(Ntp->Muon_p4(selMuon));
	  // tau ES
	  if (selTau != -1 && Ntp->PFTau_hpsDecayMode(selTau) == 0) w *= 0.88;
	  // apply data trigger efficiency to embedding
	  if (selMuon != -1) w *= RSF->HiggsTauTau_MuTau_Trigger_Mu_Eff_Data(Ntp->Muon_p4(selMuon));
	  if (selTau != -1)  w *= RSF->HiggsTauTau_MuTau_Trigger_Tau_Eff_Data(Ntp->PFTau_p4(selTau));
	}
	// Higgs pT reweighting

	if (idStripped >= DataMCType::H_tautau && idStripped <= DataMCType::H_tautau_WHZHTTH) {
		  for (unsigned i_gen = 0; i_gen < Ntp->NMCParticles(); i_gen++) {
		  	  if (Ntp->MCParticle_pdgid(i_gen) == PDGInfo::Higgs0) {
		  		  TLorentzVector genH_p4 = Ntp->MCParticle_p4(i_gen);
		  		  higgs_GenPtWeight = RSF->HiggsPtWeight(genH_p4, Ntp->getSampleHiggsMass());
		  		  higgs_GenPt = genH_p4.Pt();
		  		  w *= higgs_GenPtWeight;
		  	  }
		  }
	}

	// define booleans for different stages of selection
	setStatusBooleans();

	// remove some cuts for smoother WJet shape
	Logger(Logger::Debug) << "WJet shape" << std::endl;
	isWJetMC = (idStripped >= DataMCType::W_lnu) && (idStripped <= DataMCType::W_taunu);
	bool isWJetShapeEvent =  (wJetsBGSource == "Data") && isWJetMC; // overwrite pass-vector with relaxed categories (for WJets shape) only if wanted
	if (isWJetShapeEvent) {
	  // disable OS requirement for WJets shape in VBFLoose, VBFTight and 1JetBoost categories
	  if (categoryFlag == "VBFLoose" || categoryFlag == "VBFTight" || categoryFlag == "OneJetBoost")
		  pass.at(OppCharge) = true;
	  // relax PFTau isolation for WJets shape
	  if (categoryFlag == "VBFTight" || categoryFlag == "OneJetBoost") {
		  pass.at(NTauIso) = hasRelaxedIsoTau;
		  pass.at(NTauKin) = hasRelaxedIsoTau;
	  }
	  // relaxed category definitions are run further below
	}

	// re-define booleans as they might have changed for background methods
	setStatusBooleans();

	// run relaxed categories for background methods
	// VBFTight: full category selection for shape in WJets, relaxed in QCD
	passed_VBFTightRelaxed = helperCategory_VBFTightRelaxed(isQCDShapeEvent, nJets_, jetdEta_, nJetsInGap_, mjj_, higgsPt_);
	// VBFLoose: relaxed category selection for shape in both WJets and QCD
	passed_VBFLooseRelaxed = helperCategory_VBFLooseRelaxed(isWJetShapeEvent || isQCDShapeEvent, nJets_, jetdEta_, nJetsInGap_, mjj_);

	if (runAnalysisCuts)	status = AnalysisCuts(t,w,wobs);	// fill plots for framework
	else					status = Passed();					// make sure plots are filled somewhere else (e.g. in a derived class)
}

void HToTaumuTauh::doPlotting(){
	Logger(Logger::Verbose) << std::endl;
	//////// fill most plots after full selection
	if (status) {
		// Vertex plots
		h_NVtx.at(t).Fill(Ntp->NVtx(), w);
		for (unsigned int i_vtx = 0; i_vtx < Ntp->NVtx(); i_vtx++) {
			h_VtxZ.at(t).Fill(Ntp->Vtx(i_vtx).z(), w);
			h_VtxRho.at(t).Fill(sqrt(Ntp->Vtx(i_vtx).x() * Ntp->Vtx(i_vtx).x() + Ntp->Vtx(i_vtx).y() * Ntp->Vtx(i_vtx).y()), w);
			h_VtxNdof.at(t).Fill(Ntp->Vtx_ndof(i_vtx), w);
			h_VtxIsfake.at(t).Fill(Ntp->Vtx_isFake(i_vtx), w);
		}

		//// Object selection
		// Muons
		// plots filled with all selected muons
		for (std::vector<int>::iterator it_mu = selectedMuonsId.begin(); it_mu != selectedMuonsId.end(); ++it_mu) {
			h_MuPt.at(t).Fill(Ntp->Muon_p4(*it_mu).Pt(), w);
			h_MuEta.at(t).Fill(Ntp->Muon_p4(*it_mu).Eta(), w);
			h_MuPhi.at(t).Fill(Ntp->Muon_p4(*it_mu).Phi(), w);
			h_MuDxy.at(t).Fill(Ntp->dxySigned(Ntp->Muon_p4(*it_mu), Ntp->Muon_Poca(*it_mu), Ntp->Vtx(selVertex)), w);
			h_MuDz.at(t).Fill(Ntp->dzSigned(Ntp->Muon_p4(*it_mu), Ntp->Muon_Poca(*it_mu), Ntp->Vtx(selVertex)), w);
			h_MuRelIso.at(t).Fill(Ntp->Muon_RelIso(*it_mu), w);
		}
		// plots filled only with selected muon
		h_MuSelPt.at(t).Fill(Ntp->Muon_p4(selMuon).Pt(), w);
		h_MuSelEta.at(t).Fill(Ntp->Muon_p4(selMuon).Eta(), w);
		h_MuSelPhi.at(t).Fill(Ntp->Muon_p4(selMuon).Phi(), w);
		h_MuSelDxy.at(t).Fill(Ntp->dxySigned(Ntp->Muon_p4(selMuon), Ntp->Muon_Poca(selMuon), Ntp->Vtx(selVertex)), w);
		h_MuSelDz.at(t).Fill(Ntp->dzSigned(Ntp->Muon_p4(selMuon), Ntp->Muon_Poca(selMuon), Ntp->Vtx(selVertex)), w);
		h_MuSelRelIso.at(t).Fill(Ntp->Muon_RelIso(selMuon), w);
		// Does the muon fake the tau_ID+Iso?
		bool fakes = false;
		for (unsigned i_tau = 0; i_tau < Ntp->NPFTaus(); i_tau++) {
			if (selectPFTau_Id(i_tau) && selectPFTau_Iso(i_tau) && Ntp->Muon_p4(selMuon).DeltaR(Ntp->PFTau_p4(i_tau)) < cMuTau_dR) {
				fakes = true;
				break;
			}
		}
		h_MuSelFakesTauID.at(t).Fill(fakes, w);

		// Taus
		// plots filled with all selected Taus
		for (std::vector<int>::iterator it_tau = selectedTaus.begin(); it_tau != selectedTaus.end(); ++it_tau) {
			h_TauPt.at(t).Fill(Ntp->PFTau_p4(*it_tau).Pt(), w);
			h_TauEta.at(t).Fill(Ntp->PFTau_p4(*it_tau).Eta(), w);
			h_TauPhi.at(t).Fill(Ntp->PFTau_p4(*it_tau).Phi(), w);
			h_TauDecayMode.at(t).Fill(Ntp->PFTau_hpsDecayMode(*it_tau), w);
			h_TauIso.at(t).Fill(Ntp->PFTau_HPSPFTauDiscriminationByRawCombinedIsolationDBSumPtCorr3Hits(*it_tau), w);
		}
		// plots filled only with selected tau
		h_TauSelPt.at(t).Fill(Ntp->PFTau_p4(selTau).Pt(), w);
		h_TauSelEta.at(t).Fill(Ntp->PFTau_p4(selTau).Eta(), w);
		h_TauSelPhi.at(t).Fill(Ntp->PFTau_p4(selTau).Phi(), w);
		h_TauSelDecayMode.at(t).Fill(Ntp->PFTau_hpsDecayMode(selTau), w);
		h_TauSelIso.at(t).Fill(Ntp->PFTau_HPSPFTauDiscriminationByRawCombinedIsolationDBSumPtCorr3Hits(selTau), w);
		h_TauSelMass.at(t).Fill(Ntp->PFTau_p4(selTau).M(), w);

		// Mu-Tau correlations
		h_MuTauDR.at(t).Fill(Ntp->Muon_p4(selMuon).DeltaR(Ntp->PFTau_p4(selTau)), w);
		h_MuTauDPhi.at(t).Fill(Ntp->Muon_p4(selMuon).DeltaPhi(Ntp->PFTau_p4(selTau)), w);
		h_MuTauDEta.at(t).Fill(Ntp->Muon_p4(selMuon).Eta() - Ntp->PFTau_p4(selTau).Eta(), w);
		h_MuTauDPt.at(t).Fill(Ntp->Muon_p4(selMuon).Pt() - Ntp->PFTau_p4(selTau).Pt(), w);
		h_MuTauRelDPt.at(t).Fill((Ntp->Muon_p4(selMuon).Pt() - Ntp->PFTau_p4(selTau).Pt()) / Ntp->Muon_p4(selMuon).Pt(), w);
		h_MuPtVsTauPt.at(t).Fill(Ntp->Muon_p4(selMuon).Pt(), Ntp->PFTau_p4(selTau).Pt(), w);

		// Mu-Tau Mass
		h_visibleMass.at(t).Fill((Ntp->Muon_p4(selMuon) + Ntp->PFTau_p4(selTau)).M(), w);
		// SVFit
		clock->Start("SVFit");
		// get SVFit result from cache
		SVFitObject *svfObj = Ntp->getSVFitResult(svfitstorage, "CorrMVAMuTau", selMuon, selTau);
		clock->Stop("SVFit");

		// shape distributions for final fit
		double visMass = (Ntp->Muon_p4(selMuon) + Ntp->PFTau_p4(selTau)).M();
		h_shape_VisM.at(t).Fill(visMass, w);
		double svfMass = -999;
		if (!svfObj->isValid()) {
			Logger(Logger::Warning) << "SVFit object is invalid. SVFit mass set to -999." << std::endl;
			h_SVFitStatus.at(t).Fill(1);
		} else if (svfObj->get_mass() < visMass) {
			Logger(Logger::Warning) << "SVFit mass " << svfObj->get_mass() << " smaller than visible mass " << visMass << ". SVFit mass SVFit mass set to -999." << std::endl;
			h_SVFitStatus.at(t).Fill(2);
		} else {
			svfMass = svfObj->get_mass();
			h_SVFitStatus.at(t).Fill(0);
		}

		h_shape_SVfitM.at(t).Fill(svfMass, w);

		// ZL shape uncertainty
		if (HConfig.GetID(t) == DataMCType::DY_ll || HConfig.GetID(t) == DataMCType::DY_ee || HConfig.GetID(t) == DataMCType::DY_mumu) {
			h_shape_VisM_ZLScaleUp.at(t).Fill(1.02 * visMass);
			h_shape_VisM_ZLScaleDown.at(t).Fill(0.98 * visMass);
			h_shape_SVfitM_ZLScaleUp.at(t).Fill(1.02 * svfMass);
			h_shape_SVfitM_ZLScaleDown.at(t).Fill(0.98 * svfMass);
		}
/* todo
		// tau energy scale uncertainty
		TLorentzVector tauP4Up = 1.03 * Ntp->PFTau_p4(selTau);
		TLorentzVector tauP4Down = 0.97 * Ntp->PFTau_p4(selTau);
		clock->Start("SVFitTauESUp");
		SVFitObject *svfObjTauESUp = Ntp->getSVFitResult(svfitstorTauESUp, "CorrMVAMuTau", selMuon, selTau, 2000, "TauESUp", 1., 1.03);
		clock->Stop("SVFitTauESUp");
		clock->Start("SVFitTauESDown");
		SVFitObject *svfObjTauESDown = Ntp->getSVFitResult(svfitstorTauESDown, "CorrMVAMuTau", selMuon, selTau, 2000, "TauESDown", 1., 0.97);
		clock->Stop("SVFitTauESDown");

		double visMass_tauESUp = (Ntp->Muon_p4(selMuon) + tauP4Up).M();
		double visMass_tauEsDown = (Ntp->Muon_p4(selMuon) + tauP4Down).M();
		double svfMass_tauESUp = (svfObjTauESUp->isValid()) ? svfObjTauESUp->get_mass() : -999.;
		double svfMass_tauESDown = (svfObjTauESDown->isValid()) ? svfObjTauESDown->get_mass() : -999.;

		h_shape_VisM_TauESUp.at(t).Fill(visMass_tauESUp, w);
		h_shape_VisM_TauESDown.at(t).Fill(visMass_tauEsDown, w);
		h_shape_SVfitM_TauESUp.at(t).Fill(svfMass_tauESUp, w);
		h_shape_SVfitM_TauESDown.at(t).Fill(svfMass_tauESDown, w);

		// timing info on mass reconstruction
		h_SVFitTimeReal.at(t).Fill(clock->GetRealTime("SVFit"), 1); // filled w/o weight
		h_SVFitTimeCPU.at(t).Fill(clock->GetCpuTime("SVFit"), 1); // filled w/o weight
		h_SVFitTimeReal.at(t).Fill(clock->GetRealTime("SVFitTauESUp"), 1); // filled w/o weight
		h_SVFitTimeCPU.at(t).Fill(clock->GetCpuTime("SVFitTauESUp"), 1); // filled w/o weight
		h_SVFitTimeReal.at(t).Fill(clock->GetRealTime("SVFitTauESDown"), 1); // filled w/o weight
		h_SVFitTimeCPU.at(t).Fill(clock->GetCpuTime("SVFitTauESDown"), 1); // filled w/o weight
*/
		// QCD shape uncertainty and scaling to be done on datacard level

		// lepton charge
		h_MuCharge.at(t).Fill(Ntp->Muon_Charge(selMuon), w);
		h_TauCharge.at(t).Fill(Ntp->PFTau_Charge(selTau), w);

		// MET
		h_MetPt.at(t).Fill(Ntp->MET_CorrMVAMuTau_et(), w);
		h_MetPhi.at(t).Fill(Ntp->MET_CorrMVAMuTau_phi(), w);
		if (Ntp->PFTau_hpsDecayMode(selTau) < 5) {
			h_MetPt1ProngOnly.at(t).Fill(Ntp->MET_CorrMVAMuTau_et(), w);
			h_MetPhi1ProngOnly.at(t).Fill(Ntp->MET_CorrMVAMuTau_phi(), w);
		} else {
			h_MetPt3ProngOnly.at(t).Fill(Ntp->MET_CorrMVAMuTau_et(), w);
			h_MetPhi3ProngOnly.at(t).Fill(Ntp->MET_CorrMVAMuTau_phi(), w);
		}

		// MET leptons
		int metMuon_idx(-1), metTau_idx(-1);
		float metMuon_dR(-1), metTau_dR(-1);
		Ntp->findCorrMVAMuTauSrcMuon(selMuon, metMuon_idx, metMuon_dR);
		Ntp->findCorrMVAMuTauSrcTau(selTau, metTau_idx, metTau_dR);
		h_MetLepMuDr.at(t).Fill(metMuon_dR, w);
		h_MetLepTauDr.at(t).Fill(metTau_dR, w);
		h_MetLepNMu.at(t).Fill(Ntp->NMET_CorrMVAMuTau_srcMuons(), w);
		h_MetLepNTau.at(t).Fill(Ntp->NMET_CorrMVAMuTau_srcTaus(), w);
		h_MetLepNMuMinusNMu.at(t).Fill(Ntp->NMET_CorrMVAMuTau_srcMuons() - selectedMuons.size(), w);
		h_MetLepNTauMinusNTau.at(t).Fill(Ntp->NMET_CorrMVAMuTau_srcTaus() - selectedTaus.size(), w);
		if (Ntp->NMET_CorrMVAMuTau_srcMuons() != selectedMuons.size()) {
			h_MetLepDiffMET.at(t).Fill(Ntp->MET_CorrMVAMuTau_et(), w);
			h_MetLepDiffMETPhi.at(t).Fill(Ntp->MET_CorrMVAMuTau_phi(), w);
			h_MetLepDiffMt.at(t).Fill(value.at(MT), w);
		}

		// Jets
		h_NJetsKin.at(t).Fill(selectedJetsKin.size(), w);
		if (selectedJetsKin.size() > 0) {
			h_JetKin1Pt.at(t).Fill(Ntp->PFJet_p4(selectedJetsKin.at(0)).Pt(), w);
			h_JetKin1Eta.at(t).Fill(Ntp->PFJet_p4(selectedJetsKin.at(0)).Eta(), w);
			h_JetKin1Phi.at(t).Fill(Ntp->PFJet_p4(selectedJetsKin.at(0)).Phi(), w);
			h_JetKin1IsLooseId.at(t).Fill(Ntp->PFJet_PUJetID_looseWP(selectedJetsKin.at(0)), w);
		}
		if (selectedJetsKin.size() > 1) {
			h_JetKin2IsLooseId.at(t).Fill(Ntp->PFJet_PUJetID_looseWP(selectedJetsKin.at(1)), w);
			h_JetKin2Pt.at(t).Fill(Ntp->PFJet_p4(selectedJetsKin.at(1)).Pt(), w);
			h_JetKin2Eta.at(t).Fill(Ntp->PFJet_p4(selectedJetsKin.at(1)).Eta(), w);
			h_JetKin2Phi.at(t).Fill(Ntp->PFJet_p4(selectedJetsKin.at(1)).Phi(), w);
		}
		h_NJetsId.at(t).Fill(selectedJets.size(), w);
		if (selectedJets.size() > 0) {
			h_Jet1Pt.at(t).Fill(Ntp->PFJet_p4(selectedJets.at(0)).Pt(), w);
			h_Jet1Eta.at(t).Fill(Ntp->PFJet_p4(selectedJets.at(0)).Eta(), w);
			h_Jet1Phi.at(t).Fill(Ntp->PFJet_p4(selectedJets.at(0)).Phi(), w);
			h_Jet1IsB.at(t).Fill(Ntp->PFJet_bDiscriminator(selectedJets.at(0)) > cCat_btagDisc, w);
		}
		if (selectedJets.size() > 1) {
			h_Jet2Pt.at(t).Fill(Ntp->PFJet_p4(selectedJets.at(1)).Pt(), w);
			h_Jet2Eta.at(t).Fill(Ntp->PFJet_p4(selectedJets.at(1)).Eta(), w);
			h_Jet2Phi.at(t).Fill(Ntp->PFJet_p4(selectedJets.at(1)).Phi(), w);
			h_Jet2IsB.at(t).Fill(Ntp->PFJet_bDiscriminator(selectedJets.at(1)) > cCat_btagDisc, w);
		}

		// plot embedding weights
		if (idStripped == DataMCType::DY_mutau_embedded) {
			h_embeddingWeight_TauSpinner.at(t).Fill(Ntp->Embedding_TauSpinnerWeight()); // no weight applied
			h_embeddingWeight_SelEffWeight.at(t).Fill(Ntp->Embedding_SelEffWeight()); // no weight applied
			h_embeddingWeight_MinVisPtFilter.at(t).Fill(Ntp->Embedding_MinVisPtFilter()); // no weight applied
		}

		// plot Higgs pT weight
		if (idStripped >= DataMCType::H_tautau && idStripped <= DataMCType::H_tautau_WHZHTTH) {
			h_HiggsGenPtWeight.at(t).Fill(higgs_GenPtWeight); // no weight applied
			h_HiggsGenPt.at(t).Fill(higgs_GenPt, w);
			h_HiggsMassFromSampleName.at(t).Fill(Ntp->getSampleHiggsMass());
		}

		// variables for categorization
		h_HiggsPt.at(t).Fill(higgsPt_, w);
		h_HiggsPhi.at(t).Fill(higgsPhi, w);
		h_JetsDEta.at(t).Fill(jetdEta_, w);
		h_JetsInEtaGap.at(t).Fill(nJetsInGap_, w);
		h_JetsInvM.at(t).Fill(mjj_, w);

		// QCD shape region
		if (isQCDShapeEvent) {
			double mvis = (Ntp->Muon_p4(selMuon) + Ntp->PFTau_p4(selTau)).M();
			h_QcdShapeRegion.at(t).Fill(mvis, w);
		}
	}

	if (passedFullInclusiveSelNoMt) {
		if (Ntp->Muon_Charge(selMuon) > 0)
			h_MtMuPlusOnly.at(t).Fill(value.at(MT), w);
		if (Ntp->Muon_Charge(selMuon) < 0)
			h_MtMuMinusOnly.at(t).Fill(value.at(MT), w);

		h_MetPtNoMtCut.at(t).Fill(Ntp->MET_CorrMVAMuTau_et(), w);
		h_MetPhiNoMtCut.at(t).Fill(Ntp->MET_CorrMVAMuTau_phi(), w);

		if (Ntp->PFTau_hpsDecayMode(selTau) < 5) {
			h_Mt1ProngOnly.at(t).Fill(value.at(MT), w);
			h_MetPtNoMtCut1ProngOnly.at(t).Fill(Ntp->MET_CorrMVAMuTau_et(), w);
			h_MetPhiNoMtCut1ProngOnly.at(t).Fill(Ntp->MET_CorrMVAMuTau_phi(), w);
		} else {
			h_Mt3ProngOnly.at(t).Fill(value.at(MT), w);
			h_MetPtNoMtCut3ProngOnly.at(t).Fill(Ntp->MET_CorrMVAMuTau_et(), w);
			h_MetPhiNoMtCut3ProngOnly.at(t).Fill(Ntp->MET_CorrMVAMuTau_phi(), w);
			if (Ntp->PFTau_TIP_hassecondaryVertex(selTau)) {
				h_Mt3ProngSV.at(t).Fill(value.at(MT), w);

				double FlightLenghtSignificance = Ntp->PFTau_FlightLenght_significance(Ntp->PFTau_TIP_primaryVertex_pos(selTau), Ntp->PFTau_TIP_primaryVertex_cov(selTau),
						Ntp->PFTau_a1_lvp(selTau).Vertex(), Ntp->PFTau_a1_lvp(selTau).VertexCov());
				if (FlightLenghtSignificance > 2.2) {
					h_Mt3ProngSVFlight.at(t).Fill(value.at(MT), w);
				}
			}
		}
	}

	/////// plots filled after full muon and tau selection
	if (passedObjectsFailDiMuonVeto) {
		// Investigate events discarded by the DiMuon Veto
		if (Ntp->Muon_Charge(selMuon) == 1) {
			h_MuVetoDPtSelMuon.at(t).Fill(Ntp->Muon_p4(diMuonNeg).Pt() - Ntp->Muon_p4(selMuon).Pt(), w);
			h_MuVetoDRTau.at(t).Fill(Ntp->Muon_p4(diMuonNeg).DeltaR(Ntp->PFTau_p4(selTau)), w);
		} else if (Ntp->Muon_Charge(selMuon) == -1) {
			h_MuVetoDPtSelMuon.at(t).Fill(Ntp->Muon_p4(diMuonPos).Pt() - Ntp->Muon_p4(selMuon).Pt(), w);
			h_MuVetoDRTau.at(t).Fill(Ntp->Muon_p4(diMuonPos).DeltaR(Ntp->PFTau_p4(selTau)), w);
		}
		h_MuVetoInvM.at(t).Fill((Ntp->Muon_p4(diMuonPos) + Ntp->Muon_p4(diMuonNeg)).M(), w);
		h_MuVetoPtPositive.at(t).Fill(Ntp->Muon_p4(diMuonPos).Pt(), w);
		h_MuVetoPtNegative.at(t).Fill(Ntp->Muon_p4(diMuonNeg).Pt(), w);
		h_MuVetoDeltaR.at(t).Fill(Ntp->Muon_p4(diMuonPos).DeltaR(Ntp->Muon_p4(diMuonNeg)), w);
	}

	if (passedDiMuonVeto) {
		// Tri-lepton vetoes
		h_NMuonTriLepVeto.at(t).Fill(triLepVetoMuons.size(), w);
		h_NElecTriLepVeto.at(t).Fill(triLepVetoElecs.size(), w);
	}
	//////// plots filled after full selection without BJetVeto
	if (passedFullInclusiveSelNoBVeto) {
		h_NBJets.at(t).Fill(selectedBJets.size(), w);
		if (selectedBJets.size() > 0) {
			h_BJet1Pt.at(t).Fill(Ntp->PFJet_p4(selectedBJets.at(0)).Pt(), w);
			h_BJet1Eta.at(t).Fill(Ntp->PFJet_p4(selectedBJets.at(0)).Eta(), w);
			h_BJet1Phi.at(t).Fill(Ntp->PFJet_p4(selectedBJets.at(0)).Phi(), w);
		}
	}
}

void HToTaumuTauh::Finish() {
	Logger(Logger::Verbose) << "Start." << std::endl;

	if (wJetsBGSource == "Data") {
		if (mode == RECONSTRUCT) { // only apply data-driven numbers on "combine" level
			Logger(Logger::Info) << "WJet BG: Using data driven yield method." << std::endl;

			double sumSelEvts = 0;
			for (unsigned id = 20; id < 24; id++) {
				if (!HConfig.hasID(id))
					continue;
				int type = HConfig.GetType(id);
				// check that cross-section for WJet processes is set to -1 in Histo.txt
				double oldXSec = HConfig.GetCrossSection(id);
				if (oldXSec != -1) {
					// Histo.txt has WJet xsec unequal -1, so set it to -1 to avoid scaling by framework
					if (!HConfig.SetCrossSection(id, -1))
						Logger(Logger::Warning) << "Could not change cross section for id " << id << std::endl;
					printf("WJet process %i had xsec = %6.1f. Setting to %6.1f for data-driven WJet yield.\n", id, oldXSec, HConfig.GetCrossSection(id));
				}
				sumSelEvts += Npassed.at(type).GetBinContent(NCuts+1);
			}

			// second loop, now the total sum of all Wjets events is known, so we can scale
			for (unsigned id = 20; id < 24; id++) {
				if (!HConfig.hasID(id))
					continue;
				int type = HConfig.GetType(id);
				double rawSelEvts = Npassed.at(type).GetBinContent(NCuts+1);

				// scale all WJet histograms to data-driven yield
				ScaleAllHistOfType(type, wJetsYieldMap[categoryFlag] / sumSelEvts);
				printf("WJet process %i was scaled from yield %f to yield %f \n", id, rawSelEvts, Npassed.at(type).GetBinContent(NCuts+1));
			}
		}
		else
			Logger(Logger::Info) << "WJet BG: Data driven will be used at Combine stage, but not in this individual set." << std::endl;
	}
	else if (wJetsBGSource == "MC"){
		Logger(Logger::Info) << "WJet BG: Using MC." << std::endl;
	}
	else
		Logger(Logger::Warning) << "WJet BG: Please specify \"MC\" or \"Data\". Using MC for this run..." << std::endl;

	if(qcdShapeFromData){
		if (mode == RECONSTRUCT) { // only apply data-driven numbers on "combine" level
			Logger(Logger::Info) << "QCD BG: Using data driven estimation." << std::endl;
			if(!HConfig.hasID(DataMCType::QCD)){
				Logger(Logger::Error) << "QCD BG: Please add QCD to your Histo.txt. Abort." << std::endl;
			}
			else{
				double rawQcdShapeEvents = Npassed.at(HConfig.GetType(DataMCType::QCD)).GetBinContent(NCuts+1);
				// scale QCD histograms to data-driven yields
				TString method;
				if(qcdUseEfficiencyMethod && (qcdYieldEffMap.find(categoryFlag) != qcdYieldEffMap.end()) ) {
					// use efficiency method for QCD yield
					ScaleAllHistOfType(HConfig.GetType(DataMCType::QCD), qcdYieldEffMap[categoryFlag] / rawQcdShapeEvents);
					method = "efficiency";
				}
				else{
					// use ABCD method for QCD yield
					ScaleAllHistOfType(HConfig.GetType(DataMCType::QCD), qcdYieldABCDMap[categoryFlag] / rawQcdShapeEvents);
					method = "ABCD";
				}

				printf("QCD histogram was scaled from yield %f to yield %f (using %s method)\n", rawQcdShapeEvents, Npassed.at(HConfig.GetType(DataMCType::QCD)).GetBinContent(NCuts+1), method.Data());
			}
		}
		else
			Logger(Logger::Info) << "QCD BG: Data driven will be used at Combine stage, but not in this individual set." << std::endl;
	}
	else
		Logger(Logger::Warning) << "QCD BG: No data driven QCD background available. Histos will be empty." << std::endl;

	if(useEmbedding){
		if (mode == RECONSTRUCT) { // only apply data-driven numbers on "combine" level
			Logger(Logger::Info) << "Using embedding for DY." << std::endl;
			if(!HConfig.hasID(DataMCType::DY_mutau_embedded) || !HConfig.hasID(DataMCType::DY_tautau)){
				Logger(Logger::Error) << "Embedding: Please add DY_mutau_embedded and DY_tautau to your Histo.txt. Abort." << std::endl;
			}
			else{
				// read in skimsummary
				SkimConfig SC;
				SC.ApplySkimEfficiency(types,Npassed,Npassed_noweight);

				// yield_emb = N_MC(before mT)*eff with eff = N_emb(NCuts)/N_emb(before mT)
				// scale factor = yield_emb / N_emb(NCuts) = N_MC(before mT)/N_emb(before mT)

				// MC DY: get yield of inclusive selection w/o mT and bJetVeto cut [N_MC(before mT)]
				double dyMCYield = Npassed.at(HConfig.GetType(DataMCType::DY_tautau)).GetBinContent(MT+1);
				dyMCYield *= CrossSectionandAcceptance.at(HConfig.GetType(DataMCType::DY_tautau))*Lumi/Npassed.at(HConfig.GetType(DataMCType::DY_tautau)).GetBinContent(0);
				// Embedding DY: get yield before mT cut [N_emb(before mT)]
				double dyEmbYield   = Npassed.at(HConfig.GetType(DataMCType::DY_mutau_embedded)).GetBinContent(MT+1);

				double dyEmbScale = dyMCYield / dyEmbYield;
				// scale embedding sample to estimated yield
				ScaleAllHistOfType(HConfig.GetType(DataMCType::DY_mutau_embedded), dyEmbScale);
				// make sure that embedded is not scaled again by framework
				if (HConfig.GetCrossSection(DataMCType::DY_mutau_embedded) != -1){
					if (HConfig.SetCrossSection(DataMCType::DY_mutau_embedded, -1)){
						Logger(Logger::Info) << "Cross section for DY_mutau_embedded was set to -1" << std::endl;
					}
					else
						Logger(Logger::Warning) << "Could not change cross section for DY_mutau_embedded" << std::endl;
				}
				// do not draw MC DY sample
				suppressDrawingHistOfType(HConfig.GetType(DataMCType::DY_tautau));

				printf("Using Embedding for DY background estimation:\n");
				printf("\tNumber of selected events before mT cut:\n    DY MC: %.1f\n    DY embedding: %.1f\n", dyMCYield, dyEmbYield);
				printf("\tEmbedding yield was scaled by a factor %.5f to %.1f events\n", dyEmbScale, Npassed.at(HConfig.GetType(DataMCType::DY_mutau_embedded)).GetBinContent(NCuts+1));
				printf("\tMC DYtautau histograms will be suppressed in plots\n");
			}
		}
	}
	else{
		if(HConfig.hasID(DataMCType::DY_mutau_embedded)){
			ScaleAllHistOfType(HConfig.GetType(DataMCType::DY_mutau_embedded), 0.0);
			printf("Not using embedding: DY_mutau_embedded sample is scaled to %.1f.", Npassed.at(HConfig.GetType(DataMCType::DY_mutau_embedded)).GetBinContent(NCuts+1));
		}
	}

	// call GetHistoInfo here (instead of in Configure function), otherwise the SetCrossSection calls are not reflected
	HConfig.GetHistoInfo(types, CrossSectionandAcceptance, legend, colour);
	Selection::Finish();
}


/////////////////////////////////////////
// Definition of selection and helper functions
/////////////////////////////////////////

///////// Muons

bool HToTaumuTauh::selectMuon_Id(unsigned i, unsigned vertex){
	if(	Ntp->isSelectedMuon(i,vertex,cMu_dxy,cMu_dz) &&
		Ntp->Muon_RelIso(i) < cMu_relIso &&
		(Ntp->GetStrippedMCID() == DataMCType::DY_mutau_embedded || // no trigger matching for embedding
				Ntp->matchTrigger(Ntp->Muon_p4(i),cTriggerNames,"muon") < cMu_dRHltMatch)
		){
		return true;
	}
	return false;
}

bool HToTaumuTauh::selectMuon_Kinematics(unsigned i){
	if(	Ntp->Muon_p4(i).Pt() >= cMu_pt &&
		fabs(Ntp->Muon_p4(i).Eta()) <= cMu_eta
			){
		return true;
	}
	return false;
}

bool HToTaumuTauh::selectMuon_antiIso(unsigned i, unsigned vertex) {
	if (Ntp->isSelectedMuon(i, vertex, cMu_dxy, cMu_dz) &&
		Ntp->Muon_RelIso(i) <= 0.5 &&
		Ntp->Muon_RelIso(i) >= 0.2 &&
		Ntp->matchTrigger(Ntp->Muon_p4(i), cTriggerNames, "muon") < cMu_dRHltMatch &&
		selectMuon_Kinematics(i)) {
		return true;
	}
	return false;
}

bool HToTaumuTauh::selectMuon_diMuonVeto(unsigned i, unsigned i_vtx){
	if(	Ntp->Muon_p4(i).Pt() > 15.0 &&
		fabs(Ntp->Muon_p4(i).Eta()) < 2.4 &&
		Ntp->Muon_isPFMuon(i) &&
		Ntp->Muon_isGlobalMuon(i) &&
		Ntp->Muon_isTrackerMuon(i) &&
		Ntp->Muon_RelIso(i) < 0.3 &&
		Ntp->dz(Ntp->Muon_p4(i),Ntp->Muon_Poca(i),Ntp->Vtx(i_vtx)) < 0.2
		) {
	  return true;
	}
	return false;
}

bool HToTaumuTauh::selectMuon_triLeptonVeto(int i, int selectedMuon, unsigned i_vtx){
	if(	i != selectedMuon &&
		Ntp->isTightMuon(i,i_vtx) &&
		Ntp->dxy(Ntp->Muon_p4(i),Ntp->Muon_Poca(i),Ntp->Vtx(i_vtx)) < cMu_dxy &&
		Ntp->dz(Ntp->Muon_p4(i),Ntp->Muon_Poca(i),Ntp->Vtx(i_vtx)) < cMu_dz &&
		Ntp->Muon_RelIso(i) < 0.3 &&
		Ntp->Muon_p4(i).Pt() > cMuTriLep_pt &&
		fabs(Ntp->Muon_p4(i).Eta()) < cMuTriLep_eta
			){
			return true;
	}
	return false;
}


///////// Electrons
bool HToTaumuTauh::selectElectron_triLeptonVeto(unsigned i, unsigned i_vtx, std::vector<int> muonCollection){
	// check if elec is matched to a muon, if so this is not a good elec (should become obsolete when using top projections)
//	for(std::vector<int>::iterator it_mu = muonCollection.begin(); it_mu != muonCollection.end(); ++it_mu){
//	  if( Ntp->Electron_p4(i).DeltaR(Ntp->Muon_p4(*it_mu)) < cMuTau_dR ) {
//		  return false;
//	  }
//	}

	if ( 	Ntp->isSelectedElectron(i,i_vtx,0.045,0.2) &&
			//This electron isolation is using rho corrections, but should use deltaBeta corrections
			//documentation: https://twiki.cern.ch/twiki/bin/viewauth/CMS/EgammaEARhoCorrection
			Ntp->Electron_RelIso03(i) < 0.3 && // "For electron the default cone size if 0.3" https://twiki.cern.ch/twiki/bin/viewauth/CMS/EgammaPFBasedIsolation#Alternate_code_to_calculate_PF_I
			Ntp->Electron_p4(i).Pt() > 10.0 &&
			fabs(Ntp->Electron_p4(i).Eta()) < 2.5
			){
		return true;
	}
	return false;
}

///////// Taus

bool HToTaumuTauh::selectPFTau_Id(unsigned i){
	if ( 	Ntp->PFTau_isHPSByDecayModeFinding(i) &&
			Ntp->PFTau_isHPSAgainstElectronsLoose(i) &&
			Ntp->PFTau_isHPSAgainstMuonTight(i)
			){
		return true;
	}
	return false;
}

bool HToTaumuTauh::selectPFTau_Id(unsigned i, std::vector<int> muonCollection){
	// check if tau is matched to a muon, if so this is not a good tau
	// https://twiki.cern.ch/twiki/bin/viewauth/CMS/HiggsToTauTauWorkingSummer2013#Sync_Issues
	for(std::vector<int>::iterator it_mu = muonCollection.begin(); it_mu != muonCollection.end(); ++it_mu){
	  if( Ntp->PFTau_p4(i).DeltaR(Ntp->Muon_p4(*it_mu)) < cMuTau_dR ) {
		  return false;
	  }
	}
	// trigger matching
	if (Ntp->GetStrippedMCID() != DataMCType::DY_mutau_embedded){
		if (Ntp->matchTrigger(Ntp->PFTau_p4(i),cTriggerNames,"tau") > cTau_dRHltMatch) {
			return false;
		}
	}

	if ( 	selectPFTau_Id(i) ){
		return true;
	}
	return false;
}

bool HToTaumuTauh::selectPFTau_Iso(unsigned i){
	if ( 	Ntp->PFTau_HPSPFTauDiscriminationByRawCombinedIsolationDBSumPtCorr3Hits(i) < cTau_rawIso
			){
		return true;
	}
	return false;
}

bool HToTaumuTauh::selectPFTau_Kinematics(unsigned i){
	if ( 	Ntp->PFTau_p4(i).Pt() >= cTau_pt &&
			fabs(Ntp->PFTau_p4(i).Eta()) <= cTau_eta
			){
		return true;
	}
	return false;
}

bool HToTaumuTauh::selectPFTau_relaxedIso(unsigned i, std::vector<int> muonCollection){
	if (selectPFTau_Id(i, muonCollection) &&
		Ntp->PFTau_HPSPFTauDiscriminationByRawCombinedIsolationDBSumPtCorr3Hits(i) < 10. &&
		selectPFTau_Kinematics(i)){
		return true;
	}
	return false;
}

bool HToTaumuTauh::selectPFJet_Cleaning(unsigned i, int selectedMuon, int selectedTau){
	// clean against selected muon and tau
	if (selectedMuon >= 0) {
		if (Ntp->PFJet_p4(i).DeltaR(Ntp->Muon_p4(selectedMuon)) < cJetClean_dR) return false;
	}
	if (selectedTau >= 0){
		if (Ntp->PFJet_p4(i).DeltaR(Ntp->PFTau_p4(selectedTau)) < cJetClean_dR) return false;
	}
	return true;
}


bool HToTaumuTauh::selectPFJet_Kinematics(unsigned i){
	if ( 	fabs(Ntp->PFJet_p4(i).Eta()) < cCat_jetEta &&
			Ntp->PFJet_p4(i).Pt() > cCat_jetPt){
		return true;
	}
	return false;
}

bool HToTaumuTauh::selectPFJet_Relaxed(unsigned i, int selectedMuon, int selectedTau){
	if ( 	selectPFJet_Cleaning(i, selectedMuon, selectedTau) &&
			fabs(Ntp->PFJet_p4(i).Eta()) < 4.7 &&
			Ntp->PFJet_p4(i).Pt() > 20.0 &&
			selectPFJet_Id(i)
			){
		return true;
	}
	return false;
}

bool HToTaumuTauh::selectPFJet_Id(unsigned i){
	if (	Ntp->PFJet_PUJetID_looseWP(i) ){
		return true;
	}
	return false;
}

bool HToTaumuTauh::selectBJet(unsigned i, int selectedMuon, int selectedTau){
	// clean against selected muon and tau
	if (selectedMuon >= 0) {
		if (Ntp->PFJet_p4(i).DeltaR(Ntp->Muon_p4(selectedMuon)) < cJetClean_dR) return false;
	}
	if (selectedTau >= 0){
		if (Ntp->PFJet_p4(i).DeltaR(Ntp->PFTau_p4(selectedTau)) < cJetClean_dR) return false;
	}

	if (	fabs(Ntp->PFJet_p4(i).Eta()) < cCat_bjetEta &&
			Ntp->PFJet_p4(i).Pt() > cCat_bjetPt &&
			Ntp->PFJet_bDiscriminator(i) > cCat_btagDisc){
		return true;
	}
	return false;
}

// migrate a category into main analysis if this is chosen category
// return value: if category passed
bool HToTaumuTauh::migrateCategoryIntoMain(TString thisCategory, std::vector<float> categoryValueVector, std::vector<float> categoryPassVector, unsigned categoryNCuts) {
	bool catPassed = true;
	for (unsigned i_cut = CatCut1; i_cut < NCuts; i_cut++) {

		// migrate only if this category is the chosen one
		if (categoryFlag == thisCategory) {
			if (i_cut < categoryNCuts) {
				value.at(i_cut) = categoryValueVector.at(i_cut);
				pass.at(i_cut) = categoryPassVector.at(i_cut);
			} else {
				// cut not implemented in this category -> set to true
				value.at(i_cut) = -10.;
				pass.at(i_cut) = true;
			}
		}
		if (i_cut < categoryNCuts) {
			catPassed = catPassed && categoryPassVector.at(i_cut);
		}
	}

	return catPassed;
}

// helper category definitions for background methods
bool HToTaumuTauh::helperCategory_VBFLooseRelaxed(bool useRelaxedForPlots, unsigned NJets, double DEta, int NJetsInGap, double Mjj){
	std::vector<float> value_VBFLooseRelaxed;
	std::vector<float> pass_VBFLooseRelaxed;

	// cut implementation
	for(int i=0; i<NCuts;i++){
	value_VBFLooseRelaxed.push_back(-10.);
	pass_VBFLooseRelaxed.push_back(false);
	}

	value_VBFLooseRelaxed.at(VbfLoose_NJet) = NJets;
	pass_VBFLooseRelaxed.at(VbfLoose_NJet) = (value_VBFLooseRelaxed.at(VbfLoose_NJet) >= cut_VBFLooseRelaxed.at(VbfLoose_NJet));

	if(pass_VBFLooseRelaxed.at(VbfLoose_NJet)){
		value_VBFLooseRelaxed.at(VbfLoose_DeltaEta) = DEta;
		pass_VBFLooseRelaxed.at(VbfLoose_DeltaEta) = (fabs(value_VBFLooseRelaxed.at(VbfLoose_DeltaEta)) > cut_VBFLooseRelaxed.at(VbfLoose_DeltaEta));

		value_VBFLooseRelaxed.at(VbfLoose_NJetRapGap) = NJetsInGap;
		pass_VBFLooseRelaxed.at(VbfLoose_NJetRapGap) = (value_VBFLooseRelaxed.at(VbfLoose_NJetRapGap) <= cut_VBFLooseRelaxed.at(VbfLoose_NJetRapGap));

		value_VBFLooseRelaxed.at(VbfLoose_JetInvM) = Mjj;
		pass_VBFLooseRelaxed.at(VbfLoose_JetInvM) = (value_VBFLooseRelaxed.at(VbfLoose_JetInvM) > cut_VBFLooseRelaxed.at(VbfLoose_JetInvM));
	}
	else{
		pass_VBFLooseRelaxed.at(VbfLoose_DeltaEta) = true;
		pass_VBFLooseRelaxed.at(VbfLoose_NJetRapGap) = true;
		pass_VBFLooseRelaxed.at(VbfLoose_JetInvM) = true;
	}

	value_VBFLooseRelaxed.at(VbfLoose_NotVbfTight) = true;
	pass_VBFLooseRelaxed.at(VbfLoose_NotVbfTight) = true; // disabled cut

	// migrate into main analysis if this is chosen category
	TString cat = useRelaxedForPlots ? "VBFLoose" : "DoNotUseThisCategoryForPlotting";
	return migrateCategoryIntoMain(cat,value_VBFLooseRelaxed, pass_VBFLooseRelaxed,VbfLoose_NCuts);
}
bool HToTaumuTauh::helperCategory_VBFTightRelaxed(bool useRelaxedForPlots, unsigned NJets, double DEta, int NJetsInGap, double Mjj, double higgsPt){
	std::vector<float> value_VBFTightRelaxed;
	std::vector<float> pass_VBFTightRelaxed;

	// cut implementation
	for(int i=0; i<NCuts;i++){
	value_VBFTightRelaxed.push_back(-10.);
	pass_VBFTightRelaxed.push_back(false);
	}

	value_VBFTightRelaxed.at(VbfTight_NJet) = NJets;
	pass_VBFTightRelaxed.at(VbfTight_NJet) = (value_VBFTightRelaxed.at(VbfTight_NJet) >= cut_VBFTightRelaxed.at(VbfTight_NJet));

	if(pass_VBFTightRelaxed.at(VbfTight_NJet)){
		value_VBFTightRelaxed.at(VbfTight_DeltaEta) = DEta;
		pass_VBFTightRelaxed.at(VbfTight_DeltaEta) = (fabs(value_VBFTightRelaxed.at(VbfTight_DeltaEta)) > cut_VBFTightRelaxed.at(VbfTight_DeltaEta));

		value_VBFTightRelaxed.at(VbfTight_NJetRapGap) = NJetsInGap;
		pass_VBFTightRelaxed.at(VbfTight_NJetRapGap) = (value_VBFTightRelaxed.at(VbfTight_NJetRapGap) <= cut_VBFTightRelaxed.at(VbfTight_NJetRapGap));

		value_VBFTightRelaxed.at(VbfTight_JetInvM) = Mjj;
		pass_VBFTightRelaxed.at(VbfTight_JetInvM) = (value_VBFTightRelaxed.at(VbfTight_JetInvM) > cut_VBFTightRelaxed.at(VbfTight_JetInvM));
	}
	else{
		pass_VBFTightRelaxed.at(VbfTight_DeltaEta) = true;
		pass_VBFTightRelaxed.at(VbfTight_NJetRapGap) = true;
		pass_VBFTightRelaxed.at(VbfTight_JetInvM) = true;
	}

	value_VBFTightRelaxed.at(VbfTight_HiggsPt) = higgsPt;
	pass_VBFTightRelaxed.at(VbfTight_HiggsPt) = (value_VBFTightRelaxed.at(VbfTight_HiggsPt) > cut_VBFTightRelaxed.at(VbfTight_HiggsPt));

	// migrate into main analysis if this is chosen category
	TString cat = useRelaxedForPlots ? "VBFTight" : "DoNotUseThisCategoryForPlotting";
	return migrateCategoryIntoMain(cat,value_VBFTightRelaxed, pass_VBFTightRelaxed,VbfTight_NCuts);
}


void HToTaumuTauh::setStatusBooleans(bool resetAll){
	if(resetAll){
		// make sure that all booleans defined above are false
		for (unsigned i = 0; i<CatCut1; i++){
			if (pass.at(i) != false){
				Logger(Logger::Warning) << "pass vector not cleared properly" << std::endl;
				pass.at(i) = false;
			}
		}
		// make sure all optional category cuts are true
		for (unsigned i = CatCut1; i<NCuts; i++){
			if (pass.at(i) != true){
				Logger(Logger::Warning) << "pass vector not cleared properly" << std::endl;
				pass.at(i) = true;
			}
		}

		// set all category flags to false
		passed_VBFTight		= false;
		passed_VBFLoose		= false;
		passed_VBF			= false;
		passed_OneJetHigh	= false;
		passed_OneJetLow	= false;
		passed_OneJetBoost	= false;
		passed_ZeroJetHigh	= false;
		//passed_ZeroJetLow	= false;
		//passed_NoCategory	= false;
		passed_VBFTightRelaxed	= false;
		passed_VBFLooseRelaxed	= false;
	}
	passedVertex = pass.at(TriggerOk) && pass.at(PrimeVtx);
	passedMuId = passedVertex && pass.at(NMuId);
	passedMu = passedMuId && pass.at(NMuKin);
	passedTauIdIso = passedVertex && pass.at(NTauId) && pass.at(NTauIso);
	passedTau = passedTauIdIso && pass.at(NTauKin);
	passedObjects = passedMu && passedTau;
	passedDiMuonVeto = passedObjects && pass.at(DiMuonVeto);
	passedFullInclusiveSelNoBVeto = passedDiMuonVeto && pass.at(TriLeptonVeto) && pass.at(OppCharge) && pass.at(MT);
	passedFullInclusiveSel = passedFullInclusiveSelNoBVeto && pass.at(BJetVeto);
	// define booleans for analysis stages needed for background methods
	passedFullInclusiveSelNoMt = passedObjects && pass.at(DiMuonVeto) && pass.at(TriLeptonVeto) && pass.at(OppCharge) && pass.at(BJetVeto);
	passedFullInclusiveSelNoMtNoOS = passedObjects && pass.at(DiMuonVeto) && pass.at(TriLeptonVeto) && pass.at(BJetVeto);
	passedFullInclusiveNoTauNoMuNoCharge = passedVertex && pass.at(DiMuonVeto) && pass.at(TriLeptonVeto) && pass.at(MT) && pass.at(BJetVeto);
	// define booleans for analysis stages for additional plots
	passedObjectsFailDiMuonVeto = passedObjects && !pass.at(DiMuonVeto);

	return;
}
