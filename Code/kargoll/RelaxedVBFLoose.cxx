/*
 * RelaxedVBFLoose.cxx
 *
 *  Created on: May 8, 2015
 *      Author: kargoll
 */

#include "RelaxedVBFLoose.h"
#include "SimpleFits/FitSoftware/interface/Logger.h"

// only set cut values
RelaxedVBFLoose::RelaxedVBFLoose() {
	setCutValues();
}

RelaxedVBFLoose::RelaxedVBFLoose(unsigned nJets, float deltaEta, unsigned nJetRapGap, float jetInvM){
	nCuts_ = 4;
	cutValues_.resize(nCuts_);
	eventValues_.resize(nCuts_);
	passCut_.resize(nCuts_);

	setCutValues();

	eventValues_.at(NJet)		= nJets;
	eventValues_.at(DeltaEta)	= deltaEta;
	eventValues_.at(NJetRapGap)	= nJetRapGap;
	eventValues_.at(JetInvM)	= jetInvM;

	execute();
}

RelaxedVBFLoose::~RelaxedVBFLoose() {
}

void RelaxedVBFLoose::setCutValues() {
	cutValues_.at(NJet)			= 2;
	cutValues_.at(DeltaEta)		= 2.0;
	cutValues_.at(NJetRapGap)	= 0;
	cutValues_.at(JetInvM)		= 200.0;
}

void RelaxedVBFLoose::execute(){
	passCut_.at(NJet) = (eventValues_.at(NJet) >= cutValues_.at(NJet));

	if(passCut_.at(NJet)){
		passCut_.at(DeltaEta) = (fabs(eventValues_.at(DeltaEta)) > cutValues_.at(DeltaEta));
		passCut_.at(NJetRapGap) = (eventValues_.at(NJetRapGap) <= cutValues_.at(NJetRapGap));
		passCut_.at(JetInvM) = (eventValues_.at(JetInvM) > cutValues_.at(JetInvM));
	}
	else{
		passCut_.at(DeltaEta) = true;
		passCut_.at(NJetRapGap) = true;
		passCut_.at(JetInvM) = true;
	}
}
