#include "SVFitObject.h"
ClassImp(SVFitObject);

// default constructor, creates an invalid object
SVFitObject::SVFitObject(){
	valid_ = false;
}

// There is no proper constructor for this class here on purpose,
// in order to keep this data container class independent from other
// code. Otherwise CINT has problems with that.
// To creat an object from SVfitStandaloneAlgorithm object, have a look at
// SVfitProvider::makeObject(const SVfitStandaloneAlgorithm*, TString fitMethod)


