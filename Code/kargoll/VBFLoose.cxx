/*
 * VBFLoose.cxx
 *
 *  Created on: Mar 21, 2014
 *      Author: kargoll
 */

#include "VBFLoose.h"
#include "VBFLooseStandalone.h"
#include "SimpleFits/FitSoftware/interface/Logger.h"

VBFLoose::VBFLoose(TString Name_, TString id_):
	Category(Name_,id_)
{
	Logger(Logger::Info) << "Setting up the class VBFLoose" << std::endl;
	// run VBFLoose category
	categoryFlag = "VBFLoose";
}

VBFLoose::~VBFLoose() {
	  for(unsigned int j=0; j<Npassed.size(); j++){
	    Logger(Logger::Info) << "Selection Summary before: "
		 << Npassed.at(j).GetBinContent(1)     << " +/- " << Npassed.at(j).GetBinError(1)     << " after: "
		 << Npassed.at(j).GetBinContent(NCuts+1) << " +/- " << Npassed.at(j).GetBinError(NCuts) << std::endl;
	  }
	  Logger(Logger::Info) << "Done." << std::endl;
}

void VBFLoose::categoryConfiguration(){
	// set cut values to be the cut values of this category
	VBFLooseStandalone vbfl;
	std::vector<float> vbflCuts = vbfl.get_cutValues();
	cut.at(NJet) 		= vbflCuts.at(VBFLooseStandalone::NJet);
	cut.at(DeltaEta)	= vbflCuts.at(VBFLooseStandalone::DeltaEta);
	cut.at(NJetRapGap)	= vbflCuts.at(VBFLooseStandalone::NJetRapGap);
	cut.at(JetInvM)		= vbflCuts.at(VBFLooseStandalone::JetInvM);
	cut.at(NotVbfTight)	= vbflCuts.at(VBFLooseStandalone::NotVbfTight);

	// set histograms of category cuts
	TString hlabel;
	TString htitle;
	TString c;

	title.at(NJet)="Number VBF Jets $>=$";
	title.at(NJet)+=cut.at(NJet);
	htitle=title.at(NJet);
	htitle.ReplaceAll("$","");
	htitle.ReplaceAll("\\","#");
	hlabel="Number of Jet_{VBF}";
	c="_Cut_";c+=NJet;
	Nminus1.at(NJet) = HConfig.GetTH1D(Name+c+"_Nminus1_NJet_",htitle,6,-0.5,5.5,hlabel,"Events");
	Nminus0.at(NJet) = HConfig.GetTH1D(Name+c+"_Nminus0_NJet_",htitle,6,-0.5,5.5,hlabel,"Events");

	title.at(DeltaEta)="$\\Delta\\eta(jj) >$";
	title.at(DeltaEta)+=cut.at(DeltaEta);
	htitle=title.at(DeltaEta);
	htitle.ReplaceAll("$","");
	htitle.ReplaceAll("\\","#");
	hlabel="#Delta#eta(Jet_{VBF}^{1},Jet_{VBF}^{2})";
	c="_Cut_";c+=DeltaEta;
	Nminus1.at(DeltaEta) = HConfig.GetTH1D(Name+c+"_Nminus1_DeltaEta_",htitle,32,-8.,8.,hlabel,"Events");
	Nminus0.at(DeltaEta) = HConfig.GetTH1D(Name+c+"_Nminus0_DeltaEta_",htitle,32,-8.,8.,hlabel,"Events");

	title.at(NJetRapGap)="Number Jets in $\\eta$ gap $<=$";
	title.at(NJetRapGap)+=cut.at(NJetRapGap);
	htitle=title.at(NJetRapGap);
	htitle.ReplaceAll("$","");
	htitle.ReplaceAll("\\","#");
	hlabel="Number of Jet_{VBF} in rapidity gap";
	c="_Cut_";c+=NJetRapGap;
	Nminus1.at(NJetRapGap) = HConfig.GetTH1D(Name+c+"_Nminus1_NJetRapGap_",htitle,6,-0.5,5.5,hlabel,"Events");
	Nminus0.at(NJetRapGap) = HConfig.GetTH1D(Name+c+"_Nminus0_NJetRapGap_",htitle,6,-0.5,5.5,hlabel,"Events");

	title.at(JetInvM)="$m_{jj}(VBF) >$";
	title.at(JetInvM)+=cut.at(JetInvM);
	title.at(JetInvM)+=" GeV";
	htitle=title.at(JetInvM);
	htitle.ReplaceAll("$","");
	htitle.ReplaceAll("\\","#");
	hlabel="m_{inv}(jj) of VBF-jets";
	c="_Cut_";c+=JetInvM;
	Nminus1.at(JetInvM) = HConfig.GetTH1D(Name+c+"_Nminus1_JetInvM_",htitle,20,0.,2000.,hlabel,"Events");
	Nminus0.at(JetInvM) = HConfig.GetTH1D(Name+c+"_Nminus0_JetInvM_",htitle,20,0.,2000.,hlabel,"Events");

	title.at(NotVbfTight)="Not VBFTight $==$";
	title.at(NotVbfTight)+=cut.at(NotVbfTight);
	htitle=title.at(NotVbfTight);
	htitle.ReplaceAll("$","");
	htitle.ReplaceAll("\\","#");
	hlabel="Did not pass VBFTight cat.";
	c="_Cut_";c+=NotVbfTight;
	Nminus1.at(NotVbfTight) = HConfig.GetTH1D(Name+c+"_Nminus1_NotVbfTight_",htitle,2,-0.5,1.5,hlabel,"Events");
	Nminus0.at(NotVbfTight) = HConfig.GetTH1D(Name+c+"_Nminus0_NotVbfTight_",htitle,2,-0.5,1.5,hlabel,"Events");
}

bool VBFLoose::categorySelection(){
	std::vector<float> value_VBFLoose(NCuts,-10);
	std::vector<float> pass_VBFLoose(NCuts,false);

	VBFLooseStandalone vbfl(nJets_, jetdEta_, nJetsInGap_, mjj_, passedVBFTight_);
	std::vector<float> values	= vbfl.get_eventValues();
	std::vector<bool>  pass 	= vbfl.get_passCut();

	for (unsigned i_cut = 0; i_cut < vbfl.get_nCuts(); i_cut++){
		value_VBFLoose.at(HToTaumuTauh::CatCut1 + i_cut) = values.at(i_cut);
		pass_VBFLoose.at(HToTaumuTauh::CatCut1 + i_cut) = pass.at(i_cut);
	}

	// migrate into main analysis if this is chosen category
	return migrateCategoryIntoMain("VBFLoose",value_VBFLoose, pass_VBFLoose,NCuts);
}
