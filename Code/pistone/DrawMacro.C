#include <iostream>
#include <stdexcept>   // throw exception
#include <stdio.h>
#include <string>   //string, tostring
#include <sys/stat.h>   //mkdir
#include <sys/types.h>   //mkdir

#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TLegend.h"
#include "THStack.h"
#include "TStyle.h"

using namespace std;

// NOTE: all the names of new objects are in camelCase

// ***** classes & structures & typedef *****

/// struct to save name, label, and color info of an histogram of one dataset
struct plotAtt{

public:
    string Name;
    string Label;
    int Color;

    // constructor
    plotAtt(string name="empty", string label="empty", int color=0){
        Name = name;
        Label = label;
        Color = color;
    }

    // ohter methods
    void Set(string newname, string newlabel, int newcolor){
        Name = newname;
        Label = newlabel;
        Color = newcolor;
    }

};



/// class to save info of plotAtt of all the datasets
class allPlotAtt : public vector<plotAtt>{

private:

public:
    plotAtt plotAttributes;

    // constructor
    allPlotAtt(){

        FILE *f;
        f = fopen("../InputData/HistoM80.txt", "r");
        if(f == NULL){
            cerr << "opening file: FAILED" << endl;
            throw runtime_error("missing input file");
        }
        
        char start[35], name[35], label[35];
        int id;
        float xsec;
        unsigned int color;
        while(fscanf(f, "%s %i %f %s %s %u", start, &id, &xsec, name, label, &color) != EOF){
            plotAttributes.Set(name, label, color);
            this->push_back(plotAttributes);
            //cout<< "*****" << start <<" "<<id<<" "<<xsec<<" "<< name <<" "<< label <<" "<< color<<endl;
        }
    
        fclose(f);            
    }
    
};



/** this is the list of plots I want to do;
The strings are the names of the histrograms created in LightChargedHiggs.cxx
*/
const char* list[] = {"goodMuons", "goodMuonPt", "goodMuonEta", "transMass", "goodTaus", "goodTauPt", "goodTauEta", "goodTauPhi", "goodJets", "goodJet1stPt", "goodJet1stEta", "goodJet1stMass", "goodJet2ndPt", "goodJet2ndEta", "goodJet2ndMass", "METEt", "METPhi"};

/// class to have a list of variables to plot
class varToPlot : public vector<string>{

private:


public:

    // constructor
    varToPlot() : vector<string>(list, list + sizeof(list) / sizeof(char*)) {}
 
};


/// class to have my own legend
class myLegend : public TLegend{

private:

public:
    
    // constructor
    myLegend(double width=.65, double height=.18) : TLegend(.9-width, .88-height, .9, .88){
        this->SetBorderSize(0);
        this->SetFillStyle(4000);
        this->SetFillColor(0);
        this->SetTextSize(0.03);
        this->SetMargin(0.15);
        this->SetNColumns(3);
        this->SetColumnSeparation(0.05);
    }

};



int main(){

    // create a directory img where to save plots
    mkdir("img", 0755);   // dir where save plots w/ all the single bkgs indicated in the legend
    mkdir("imgBkg", 0755);   // dir where save plots w/ main bkg indicated in the legend

    // constants
    string prefix = "lightchargedhiggs_default_";
    
    
    // variables declaration
    allPlotAtt alldatasets;
    string titleX, titleY;

    varToPlot mylist;


    // objects to draw plots
    TCanvas *c = new TCanvas("c", "c", 200, 10, 750, 750);
    myLegend leg;
    myLegend leg2(.22,.22);
    leg2.SetNColumns(1);

    // Open the root file with histograms
    TFile *rootfile;
    rootfile = new TFile(("../../LOCAL_COMBINED_"+prefix+"LumiScaled.root").c_str(), "READ");
    if(!rootfile){
        cerr << "opening file: FAILED" << endl;
        throw runtime_error("missing input rootfile");
    }


    for(unsigned int i=0; i<mylist.size(); ++i){
        
        THStack hs_bkg;
        TH1D* h_total;   // to draw errors bars for MCs
        TH1D* h_data;
        TH1D* h_sig;

        // histograms for aggregated bkg
        TH1D* h_wjets;
        TH1D* h_dy;
        TH1D* h_dibosons;
        TH1D* h_ttbar;
        TH1D* h_singlet;
        
        THStack hs_bkg2;

        
        string vartmp = mylist.at(i);

        // some booleans necessary for h->Add(htmp)
        // In fact, if h is still empty, it's necessary to do h->Clone()
        // otherwise ROOT breaks
        bool total = false;
        bool wjets = false;
        bool dy = false;
        bool dibosons = false;
        bool singlet = false;



        for(unsigned int j=0; j<alldatasets.size(); ++j){
            
            TH1D* htmp; // temporary histogram
            string nametmp = alldatasets.at(j).Name;
            string labeltmp = alldatasets.at(j).Label;
            int colortmp = alldatasets.at(j).Color;

            //cout << nametmp << endl;
            //cout << prefix+vartmp+nametmp <<endl;

            htmp = (TH1D*) rootfile->Get((prefix+vartmp+nametmp).c_str());
            if(!htmp){
                cerr << "getting histogram: FAILED" << endl;
                throw runtime_error("missing histogram");
            }       

            if(j==0){
                titleX = htmp->GetXaxis()->GetTitle();
                titleY = htmp->GetYaxis()->GetTitle();
            }

            if(nametmp == "Data"){
                cout << "set: "<< nametmp << " doing Data" <<endl;
                h_data = (TH1D*) htmp->Clone();
                h_data->SetMarkerColor(colortmp);
                if(i==0) leg.AddEntry(h_data, (labeltmp).c_str(), "pe");
                if(i==0) leg2.AddEntry(h_data, (labeltmp).c_str(), "pe");
            } // if data
            
            else if(nametmp == "HplusBWB_M80"){
                cout << "set: "<< nametmp << " doing H+" <<endl;
                h_sig = (TH1D*) htmp->Clone();
                h_sig->SetLineColor(colortmp);
                h_sig->SetLineWidth(4);
                //h_sig->SetFillColor(colortmp);
                if(i==0) leg.AddEntry(h_sig, (labeltmp).c_str(), "L");
                if(i==0) leg2.AddEntry(h_sig, (labeltmp).c_str(), "L");
            } // if signal

            else{
                cout << "set: "<< nametmp << " doing bkg" <<endl;
                htmp->SetLineColor(1);
                htmp->SetFillColor(colortmp);
                //if(nametmp=="MC_WJ" || nametmp=="MC_WJ_tau") continue; 
                hs_bkg.Add(htmp);

                if(total==false){
                    h_total = (TH1D*) htmp->Clone();
                    h_total->SetFillStyle(3005);
                    h_total->SetFillColor(1);
                    h_total->SetLineColor(18);
                    h_total->SetMarkerColor(1);
                    h_total->SetMarkerSize(0.001);
                    //h_total->Sumw2();
                    total = true;
                }
                else{
                    h_total->Add(htmp);
                }
                
                if(i==0) leg.AddEntry(htmp, (labeltmp).c_str(), "F");


                // W+jets
                if(nametmp=="MC_WJ" || nametmp=="MC_WJ_tau"){
                    if(wjets==false){
                        h_wjets = (TH1D*) htmp->Clone();
                        h_wjets->SetLineColor(1);
                        h_wjets->SetFillColor(colortmp);
                        if(i==0) leg2.AddEntry(h_wjets, "W+jets", "F");
                        wjets = true;
                    }
                    else{
                        h_wjets->Add(htmp);
                    }            
                }
                // DY
                if(nametmp=="MC_DY_tautau" || nametmp=="MC_DY_mumu" || nametmp=="MC_DY_ee"){
                    if(dy==false){
                        h_dy = (TH1D*) htmp->Clone();
                        h_dy->SetLineColor(1);
                        h_dy->SetFillColor(colortmp);
                        if(i==0) leg2.AddEntry(h_dy, "DY", "F");
                        dy = true;
                    }
                    else{
                        h_dy->Add(htmp);
                    }            
                }
                // dibosons
                if(nametmp=="MC_WW_2l2nu" || nametmp=="MC_WZ_2l2q" || nametmp=="MC_WZ_3l1nu" || nametmp=="MC_ZZ_4l" || nametmp=="MC_ZZ_2l2nu" || nametmp=="MC_ZZ_2l2q"){
                    if(dibosons==false){
                        h_dibosons = (TH1D*) htmp->Clone();
                        h_dibosons->SetLineColor(1);
                        h_dibosons->SetFillColor(colortmp);
                        if(i==0) leg2.AddEntry(h_dibosons, "dibosons", "F");
                        dibosons = true;
                    }
                    else{
                        h_dibosons->Add(htmp);
                    }            
                }
                // ttbar
                if(nametmp=="MC_ttbar"){
                    h_ttbar = (TH1D*) htmp->Clone();
                    if(i==0) leg2.AddEntry(h_ttbar, "t#bar{t}", "F");
                }
                // single top
                if(nametmp=="MC_tW" || nametmp=="MC_tbarW"){
                    if(singlet==false){
                        h_singlet = (TH1D*) htmp->Clone();
                        h_singlet->SetLineColor(1);
                        h_singlet->SetFillColor(colortmp);
                        if(i==0) leg2.AddEntry(h_singlet, "single top", "F");
                        singlet = true;
                    }
                    else{
                        h_singlet->Add(htmp);
                    }            
                }

            } // if bkg

 
        } // loop over all the datasets

        
        hs_bkg2.Add(h_wjets);
        hs_bkg2.Add(h_dy);
        hs_bkg2.Add(h_dibosons);
        hs_bkg2.Add(h_ttbar);
        hs_bkg2.Add(h_singlet);
        

        c->cd();

        c->SetLogy(0);
        
        gPad->SetMargin(0.15,0.05,0.15,0.1);
        gPad->SetTicks(1, 1);       
 
        double max = h_data->GetBinContent(h_data->GetMaximumBin());


        // plot w/ all the bkgs indicated in the legend
        hs_bkg.SetMinimum(0);
        hs_bkg.SetMaximum(1.7*max);
        hs_bkg.Draw("hist");

        hs_bkg.GetXaxis()->SetTitle((titleX).c_str());
        hs_bkg.GetXaxis()->SetTitleOffset(1.2);
        hs_bkg.GetXaxis()->SetTitleSize(0.05);
        hs_bkg.GetXaxis()->SetLabelSize(0.05);
    
        hs_bkg.GetYaxis()->SetTitle((titleY).c_str());
        hs_bkg.GetYaxis()->SetTitleOffset(1.5);
        hs_bkg.GetYaxis()->SetTitleSize(0.05);
        hs_bkg.GetYaxis()->SetLabelSize(0.05);
        
        h_total->Draw("E2same");

        h_sig->Draw("histsame");
        h_data->Draw("EPsame");
        h_data->Draw("axissame");
        leg.Draw();
        
        c->SaveAs(("./img/"+vartmp+".eps").c_str());


        c->SetLogy(1);
        
        hs_bkg.SetMinimum(0.01);
        hs_bkg.SetMaximum(100*max);

        c->SaveAs(("./img/"+vartmp+"log.eps").c_str());


        
        c->SetLogy(0);

        // plot w/ the bkgs grouped
        hs_bkg2.SetMinimum(0);
        hs_bkg2.SetMaximum(1.7*max);
        hs_bkg2.Draw("hist");

        hs_bkg2.GetXaxis()->SetTitle((titleX).c_str());
        hs_bkg2.GetXaxis()->SetTitleOffset(1.2);
        hs_bkg2.GetXaxis()->SetTitleSize(0.05);
        hs_bkg2.GetXaxis()->SetLabelSize(0.05);
    
        hs_bkg2.GetYaxis()->SetTitle((titleY).c_str());
        hs_bkg2.GetYaxis()->SetTitleOffset(1.5);
        hs_bkg2.GetYaxis()->SetTitleSize(0.05);
        hs_bkg2.GetYaxis()->SetLabelSize(0.05);
        
        h_total->Draw("E2same");

        h_sig->Draw("histsame");
        h_data->Draw("EPsame");
        h_data->Draw("axissame");
        leg2.Draw();
        
        c->SaveAs(("./imgBkg/"+vartmp+".eps").c_str());


        c->SetLogy(1);
        
        hs_bkg2.SetMinimum(0.01);
        hs_bkg2.SetMaximum(100*max);

        c->SaveAs(("./imgBkg/"+vartmp+"log.eps").c_str());

        c->Clear();

    } // loop over mylist



    rootfile->Close();


    return 0;

} // main
