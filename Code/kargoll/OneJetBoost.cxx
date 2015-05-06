/*
 * OneJetBoost.cxx
 *
 *  Created on: Mar 21, 2014
 *      Author: kargoll
 */

#include "OneJetBoost.h"
#include "SimpleFits/FitSoftware/interface/Logger.h"

OneJetBoost::OneJetBoost(TString Name_, TString id_):
	HToTaumuTauh(Name_,id_)
{
	Logger(Logger::Info) << "Setting up the class OneJetBoost" << std::endl;
	// run OneJetBoost category
	categoryFlag = "OneJetBoost";
}

OneJetBoost::~OneJetBoost() {
	  for(unsigned int j=0; j<Npassed.size(); j++){
	    Logger(Logger::Info) << "Selection Summary before: "
		 << Npassed.at(j).GetBinContent(1)     << " +/- " << Npassed.at(j).GetBinError(1)     << " after: "
		 << Npassed.at(j).GetBinContent(NCuts+1) << " +/- " << Npassed.at(j).GetBinError(NCuts) << std::endl;
	  }
	  Logger(Logger::Info) << "Done." << std::endl;
}

void OneJetBoost::categoryConfiguration(){
	// set cut values to be the cut values of this category
	cut.at(OneJetBoost_NJet)	= 1;
	cut.at(OneJetBoost_NotVbf)	= true;
	cut.at(OneJetBoost_TauPt)	= cCat_splitTauPt;
	cut.at(OneJetBoost_HiggsPt)	= 100.0;

	// set histograms of category cuts
	TString hlabel;
	TString htitle;
	TString c;

	title.at(OneJetBoost_NJet)="Number Jets $>=$";
	title.at(OneJetBoost_NJet)+=cut.at(OneJetBoost_NJet);
	htitle=title.at(OneJetBoost_NJet);
	htitle.ReplaceAll("$","");
	htitle.ReplaceAll("\\","#");
	hlabel="Number of Jets";
	c="_Cut_";c+=OneJetBoost_NJet;
	Nminus1.at(OneJetBoost_NJet) = HConfig.GetTH1D(Name+c+"_Nminus1_OneJetBoost_NJet_",htitle,11,-0.5,10.5,hlabel,"Events");
	Nminus0.at(OneJetBoost_NJet) = HConfig.GetTH1D(Name+c+"_Nminus0_OneJetBoost_NJet_",htitle,11,-0.5,10.5,hlabel,"Events");

	title.at(OneJetBoost_NotVbf)="No VBF $==$";
	title.at(OneJetBoost_NotVbf)+=cut.at(OneJetBoost_NotVbf);
	htitle=title.at(OneJetBoost_NotVbf);
	htitle.ReplaceAll("$","");
	htitle.ReplaceAll("\\","#");
	hlabel="Did not pass VBF cat.";
	c="_Cut_";c+=OneJetBoost_NotVbf;
	Nminus1.at(OneJetBoost_NotVbf) = HConfig.GetTH1D(Name+c+"_Nminus1_OneJetBoost_NotVbf_",htitle,2,-0.5,1.5,hlabel,"Events");
	Nminus0.at(OneJetBoost_NotVbf) = HConfig.GetTH1D(Name+c+"_Nminus0_OneJetBoost_NotVbf_",htitle,2,-0.5,1.5,hlabel,"Events");

	title.at(OneJetBoost_TauPt)="$p_{T}(\\tau_{h}) >=$";
	title.at(OneJetBoost_TauPt)+=cut.at(OneJetBoost_TauPt);
	title.at(OneJetBoost_TauPt)+=" GeV";
	htitle=title.at(OneJetBoost_TauPt);
	htitle.ReplaceAll("$","");
	htitle.ReplaceAll("\\","#");
	hlabel="p_{T}(\\tau_{h})/GeV";
	c="_Cut_";c+=OneJetBoost_TauPt;
	Nminus1.at(OneJetBoost_TauPt) = HConfig.GetTH1D(Name+c+"_Nminus1_OneJetBoost_TauPt_",htitle,50,0.,200.,hlabel,"Events");
	Nminus0.at(OneJetBoost_TauPt) = HConfig.GetTH1D(Name+c+"_Nminus0_OneJetBoost_TauPt_",htitle,50,0.,200.,hlabel,"Events");

	title.at(OneJetBoost_HiggsPt)="$p_{T}(H) >=$";
	title.at(OneJetBoost_HiggsPt)+=cut.at(OneJetBoost_HiggsPt);
	title.at(OneJetBoost_HiggsPt)+=" GeV";
	htitle=title.at(OneJetBoost_HiggsPt);
	htitle.ReplaceAll("$","");
	htitle.ReplaceAll("\\","#");
	hlabel="p_{T} of Higgs candidate";
	c="_Cut_";c+=OneJetBoost_HiggsPt;
	Nminus1.at(OneJetBoost_HiggsPt) = HConfig.GetTH1D(Name+c+"_Nminus1_OneJetBoost_HiggsPt_",htitle,50,0.,200.,hlabel,"Events");
	Nminus0.at(OneJetBoost_HiggsPt) = HConfig.GetTH1D(Name+c+"_Nminus0_OneJetBoost_HiggsPt_",htitle,50,0.,200.,hlabel,"Events");
}

bool OneJetBoost::categorySelection(){
	bool categoryPass = true;
	std::vector<float> value_OneJetBoost(NCuts,-10);
	std::vector<float> pass_OneJetBoost(NCuts,false);

	value_OneJetBoost.at(OneJetBoost_NJet) = nJets_;
	pass_OneJetBoost.at(OneJetBoost_NJet) = ( value_OneJetBoost.at(OneJetBoost_NJet) >= cut.at(OneJetBoost_NJet) );

	value_OneJetBoost.at(OneJetBoost_NotVbf) = passedVBF_;
	pass_OneJetBoost.at(OneJetBoost_NotVbf) = ( value_OneJetBoost.at(OneJetBoost_NotVbf) == cut.at(OneJetBoost_NotVbf) );

	if (selTau == -1){
		// TauPt cut is set to true for nice N-0 and N-1 plots
		value_OneJetBoost.at(OneJetBoost_TauPt) = -10.;
		pass_OneJetBoost.at(OneJetBoost_TauPt) = true;
		// whole category is failing selection, to avoid NCat > 1
		categoryPass = false;
	}
	else{
		value_OneJetBoost.at(OneJetBoost_TauPt) = tauPt_;
		pass_OneJetBoost.at(OneJetBoost_TauPt) = ( value_OneJetBoost.at(OneJetBoost_TauPt) >= cut.at(OneJetBoost_TauPt) );
	}

	value_OneJetBoost.at(OneJetBoost_HiggsPt) = higgsPt_;
	pass_OneJetBoost.at(OneJetBoost_HiggsPt) = (value_OneJetBoost.at(OneJetBoost_HiggsPt) >= cut.at(OneJetBoost_HiggsPt));

	// migrate into main analysis if this is chosen category
	categoryPass = migrateCategoryIntoMain("OneJetBoost",value_OneJetBoost, pass_OneJetBoost,OneJetBoost_NCuts) && categoryPass;
	return categoryPass;
}
