#ifndef LightChargedHiggs_h
#define LightChargedHiggs_h

#include <vector>

#include "Selection.h"
#include "TString.h"

using namespace std;

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
    vector<TH1D> NVtx;
    vector<TH1D> NGoodVtx;
    vector<TH1D> NTrackperVtx;


    // Muons
    // before cuts
    //vector<TH1D> muonPt;
    //vector<TH1D> muonEta;

    // after selection
    vector<TH1D> goodMuons;
    vector<TH1D> goodMuonPt;
    vector<TH1D> goodMuonEta;

    // transverse mass(mu, MET)
    vector<TH1D> transMass;

    // Tau
    // before cuts
    //vector<TH1D> tauPt;
    //vector<TH1D> tauEta;
    //vector<TH1D> tauPhi;

    // after selection
    vector<TH1D> goodTaus;
    vector<TH1D> goodTauPt;
    vector<TH1D> goodTauEta;
    vector<TH1D> goodTauPhi;


    // Jets
    // before cuts
    // jet1st
    //vector<TH1D> jet1stPt;
    //vector<TH1D> jet1stEta;
    //vector<TH1D> jet1stMass;

    // jet2nd
    //vector<TH1D> jet2ndPt;
    //vector<TH1D> jet2ndEta;
    //vector<TH1D> jet2ndMass;

    // after selection
    vector<TH1D> goodJets;

    // jet1st
    vector<TH1D> goodJet1stPt;
    vector<TH1D> goodJet1stEta;
    vector<TH1D> goodJet1stMass;

    // jet2nd
    vector<TH1D> goodJet2ndPt;
    vector<TH1D> goodJet2ndEta;
    vector<TH1D> goodJet2ndMass;

    // MET
    vector<TH1D> METEt;
    vector<TH1D> METPhi;


    // cut values
    double mu_pt, mu_eta, mu_relIso; //muons
    double tau_pt, tau_eta; //taus
    double jet_pt, jet_eta, jetClean_dR, bjetDiscr; //jets

    

    // definitions of functions
    bool JetCleaning(unsigned int i, int muon1st, int tau1st);


};
#endif
