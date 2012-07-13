#ifndef ChargedHiggs_dilepontic_h
#define ChargedHiggs_dilepontic_h

#include "Selection.h"
#include <vector>
#include "TString.h"

class ChargedHiggs_dilepontic : public Selection {

 public:
  ChargedHiggs_dilepontic(TString Name_, TString id_);
  virtual ~ChargedHiggs_dilepontic();

  virtual void  Configure();

  enum cuts {TriggerOk=0, 
	     PrimeVtx,
	     NTauKinFit,
	     NTauPt,
	     NTauEta,
	     NMuons,
	     MuRelIso,
	     ElectronVeto,
	     MuonVeto,
	     N2Jets,
             NBJets,
             MET,
             HT,
	     Charge,
	     MT,
	     MuMETdphi,
	     TauMETdphi,
	     NCuts};

 protected:
  virtual void doEvent();
  virtual void Store_ExtraDist();

 private:
  // Selection Variables

  std::vector<TH1D> NVtx;
  std::vector<TH1D> NGoodVtx;
  std::vector<TH1D> NTrackperVtx;
  std::vector<TH2D> TagEtaPT;
  std::vector<TH1D> ChargedHiggsMT;
  std::vector<TH2D> METvsMT;
  std::vector<TH2D> MTvsMuMETdphi;


  double tau_pt,tau_eta,jet_pt,jet_eta,muon_pt,muon_eta;

};
#endif