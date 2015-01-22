/*
 * SVfitProvider.cxx
 *
 *  Created on: Jan 19, 2015
 *      Author: kargoll
 */

#include "SVfitProvider.h"

void SVfitProvider::createSvFitAlgo() {
	svFitAlgo_ = new SVfitStandaloneAlgorithm(inputTauLeptons_, inputMet_.met3D(), inputMet_.significanceMatrix(), verbosity_);
	svFitAlgo_->addLogM(addLogM_);
	if (maxObjFunctionCalls_ >= 0) svFitAlgo_->maxObjFunctionCalls(maxObjFunctionCalls_);
	if (metPower_ >= 0)	svFitAlgo_->metPower(metPower_);
	isSetup_ = true;
}

// constructor to be used in analysis
SVfitProvider::SVfitProvider(Ntuple_Controller* const Ntp, objects::MET& met, TString typeLep1, int idxLep1, TString typeLep2, int idxLep2, int verbosity){
	ntp_ = Ntp;
	inputMet_ = met;

	addMeasuredLepton(typeLep1, idxLep1);
	addMeasuredLepton(typeLep2, idxLep2);

	verbosity_ = verbosity;

	// use default settings for SVfit
	// can be switched by individual functions
	fitMethod_ = "MarkovChain";
	addLogM_ = false;
	maxObjFunctionCalls_ = -1;
	metPower_ = -1;

	createSvFitAlgo();
}

SVfitProvider::~SVfitProvider() {
	delete svFitAlgo_;
}

// execute SVfit
void SVfitProvider::run(){
	if ( !isSetup_ )
		createSvFitAlgo();

	if(fitMethod_ == "MarkovChain")
		svFitAlgo_->integrateMarkovChain();
	else if(fitMethod_ == "Vegas")
		svFitAlgo_->integrateVEGAS();
	else if(fitMethod_ == "Minuit")
		svFitAlgo_->fit();
	else{
		std::cout << "WARNING: Method " << fitMethod_ << " not available for SVfit. Using MarkovChain integration..." << std::endl;
		svFitAlgo_->integrateMarkovChain();
	}

}

// convert TLorentzVector into ROOT::Math::LorentzVector
svFitStandalone::LorentzVector SVfitProvider::convert_p4Vect(const TLorentzVector& in){
	return svFitStandalone::LorentzVector(in.X(), in.Y(), in.Z(), in.T());
}

void SVfitProvider::addMeasuredLepton(TString type, int index){
	svFitStandalone::MeasuredTauLepton lep;
	type.ToLower();
	if (type == "mu"){
		lep = svFitStandalone::MeasuredTauLepton(svFitStandalone::kTauToMuDecay, convert_p4Vect(ntp_->Muon_p4(index)));
	}
	else if (type == "ele"){
		lep = svFitStandalone::MeasuredTauLepton(svFitStandalone::kTauToElecDecay, convert_p4Vect(ntp_->Electron_p4(index)));
	}
	else if (type == "tau"){
		lep = svFitStandalone::MeasuredTauLepton(svFitStandalone::kTauToHadDecay, convert_p4Vect(ntp_->PFTau_p4(index)));
	}
	else
		std::cout << "ERROR: Object type " << type << " not implemented in SVfitProvider." << std::endl;

	inputTauLeptons_.push_back(lep);
	isSetup_ = false;
}
