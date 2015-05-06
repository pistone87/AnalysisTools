/*
 * VBFTight.h
 *
 *  Created on: Mar 21, 2014
 *      Author: kargoll
 */

#ifndef VBFTIGHT_H_
#define VBFTIGHT_H_

#include "Category.h"

class VBFTight: public Category {
public:
	VBFTight(TString Name_, TString id_);
	virtual ~VBFTight();

	// enumerator for the cuts in this category
	enum cuts_VBFTight {
		VbfTight_NJet	= CatCut1,
		VbfTight_DeltaEta,
		VbfTight_NJetRapGap,
		VbfTight_JetInvM,
		VbfTight_HiggsPt,
		VbfTight_NCuts
	};

protected:
	void categoryConfiguration();
	bool categorySelection();
};

#endif /* VBFTIGHT_H_ */
