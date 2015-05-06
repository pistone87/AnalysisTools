/*
 * OneJetHigh.cxx
 *
 *  Created on: Mar 21, 2014
 *      Author: kargoll
 */

#include "OneJetHigh.h"
#include "SimpleFits/FitSoftware/interface/Logger.h"

OneJetHigh::OneJetHigh(TString Name_, TString id_):
	HToTaumuTauh(Name_,id_)
{
	Logger(Logger::Info) << "Setting up the class OneJetHigh" << std::endl;
	// run OneJetHigh category
	categoryFlag = "OneJetHigh";
}

OneJetHigh::~OneJetHigh() {
	  for(unsigned int j=0; j<Npassed.size(); j++){
	    Logger(Logger::Info) << "Selection Summary before: "
		 << Npassed.at(j).GetBinContent(1)     << " +/- " << Npassed.at(j).GetBinError(1)     << " after: "
		 << Npassed.at(j).GetBinContent(NCuts+1) << " +/- " << Npassed.at(j).GetBinError(NCuts) << std::endl;
	  }
	  Logger(Logger::Info) << "Done." << std::endl;
}

void OneJetHigh::categoryConfiguration(){
	// to be called only if OneJetHigh is chosen category

	// set cut values to be the cut values of this category
	cut.at(OneJetHigh_NJet) = 1;
	cut.at(OneJetHigh_NotVbf) = true;
	cut.at(OneJetHigh_TauPt) = cCat_splitTauPt;
	cut.at(OneJetHigh_HiggsPt) = 100.0;

	// set histograms of category cuts
	TString hlabel;
	TString htitle;
	TString c;

	title.at(OneJetHigh_NJet)="Number Jets $>=$";
	title.at(OneJetHigh_NJet)+=cut.at(OneJetHigh_NJet);
	htitle=title.at(OneJetHigh_NJet);
	htitle.ReplaceAll("$","");
	htitle.ReplaceAll("\\","#");
	hlabel="Number of Jets";
	c="_Cut_";c+=OneJetHigh_NJet;
	Nminus1.at(OneJetHigh_NJet) = HConfig.GetTH1D(Name+c+"_Nminus1_OneJetHigh_NJet_",htitle,11,-0.5,10.5,hlabel,"Events");
	Nminus0.at(OneJetHigh_NJet) = HConfig.GetTH1D(Name+c+"_Nminus0_OneJetHigh_NJet_",htitle,11,-0.5,10.5,hlabel,"Events");

	title.at(OneJetHigh_NotVbf)="No VBF $==$";
	title.at(OneJetHigh_NotVbf)+=cut.at(OneJetHigh_NotVbf);
	htitle=title.at(OneJetHigh_NotVbf);
	htitle.ReplaceAll("$","");
	htitle.ReplaceAll("\\","#");
	hlabel="Did not pass VBF cat.";
	c="_Cut_";c+=OneJetHigh_NotVbf;
	Nminus1.at(OneJetHigh_NotVbf) = HConfig.GetTH1D(Name+c+"_Nminus1_OneJetHigh_NotVbf_",htitle,2,-0.5,1.5,hlabel,"Events");
	Nminus0.at(OneJetHigh_NotVbf) = HConfig.GetTH1D(Name+c+"_Nminus0_OneJetHigh_NotVbf_",htitle,2,-0.5,1.5,hlabel,"Events");

	title.at(OneJetHigh_TauPt)="$p_{T}(\\tau_{h}) >=$";
	title.at(OneJetHigh_TauPt)+=cut.at(OneJetHigh_TauPt);
	title.at(OneJetHigh_TauPt)+=" GeV";
	htitle=title.at(OneJetHigh_TauPt);
	htitle.ReplaceAll("$","");
	htitle.ReplaceAll("\\","#");
	hlabel="p_{T}(\\tau_{h})/GeV";
	c="_Cut_";c+=OneJetHigh_TauPt;
	Nminus1.at(OneJetHigh_TauPt) = HConfig.GetTH1D(Name+c+"_Nminus1_OneJetHigh_TauPt_",htitle,50,0.,200.,hlabel,"Events");
	Nminus0.at(OneJetHigh_TauPt) = HConfig.GetTH1D(Name+c+"_Nminus0_OneJetHigh_TauPt_",htitle,50,0.,200.,hlabel,"Events");

	title.at(OneJetHigh_HiggsPt)="$p_{T}(H) <$";
	title.at(OneJetHigh_HiggsPt)+=cut.at(OneJetHigh_HiggsPt);
	title.at(OneJetHigh_HiggsPt)+=" GeV";
	htitle=title.at(OneJetHigh_HiggsPt);
	htitle.ReplaceAll("$","");
	htitle.ReplaceAll("\\","#");
	hlabel="p_{T} of Higgs candidate";
	c="_Cut_";c+=OneJetHigh_HiggsPt;
	Nminus1.at(OneJetHigh_HiggsPt) = HConfig.GetTH1D(Name+c+"_Nminus1_OneJetHigh_HiggsPt_",htitle,50,0.,200.,hlabel,"Events");
	Nminus0.at(OneJetHigh_HiggsPt) = HConfig.GetTH1D(Name+c+"_Nminus0_OneJetHigh_HiggsPt_",htitle,50,0.,200.,hlabel,"Events");
}

bool OneJetHigh::categorySelection(){
	bool categoryPass = true;
	std::vector<float> value_OneJetHigh(NCuts,-10);
	std::vector<float> pass_OneJetHigh(NCuts,false);

	value_OneJetHigh.at(OneJetHigh_NJet) = nJets_;
	pass_OneJetHigh.at(OneJetHigh_NJet) = ( value_OneJetHigh.at(OneJetHigh_NJet) >= cut.at(OneJetHigh_NJet) );

	value_OneJetHigh.at(OneJetHigh_NotVbf) = !passedVBF_;
	pass_OneJetHigh.at(OneJetHigh_NotVbf) = ( value_OneJetHigh.at(OneJetHigh_NotVbf) == cut.at(OneJetHigh_NotVbf) );

	if (selTau == -1){
		// TauPt cut is set to true for nice N-0 and N-1 plots
		value_OneJetHigh.at(OneJetHigh_TauPt) = -10.;
		pass_OneJetHigh.at(OneJetHigh_TauPt) = true;
		// whole category is failing selection, to avoid NCat > 1
		categoryPass = false;
	}
	else{
		value_OneJetHigh.at(OneJetHigh_TauPt) = tauPt_;
		pass_OneJetHigh.at(OneJetHigh_TauPt) = ( value_OneJetHigh.at(OneJetHigh_TauPt) >= cut.at(OneJetHigh_TauPt) );
	}

	value_OneJetHigh.at(OneJetHigh_HiggsPt) = higgsPt_;
	pass_OneJetHigh.at(OneJetHigh_HiggsPt) = (value_OneJetHigh.at(OneJetHigh_HiggsPt) < cut.at(OneJetHigh_HiggsPt));

	// migrate into main analysis if this is chosen category
	categoryPass = migrateCategoryIntoMain("OneJetHigh",value_OneJetHigh, pass_OneJetHigh,OneJetHigh_NCuts) && categoryPass;
	return categoryPass;
}
