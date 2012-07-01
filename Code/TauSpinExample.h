#ifndef TauSpinExample_h
#define TauSpinExample_h

#include "Selection.h"
#include <vector>
#include "TString.h"

class TauSpinExample : public Selection {

 public:
  TauSpinExample(TString Name_, TString id_);
  virtual ~TauSpinExample();

  virtual void  Configure();
  virtual void  Finish();
  enum cuts {isZtautauto3pimu=0,NCuts};

 protected:
  virtual void doEvent();
  virtual void Store_ExtraDist();

 private:
  // Selection Variables
  std::vector<TH1D> LongitudinalPolarization;
  std::vector<TH1D> LongitudinalPolarization_Spin;
  std::vector<TH1D> LongitudinalPolarization_UnSpin;
  std::vector<TH1D> LongitudinalPolarization_FlipSpin;

  std::vector<TH1D> mu_ExoverEtau;
  std::vector<TH1D> mu_ExoverEtau_hplus;
  std::vector<TH1D> mu_ExoverEtau_hminus;
  std::vector<TH1D> mu_ExoverEtau_Spin;
  std::vector<TH1D> mu_ExoverEtau_UnSpin;
  std::vector<TH1D> mu_ExoverEtau_FlipSpin;
  std::vector<TH1D> mu_WT_Spin;
  std::vector<TH1D> mu_WT_UnSpin;
  std::vector<TH1D> mu_WT_FlipSpin;
  std::vector<TH1D> mu_PtRatio_hplus;
  std::vector<TH1D> mu_PtRatio_hminus;

  std::vector<TH1D> pi_ExoverEtau;
  std::vector<TH1D> pi_ExoverEtau_hplus;
  std::vector<TH1D> pi_ExoverEtau_hminus;
  std::vector<TH1D> pi_ExoverEtau_Spin;
  std::vector<TH1D> pi_ExoverEtau_UnSpin;
  std::vector<TH1D> pi_ExoverEtau_FlipSpin;
  std::vector<TH1D> pi_WT_Spin;
  std::vector<TH1D> pi_WT_UnSpin;
  std::vector<TH1D> pi_WT_FlipSpin;
  std::vector<TH1D> pi_PtRatio_hplus;
  std::vector<TH1D> pi_PtRatio_hminus;

  std::vector<TH1D> a1_ExoverEtau;
  std::vector<TH1D> a1_ExoverEtau_hplus;
  std::vector<TH1D> a1_ExoverEtau_hminus;
  std::vector<TH1D> a1_ExoverEtau_Spin;
  std::vector<TH1D> a1_ExoverEtau_UnSpin;
  std::vector<TH1D> a1_ExoverEtau_FlipSpin;
  std::vector<TH1D> a1_WT_Spin;
  std::vector<TH1D> a1_WT_UnSpin;
  std::vector<TH1D> a1_WT_FlipSpin;
  std::vector<TH1D> a1_PtRatio_hplus;
  std::vector<TH1D> a1_PtRatio_hminus;

  std::vector<TH1D> rho_ExoverEtau;
  std::vector<TH1D> rho_ExoverEtau_hplus;
  std::vector<TH1D> rho_ExoverEtau_hminus;
  std::vector<TH1D> rho_ExoverEtau_Spin;
  std::vector<TH1D> rho_ExoverEtau_UnSpin;
  std::vector<TH1D> rho_ExoverEtau_FlipSpin;
  std::vector<TH1D> rho_WT_Spin;
  std::vector<TH1D> rho_WT_UnSpin;
  std::vector<TH1D> rho_WT_FlipSpin;
  std::vector<TH1D> rho_PtRatio_hplus;
  std::vector<TH1D> rho_PtRatio_hminus;

  std::vector<TH1D> a1_cosbeta_hminus;
  std::vector<TH1D> a1_cosbeta_hplus;


};
#endif
