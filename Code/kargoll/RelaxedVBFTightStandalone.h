/*
 * RelaxedVBFTightStandalone.h
 *
 *  Created on: May 8, 2015
 *      Author: kargoll
 */

#ifndef RelaxedVBFTightStandalone_H_
#define RelaxedVBFTightStandalone_H_

#include "CategoryStandalone.h"

class RelaxedVBFTightStandalone: public CategoryStandalone {
public:
	RelaxedVBFTightStandalone();
	RelaxedVBFTightStandalone(unsigned nJets, float deltaEta, unsigned nJetRapGap, float jetInvM, float higgsPt);
	virtual ~RelaxedVBFTightStandalone();

	// enumerator for the cuts in this category
	enum cuts_RelaxedVBFTightStandalone {
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

#endif /* RelaxedVBFTightStandalone_H_ */
