/*
 * RelaxedVBFTight.h
 *
 *  Created on: May 8, 2015
 *      Author: kargoll
 */

#ifndef RELAXEDVBFTIGHT_H_
#define RELAXEDVBFTIGHT_H_

#include "CategoryStandalone.h"

class RelaxedVBFTight: public CategoryStandalone {
public:
	RelaxedVBFTight();
	RelaxedVBFTight(unsigned nJets, float deltaEta, unsigned nJetRapGap, float jetInvM, float higgsPt);
	virtual ~RelaxedVBFTight();

	// enumerator for the cuts in this category
	enum cuts_relaxedVBFTight {
		NJet	= 0,
		DeltaEta,
		NJetRapGap,
		JetInvM,
		HiggsPt,
		NCuts
	};

private:
	void execute();
	void setCutValues();
};

#endif /* RELAXEDVBFTIGHT_H_ */
