#ifndef LightChargedHiggs_h
#define LightChargedHiggs_h

#include <vector>

#include "Selection.h"
#include "TString.h"

class LightChargedHiggs : public Selection {

 public:
  LightChargedHiggs(TString Name_, TString id_);
  virtual ~LightChargedHiggs();

  virtual void  Configure();

  enum cuts {TriggerOk=0,
	     PrimeVtx,
	     NMu,
             NJets,
	     NCuts};


 protected:
  virtual void doEvent();
  virtual void Store_ExtraDist();
  virtual void Finish();


 private:
  //
  // Selection Variables
  //

  // vertices
  std::vector<TH1D> NVtx;
  std::vector<TH1D> NGoodVtx;
  std::vector<TH1D> NTrackperVtx;


  //muons
  std::vector<TH1D> goodmuons;
  std::vector<TH1D> muonPt;
  std::vector<TH1D> muonEta;

  double mu_pt, mu_eta, mu_relIso;


  //jets
  std::vector<TH1D> goodjets;
  std::vector<TH1D> jetPt;
  std::vector<TH1D> jetEta;
  std::vector<TH1D> jetMass;

  double jet_pt, jet_eta;


};
#endif
