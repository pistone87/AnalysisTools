/*
 * VBFTight.cxx
 *
 *  Created on: Mar 21, 2014
 *      Author: kargoll
 */

#include "VBFTight.h"
#include "SimpleFits/FitSoftware/interface/Logger.h"

VBFTight::VBFTight(TString Name_, TString id_):
	HToTaumuTauh(Name_,id_)
{
	Logger(Logger::Info) << "Setting up the class VBFTight" << std::endl;
	// run VBFTight category
	categoryFlag = "VBFTight";

	// run Categories using embedding
	useEmbedding = true;

	// run Categories using data-driven WJets BG
	wJetsBGSource = "Data";

	// run Categories using data-driven QCD BG
	qcdShapeFromData = true;
}

VBFTight::~VBFTight() {
	  for(unsigned int j=0; j<Npassed.size(); j++){
	    Logger(Logger::Info) << "Selection Summary before: "
		 << Npassed.at(j).GetBinContent(1)     << " +/- " << Npassed.at(j).GetBinError(1)     << " after: "
		 << Npassed.at(j).GetBinContent(NCuts+1) << " +/- " << Npassed.at(j).GetBinError(NCuts) << std::endl;
	  }
	  Logger(Logger::Info) << "Done." << std::endl;
}

void VBFTight::categoryConfiguration(){
	// set cut values to be the cut values of this category
	cut.at(VbfTight_NJet) 		= 2;
	cut.at(VbfTight_DeltaEta)	= 4.0;
	cut.at(VbfTight_NJetRapGap)	= 0;
	cut.at(VbfTight_JetInvM)	= 700.0;
	cut.at(VbfTight_HiggsPt)	= 100.0;

	// set histograms of category cuts
	TString hlabel;
	TString htitle;
	TString c;

	title.at(VbfTight_NJet)="Number VBF Jets $>=$";
	title.at(VbfTight_NJet)+=cut.at(VbfTight_NJet);
	htitle=title.at(VbfTight_NJet);
	htitle.ReplaceAll("$","");
	htitle.ReplaceAll("\\","#");
	hlabel="Number of Jet_{VBF}";
	c="_Cut_";c+=VbfTight_NJet;
	Nminus1.at(VbfTight_NJet) = HConfig.GetTH1D(Name+c+"_Nminus1_VbfTight_NJet_",htitle,6,-0.5,5.5,hlabel,"Events");
	Nminus0.at(VbfTight_NJet) = HConfig.GetTH1D(Name+c+"_Nminus0_VbfTight_NJet_",htitle,6,-0.5,5.5,hlabel,"Events");

	title.at(VbfTight_DeltaEta)="$\\Delta\\eta(jj) >$";
	title.at(VbfTight_DeltaEta)+=cut.at(VbfTight_DeltaEta);
	htitle=title.at(VbfTight_DeltaEta);
	htitle.ReplaceAll("$","");
	htitle.ReplaceAll("\\","#");
	hlabel="#Delta#eta(Jet_{VBF}^{1},Jet_{VBF}^{2})";
	c="_Cut_";c+=VbfTight_DeltaEta;
	Nminus1.at(VbfTight_DeltaEta) = HConfig.GetTH1D(Name+c+"_Nminus1_VbfTight_DeltaEta_",htitle,32,-8.,8.,hlabel,"Events");
	Nminus0.at(VbfTight_DeltaEta) = HConfig.GetTH1D(Name+c+"_Nminus0_VbfTight_DeltaEta_",htitle,32,-8.,8.,hlabel,"Events");

	title.at(VbfTight_NJetRapGap)="Number Jets in $\\eta$ gap $<=$";
	title.at(VbfTight_NJetRapGap)+=cut.at(VbfTight_NJetRapGap);
	htitle=title.at(VbfTight_NJetRapGap);
	htitle.ReplaceAll("$","");
	htitle.ReplaceAll("\\","#");
	hlabel="Number of Jet_{VBF} in rapidity gap";
	c="_Cut_";c+=VbfTight_NJetRapGap;
	Nminus1.at(VbfTight_NJetRapGap) = HConfig.GetTH1D(Name+c+"_Nminus1_VbfTight_NJetRapGap_",htitle,6,-0.5,5.5,hlabel,"Events");
	Nminus0.at(VbfTight_NJetRapGap) = HConfig.GetTH1D(Name+c+"_Nminus0_VbfTight_NJetRapGap_",htitle,6,-0.5,5.5,hlabel,"Events");

	title.at(VbfTight_JetInvM)="$m_{jj}(VBF) >$";
	title.at(VbfTight_JetInvM)+=cut.at(VbfTight_JetInvM);
	title.at(VbfTight_JetInvM)+=" GeV";
	htitle=title.at(VbfTight_JetInvM);
	htitle.ReplaceAll("$","");
	htitle.ReplaceAll("\\","#");
	hlabel="m_{inv}(jj) of VBF-jets";
	c="_Cut_";c+=VbfTight_JetInvM;
	Nminus1.at(VbfTight_JetInvM) = HConfig.GetTH1D(Name+c+"_Nminus1_VbfTight_JetInvM_",htitle,20,0.,2000.,hlabel,"Events");
	Nminus0.at(VbfTight_JetInvM) = HConfig.GetTH1D(Name+c+"_Nminus0_VbfTight_JetInvM_",htitle,20,0.,2000.,hlabel,"Events");

	title.at(VbfTight_HiggsPt)="$p_{T}(H) >$";
	title.at(VbfTight_HiggsPt)+=cut.at(VbfTight_HiggsPt);
	title.at(VbfTight_HiggsPt)+=" GeV";
	htitle=title.at(VbfTight_HiggsPt);
	htitle.ReplaceAll("$","");
	htitle.ReplaceAll("\\","#");
	hlabel="p_{T} of Higgs candidate";
	c="_Cut_";c+=VbfTight_HiggsPt;
	Nminus1.at(VbfTight_HiggsPt) = HConfig.GetTH1D(Name+c+"_Nminus1_VbfTight_HiggsPt_",htitle,25,0.,250.,hlabel,"Events");
	Nminus0.at(VbfTight_HiggsPt) = HConfig.GetTH1D(Name+c+"_Nminus0_VbfTight_HiggsPt_",htitle,25,0.,250.,hlabel,"Events");
}

bool VBFTight::categorySelection(){
	std::vector<float> value_VBFTight(NCuts,-10);
	std::vector<float> pass_VBFTight(NCuts,false);

	value_VBFTight.at(VbfTight_NJet) = nJets_;
	pass_VBFTight.at(VbfTight_NJet) = (value_VBFTight.at(VbfTight_NJet) >= cut.at(VbfTight_NJet));

	if(pass_VBFTight.at(VbfTight_NJet)){
		value_VBFTight.at(VbfTight_DeltaEta) = jetdEta_;
		pass_VBFTight.at(VbfTight_DeltaEta) = (fabs(value_VBFTight.at(VbfTight_DeltaEta)) > cut.at(VbfTight_DeltaEta));

		value_VBFTight.at(VbfTight_NJetRapGap) = nJetsInGap_;
		pass_VBFTight.at(VbfTight_NJetRapGap) = (value_VBFTight.at(VbfTight_NJetRapGap) <= cut.at(VbfTight_NJetRapGap));

		value_VBFTight.at(VbfTight_JetInvM) = mjj_;
		pass_VBFTight.at(VbfTight_JetInvM) = (value_VBFTight.at(VbfTight_JetInvM) > cut.at(VbfTight_JetInvM));
	}
	else{
		pass_VBFTight.at(VbfTight_DeltaEta) = true;
		pass_VBFTight.at(VbfTight_NJetRapGap) = true;
		pass_VBFTight.at(VbfTight_JetInvM) = true;
	}

	value_VBFTight.at(VbfTight_HiggsPt) = higgsPt_;
	pass_VBFTight.at(VbfTight_HiggsPt) = (value_VBFTight.at(VbfTight_HiggsPt) > cut.at(VbfTight_HiggsPt));

	// migrate into main analysis if this is chosen category
	return migrateCategoryIntoMain("VBFTight",value_VBFTight, pass_VBFTight,VbfTight_NCuts);
}
