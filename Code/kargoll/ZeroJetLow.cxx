/*
 * ZeroJetLow.cxx
 *
 *  Created on: Mar 21, 2014
 *      Author: kargoll
 */

#include "ZeroJetLow.h"
#include "SimpleFits/FitSoftware/interface/Logger.h"

ZeroJetLow::ZeroJetLow(TString Name_, TString id_):
	Category(Name_,id_)
{
	Logger(Logger::Info) << "Setting up the class ZeroJetLow" << std::endl;
	// run ZeroJetLow category
	categoryFlag = "ZeroJetLow";
}

ZeroJetLow::~ZeroJetLow() {
	  for(unsigned int j=0; j<Npassed.size(); j++){
	    Logger(Logger::Info) << "Selection Summary before: "
		 << Npassed.at(j).GetBinContent(1)     << " +/- " << Npassed.at(j).GetBinError(1)     << " after: "
		 << Npassed.at(j).GetBinContent(NCuts+1) << " +/- " << Npassed.at(j).GetBinError(NCuts) << std::endl;
	  }
	  Logger(Logger::Info) << "Done." << std::endl;
}

void ZeroJetLow::categoryConfiguration(){
	// set cut values for this category
	cut.at(ZeroJetLow_NJet)		= 0;
	cut.at(ZeroJetLow_TauPt)	= cCat_splitTauPt;

	// set histograms of category cuts
	TString hlabel;
	TString htitle;
	TString c;

	title.at(ZeroJetLow_NJet)="Number Jets $<=$";
	title.at(ZeroJetLow_NJet)+=cut.at(ZeroJetLow_NJet);
	htitle=title.at(ZeroJetLow_NJet);
	htitle.ReplaceAll("$","");
	htitle.ReplaceAll("\\","#");
	hlabel="Number of Jets";
	c="_Cut_";c+=ZeroJetLow_NJet;
	Nminus1.at(ZeroJetLow_NJet) = HConfig.GetTH1D(Name+c+"_Nminus1_ZeroJetLow_NJet_",htitle,11,-0.5,10.5,hlabel,"Events");
	Nminus0.at(ZeroJetLow_NJet) = HConfig.GetTH1D(Name+c+"_Nminus0_ZeroJetLow_NJet_",htitle,11,-0.5,10.5,hlabel,"Events");

	title.at(ZeroJetLow_TauPt)="$p_{T}(\\tau_{h}) <$";
	title.at(ZeroJetLow_TauPt)+=cut.at(ZeroJetLow_TauPt);
	title.at(ZeroJetLow_TauPt)+=" GeV";
	htitle=title.at(ZeroJetLow_TauPt);
	htitle.ReplaceAll("$","");
	htitle.ReplaceAll("\\","#");
	hlabel="p_{T}(\\tau_{h})/GeV";
	c="_Cut_";c+=ZeroJetLow_TauPt;
	Nminus1.at(ZeroJetLow_TauPt) = HConfig.GetTH1D(Name+c+"_Nminus1_ZeroJetLow_TauPt_",htitle,50,0.,200.,hlabel,"Events");
	Nminus0.at(ZeroJetLow_TauPt) = HConfig.GetTH1D(Name+c+"_Nminus0_ZeroJetLow_TauPt_",htitle,40,0.,200.,hlabel,"Events");
}

bool ZeroJetLow::categorySelection(){
	bool categoryPass = true;
	std::vector<float> value_ZeroJetLow(NCuts,-10);
	std::vector<float> pass_ZeroJetLow(NCuts,false);

	value_ZeroJetLow.at(ZeroJetLow_NJet) = nJets_;
	pass_ZeroJetLow.at(ZeroJetLow_NJet) = ( value_ZeroJetLow.at(ZeroJetLow_NJet) <= cut.at(ZeroJetLow_NJet) );
	std::cout << " Event with " << nJets_ << " jets has passed cut: " << pass_ZeroJetLow.at(ZeroJetLow_NJet) << std::endl;

	if (selTau == -1){
		// TauPt cut is set to true for nice N-0 and N-1 plots
		value_ZeroJetLow.at(ZeroJetLow_TauPt) = -10.;
		pass_ZeroJetLow.at(ZeroJetLow_TauPt) = true;
		// whole category is failing selection, to avoid NCat > 1
		categoryPass = false;
	}
	else{
		value_ZeroJetLow.at(ZeroJetLow_TauPt) = tauPt_;
		pass_ZeroJetLow.at(ZeroJetLow_TauPt) = ( value_ZeroJetLow.at(ZeroJetLow_TauPt) < cut.at(ZeroJetLow_TauPt) );
	}

	// migrate into main analysis if this is chosen category
	categoryPass = migrateCategoryIntoMain("ZeroJetLow",value_ZeroJetLow, pass_ZeroJetLow,ZeroJetLow_NCuts) && categoryPass;
	return categoryPass;

}
