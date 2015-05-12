/*
 * RelaxedVBFLooseStandalone.h
 *
 *  Created on: May 8, 2015
 *      Author: kargoll
 */

#ifndef RelaxedVBFLooseStandalone_H_
#define RelaxedVBFLooseStandalone_H_

#include "CategoryStandalone.h"

class RelaxedVBFLooseStandalone: public CategoryStandalone {
public:
	RelaxedVBFLooseStandalone();
	RelaxedVBFLooseStandalone(unsigned nJets, float deltaEta, unsigned nJetRapGap, float jetInvM);
	virtual ~RelaxedVBFLooseStandalone();

	// enumerator for the cuts in this category
	enum cuts_RelaxedVBFLooseStandalone {
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

#endif /* RelaxedVBFLooseStandalone_H_ */
