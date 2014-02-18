#include <stdio.h>

#include <TCanvas.h>
#include <TFile.h>
#include <TH1.h>
#include <TLegend.h>


using namespace std;




// ***** drawPlots *****
// function to draw plots for each cut (cuts added manually)
// cutN is just a label for the saved canvas
void drawPlots(string cutN){

  TFile* f;
  f = new TFile("output.root", "READ");

  TH1D* h_goodmuons;
  TH1D* h_pt;
  TH1D* h_eta;

  h_goodmuons = (TH1D*) f->Get("lightchargedhiggs_default_goodmuonsHplusBWB_M80");
  h_pt = (TH1D*) f->Get("lightchargedhiggs_default_muonPtHplusBWB_M80");
  h_eta = (TH1D*) f->Get("lightchargedhiggs_default_muonEtaHplusBWB_M80");

  TCanvas* c = new TCanvas("c", "c", 1);
  c->cd();

  gPad->SetMargin(0.15, 0.1, 0.15, 0.1);

  h_goodmuons->SetMarkerSize(0.8);
  h_goodmuons->SetLineColor(kBlack);
  h_goodmuons->Draw("hist");
  c->SaveAs(("~/Desktop/img/goodmuonsPlot"+cutN+".png").c_str());
  c->SaveAs(("~/Desktop/img/goodmuonsPlot"+cutN+".pdf").c_str());
  
  h_pt->SetMarkerSize(0.8);
  h_pt->Draw();
  c->SaveAs(("~/Desktop/img/ptPlot"+cutN+".png").c_str());
  c->SaveAs(("~/Desktop/img/ptPlot"+cutN+".pdf").c_str());
 
  h_eta->SetMarkerSize(0.8);
  h_eta->Draw();
  c->SaveAs(("~/Desktop/img/etaPlot"+cutN+".png").c_str());
  c->SaveAs(("~/Desktop/img/etaPlot"+cutN+".pdf").c_str());
  
  c->Close();

}
// ----- drawPlots -----



// ***** drawPlots_sig *****
// function to draw plots only for signal events
void drawPlots_sig(){
  
  TFile* f;
  f = new TFile("LOCAL_COMBINED_lightchargedhiggs_default.root", "READ");

  TH1D* h_goodmuons;
  TH1D* h_pt;
  TH1D* h_eta;

  h_goodmuons = (TH1D*) f->Get("lightchargedhiggs_default_goodmuonsHplusBWB_M80");
  h_pt = (TH1D*) f->Get("lightchargedhiggs_default_muonPtHplusBWB_M80");
  h_eta = (TH1D*) f->Get("lightchargedhiggs_default_muonEtaHplusBWB_M80");

  TCanvas* c = new TCanvas("c", "c", 1);
  c->cd();

  gPad->SetMargin(0.15, 0.1, 0.15, 0.1);


  // ** number of good muons **
  h_goodmuons->SetLineColor(kBlack);
  h_goodmuons->Draw("hist");
  c->SaveAs("~/Desktop/img/goodmuonsPlot_sig.png");
  c->SaveAs("~/Desktop/img/goodmuonsPlot_sig.pdf");


  // ** muon pt distribution **  
  h_pt->SetMarkerSize(0.8);
  h_pt->Draw();
  c->SaveAs("~/Desktop/img/ptPlot_sig.png");
  c->SaveAs("~/Desktop/img/ptPlot_sig.pdf");
 

  // ** muon eta distribution **
  h_eta->SetMarkerSize(0.8);
  h_eta->Draw();
  c->SaveAs("~/Desktop/img/etaPlot_sig.png");
  c->SaveAs("~/Desktop/img/etaPlot_sig.pdf");
  
  c->Close();
  
}
// ----- drawPlots_sig -----



// ***** drawPlots_sig_bkg *****
// function to draw plots for signal  and bkg events
void drawPlots_sig_bkg(){

  TFile* f;
  f = new TFile("LOCAL_COMBINED_lightchargedhiggs_default.root", "READ");

  //histo sig
  TH1D* h_goodmuons;
  TH1D* h_pt;
  TH1D* h_eta;

  //histo bkg
  TH1D* h_goodmuons_ttbar;
  TH1D* h_pt_ttbar;
  TH1D* h_eta_ttbar;

  h_goodmuons = (TH1D*) f->Get("lightchargedhiggs_default_goodmuonsHplusBWB_M80");
  h_pt = (TH1D*) f->Get("lightchargedhiggs_default_muonPtHplusBWB_M80");
  h_eta = (TH1D*) f->Get("lightchargedhiggs_default_muonEtaHplusBWB_M80");

  h_goodmuons_ttbar = (TH1D*) f->Get("lightchargedhiggs_default_goodmuonsttbar");
  h_pt_ttbar = (TH1D*) f->Get("lightchargedhiggs_default_muonPtttbar");
  h_eta_ttbar = (TH1D*) f->Get("lightchargedhiggs_default_muonEtattbar");

  TCanvas* c = new TCanvas("c", "c", 1);
  c->cd();

  gPad->SetMargin(0.15, 0.1, 0.15, 0.1);


  TLegend* leg = new TLegend(0.7, 0.75, 0.85, 0.85);
  leg->SetTextSize(0.030);
  leg->SetFillColor(kWhite);
  leg->SetLineColor(kWhite);

  // ** number of good muons **
  leg->AddEntry(h_goodmuons_ttbar, "t#bar{t} MC", "F");
  leg->AddEntry(h_goodmuons, "H^{+}bWb MC", "L");
  h_goodmuons_ttbar->SetTitle("H^{+}bWb (m_{H^{+}} = 80 GeV) and t#bar{t}");
  h_goodmuons_ttbar->SetLineColor(kGreen-2);
  h_goodmuons_ttbar->SetFillColor(kGreen-2);
  h_goodmuons_ttbar->Draw("hist");
  h_goodmuons->SetLineColor(kBlack);
  h_goodmuons->Draw("histsame");
  leg->Draw("same");
  c->SaveAs("~/Desktop/img/goodmuonsPlot_sig_bkg.png");
  c->SaveAs("~/Desktop/img/goodmuonsPlot_sig_bkg.pdf");
  
  c->Clear();
  leg->Clear();

  // ** muon pt distribution **
  leg->AddEntry(h_pt_ttbar, "t#bar{t} MC", "F");
  leg->AddEntry(h_pt, "H^{+}bWb MC", "P");
  h_pt_ttbar->SetTitle("H^{+}bWb (m_{H^{+}} = 80 GeV) and t#bar{t}");
  h_pt_ttbar->SetLineColor(kGreen-2);
  h_pt_ttbar->SetFillColor(kGreen-2);
  h_pt_ttbar->Draw("hist");
  h_pt->SetMarkerSize(0.8);
  h_pt->SetMarkerColor(kBlack);
  h_pt->Draw("same");
  leg->Draw("same");
  c->SaveAs("~/Desktop/img/ptPlot_sig_bkg.png");
  c->SaveAs("~/Desktop/img/ptPlot_sig_bkg.pdf");
 
  c->Clear();
  leg->Clear();

  // ** muon eta distribution **
  leg->AddEntry(h_eta_ttbar, "t#bar{t} MC", "F");
  leg->AddEntry(h_eta, "H^{+}bWb MC", "P");
  h_eta_ttbar->SetTitle("H^{+}bWb (m_{H^{+}} = 80 GeV) and t#bar{t}");
  h_eta_ttbar->SetLineColor(kGreen-2);
  h_eta_ttbar->SetFillColor(kGreen-2);
  h_eta_ttbar->Draw("hist");
  h_eta->SetMarkerSize(0.8);
  h_eta->SetMarkerColor(kBlack);
  h_eta->Draw("same");
  leg->Draw("same");
  c->SaveAs("~/Desktop/img/etaPlot_sig_bkg.png");
  c->SaveAs("~/Desktop/img/etaPlot_sig_bkg.pdf");
  
  c->Close();

}
// ----- drawPlots_sig_bkg -----


