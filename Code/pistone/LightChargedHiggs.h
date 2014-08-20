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
                //NMu,
                NTightMu,
                NTightMuIso,
                NTau,
                NTau3Prong,
                NTauClean,
                NTauMediumIso,
                NJetsClean,
                NJetsID,
                NJets,
                NBJets,
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


    // Muons
    // before cuts
    //std::vector<TH1D> muonPt;
    //std::vector<TH1D> muonEta;

    // after selection
    std::vector<TH1D> goodMuons;
    std::vector<TH1D> goodMuonPt;
    std::vector<TH1D> goodMuonEta;

    // transverse mass(mu, MET)
    std::vector<TH1D> transMass;

    // Tau
    // before cuts
    //std::vector<TH1D> tauPt;
    //std::vector<TH1D> tauEta;
    //std::vector<TH1D> tauPhi;

    // after selection
    std::vector<TH1D> goodTaus;
    std::vector<TH1D> goodTauPt;
    std::vector<TH1D> goodTauEta;
    std::vector<TH1D> goodTauPhi;


    // Jets
    // before cuts
    // jet1st
    //std::vector<TH1D> jet1stPt;
    //std::vector<TH1D> jet1stEta;
    //std::vector<TH1D> jet1stMass;

    // jet2nd
    //std::vector<TH1D> jet2ndPt;
    //std::vector<TH1D> jet2ndEta;
    //std::vector<TH1D> jet2ndMass;

    // after selection
    std::vector<TH1D> goodJets;

    // jet1st
    std::vector<TH1D> goodJet1stPt;
    std::vector<TH1D> goodJet1stEta;
    std::vector<TH1D> goodJet1stMass;

    // jet2nd
    std::vector<TH1D> goodJet2ndPt;
    std::vector<TH1D> goodJet2ndEta;
    std::vector<TH1D> goodJet2ndMass;

    // MET
    std::vector<TH1D> METEt;
    std::vector<TH1D> METPhi;


    // cut values
    double mu_pt, mu_eta, mu_relIso; //muons
    double tau_pt, tau_eta; //taus
    double jet_pt, jet_eta, jetClean_dR, bjetDiscr; //jets

    

    // definitions of functions
    bool JetCleaning(unsigned int i, int muon1st, int tau1st);


};
#endif
