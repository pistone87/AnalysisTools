/*
 * ZeroJetHigh.cxx
 *
 *  Created on: Mar 21, 2014
 *      Author: kargoll
 */

#include "ZeroJetHigh.h"
#include "SimpleFits/FitSoftware/interface/Logger.h"

ZeroJetHigh::ZeroJetHigh(TString Name_, TString id_):
	HToTaumuTauh(Name_,id_)
{
	Logger(Logger::Info) << "Setting up the class ZeroJetHigh" << std::endl;
	// run ZeroJetHigh category
	categoryFlag = "ZeroJetHigh";
}

ZeroJetHigh::~ZeroJetHigh() {
	  for(unsigned int j=0; j<Npassed.size(); j++){
	    Logger(Logger::Info) << "Selection Summary before: "
		 << Npassed.at(j).GetBinContent(1)     << " +/- " << Npassed.at(j).GetBinError(1)     << " after: "
		 << Npassed.at(j).GetBinContent(NCuts+1) << " +/- " << Npassed.at(j).GetBinError(NCuts) << std::endl;
	  }
	  Logger(Logger::Info) << "Done." << std::endl;
}

void ZeroJetHigh::categoryConfiguration(){
	// set cut values to be the cut values of this category
	cut.at(ZeroJetHigh_NJet)	= 0;
	cut.at(ZeroJetHigh_TauPt)	= cCat_splitTauPt;

	// set histograms of category cuts
	TString hlabel;
	TString htitle;
	TString c;

	title.at(ZeroJetHigh_NJet)="Number Jets $<=$";
	title.at(ZeroJetHigh_NJet)+=cut.at(ZeroJetHigh_NJet);
	htitle=title.at(ZeroJetHigh_NJet);
	htitle.ReplaceAll("$","");
	htitle.ReplaceAll("\\","#");
	hlabel="Number of Jets";
	c="_Cut_";c+=ZeroJetHigh_NJet;
	Nminus1.at(ZeroJetHigh_NJet) = HConfig.GetTH1D(Name+c+"_Nminus1_ZeroJetHigh_NJet_",htitle,11,-0.5,10.5,hlabel,"Events");
	Nminus0.at(ZeroJetHigh_NJet) = HConfig.GetTH1D(Name+c+"_Nminus0_ZeroJetHigh_NJet_",htitle,11,-0.5,10.5,hlabel,"Events");

	title.at(ZeroJetHigh_TauPt)="$p_{T}(\\tau_{h}) >=$";
	title.at(ZeroJetHigh_TauPt)+=cut.at(ZeroJetHigh_TauPt);
	title.at(ZeroJetHigh_TauPt)+=" GeV";
	htitle=title.at(ZeroJetHigh_TauPt);
	htitle.ReplaceAll("$","");
	htitle.ReplaceAll("\\","#");
	hlabel="p_{T}(\\tau_{h})/GeV";
	c="_Cut_";c+=ZeroJetHigh_TauPt;
	Nminus1.at(ZeroJetHigh_TauPt) = HConfig.GetTH1D(Name+c+"_Nminus1_ZeroJetHigh_TauPt_",htitle,50,0.,200.,hlabel,"Events");
	Nminus0.at(ZeroJetHigh_TauPt) = HConfig.GetTH1D(Name+c+"_Nminus0_ZeroJetHigh_TauPt_",htitle,50,0.,200.,hlabel,"Events");
}

bool ZeroJetHigh::categorySelection(){
	bool categoryPass = true;
	std::vector<float> value_ZeroJetHigh(NCuts,-10);
	std::vector<float> pass_ZeroJetHigh(NCuts,false);

	value_ZeroJetHigh.at(ZeroJetHigh_NJet) = nJets_;
	pass_ZeroJetHigh.at(ZeroJetHigh_NJet) = ( value_ZeroJetHigh.at(ZeroJetHigh_NJet) <= cut.at(ZeroJetHigh_NJet) );

	if (selTau == -1){
		// TauPt cut is set to true for nice N-0 and N-1 plots
		value_ZeroJetHigh.at(ZeroJetHigh_TauPt) = -10.;
		pass_ZeroJetHigh.at(ZeroJetHigh_TauPt) = true;
		// whole category is failing selection, to avoid NCat > 1
		categoryPass = false;
	}
	else{
		value_ZeroJetHigh.at(ZeroJetHigh_TauPt) = tauPt_;
		pass_ZeroJetHigh.at(ZeroJetHigh_TauPt) = ( value_ZeroJetHigh.at(ZeroJetHigh_TauPt) >= cut.at(ZeroJetHigh_TauPt) );
	}

	// migrate into main analysis if this is chosen category
	categoryPass = migrateCategoryIntoMain("ZeroJetHigh",value_ZeroJetHigh, pass_ZeroJetHigh,ZeroJetHigh_NCuts) && categoryPass;
	return categoryPass;
}
