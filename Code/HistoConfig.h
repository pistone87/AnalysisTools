#ifndef HistoConfig_h
#define HistoConfig_h

#include <vector>
#include "TString.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3F.h"
#include "TString.h"

class HistoConfig {

 public:
  HistoConfig();
  virtual ~HistoConfig();

  bool Load();
  bool Load(TString Name_);


  std::vector<TH1D> GetTH1D(TString name,TString title, int nbins, double min, double max, TString xaxis="",TString yaxis="Events");
  std::vector<TH1D> GetTH1D(TString name,TString title, int nbins, double* xbins, TString xaxis="",TString yaxis="Events");
  std::vector<TH2D> GetTH2D(TString name,TString title, int nbinsx, double minx, double maxx, 
			    int nbinsy,double miny, double maxy, TString xaxis="", TString yaxis="");

  std::vector<TH3F> GetTH3F(TString name,TString title, int nbinsx, double minx, double maxx,
                            int nbinsy,double miny, double maxy,int nbinsz,double minz,double maxz, 
			    TString xaxis="", TString yaxis="",TString zaxis="");

  bool GetHisto(bool isdata,int64_t id,unsigned int &histo);
  unsigned int GetNHisto();
  TString GetName(unsigned int i);
  TString GetLeg(unsigned int i);
  double  GetCrossSection(int64_t id);
  bool SetCrossSection(int64_t id, double xsec);
  void GetHistoInfo(std::vector<int64_t> &types,std::vector<float> &CrossSectionandAcceptance,std::vector<TString> &legend,std::vector<int> &colour);
  bool hasID(int64_t id_);
  int64_t GetID(unsigned int i);
  int GetType(int64_t id);
  static bool isloaded() { return loaded; }

 private:
  static std::vector<int64_t>      ID;
  static std::vector<double>       CS;
  static std::vector<TString>      HistoName;
  static std::vector<TString>      HistoLegend;
  static std::vector<int>          HistoColour;
  static bool                      loaded;
};
#endif
