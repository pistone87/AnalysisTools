#include "Selection.h"

#include "Tables.h"
#include "Plots.h"
#include "SkimConfig.h"
#include <cstdlib>
#include <map>
#include <algorithm>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <sstream>

Selection::Selection(TString Name_, TString id_):
  Selection_Base(Name_,id_)
  ,NGoodFiles(0)
  ,NBadFiles(0)
  ,isStored(false)
  ,data(0)
  ,HConfig()
{
  if(Name_)
  ListofBadFiles.clear();
}

Selection::~Selection(){
  //Check that the correct number of events are run over
  //SkimConfig SC;
  //SC.CheckNEvents(types,nevents_noweight_default);

  //Check the number of files read
  std::cout << Get_Name() << " NGoodFile= " <<  NGoodFiles << " NBadFiles=" << NBadFiles << std::endl;
  if(ListofBadFiles.size()>0)std::cout <<  "WARNING: Output could be comprimised!!! \nList of Bad Files:" << std::endl;
  for(int i=0; i< ListofBadFiles.size(); i++){
    std::cout <<  ListofBadFiles.at(i) << std::endl;
  }
}

void Selection::ConfigureHistograms(){
  if(!isStored){
    if(verbose) std::cout << "Selection::ConfigureHistograms() starting" << std::endl;
    isStored=true;
    Store_ExtraDist();
    for(unsigned int j=0; j<Npassed.size();j++){
      TString name=Npassed.at(j).GetName();
      name+="_noweight";
      Npassed_noweight.push_back((*((TH1D*)Npassed.at(j).Clone(name))));
      //Npassed_noweight.at(j).Sumw2();
      //Npassed.at(j).Sumw2();
    }
    if(verbose) std::cout << "Selection::ConfigureHistograms() Finished" << std::endl;
  }
}

void Selection::LoadResults(std::vector<TString> files){ 
  if(!isStored){
    ConfigureHistograms();
  }
  for(int f=0;f<files.size();f++){
    TString file=files.at(f);
    if(!file.Contains(".root")){
      vector<TString> filelist;
      string dir=file.Data();
      DIR *dp;
      struct dirent *dirp;
      if((dp  = opendir(dir.c_str())) == NULL) {
	cout << "Error(" << errno << ") opening " << dir << endl;
      }
      else{
	while ((dirp = readdir(dp)) != NULL) {
	  filelist.push_back(string(dirp->d_name));
	}
	closedir(dp);
      }
      TString ID=Get_Name()+".root";
      for(int i=0;i<filelist.size();i++){
	if(filelist.at(i).Contains(ID)){
	  file+=filelist.at(i);
	  break;
	}
      }
    }
    if(file.Contains("root")){
      TFile *f=TFile::Open(file,"READ");
      std::cout << "Selection::LoadResults " << file << std::endl;
      TString hname;
      if(f->IsOpen()){
	for(unsigned int i=0; i<Nminus1.size(); i++){
	  for(unsigned int j=0; j<Nminus1.at(i).size();j++){
	    hname=(Nminus1.at(i).at(j)).GetName();
	    Nminus1.at(i).at(j).Add((TH1*)f->Get(hname),1.000);
	    hname=(Nminus0.at(i).at(j)).GetName();
	    Nminus0.at(i).at(j).Add((TH1*)f->Get(hname),1.000);
	    if(distindx.at(i)){
	      hname=(Nminus1dist.at(i).at(j)).GetName();
	      Nminus1dist.at(i).at(j).Add((TH1*)f->Get(hname),1.000);
	      hname=(Accumdist.at(i).at(j)).GetName();
	      Accumdist.at(i).at(j).Add((TH1*)f->Get(hname),1.000);
	    }
	    if(i==0){
	      hname=((Npassed.at(j)).GetName());
	      TH1* temp=(TH1*)f->Get(hname);
	      Npassed.at(j).Add(temp,1.000);
	      hname=((Npassed_noweight.at(j)).GetName());
	      TH1* tempnw=(TH1*)f->Get(hname);
	      Npassed_noweight.at(j).Add(tempnw,1.000);
	      for(unsigned int k=0; k<Extradist1d.size();k++){
		hname=(Extradist1d.at(k)->at(j)).GetName();
		Extradist1d.at(k)->at(j).Add((TH1*)f->Get(hname),1.000);
	      }
	      for(unsigned int k=0; k<Extradist2d.size();k++){
		TString n=Extradist2d.at(k)->at(j).GetName();
		if(!n.Contains("egammaMap")){
		  hname=(Extradist2d.at(k)->at(j)).GetName();
		  Extradist2d.at(k)->at(j).Add((TH1*)f->Get(hname),1.000);
		}
	      }
              for(unsigned int k=0; k<Extradist3d.size();k++){
                TString n=Extradist3d.at(k)->at(j).GetName();
		hname=(Extradist3d.at(k)->at(j)).GetName();
		Extradist3d.at(k)->at(j).Add((TH1*)f->Get(hname),1.000);
              }

	    }
	  }
	}
	NGoodFiles++;
      }
      else{
	NBadFiles++;
	std::cout << "WARNING: " << file << " NOT OPENED" << std::endl;
	ListofBadFiles.push_back(file);
      }
      f->Close();
    }
    else{
      NBadFiles++;
      std::cout << "WARNING: File missing in: "  << file << std::endl;
      ListofBadFiles.push_back(file);
    }
  }
}

bool Selection::AnalysisCuts(int t,double w,double wobjs){
  int ncuts=Nminus1.size();
  if(Npassed.size()!=Npassed_noweight.size()){
    std::cout << "ERROR Histograms not Configured. Please fix your code!!!! Running Selection::ConfigureHistograms()" << std::endl;
    Selection::ConfigureHistograms();
  }
   if(0<=t && t<types.size()){
    int nfail=0;
    int fail=-1;
    Npassed.at(t).Fill(-0.5,w);
    Npassed_noweight.at(t).Fill(-0.5,1);
    for(int i=0; i<ncuts;i++){
      if(!pass.at(i)){
	fail=i;
	nfail++;
      }
      if(nfail==0){
	Npassed.at(t).Fill((float)i+0.5,w*wobjs);
	Npassed_noweight.at(t).Fill((float)i+0.5,1);
	if(i+1<ncuts){
	  if(distindx.at(i+1)){
	    for(int k=0;k<dist.at(i+1).size();k++){
	      Accumdist.at(i+1).at(t).Fill(dist.at(i+1).at(k),w*wobjs);
	    }
	  }
	}
	if(i==0){
	  if(distindx.at(i)){
	    for(int k=0;k<dist.at(i).size();k++){
	      Accumdist.at(i).at(t).Fill(dist.at(i).at(k),w*wobjs);
	    }
	  }
	}
      }
    }
    if(nfail<=1){
      for(int i=0; i<ncuts;i++){
	if(fail==i || nfail==0){
	  Nminus1.at(i).at(t).Fill(value.at(i),w*wobjs);
	  if(distindx.at(i)){
	    for(int k=0;k<dist.at(i).size();k++){
	      Nminus1dist.at(i).at(t).Fill(dist.at(i).at(k),w*wobjs);
	    }
	  }
	}
      }
      
      if(nfail==0){
	for(int i=0; i<ncuts;i++){
	  Nminus0.at(i).at(t).Fill(value.at(i),w*wobjs);
	}
	return true;
      }
    }
  }
   //   std::cout << "Failed --- requesting t=" <<  t << " from max " << types.size() <<  std::endl;
  return false;
}

void  Selection::Finish(){
  if(Npassed.size()!=Npassed_noweight.size()){
    std::cout << "ERROR Histograms not Configured. Please fix your code!!!! Running Selection::ConfigureHistograms()" << std::endl;
    Selection::ConfigureHistograms();
  }
  if(!isStored){
    ConfigureHistograms();
  }
  std::cout << "Writing out "+Name+".root ..." << std::endl;
  TString fName;
  if(runtype==GRID)         fName="GRID_";
  if(runtype==Local)        fName="LOCAL_";
  if(mode==RECONSTRUCT)     fName+="COMBINED_";
  if(mode==ANALYSIS)        fName+="ANALYSIS_";
  fName+=Name;

  Save(fName);// Save file with unweighted events - required for combining code

  std::cout << "Writing out "+Name+".root Complete" << std::endl;
  SkimConfig SC;
  SC.ApplySkimEfficiency(types,Npassed,Npassed_noweight);
	for(int i=0; i<Npassed.size();i++){
		nevents_noweight_default.push_back(Npassed_noweight.at(i).GetBinContent(1));
	}

  // For local jobs produce pdf file
  if(runtype!=GRID){
    Tables T(Name);
    //Check that the correct number of events are run over and make Table
    SC.CheckNEvents(types,nevents_noweight_default);
    // Make Tables
    T.MakeNEventsTable(Npassed,title);

    // weight all Histograms
    for(int i=0;i<CrossSectionandAcceptance.size();i++){
      std::cout << i << " CrossSectionandAcceptance " << CrossSectionandAcceptance.at(i) << " " << Npassed.at(i).GetBinContent(0) << " " << Npassed_noweight.at(i).GetBinContent(0) << std::endl;
      if(CrossSectionandAcceptance.at(i)>0) ScaleAllHistOfType(i,Lumi*CrossSectionandAcceptance.at(i)/Npassed.at(i).GetBinContent(0));
    }
    Save(fName+"_LumiScaled");// Save file with Lumi-scaled events - required for combining code

    ///Now make the plots
    std::cout << "Printing Plots " << std::endl;
    system("rm EPS/*.eps");
    Plots P;
    P.Plot1D(Nminus1,colour,legend);
    for(unsigned int i=0; i<Nminus1.size();i++){
      P.Plot1DSignificance(Nminus1.at(i),true,false,colour,legend);
      P.Plot1DSignificance(Nminus1.at(i),false,true,colour,legend);
      P.Plot1Dsigtobkg(Nminus1.at(i),true,false,colour,legend);
      P.Plot1Dsigtobkg(Nminus1.at(i),false,true,colour,legend);
      P.Plot1D_DataMC_Compare(Nminus1.at(i),colour,legend);
    }
    P.Plot1D(Nminus0,colour,legend);
    P.Plot1D(Nminus1dist,colour,legend);
    P.Plot1D(Accumdist,colour,legend);
    

    for(unsigned int i=0; i<Extradist1d.size();i++){
      P.Plot1D((*Extradist1d.at(i)),colour,legend);
      if(Lumi>0){
	P.Plot1DSignificance((*Extradist1d.at(i)),true,false,colour,legend);
	P.Plot1DSignificance((*Extradist1d.at(i)),false,true,colour,legend);
	P.Plot1Dsigtobkg((*Extradist1d.at(i)),true,false,colour,legend);
	P.Plot1Dsigtobkg((*Extradist1d.at(i)),false,true,colour,legend);
	P.Plot1D_DataMC_Compare((*Extradist1d.at(i)),colour,legend);
      }
    }
    for(unsigned int i=0; i<Extradist2d.size();i++){
      P.Plot2D((*Extradist2d.at(i)),colour,legend);
    }
    for(unsigned int i=0; i<Extradist3d.size();i++){
      P.Plot3D((*Extradist3d.at(i)),colour,legend);
    }
    
    std::cout << "Writing out "<< Name << ".tex" << std::endl;
    T.MakeEffTable(Npassed,title,Lumi,CrossSectionandAcceptance);
    T.AddPlots(title);
    T.GeneratePDF();
    std::cout << "Plots and Tables Complete"<< std::endl;
  }

}


void Selection::Save(TString fName){
  TFile f(fName+".root","RECREATE");
  for(unsigned int i=0; i<Nminus1.size(); i++){
    for(unsigned int j=0; j<Nminus1.at(i).size();j++){
      Nminus1.at(i).at(j).Write((Nminus1.at(i).at(j)).GetName());
      Nminus0.at(i).at(j).Write((Nminus0.at(i).at(j)).GetName());
      if(distindx.at(i)){
        Nminus1dist.at(i).at(j).Write((Nminus1dist.at(i).at(j)).GetName());
        Accumdist.at(i).at(j).Write((Accumdist.at(i).at(j)).GetName());
      }
      if(i==0){
        Npassed.at(j).Write((Npassed.at(j)).GetName());
        Npassed_noweight.at(j).Write((Npassed_noweight.at(j)).GetName());
        for(unsigned int i=0; i<Extradist1d.size();i++){
          Extradist1d.at(i)->at(j).Write((Extradist1d.at(i)->at(j)).GetName());
        }
        for(unsigned int i=0; i<Extradist2d.size();i++){
          Extradist2d.at(i)->at(j).Write((Extradist2d.at(i)->at(j)).GetName());
        }
        for(unsigned int i=0; i<Extradist3d.size();i++){
          Extradist3d.at(i)->at(j).Write((Extradist3d.at(i)->at(j)).GetName());
        }
      }
    }
  }
  f.Close();
}

bool Selection::Passed(){
  for(int i=0; i<pass.size();i++){
    if(!pass.at(i)) return false;
  }
  return true;
}

bool Selection::NMinusL(int a, int b, int c, int d, int e){
  bool good=true;
  for(int i=0; i<(int)(pass.size()); i++){
    if(i!=a && i!=b && i!=c && i!=d && i!=e){
      if(!pass.at(i)) good=false;
    }
  }
  return good;
}

bool Selection::NMinus1(int a){
  return Selection::NMinusL(a);
}
bool Selection::NMinus2(int a, int b){
  return Selection::NMinusL(a,b);
}



double Selection::Compute(double thisdata,double thissignal, double thissignalTotal, double thisbkg, 
			  double data,double signal,double signalTotal, double bkg){

  double thisCS=(thisdata-thisbkg)*(thissignal/thissignalTotal);
  double CS=(data-bkg)*(signal/signalTotal);
  return fabs(thisCS-CS);
}

void Selection::EvaluateSystematics(Selection_Base* &selectionsys, double w){
  Selection *selsys=(Selection*)selectionsys;
  for(int j=0; j<Npassed.size();j++){
    for(int l=0; l<=Npassed.at(j).GetNbinsX();l++){
      double err=Npassed.at(j).GetBinError(l);
      if(Npassed.at(j).GetBinContent(l)!=0){
	Npassed.at(j).SetBinError(l,sqrt(err*err+w*w*pow(selsys->Get_Npassed().at(j).GetBinContent(l)-Npassed.at(j).GetBinContent(l),2.0)));
      }
    }
    for(int k=0;k<Nminus1.size();k++){
      for(int l=0; l<=Nminus1.at(k).at(j).GetNbinsX();l++){
	double err=Nminus1.at(k).at(j).GetBinError(l);
	Nminus1.at(k).at(j).SetBinError(l,sqrt(err*err+w*w*pow(selsys->Get_Nminus1().at(k).at(j).GetBinContent(l)-Nminus1.at(k).at(j).GetBinContent(l),2.0)));
      }
      for(int l=0; l<=Nminus0.at(k).at(j).GetNbinsX();l++){
	double err=Nminus0.at(k).at(j).GetBinError(l);
	Nminus0.at(k).at(j).SetBinError(l,sqrt(err*err+w*w*pow(selsys->Get_Nminus0().at(k).at(j).GetBinContent(l)-Nminus0.at(k).at(j).GetBinContent(l),2.0)));
      }
      if(distindx.at(k)){
	for(int l=0; l<=Nminus1dist.at(k).at(j).GetNbinsX();l++){
	  double err=Nminus1dist.at(k).at(j).GetBinError(l);
	  Nminus1dist.at(k).at(j).SetBinError(l,sqrt(err*err+w*w*pow(selsys->Get_Nminus1dist().at(k).at(j).GetBinContent(l)-Nminus1dist.at(k).at(j).GetBinContent(l),2.0)));
	}
	for(int l=0; l<=Accumdist.at(k).at(j).GetNbinsX();l++){
	  double err=Accumdist.at(k).at(j).GetBinError(l);
	  Accumdist.at(k).at(j).SetBinError(l,sqrt(err*err+w*w*pow(selsys->Get_Accumdist().at(k).at(j).GetBinContent(l)-Accumdist.at(k).at(j).GetBinContent(l),2.0)));
	}
      }
    }
    for(int k=0;k<Extradist1d.size();k++){
      for(int l=0; l<=Extradist1d.at(k)->at(j).GetNbinsX();l++){
	double err=Extradist1d.at(k)->at(j).GetBinError(l);
	Extradist1d.at(k)->at(j).SetBinError(l,sqrt(err*err+w*w*pow(selsys->Get_Extradist1d().at(k)->at(j).GetBinContent(l)-Extradist1d.at(k)->at(j).GetBinContent(l),2.0)));
      }
    }
    for(int k=0;k<Extradist2d.size();k++){
      for(int l=0; l<=Extradist2d.at(k)->at(j).GetBin(Extradist2d.at(k)->at(j).GetNbinsX(),Extradist2d.at(k)->at(j).GetNbinsY());l++){
	double err=Extradist2d.at(k)->at(j).GetBinError(l);
	Extradist2d.at(k)->at(j).SetBinError(l,sqrt(err*err+w*w*pow(selsys->Get_Extradist2d().at(k)->at(j).GetBinContent(l)-Extradist2d.at(k)->at(j).GetBinContent(l),2.0)));
      }
    }
  }
}

TString Selection::splitString(const std::string &s, char delim, std::string splitpoint){
	/* input string is split by given delimiter.
	 * output string is stiched back together from string fragments including the given splitpoint but no further.
	 */
	std::stringstream ss(s);
	std::string item;
	TString outstring = "";
	while(std::getline(ss,item,delim)){
		outstring+=item;
		outstring+=delim;
		if(item.find(splitpoint)!=std::string::npos) break;
	}
	return outstring;
}

void Selection::ResetEvent(){
  for(int i=0; i<pass.size();i++){
    pass.at(i)=false;
  }
  for(int i=0; i<dist.size();i++){
    dist.at(i).clear();
  }
}

void Selection::ScaleAllHistOfType(unsigned int t,float w){
  for(unsigned int i=0; i<Nminus1.size(); i++){
    if(Nminus1.at(i).size()>t)Nminus1.at(i).at(t).Scale(w);
    if(Nminus0.at(i).size()>t)Nminus0.at(i).at(t).Scale(w);
    if(distindx.at(i)){
      if(Nminus1dist.at(i).size()>t)Nminus1dist.at(i).at(t).Scale(w);
      if(Accumdist.at(i).size()>t)Accumdist.at(i).at(t).Scale(w);
    }
  }
  if(Npassed.size()>t)Npassed.at(t).Scale(w);
  for(unsigned int k=0; k<Extradist1d.size();k++){
    if(Extradist1d.at(k)->size()>t)Extradist1d.at(k)->at(t).Scale(w);
  }
  for(unsigned int k=0; k<Extradist2d.size();k++){
    if(Extradist2d.at(k)->size()>t)Extradist2d.at(k)->at(t).Scale(w);
  }
}

