/*
 * ZeroJetHigh.h
 *
 *  Created on: Mar 21, 2014
 *      Author: kargoll
 */

#ifndef ZEROJETHIGH_H_
#define ZEROJETHIGH_H_

#include "Category.h"

class ZeroJetHigh: public Category {
public:
	ZeroJetHigh(TString Name_, TString id_);
	virtual ~ZeroJetHigh();

	// enumerator for the cuts in this category
	enum cuts_ZeroJetHigh {
		ZeroJetHigh_NJet = HToTaumuTauh::CatCut1,
		ZeroJetHigh_TauPt,
		ZeroJetHigh_NCuts
	};

protected:
	void categoryConfiguration();
	bool categorySelection();
};

#endif /* ZEROJETHIGH_H_ */
