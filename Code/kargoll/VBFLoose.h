/*
 * VBFLoose.h
 *
 *  Created on: Mar 21, 2014
 *      Author: kargoll
 */

#ifndef VBFLOOSE_H_
#define VBFLOOSE_H_

#include "Category.h"

class VBFLoose: public Category {
public:
	VBFLoose(TString Name_, TString id_);
	virtual ~VBFLoose();

	// enumerator for the cuts in this category
	enum cuts_VBFLoose {
		VbfLoose_NJet	= CatCut1,
		VbfLoose_DeltaEta,
		VbfLoose_NJetRapGap,
		VbfLoose_JetInvM,
		VbfLoose_NotVbfTight,
		VbfLoose_NCuts
	};

protected:
	void categoryConfiguration();
	bool categorySelection();
};

#endif /* VBFLOOSE_H_ */
