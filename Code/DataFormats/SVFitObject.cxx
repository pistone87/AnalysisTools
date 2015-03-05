#include "SVFitObject.h"
ClassImp(SVFitObject);

// default constructor, creates an invalid object
SVFitObject::SVFitObject(){
	isValid_ = false;

	// dummy values
	a = -999;
}

