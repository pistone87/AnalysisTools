#include "Plots.h"

#include "THStack.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TFile.h"
#include "TGaxis.h"
#include "TView.h"
#include "TMath.h"
#include <cmath>
#include <iostream>

int Plots::plotLabel=0;
TString Plots::File_;
std::vector<TString> Plots::HistogramNames_;

Plots::Plots():
  doscale(false)
  ,verbose(false)
  ,dooneprofile(false)
{
  
}

Plots::~Plots(){
}

void Plots::Set_Plot_Type(TString style, TString label){
  style.ToLower();
  label.ToLower();
  if(label.Contains("internal"))    plotLabel=cmsInternal;
  if(label.Contains("private"))     plotLabel=cmsPrivate;
  if(label.Contains("preliminary")) plotLabel=cmsPreliminary;
  if(label.Contains("public"))      plotLabel=cmsPublic;
  if(style.Contains("style1")){      CMSStyle1();}
  else if(style.Contains("style2")){ CMSStyle2();}
  else{ CMSStyle2();}
}

void Plots::Plot1D(std::vector<TH1D> histo,float Lumi,std::vector<float> CrossSectionandAcceptance,std::vector<float> nevents,std::vector<int> colour,std::vector<TString> legend){
  std::vector<std::vector<TH1D> > histos;
  histos.push_back(histo);
  Plot1D(histos,Lumi,CrossSectionandAcceptance,nevents,colour,legend);
}


void Plots::SaveHistograms(TString File, std::vector<TString> HistogramNames){
  File_=File;
  HistogramNames_=HistogramNames;
}

void Plots::Plot1D(std::vector<std::vector<TH1D> > histo,float Lumi,std::vector<float> CrossSectionandAcceptance,std::vector<float> nevents,std::vector<int> colour,std::vector<TString> legend){
 
  std::cout << "Plots::Plot1D" << std::endl;
  TCanvas c("c","c",200,10,750,750);
  c.Update();
  c.Clear();
  c.Update();
  TLatex latex;
  latex.SetTextSize(0.03);
  latex.SetNDC();

    for(int j=0;j<histo.size();j++){
      if(verbose)std::cout << "Plots::Plot1D " << histo.size() << " j= " << j << " color.size()= " << colour.size() << " " 
		<< histo.at(j).size() << std::endl;
      if(histo.at(j).size()>0 && histo.at(j).size()==colour.size()){
	TLegend leg(0.25,0.75,0.9,0.925);
	//double yleg = 0.925 - histo.at(j).size()*0.04;
	//TLegend leg(0.65,yleg,0.9,0.925);
	leg.SetBorderSize(0);
	leg.SetFillStyle(4000);
	leg.SetFillColor(0);
	leg.SetTextSize(0.03);
	leg.SetMargin(0.15);
	leg.SetNColumns(3);
	leg.SetColumnSeparation(0.05);
	TH1D Total("Total","Total",histo.at(j).at(0).GetNbinsX(),
		   histo.at(j).at(0).GetXaxis()->GetXmin(),
		   histo.at(j).at(0).GetXaxis()->GetXmax());
	Total.SetFillStyle(3005);
	Total.SetFillColor(1);
	Total.SetLineColor(18);
	Total.SetMarkerColor(1);
	Total.SetMarkerSize(0.001);
	Total.Sumw2();
	TString N="MCStack";
	N+=j;
	N+=histo.at(j).at(0).GetName();
	THStack MCHistoStack(N,histo.at(j).at(0).GetName());
	double MC_Integral(0);
	double Data_Integral(0);
	bool cflag=true;
	TString l="";
	histo.at(j).at(0).SetLineColor(colour.at(0));
	histo.at(j).at(0).SetMarkerColor(colour.at(0));
	if(verbose) std::cout <<  histo.at(j).at(0).GetTitle() << std::endl;
	for(int i=histo.at(j).size()-1; i>=0;i--){
	  histo.at(j).at(i).SetLineColor(colour.at(i));
	  histo.at(j).at(i).SetFillColor(colour.at(i));
	  if(i+1<histo.at(j).size()){
	    if(legend.at(i)!=l){
	      l=legend.at(i);
	      cflag=true;
	    }
	  } 
	  if(cflag && histo.at(j).at(i).Integral()>0){
	    histo.at(j).at(i).SetLineColor(1);
	    cflag=false;
	  } 
	}
	double theIntegral(0);
	for(int i=0; i<histo.at(j).size();i++){
	  theIntegral+=histo.at(j).at(i).Integral();
	  if(i==0){
	    Data_Integral+=histo.at(j).at(0).Integral();
	    leg.AddEntry(&histo.at(j).at(i),legend.at(i),"pe");
	    theIntegral=0;
	  }
	  else{
	    if(colour.size()<=i+1 && theIntegral>=0){
	      histo.at(j).at(i).SetLineColor(1);
	      leg.AddEntry(&histo.at(j).at(i),legend.at(i),"F");
	      theIntegral=0;
	    }
	    else if(colour.size()>i+1){
	      if(colour.at(i)!=colour.at(i+1) && theIntegral>=0){
		histo.at(j).at(i).SetLineColor(1);
		leg.AddEntry(&histo.at(j).at(i),legend.at(i),"F");
		theIntegral=0;
	      }
	    }
	    if(nevents.at(i)>0){
	      if(nevents.at(i)>0 && Lumi>0 && CrossSectionandAcceptance.at(i)>0){
		histo.at(j).at(i).Scale(Lumi*CrossSectionandAcceptance.at(i)/nevents.at(i));
	      }
	      if(histo.at(j).at(i).Integral()>0 ){
		MCHistoStack.Add(&histo.at(j).at(i));
		Total.Add(&histo.at(j).at(i));
	      }
	    }
	    MC_Integral+=histo.at(j).at(i).Integral();
	    if(verbose) std::cout << histo.at(j).at(i).GetTitle() << " " << histo.at(j).at(i).Integral() << " i= " << i << std::endl;
	  }
	}
	if(doscale){
	  for(int i=0; i<histo.at(j).size();i++){
	    if(i!=0 && Data_Integral>0 && MC_Integral>0) histo.at(j).at(i).Scale(Data_Integral/MC_Integral);
	  }
	  if(Data_Integral>0 && MC_Integral>0)Total.Scale(Data_Integral/MC_Integral);
	}
	if(verbose) std::cout << "A" << std::endl;
	for(int l=0;l<2;l++){
	  c.Clear();
	  c.SetLogy(0);
	  if(l==1){
	    c.Clear();
	    c.SetLogy(1);
	  }
	  if(Data_Integral>0){
	    double max=histo.at(j).at(0).GetBinContent(histo.at(j).at(0).GetMaximumBin());
	    if(l==1){
	    histo.at(j).at(0).SetMinimum(0.01);
	    histo.at(j).at(0).SetMaximum(100*max);
	    //histo.at(j).at(0).SetMaximum(600*max); //cleo
	    } 
	    else {
	      histo.at(j).at(0).SetMinimum(0);
	      histo.at(j).at(0).SetMaximum(1.7*max);
	      //histo.at(j).at(0).SetMaximum(2.5*max); //cleo
	    }
	    histo.at(j).at(0).Draw("E");
	    MCHistoStack.Draw("Histsame");
	    Total.Draw("E2same");
	    histo.at(j).at(0).Draw("Esame");
	    histo.at(j).at(0).Draw("axissame");
	  TString hTitle=histo.at(j).at(0).GetName();
	  if(hTitle.Contains("KSTest")){
	    double KS=histo.at(j).at(0).KolmogorovTest(&Total); 
	    hTitle=histo.at(j).at(0).GetTitle();
	    hTitle+=" KS=";
	    hTitle+=KS;
	    latex.DrawLatex(0.225, 0.95,hTitle);
	  }
	  if(verbose) std::cout << "Data: " << histo.at(j).at(0).Integral() << " MC: " << Total.Integral() << std::endl; 
	  }
	else{
	  MCHistoStack.Draw("Hist");
	  double max=Total.GetBinContent(Total.GetMaximumBin());
	  if(l==1){
	    histo.at(j).at(0).SetMinimum(0.01);
	    if(max!=0){
	      histo.at(j).at(0).SetMaximum(100*max);
	      //histo.at(j).at(0).SetMaximum(600*max); //cleo
	    }
	    else{
	      histo.at(j).at(0).SetMaximum(100);
	    }
	  } 
	  else {
	    histo.at(j).at(0).SetMinimum(0);
	    histo.at(j).at(0).SetMaximum(1.7*max);
	    //histo.at(j).at(0).SetMaximum(2.5*max); //cleo
	  }
	  histo.at(j).at(0).Draw("E");
	  MCHistoStack.Draw("Histsame");
	  MCHistoStack.Draw("axissame");
	  Total.Draw("E2same");
	}	
	leg.Draw();
	c.Update();
	TString name=histo.at(j).at(0).GetName();
	if(l==1){
	  name+="_log";
	}
	name+="_index_";
	name+=j;
	TString EPSName="EPS/";
	EPSName+=name;
	EPSName+=".eps";
	if(l==0 || !name.Contains("Nminus")  || name.Contains("Accumdist") ){
	  c.Print(EPSName);
	}
	if(l==0){
	  for(int a=0;a<HistogramNames_.size();a++){
	    if(histo.at(j).at(0).GetName()==HistogramNames_.at(a)){
	      TFile f(File_+"_EXTRA.root","RECREATE");
	      TString n=HistogramNames_.at(a);
	      n.ReplaceAll("Data","");
	      int s=histo.at(j).size()-1;
	      histo.at(j).at(0).Write((histo.at(j).at(0)).GetName());
	      histo.at(j).at(s).Write(n+"sig");
	      for(int t=2;t<s;t++){
		histo.at(j).at(2).Add(&histo.at(j).at(t));
	      }
	      histo.at(j).at(2).Write(n+"background");
	      f.Close();
	    }
	  }
	}
      }
      }
    }
    std::cout << "Plot1D done "  << std::endl;
}


void Plots::Plot1DSignificance(std::vector<TH1D> histo, bool gt,bool lt,float Lumi,std::vector<float> CrossSectionandAcceptance,std::vector<float> nevents,std::vector<int> colour,std::vector<TString> legend){
  // plots significance: assumes the histograms have been normilized
  std::cout << "Plots::Plot1DSignificance" << std::endl;
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadBottomMargin(0.20);
  gStyle->SetPadLeftMargin(0.175);
  gStyle->SetPadRightMargin(0.05);
  TCanvas c("c","c",200,10,750,750);
  c.Update();
  c.Clear();
  c.Update();

    histo.at(0).Reset();
    if(histo.size()>0){
     for(int i=0; i<histo.size();i++){ 
       if(nevents.at(i)>0 && Lumi>0 && CrossSectionandAcceptance.at(i)>0){
	 histo.at(i).Scale(Lumi*CrossSectionandAcceptance.at(i)/nevents.at(i));
	 
       }
     }      
      for(int b=1; b<=histo.at(0).GetNbinsX();b++){
	if(gt){
	  double sig=histo.at(histo.size()-1).Integral(b,histo.at(0).GetNbinsX()+1);
	  double bkg(0);
	  for(int i=1; i<histo.size()-1;i++){
	    bkg+=histo.at(i).Integral(b,histo.at(0).GetNbinsX()+1);
	  }
	  if(/*1+sig/bkg >0 && */(sig+bkg)!=0 ){
	    histo.at(0).SetBinContent(b,sig/sqrt(sig+bkg));//(b,sqrt(2)*sqrt((sig+bkg)*TMath::Log(1+sig/bkg)-sig));//(b,sig/sqrt(sig+bkg));
	  }
	  else{
	    histo.at(0).SetBinContent(b,0);
	  }
	}
	if(lt){
	  double sig=histo.at(histo.size()-1).Integral(0,b);
	  double bkg(0);
	  for(int i=1; i<histo.size()-1;i++){
	    bkg+=histo.at(i).Integral(0,b);
	  }
	  if(/*1+sig/bkg >0 && */sig+bkg!=0){
	    histo.at(0).SetBinContent(b,sig/sqrt(sig+bkg));//(b,sqrt(2)*sqrt((sig+bkg)*TMath::Log(1+sig/bkg)-sig));
	  }
	  else{
	    histo.at(0).SetBinContent(b,0);
	  }
	}
      }
      
      
      histo.at(0).SetYTitle("Signal/#sqrt{Signal+Bkg}");//("#sqrt{2((S+B)ln(1+S/B)-S)}");//("Signal/#sqrt{Signal+Bkg}");
      histo.at(0).Draw("hist");
      histo.at(0).Draw("axissame");
      
      TString name=histo.at(0).GetName();
      name+="_sig";
      if(lt) name+="lt";
      if(gt) name+="gt";
      name+="_index_";
      name+=0;
      TString EPSName="EPS/";
      EPSName+=name;
      EPSName+=".eps";
     
      if( !name.Contains("Accumdist") ){
	c.Print(EPSName);
	Int_t bmax=histo.at(0).GetMaximumBin();
	if(gt)bmax+=1;
	if(verbose)std::cout << "Opt. Sig. " <<  EPSName << " Sig: " <<   histo.at(0).GetMaximum()
		  << " bin: " <<  bmax <<   std::endl;
      }
    }
  
  std::cout << "Plots::Plot1DSignificance done" << std::endl;
}

void Plots::Plot1Dsigtobkg(std::vector<TH1D> histo, bool gt,bool lt,float Lumi,std::vector<float> CrossSectionandAcceptance,std::vector<float> nevents,std::vector<int> colour,std::vector<TString> legend){
  // plots significance: assumes the histograms have been normilized
  std::cout << "Plots::Plot1Dsigtobkg" << std::endl;
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadBottomMargin(0.20);
  gStyle->SetPadLeftMargin(0.175);
  gStyle->SetPadRightMargin(0.05);
  TCanvas c("c","c",200,10,750,750);
  c.Update();
  c.Clear();
  c.Update();

    histo.at(0).Reset();
    if(histo.size()>2){
     for(int i=0; i<histo.size();i++){ 
       if(nevents.at(i)>0 && Lumi>0 && CrossSectionandAcceptance.at(i)>0){
	 histo.at(i).Scale(Lumi*CrossSectionandAcceptance.at(i)/nevents.at(i));
       }
     }
      for(int b=1; b<=histo.at(0).GetNbinsX();b++){
	if(gt){
	  double sig=histo.at(histo.size()-1).Integral(b,histo.at(0).GetNbinsX()+1);
	  double bkg(0);
	  for(int i=1; i<histo.size()-1;i++){
	    bkg+=histo.at(i).Integral(b,histo.at(0).GetNbinsX()+1);
	  }
	  if((bkg+sig)!=0){
	    histo.at(0).SetBinContent(b,sig/(sig+bkg));
	  }
	  else{
	    histo.at(0).SetBinContent(b,0);
	  }
	}
	if(lt){
	  double sig=histo.at(histo.size()-1).Integral(0,b);
	  double bkg(0);
	  for(int i=1; i<histo.size()-1;i++){
	    bkg+=histo.at(i).Integral(0,b);
	  }
	  if(sig+bkg!=0){
	    histo.at(0).SetBinContent(b,sig/(sig+bkg));
	  }
	  else{
	    histo.at(0).SetBinContent(b,0);
	  }
	}
      }

      for(int b=1; b<=histo.at(0).GetNbinsX();b++){
        double total=histo.at(histo.size()-1).Integral();
        if(gt){
          double sig=histo.at(histo.size()-1).Integral(b,histo.at(0).GetNbinsX()+1);
	  if(total!=0){
            histo.at(1).SetBinContent(b,sig/total);
          }
          else{
            histo.at(1).SetBinContent(b,0);
	  }
	}
      	if(lt){
	  double sig=histo.at(histo.size()-1).Integral(0,b);
	  if(total!=0){
	    histo.at(1).SetBinContent(b,sig/total);
	  }
	  else{
	    histo.at(1).SetBinContent(b,0);
	  }
	}
      }

     
      histo.at(0).SetYTitle("Purity (Black)/Efficiency (Red)");
      histo.at(0).SetMinimum(0.0);
      histo.at(0).SetMaximum(1.0);
      histo.at(0).Draw("hist");
      histo.at(0).Draw("axissame");
      histo.at(1).SetLineColor(2);
      histo.at(1).Draw("histsame");

      TString name=histo.at(0).GetName();
      name+="_sigtobkg";
      if(lt) name+="lt";
      if(gt) name+="gt";
      name+="_index_";
      name+=0;
      TString EPSName="EPS/";
      EPSName+=name;
      EPSName+=".eps";
      if( !name.Contains("Accumdist") ){
	c.Print(EPSName);
      }
    }
  std::cout << "Plots::Plot1Dsigtobkg done" << std::endl;
}



void Plots::Plot1D_DataMC_Compare(std::vector<TH1D> histo, float Lumi,std::vector<float> CrossSectionandAcceptance,std::vector<float> nevents,std::vector<int> colour,std::vector<TString> legend){
  std::cout << "Plots::Plot1D_DataMC_Compare Start" << std::endl;
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadBottomMargin(0.20);
  gStyle->SetPadLeftMargin(0.175);
  gStyle->SetPadRightMargin(0.05);
  TCanvas c("c","c",200,10,750,750);
  c.Clear();

    if(histo.size()>4){
      for(int i=0; i<histo.size();i++){ 
	if(nevents.at(i)>0 && Lumi>0 && CrossSectionandAcceptance.at(i)>=0){
	  histo.at(i).Scale(Lumi*CrossSectionandAcceptance.at(i)/nevents.at(i));
	}
	if(i>1){
	  histo.at(1).Add(&histo.at(i),1);
	}
      }      
      if(doscale){
	if(verbose)std::cout << "Comparison Integral Scale:" << histo.at(0).Integral() << " " << histo.at(1).Integral() << std::endl;
	histo.at(1).Scale(histo.at(0).Integral()/histo.at(1).Integral());
      }
      for(int b=1; b<=histo.at(0).GetNbinsX();b++){
	double data=histo.at(0).GetBinContent(b);
	double dataerror=histo.at(0).GetBinError(b);
        double mc=histo.at(1).GetBinContent(b);
        double mcerror=histo.at(1).GetBinError(b);
	if(mc!=0){
	  histo.at(0).SetBinContent(b,data/mc);
	  histo.at(0).SetBinError(b,dataerror/mc);
          histo.at(2).SetBinContent(b,1);
          histo.at(2).SetBinError(b,mcerror/mc);

	}
	else{
	  histo.at(0).SetBinContent(b,0);
	  histo.at(0).SetBinError(b,0);
	  histo.at(2).SetBinContent(b,0);
          histo.at(2).SetBinError(b,2);
	}
        histo.at(1).SetBinContent(b,data-mc);
        histo.at(1).SetBinError(b,dataerror);
        histo.at(3).SetBinContent(b,0);
        histo.at(3).SetBinError(b,mcerror);

      }
      
      histo.at(0).SetYTitle("Data/MC");
      histo.at(0).SetMinimum(0.0);
      histo.at(0).SetMaximum(2.0);
      histo.at(0).Draw("E");
      histo.at(0).Draw("axissame");
      histo.at(2).SetFillColor(5);
      histo.at(2).SetLineColor(5);
      histo.at(2).SetMarkerColor(5);
      histo.at(2).SetMarkerSize(0.0001);
      histo.at(2).Draw("E2same");
      histo.at(0).Draw("Esame");
      TString name=histo.at(0).GetName();
      name+="_sig";
      name+="_index_";
      name+=0;
      TString EPSName="EPS/";
      EPSName+=name;
      EPSName+=".eps";
      if(  !name.Contains("Accumdist") ){
	c.Print(EPSName);
      }
      ///////////////////////////////////////////////////////////////////////////////////////////////////////
      histo.at(1).SetYTitle("Data-MC");
      histo.at(1).Draw("E");
      histo.at(1).Draw("axissame");
      histo.at(3).SetFillColor(5);
      histo.at(3).SetLineColor(5);
      histo.at(3).SetMarkerColor(5);
      histo.at(3).SetMarkerSize(0.0001);
      histo.at(3).Draw("E2same");
      histo.at(1).Draw("Esame");

      name=histo.at(0).GetName();
      name+="_sigtobkg";
      name+="_index_";
      name+=0;
      EPSName="EPS/";
      EPSName+=name;
      EPSName+=".eps";
      if(  !name.Contains("Accumdist") ){
        c.Print(EPSName);
      }

    }
  std::cout << "Plots::Plot1D_DataMC_Compare Finished" << std::endl;
}






void Plots::Plot2D(std::vector<TH2D>  histo,float Lumi,std::vector<float> CrossSectionandAcceptance,std::vector<float> nevents,std::vector<int> colour,std::vector<TString> legend){
  std::cout << "Plots::Plot2D" << std::endl;
  gStyle->SetPadTopMargin(0.10);
  gStyle->SetPadBottomMargin(0.22);
  gStyle->SetPadLeftMargin(0.180);
  gStyle->SetPadRightMargin(0.175);
  TCanvas c("c","c",200,10,750,750);
  if(verbose)std::cout << "Starting Plot2D" << histo.size() << " " << CrossSectionandAcceptance.size() << " " 
	    << colour.size() << " " << legend.size() << std::endl;
  c.Clear();
  int size=18;
  double xoffset=2.0;
  double yoffset=2.5;
  if(histo.size()>0){
    int ntypes(0);
    TString LegType="";
    double integral=0;
    for(int i=0; i<histo.size();i++){
      if(LegType!=legend.at(i)){
	LegType=legend.at(i);
	if(verbose)std::cout << "Legend " << legend.at(i) << " Integal " << integral <<  std::endl;
	integral=0;
	for(int j=i; j<histo.size();j++){
	  if(LegType==legend.at(j)) integral+=histo.at(j).Integral();
	}
	if(integral!=0) ntypes++;
	integral=0;
      }
    }
    ntypes+=1;
    if(verbose)std::cout << "ntypes " << ntypes << std::endl;
    if(ntypes==1){
      c.Divide(1,1);
      size=26;
      xoffset=1.5;
      yoffset=2.0;
    }
    if(ntypes==2){
      c.Divide(2,2);
      //size=20;
      //xoffset=6.0;
      //yoffset=5.0;
    }
    if(ntypes==3 || ntypes==4){
      c.Divide(2,2);
    }
    if(ntypes==5 || ntypes==6){
      c.Divide(2,3);
      size=20;
      xoffset=6.0;
      yoffset=5.0;
    }
    if(ntypes==7 || ntypes==8 || ntypes==9){
      c.Divide(3,3);
      size=10;
      xoffset=6.0;
      yoffset=5.0;
    }
    if(ntypes==10 || ntypes==11 || ntypes==12){
      c.Divide(3,4);
      size=10;
      xoffset=6.0;
      yoffset=5.0;
    }
    
    if(verbose)std::cout << "A " << ntypes << std::endl;
    std::vector<unsigned int> Goodhistoindx;
    std::vector<int> GoodhistoCanvas;
    int type=0;
    int index=0;
    for(int i=0; i<histo.size();i++){
      if(nevents.at(i)>0 && i!=0){
	if(ntypes>1){
	  if(nevents.at(i)>0 && Lumi>0 && CrossSectionandAcceptance.at(i)>0){
	    histo.at(i).Scale(Lumi*CrossSectionandAcceptance.at(i)/nevents.at(i));
	  }
	}
      }
    }
    index=0;
    LegType="";
    if(verbose)std::cout << "A " << ntypes << std::endl;

    std::vector<TH2D>::iterator it;
    it = histo.begin();
    TString tempname=histo.at(0).GetName();
    tempname+="TotalMC";
    histo.insert(it+1,*((TH2D*)histo.at(0).Clone(tempname.Data())));
    std::vector<TString>::iterator itleg=legend.begin();   
    legend.insert(itleg+1,"Total MC");
    std::vector<int>::iterator itcol=colour.begin();
    colour.insert(itcol+1,55);
    histo.at(1).Reset();
    for(int i=2; i<histo.size();i++){
      histo.at(1).Add(&histo.at(i));
    }
    if(verbose)std::cout << "B " << ntypes << std::endl;
    LegType="null";
    for(int i=0; i<histo.size();i++){
      if(verbose)std::cout << "histo i" << i << std::endl;
      if(LegType!=legend.at(i)){
	index=i;
	LegType=legend.at(i);
	for(int j=i+1; j<histo.size();j++){
	  if(verbose)std::cout << "histo j" << j << std::endl;
	  if(legend.at(j)==legend.at(i)) histo.at(i).Add(&histo.at(j));
	}
	if(verbose)std::cout << "legend " << i << " " << histo.at(index).Integral() << std::endl;  
	if(histo.at(index).Integral()!=0 ){
	  if(verbose)std::cout << "legend " << index << histo.at(index).Integral()  << std::endl;
	  type++;
	  Goodhistoindx.push_back(index);
	  GoodhistoCanvas.push_back(type);
	  c.cd(type);
	  histo.at(index).Draw("colz");
	  if(verbose)std::cout << " Drawing " << type << " " << index << " " << i << " " << legend.at(i) << std::endl;
	  c.Update();
	  TLatex l;
	  l.SetNDC();
	  l.DrawLatex(0.25, 0.90, legend.at(i));
	  index++;

	  if(dooneprofile){
	    c.Clear();
	    histo.at(index-1).Draw("colz");
	    TString name=histo.at(0).GetName();
	    name+="_index_";
	    name+=index;
	    TString EPSName="EPS/";
	    EPSName+=name;
	    EPSName+=".eps";
	    c.Print(EPSName);

	  }
	}
      }
    }
    TString name=histo.at(0).GetName();
    TString EPSName="EPS/";
    if(!dooneprofile){
      c.Update();
      name=histo.at(0).GetName();
      name+="_index_";
      name+=0;
      EPSName="EPS/";
      EPSName+=name;
      EPSName+=".eps";
      c.Print(EPSName);
    }
    if(verbose)std::cout << "Histo 2D " << EPSName << std::endl;
    
    //////////////////////////////////////////////////////
    //
    // Now add profile histograms
    //
    if(dooneprofile){
      c.Clear();
      c.Divide(1,1);
    }
    std::cout << Goodhistoindx.size() << std::endl;
    std::vector<TH1D*> ProfileStat;
    std::vector<TH1D*> ProfileRMS;
    ProfileStat.clear();
    ProfileRMS.clear();
    for(int p=0; p<3;p++){
      if(verbose)std::cout << "Now adding Profile" << std::endl;
      int l=0;
      int type=0;
      for(int j=0; j<Goodhistoindx.size();j++){
	bool filled=false;
	double m_max(0),m_min(0), meanmax(-10000);
	int i=Goodhistoindx.at(j);
	int profCanvas=GoodhistoCanvas.at(j);
	c.cd(profCanvas);
	if(verbose)std::cout << "legend " <<  legend.at(i) << " " << histo.at(i).Integral() << " i=" << i  << " " << histo.size() 
		  << colour.size() << std::endl;
	TString n="Profile_";
	if(verbose)std::cout << "A" << ProfileStat.size() << " " << ProfileRMS.size() <<  std::endl;
	n+=i;
	n+="_";
	n+=j;
	n+="_p";
	n+=p;
	n+="_";
	n+=histo.at(i).GetName();
	if(histo.at(i).GetNbinsX()==109){
	  ProfileStat.push_back(new TH1D(n,n,41,histo.at(i).GetXaxis()->GetXmin(),
					 40.5 ));

	}
	else{
	ProfileStat.push_back(new TH1D(n,n,histo.at(i).GetNbinsX(),histo.at(i).GetXaxis()->GetXmin(),
				       histo.at(i).GetXaxis()->GetXmax() ));
	}
	ProfileStat.at(ProfileStat.size()-1)->SetLineWidth(1);
	ProfileStat.at(ProfileStat.size()-1)->SetLineColor(1);
	ProfileStat.at(ProfileStat.size()-1)->SetFillStyle(1001);
	ProfileStat.at(ProfileStat.size()-1)->SetMarkerColor(1);
	ProfileStat.at(ProfileStat.size()-1)->SetMarkerSize(0.35);
	//ProfileStat.at(ProfileStat.size()-1)->SetMarkerStyle(8);
	if(verbose)std::cout << "B" << std::endl;
	ProfileRMS.push_back(new TH1D(n+"RMS",n+"RMS",histo.at(i).GetNbinsX(),histo.at(i).GetXaxis()->GetXmin(),
				      histo.at(i).GetXaxis()->GetXmax()));
	if(verbose)std::cout << "B1 - " << ProfileStat.size() << " " << ProfileRMS.size() << " " << colour.size() << std::endl;
        ProfileRMS.at(ProfileStat.size()-1)->SetLineWidth(1);
	if(verbose)std::cout << "1" << std::endl;
	ProfileRMS.at(ProfileStat.size()-1)->SetLineColor(colour.at(i));
	if(verbose)std::cout << "2" << std::endl;
	ProfileRMS.at(ProfileStat.size()-1)->SetFillStyle(1001);
	if(verbose)std::cout << "3" << std::endl;
	ProfileRMS.at(ProfileStat.size()-1)->SetFillColor(7);
	if(verbose)std::cout << "3" << std::endl;
	ProfileRMS.at(ProfileStat.size()-1)->SetMarkerColor(colour.at(i));
	if(verbose)std::cout << "4" << std::endl;
	ProfileRMS.at(ProfileStat.size()-1)->SetMarkerSize(0.35);
	//ProfileRMS.at(ProfileStat.size()-1)->SetMarkerStyle(8);
	if(verbose)std::cout << "C" << std::endl;
	for(int k=1;k<=histo.at(i).GetNbinsX();k++){
	  TH1D *hp=histo.at(i).ProjectionY("p"+n,k-1,k);
	  double MEAN=hp->GetMean();
	  double MEANERR=hp->GetMeanError();
	  double RMS=hp->GetRMS();
	  if(p==2){
	    MEAN=RMS;
	    MEANERR=hp->GetRMSError();
	  }
	  if(!filled && hp->Integral()>0){
	    m_max=MEAN+fabs(MEANERR)+0.1;
	    m_min=MEAN-fabs(MEANERR)-0.1;
	    filled=true;
	  }
	  if(p==0)RMS=0;
	  if(MEAN>-10000 && MEAN<10000){
	    ProfileStat.at(ProfileStat.size()-1)->SetBinContent(k,MEAN);
	    ProfileStat.at(ProfileStat.size()-1)->SetBinError(k,MEANERR);
	  }
	  if(RMS>0 && RMS<100000){
	    ProfileRMS.at(ProfileStat.size()-1)->SetBinContent(k,MEAN);
	    ProfileRMS.at(ProfileStat.size()-1)->SetBinError(k,fabs(RMS));
	  }
	  if(MEAN>meanmax) meanmax=MEAN;
	  if((MEAN+fabs(MEANERR)+fabs(RMS))>m_max) m_max=(MEAN+fabs(MEANERR)+fabs(RMS));
	  if((MEAN-fabs(MEANERR)-fabs(RMS))<m_min) m_min=(MEAN-fabs(MEANERR)-fabs(RMS));
	}
	if(verbose)std::cout << "C" << std::endl;
	if(verbose)std::cout << "min: " << m_min << " " << m_max << " " << histo.at(0).GetTitle() << " " << p << " " << meanmax
		  << " " << histo.at(i).Integral() << std::endl; 
	ProfileStat.at(ProfileStat.size()-1)->SetMaximum(m_max);//(histo.at(i).GetYaxis()->GetXmax());//(m_max);
	ProfileStat.at(ProfileStat.size()-1)->SetMinimum(m_min);//(histo.at(i).GetYaxis()->GetXmin());//(m_min);
	ProfileStat.at(ProfileStat.size()-1)->GetXaxis()->SetTitle(histo.at(i).GetXaxis()->GetTitle());
	TString temp="Mean of ";
	if(p==2) temp="RMS of ";
	temp+=histo.at(0).GetYaxis()->GetTitle();
	ProfileStat.at(ProfileStat.size()-1)->GetYaxis()->SetTitle(temp);
	if(verbose)std::cout << "D" << std::endl;
	if(((Goodhistoindx.at(j)==0 || Goodhistoindx.at(j)==1 && p!=1) && dooneprofile) ||  !dooneprofile ){
	  if(Goodhistoindx.at(j)==1){
	    ProfileStat.at(ProfileStat.size()-1)->SetFillColor(5);
	    if(p==1){
	      ProfileStat.at(ProfileStat.size()-1)->Draw("pe");
	      ProfileRMS.at(ProfileStat.size()-1)->Draw("E2 same");
	      ProfileStat.at(ProfileStat.size()-1)->Draw("pe same");
	    }
	    if(p!=1){
	      //c.cd(profCanvas-1);
	      ProfileStat.at(ProfileStat.size()-1)->SetLineColor(1);
	      ProfileStat.at(ProfileStat.size()-1)->SetMarkerColor(1);
	      ProfileStat.at(ProfileStat.size()-1)->SetFillColor(5);
	      ProfileStat.at(ProfileStat.size()-1)->Draw("E2 same");
	      ProfileStat.at(ProfileStat.size()-2)->Draw("pe same");
	    }
	  }
	  else if(p==0){
	    ProfileStat.at(ProfileStat.size()-1)->Draw("pe");
	  }
	  else if(p==1){
	    ProfileStat.at(ProfileStat.size()-1)->Draw("pe");
	    ProfileRMS.at(ProfileStat.size()-1)->Draw("E2 same");
	    ProfileStat.at(ProfileStat.size()-1)->Draw("pe same");
	  }
	  else if(p==2){
	    ProfileStat.at(ProfileStat.size()-1)->Draw("pe");
	  }
	  if(!dooneprofile){
	    if(verbose)std::cout << "E" << std::endl;
	    c.Update();
	    TLatex l;
	    l.SetNDC();
	    c.cd(profCanvas);
	    l.DrawLatex(0.25, 0.90, legend.at(i));
	  }
	}
      }
      name=histo.at(0).GetName();
      name+="_Profile_";
      name+=p;
      name+="_index_";
      name+=0;
      EPSName="EPS/";
      EPSName+=name;
      EPSName+=".eps";
      c.Print(EPSName);
    }
    for(int q=0;q< ProfileStat.size();q++){
      delete ProfileStat.at(q);
      delete ProfileRMS.at(q);
    }
    ProfileStat.clear();
    ProfileRMS.clear();
  }
}



void Plots::Plot3D(std::vector<TH3F>  histo,float Lumi,std::vector<float> CrossSectionandAcceptance,std::vector<float> nevents,std::vector<int> colour,std::vector<TString> legend){
  std::cout << "Plots::Plot3D" << std::endl;
  gStyle->SetPadTopMargin(0.10);
  gStyle->SetPadBottomMargin(0.22);
  gStyle->SetPadLeftMargin(0.180);
  gStyle->SetPadRightMargin(0.175);
  gStyle->SetTitleXOffset(2.4);
  gStyle->SetTitleYOffset(2.4);

  TCanvas c("c","c",200,10,750,750);
  if(histo.size()>0){
    if(verbose)std::cout << "Starting Plot3D" << histo.size() << " " << CrossSectionandAcceptance.size() << " " << colour.size() << " " << legend.size() << std::endl;
    c.Clear();
    c.Divide(2,2);
    int type=0;
    int index=0;
    for(int i=0; i<histo.size();i++){
      if(nevents.at(i)>0 && i!=0){
	if(nevents.at(i)>0 && Lumi>0 && CrossSectionandAcceptance.at(i)>0){
	  histo.at(i).Scale(Lumi*CrossSectionandAcceptance.at(i)/nevents.at(i));
	}
      }
    }
    std::cout << "A" << std::endl;
    TLegend leg(0.1,0.4,0.9,0.9);
    leg.SetBorderSize(0);
    leg.SetFillStyle(4000);
    leg.SetFillColor(0);
    leg.SetTextSize(0.03);
    leg.SetMargin(0.15);
    leg.SetNColumns(1);
    leg.SetColumnSeparation(0.05);
    for(unsigned int p=0;p<4;p++){
      for(int i=0; i<histo.size();i++){
	histo.at(i).SetMarkerSize(0.25);
	histo.at(i).SetMarkerColor(colour.at(i));
	histo.at(i).GetXaxis()->SetTitleOffset(2.0);
	histo.at(i).GetYaxis()->SetTitleOffset(2.0);
	histo.at(i).GetZaxis()->SetTitleOffset(2.0);
	if(p==0)gPad->SetPhi(30);
        if(p==1)gPad->SetPhi(150);
        if(p==2)gPad->SetPhi(240);
	if(colour.at(i)==10)histo.at(i).SetMarkerColor(16);
	if(colour.at(i)==414)histo.at(i).SetMarkerColor(3);
	if(p!=3){
	  c.cd(p+1);

	  if(i==0){ histo.at(i).Draw();  }
	  else{histo.at(i).Draw("same");}
	}
	else{
	  leg.AddEntry(&histo.at(i),legend.at(i),"pe");
	}
      }
    }
    c.cd(4);
    leg.Draw();
    TString name=histo.at(0).GetName();
    TString EPSName="EPS/";
    c.Update();
    name=histo.at(0).GetName();
    name+="_index_";
    name+=0;
    EPSName="EPS/";
    EPSName+=name;
    EPSName+=".eps";
    c.Print(EPSName);
    if(verbose)std::cout << "Histo 3D " << EPSName << std::endl;
  }
}

void Plots::CMSStyle1(){
  std::cout << "Configuring Plots::CMSStyle1()" << std::endl;
  // use plain black on white colors
  Int_t icol=0; // WHITE 
  gStyle->SetFrameBorderMode(icol);
  gStyle->SetFrameFillColor(icol);
  gStyle->SetCanvasBorderMode(icol);
  gStyle->SetCanvasColor(icol);
  gStyle->SetPadBorderMode(icol);
  gStyle->SetPadColor(icol);
  gStyle->SetStatColor(icol);
  //gStyle->SetFillColor(icol); // don't use: white fill color for *all* objects
  // set the paper & margin sizes                                               
  gStyle->SetPaperSize(20,26);

  // set margin sizes
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadBottomMargin(0.16);
  gStyle->SetPadLeftMargin(0.16);

  // set title offsets (for axis label)
  gStyle->SetTitleXOffset(1.4);
  gStyle->SetTitleYOffset(1.4);

  // use large fonts
  //Int_t font=72; // Helvetica italics                                                                                                                                                                                                      
  Int_t font=42; // Helvetica
  Double_t tsize=0.05;
  gStyle->SetTextFont(font);

  gStyle->SetTextSize(tsize);
  gStyle->SetLabelFont(font,"x");
  gStyle->SetTitleFont(font,"x");
  gStyle->SetLabelFont(font,"y");
  gStyle->SetTitleFont(font,"y");
  gStyle->SetLabelFont(font,"z");
  gStyle->SetTitleFont(font,"z");

  gStyle->SetLabelSize(tsize,"x");
  gStyle->SetTitleSize(tsize,"x");
  gStyle->SetLabelSize(tsize,"y");
  gStyle->SetTitleSize(tsize,"y");
  gStyle->SetLabelSize(tsize,"z");
  gStyle->SetTitleSize(tsize,"z");

  // use bold lines and markers
  gStyle->SetMarkerStyle(20);
  gStyle->SetMarkerSize(1.2);
  gStyle->SetHistLineWidth(2);
  gStyle->SetLineStyleString(2,"[12 12]"); // postscript dashes

  // get rid of X error bars
  //gStyle->SetErrorX(0.001);
  // get rid of error bar caps
  gStyle->SetEndErrorSize(0.);

  // do not display any of the standard histogram decorations
  gStyle->SetOptTitle(0);
  //gStyle->SetOptStat(1111);
  gStyle->SetOptStat(0);
  //gStyle->SetOptFit(1111);
  gStyle->SetOptFit(0);
  // put tick marks on top and RHS of plots
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetPalette(1);
  gROOT->ForceStyle(true);

}




void Plots::CMSStyle2(){
  std::cout << "Configuring Plots::CMSStyle2()" << std::endl;
  // use plain black on white colors
  Int_t icol=0; // WHITE
  gStyle->SetFrameBorderMode(icol);
  gStyle->SetFrameFillColor(icol);
  gStyle->SetCanvasBorderMode(icol);
  gStyle->SetCanvasColor(icol);
  gStyle->SetPadBorderMode(icol);
  gStyle->SetPadColor(icol);
  gStyle->SetStatColor(icol);
  //gStyle->SetFillColor(icol); // don't use: white fill color for *all* objects

  // set the paper & margin sizes
  gStyle->SetPaperSize(20,26);

  // set margin sizes
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadBottomMargin(0.16);
  gStyle->SetPadLeftMargin(0.16);

  // set title offsets (for axis label)
  gStyle->SetTitleXOffset(1.4);
  gStyle->SetTitleYOffset(1.4);

  // use large fonts
  //Int_t font=72; // Helvetica italics
  Int_t font=42; // Helvetica
  Double_t tsize=0.05;
  gStyle->SetTextFont(font);

  gStyle->SetTextSize(tsize);
  gStyle->SetLabelFont(font,"x");
  gStyle->SetTitleFont(font,"x");
  gStyle->SetLabelFont(font,"y");
  gStyle->SetTitleFont(font,"y");
  gStyle->SetLabelFont(font,"z");
  gStyle->SetTitleFont(font,"z");
  
  gStyle->SetLabelSize(tsize,"x");
  gStyle->SetTitleSize(tsize,"x");
  gStyle->SetLabelSize(tsize,"y");
  gStyle->SetTitleSize(tsize,"y");
  gStyle->SetLabelSize(tsize,"z");
  gStyle->SetTitleSize(tsize,"z");

  // use bold lines and markers
  gStyle->SetMarkerStyle(20);
  gStyle->SetMarkerSize(1.2);
  gStyle->SetHistLineWidth(2);
  gStyle->SetLineStyleString(2,"[12 12]"); // postscript dashes

  // get rid of X error bars 
  //gStyle->SetErrorX(0.001);
  // get rid of error bar caps
  gStyle->SetEndErrorSize(0.);

  // do not display any of the standard histogram decorations
  gStyle->SetOptTitle(0);
  //gStyle->SetOptStat(1111);
  gStyle->SetOptStat(0);
  //gStyle->SetOptFit(1111);
  gStyle->SetOptFit(0);

  // put tick marks on top and RHS of plots
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetPalette(1);  
  gROOT->ForceStyle(true);    
  
}



void Plots::CMSLabel(Double_t x,Double_t y,Color_t color)
{
  TLatex l; //l.SetTextAlign(12); l.SetTextSize(tsize);                                                                                                                                                                                      
  l.SetNDC();
  l.SetTextFont(72);
  l.SetTextColor(color);

  double delx = 0.115*696*gPad->GetWh()/(472*gPad->GetWw());

  l.DrawLatex(x,y,"CMS");
  TLatex p;
  p.SetNDC();
  p.SetTextFont(42);
  p.SetTextColor(color);
  if(plotLabel==cmsPrivate){
    p.DrawLatex(x+delx,y,"Private");
  }
  if(plotLabel==cmsPreliminary){
    p.DrawLatex(x+delx,y,"Preliminary");
  }
}



