/*
 * VBFLoose.cxx
 *
 *  Created on: Mar 21, 2014
 *      Author: kargoll
 */

#include "VBFLoose.h"
#include "SimpleFits/FitSoftware/interface/Logger.h"

VBFLoose::VBFLoose(TString Name_, TString id_):
	HToTaumuTauh(Name_,id_)
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
	cut.at(VbfLoose_NJet) 		= 2;
	cut.at(VbfLoose_DeltaEta)	= 3.5;
	cut.at(VbfLoose_NJetRapGap) = 0;
	cut.at(VbfLoose_JetInvM)	= 500.0;
	cut.at(VbfLoose_NotVbfTight)= true;

	// set histograms of category cuts
	TString hlabel;
	TString htitle;
	TString c;

	title.at(VbfLoose_NJet)="Number VBF Jets $>=$";
	title.at(VbfLoose_NJet)+=cut.at(VbfLoose_NJet);
	htitle=title.at(VbfLoose_NJet);
	htitle.ReplaceAll("$","");
	htitle.ReplaceAll("\\","#");
	hlabel="Number of Jet_{VBF}";
	c="_Cut_";c+=VbfLoose_NJet;
	Nminus1.at(VbfLoose_NJet) = HConfig.GetTH1D(Name+c+"_Nminus1_VbfLoose_NJet_",htitle,6,-0.5,5.5,hlabel,"Events");
	Nminus0.at(VbfLoose_NJet) = HConfig.GetTH1D(Name+c+"_Nminus0_VbfLoose_NJet_",htitle,6,-0.5,5.5,hlabel,"Events");

	title.at(VbfLoose_DeltaEta)="$\\Delta\\eta(jj) >$";
	title.at(VbfLoose_DeltaEta)+=cut.at(VbfLoose_DeltaEta);
	htitle=title.at(VbfLoose_DeltaEta);
	htitle.ReplaceAll("$","");
	htitle.ReplaceAll("\\","#");
	hlabel="#Delta#eta(Jet_{VBF}^{1},Jet_{VBF}^{2})";
	c="_Cut_";c+=VbfLoose_DeltaEta;
	Nminus1.at(VbfLoose_DeltaEta) = HConfig.GetTH1D(Name+c+"_Nminus1_VbfLoose_DeltaEta_",htitle,32,-8.,8.,hlabel,"Events");
	Nminus0.at(VbfLoose_DeltaEta) = HConfig.GetTH1D(Name+c+"_Nminus0_VbfLoose_DeltaEta_",htitle,32,-8.,8.,hlabel,"Events");

	title.at(VbfLoose_NJetRapGap)="Number Jets in $\\eta$ gap $<=$";
	title.at(VbfLoose_NJetRapGap)+=cut.at(VbfLoose_NJetRapGap);
	htitle=title.at(VbfLoose_NJetRapGap);
	htitle.ReplaceAll("$","");
	htitle.ReplaceAll("\\","#");
	hlabel="Number of Jet_{VBF} in rapidity gap";
	c="_Cut_";c+=VbfLoose_NJetRapGap;
	Nminus1.at(VbfLoose_NJetRapGap) = HConfig.GetTH1D(Name+c+"_Nminus1_VbfLoose_NJetRapGap_",htitle,6,-0.5,5.5,hlabel,"Events");
	Nminus0.at(VbfLoose_NJetRapGap) = HConfig.GetTH1D(Name+c+"_Nminus0_VbfLoose_NJetRapGap_",htitle,6,-0.5,5.5,hlabel,"Events");

	title.at(VbfLoose_JetInvM)="$m_{jj}(VBF) >$";
	title.at(VbfLoose_JetInvM)+=cut.at(VbfLoose_JetInvM);
	title.at(VbfLoose_JetInvM)+=" GeV";
	htitle=title.at(VbfLoose_JetInvM);
	htitle.ReplaceAll("$","");
	htitle.ReplaceAll("\\","#");
	hlabel="m_{inv}(jj) of VBF-jets";
	c="_Cut_";c+=VbfLoose_JetInvM;
	Nminus1.at(VbfLoose_JetInvM) = HConfig.GetTH1D(Name+c+"_Nminus1_VbfLoose_JetInvM_",htitle,20,0.,2000.,hlabel,"Events");
	Nminus0.at(VbfLoose_JetInvM) = HConfig.GetTH1D(Name+c+"_Nminus0_VbfLoose_JetInvM_",htitle,20,0.,2000.,hlabel,"Events");

	title.at(VbfLoose_NotVbfTight)="Not VBFTight $==$";
	title.at(VbfLoose_NotVbfTight)+=cut.at(VbfLoose_NotVbfTight);
	htitle=title.at(VbfLoose_NotVbfTight);
	htitle.ReplaceAll("$","");
	htitle.ReplaceAll("\\","#");
	hlabel="Did not pass VBFTight cat.";
	c="_Cut_";c+=VbfLoose_NotVbfTight;
	Nminus1.at(VbfLoose_NotVbfTight) = HConfig.GetTH1D(Name+c+"_Nminus1_NotVbfTight_",htitle,2,-0.5,1.5,hlabel,"Events");
	Nminus0.at(VbfLoose_NotVbfTight) = HConfig.GetTH1D(Name+c+"_Nminus0_NotVbfTight_",htitle,2,-0.5,1.5,hlabel,"Events");
}

bool VBFLoose::categorySelection(){
	std::vector<float> value_VBFLoose(NCuts,-10);
	std::vector<float> pass_VBFLoose(NCuts,false);

	value_VBFLoose.at(VbfLoose_NJet) = nJets_;
	pass_VBFLoose.at(VbfLoose_NJet) = (value_VBFLoose.at(VbfLoose_NJet) >= cut.at(VbfLoose_NJet));

	if(pass_VBFLoose.at(VbfLoose_NJet)){
		value_VBFLoose.at(VbfLoose_DeltaEta) = jetdEta_;
		pass_VBFLoose.at(VbfLoose_DeltaEta) = (fabs(value_VBFLoose.at(VbfLoose_DeltaEta)) > cut.at(VbfLoose_DeltaEta));

		value_VBFLoose.at(VbfLoose_NJetRapGap) = nJetsInGap_;
		pass_VBFLoose.at(VbfLoose_NJetRapGap) = (value_VBFLoose.at(VbfLoose_NJetRapGap) <= cut.at(VbfLoose_NJetRapGap));

		value_VBFLoose.at(VbfLoose_JetInvM) = mjj_;
		pass_VBFLoose.at(VbfLoose_JetInvM) = (value_VBFLoose.at(VbfLoose_JetInvM) > cut.at(VbfLoose_JetInvM));
	}
	else{
		pass_VBFLoose.at(VbfLoose_DeltaEta) = true;
		pass_VBFLoose.at(VbfLoose_NJetRapGap) = true;
		pass_VBFLoose.at(VbfLoose_JetInvM) = true;
	}

	value_VBFLoose.at(VbfLoose_NotVbfTight) = !passedVBFTight_;
	pass_VBFLoose.at(VbfLoose_NotVbfTight) = ( value_VBFLoose.at(VbfLoose_NotVbfTight) == cut.at(VbfLoose_NotVbfTight) );

	// migrate into main analysis if this is chosen category
	return migrateCategoryIntoMain("VBFLoose",value_VBFLoose, pass_VBFLoose,VbfLoose_NCuts);
}
