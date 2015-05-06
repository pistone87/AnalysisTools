/*
 * OneJetLow.cxx
 *
 *  Created on: Mar 21, 2014
 *      Author: kargoll
 */

#include "OneJetLow.h"
#include "SimpleFits/FitSoftware/interface/Logger.h"

OneJetLow::OneJetLow(TString Name_, TString id_):
	HToTaumuTauh(Name_,id_)
{
	Logger(Logger::Info) << "Setting up the class OneJetLow" << std::endl;
	// run OneJetLow category
	categoryFlag = "OneJetLow";
}

OneJetLow::~OneJetLow() {
	  for(unsigned int j=0; j<Npassed.size(); j++){
	    Logger(Logger::Info) << "Selection Summary before: "
		 << Npassed.at(j).GetBinContent(1)     << " +/- " << Npassed.at(j).GetBinError(1)     << " after: "
		 << Npassed.at(j).GetBinContent(NCuts+1) << " +/- " << Npassed.at(j).GetBinError(NCuts) << std::endl;
	  }
	  Logger(Logger::Info) << "Done." << std::endl;
}

void OneJetLow::categoryConfiguration(){
	// set cut values to be the cut values of this category
	cut.at(OneJetLow_NJet) = 1;
	cut.at(OneJetLow_NotVbf) = true;
	cut.at(OneJetLow_TauPt) = cCat_splitTauPt;

	// set histograms of category cuts
	TString hlabel;
	TString htitle;
	TString c;

	title.at(OneJetLow_NJet)="Number Jets $>=$";
	title.at(OneJetLow_NJet)+=cut.at(OneJetLow_NJet);
	htitle=title.at(OneJetLow_NJet);
	htitle.ReplaceAll("$","");
	htitle.ReplaceAll("\\","#");
	hlabel="Number of Jets";
	c="_Cut_";c+=OneJetLow_NJet;
	Nminus1.at(OneJetLow_NJet) = HConfig.GetTH1D(Name+c+"_Nminus1_OneJetLow_NJet_",htitle,11,-0.5,10.5,hlabel,"Events");
	Nminus0.at(OneJetLow_NJet) = HConfig.GetTH1D(Name+c+"_Nminus0_OneJetLow_NJet_",htitle,11,-0.5,10.5,hlabel,"Events");

	title.at(OneJetLow_NotVbf)="Not VBFTight or VBFLoose $==$";
	title.at(OneJetLow_NotVbf)+=cut.at(OneJetLow_NotVbf);
	htitle=title.at(OneJetLow_NotVbf);
	htitle.ReplaceAll("$","");
	htitle.ReplaceAll("\\","#");
	hlabel="Did not pass VBF cat.";
	c="_Cut_";c+=OneJetLow_NotVbf;
	Nminus1.at(OneJetLow_NotVbf) = HConfig.GetTH1D(Name+c+"_Nminus1_OneJetLow_NotVBF_",htitle,2,-0.5,1.5,hlabel,"Events");
	Nminus0.at(OneJetLow_NotVbf) = HConfig.GetTH1D(Name+c+"_Nminus0_OneJetLow_NotVBF_",htitle,2,-0.5,1.5,hlabel,"Events");

	title.at(OneJetLow_TauPt)="$p_{T}(\\tau_{h}) <$";
	title.at(OneJetLow_TauPt)+=cut.at(OneJetLow_TauPt);
	title.at(OneJetLow_TauPt)+=" GeV";
	htitle=title.at(OneJetLow_TauPt);
	htitle.ReplaceAll("$","");
	htitle.ReplaceAll("\\","#");
	hlabel="p_{T}(\\tau_{h})/GeV";
	c="_Cut_";c+=OneJetLow_TauPt;
	Nminus1.at(OneJetLow_TauPt) = HConfig.GetTH1D(Name+c+"_Nminus1_OneJetLow_TauPt_",htitle,50,0.,200.,hlabel,"Events");
	Nminus0.at(OneJetLow_TauPt) = HConfig.GetTH1D(Name+c+"_Nminus0_OneJetLow_TauPt_",htitle,50,0.,200.,hlabel,"Events");
}

bool OneJetLow::categorySelection(){
	bool categoryPass = true;
	std::vector<float> value_OneJetLow(NCuts,-10);
	std::vector<float> pass_OneJetLow(NCuts,false);

	value_OneJetLow.at(OneJetLow_NJet) = nJets_;
	pass_OneJetLow.at(OneJetLow_NJet) = ( value_OneJetLow.at(OneJetLow_NJet) >= cut.at(OneJetLow_NJet) );

	value_OneJetLow.at(OneJetLow_NotVbf) = !passedVBF_;
	pass_OneJetLow.at(OneJetLow_NotVbf) = ( value_OneJetLow.at(OneJetLow_NotVbf) == cut.at(OneJetLow_NotVbf) );

	if (selTau == -1){
		// TauPt cut is set to true for nice N-0 and N-1 plots
		value_OneJetLow.at(OneJetLow_TauPt) = -10.;
		pass_OneJetLow.at(OneJetLow_TauPt) = true;
		// whole category is failing selection, to avoid NCat > 1
		categoryPass = false;
	}
	else{
		value_OneJetLow.at(OneJetLow_TauPt) = tauPt_;
		pass_OneJetLow.at(OneJetLow_TauPt) = ( value_OneJetLow.at(OneJetLow_TauPt) < cut.at(OneJetLow_TauPt) );
	}

	// migrate into main analysis if this is chosen category
	categoryPass = migrateCategoryIntoMain("OneJetLow",value_OneJetLow, pass_OneJetLow,OneJetLow_NCuts) && categoryPass;
	return categoryPass;
}
