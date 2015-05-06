/*
 * OneJetHigh.h
 *
 *  Created on: Mar 21, 2014
 *      Author: kargoll
 */

#ifndef ONEJETHIGH_H_
#define ONEJETHIGH_H_

#include "Category.h"

class OneJetHigh: public Category {
public:
	OneJetHigh(TString Name_, TString id_);
	virtual ~OneJetHigh();

	// enumerator for the cuts in this category
	enum cuts_OneJetHigh {
		OneJetHigh_NJet = CatCut1,
		OneJetHigh_NotVbf,
		OneJetHigh_TauPt,
		OneJetHigh_HiggsPt,
		OneJetHigh_NCuts
	};

protected:
	void categoryConfiguration();
	bool categorySelection();
};

#endif /* ONEJETHIGH_H_ */
