/*
 * CategoryStandalone.h
 *
 *  Created on: May 8, 2015
 *      Author: kargoll
 */

#ifndef CATEGORYSTANDALONE_H_
#define CATEGORYSTANDALONE_H_

#include <vector>
#include <cmath>

class CategoryStandalone {
public:
	CategoryStandalone();
	virtual ~CategoryStandalone();

	bool passed();

	unsigned get_nCuts() const {return nCuts_;}
	const std::vector<float>& get_cutValues() const {return cutValues_;}
	const std::vector<float>& get_eventValues() const {return eventValues_;}
	const std::vector<bool>& get_passCut() const {return passCut_;}

protected:
	unsigned nCuts_;
	std::vector<float> cutValues_;
	std::vector<float> eventValues_;
	std::vector<bool> passCut_;

	virtual void execute() = 0;

	virtual void setCutValues() = 0;
};

#endif /* CATEGORYSTANDALONE_H_ */
