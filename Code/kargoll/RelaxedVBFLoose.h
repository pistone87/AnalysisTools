/*
 * RelaxedVBFLoose.h
 *
 *  Created on: May 8, 2015
 *      Author: kargoll
 */

#ifndef RELAXEDVBFLOOSE_H_
#define RELAXEDVBFLOOSE_H_

#include "CategoryStandalone.h"

class RelaxedVBFLoose: public CategoryStandalone {
public:
	RelaxedVBFLoose();
	RelaxedVBFLoose(unsigned nJets, float deltaEta, unsigned nJetRapGap, float jetInvM);
	virtual ~RelaxedVBFLoose();

	// enumerator for the cuts in this category
	enum cuts_relaxedVBFLoose {
		NJet	= 0,
		DeltaEta,
		NJetRapGap,
		JetInvM,
		NCuts
	};

private:
	void execute();
	void setCutValues();
};

#endif /* RELAXEDVBFLOOSE_H_ */
