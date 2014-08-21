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
const char* list[] = {"goodMuons_", "goodMuonPt_", "goodMuonEta_", "transMass_", "goodTaus_", "goodTauPt_", "goodTauEta_", "goodTauPhi_", "goodJets_", "goodJet1stPt_", "goodJet1stEta_", "goodJet1stMass_", "goodJet2ndPt_", "goodJet2ndEta_", "goodJet2ndMass_", "METEt", "METPhi"};

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



// ***** drawPlots *****
void drawPlots(){

    string prefix = "lightchargedhiggs_default_";

    cout << "Opening the root file" << endl;
    TFile* f;
    f = new TFile(("../../LOCAL_COMBINED_"+prefix+"LumiScaled.root").c_str(), "READ");
    if(!f){
        cerr << "opening file: FAILED" << endl;
        throw runtime_error("missing input file");
    }

    THStack hs;

    cout << "Getting the histrogram" << endl;
    TH1D *h1, *h2;

    h1 = (TH1D*) f->Get("lightchargedhiggs_default_METEtHplusBWB_M80"); 
    if(!h1){
        cerr << "getting histogram: FAILED" << endl;
        throw runtime_error("missing histogram");
    }   

    h2 = (TH1D*) f->Get("lightchargedhiggs_default_METEtMC_tbarW"); 
    if(!h2){
        cerr << "getting histogram: FAILED" << endl;
        throw runtime_error("missing histogram");
    }   

    hs.Add(h1);
    hs.Add(h2);
    
    TCanvas* c = new TCanvas("c", "c", 1);
    c->cd();

    gPad->SetMargin(0.15, 0.1, 0.15, 0.1);

    hs.Draw("hist");

    c->SaveAs("./img/plot.eps");

    c->Close();
    f->Close();

}
// ----- drawPlots -----



int main(){

    // create a directory img where to save plots
    mkdir("img", 0755);
        

    // constants
    string prefix = "lightchargedhiggs_default_";
    
    
    // variables declaration
    allPlotAtt alldatasets;
    string titleX, titleY;

    varToPlot mylist;


    // objects to draw plots
    TCanvas *c = new TCanvas("c", "c", 1);//200, 10, 750, 750);
    myLegend leg;

    // Open the root file with histograms
    TFile *rootfile;
    rootfile = new TFile(("../../LOCAL_COMBINED_"+prefix+"LumiScaled.root").c_str(), "READ");
    if(!rootfile){
        cerr << "opening file: FAILED" << endl;
        throw runtime_error("missing input rootfile");
    }


    for(unsigned int i=0; i<mylist.size(); ++i){
        
        THStack hs_bkg;
        TH1D* h_data;
        TH1D* h_sig;

        string vartmp = mylist.at(i);

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
            } // if data
            
            else if(nametmp == "HplusBWB_M80"){
                cout << "set: "<< nametmp << " doing H+" <<endl;
                h_sig = (TH1D*) htmp->Clone();
                h_sig->SetLineColor(1);
                h_sig->SetFillColor(colortmp);
                if(i==0) leg.AddEntry(h_sig, (labeltmp).c_str(), "F");
            } // if signal

            else{
                cout << "set: "<< nametmp << " doing bkg" <<endl;
                htmp->SetLineColor(1);
                htmp->SetFillColor(colortmp);
                if(nametmp=="MC_WJ" || nametmp=="MC_WJ_tau") continue; 
                hs_bkg.Add(htmp);
                if(i==0) leg.AddEntry(htmp, (labeltmp).c_str(), "F");
            } // if bkg

 
        } // loop over all the datasets

        c->cd();

        c->SetLogy(0);
        
        gPad->SetMargin(0.15,0.1,0.15,0.1);
        gPad->SetTicks(1, 1);       
 
        double max = h_data->GetBinContent(h_data->GetMaximumBin());

        hs_bkg.SetMinimum(0);
        hs_bkg.SetMaximum(1.5*max);
        hs_bkg.Draw("hist");

        hs_bkg.GetXaxis()->SetTitle((titleX).c_str());
        hs_bkg.GetXaxis()->SetTitleOffset(1.2);
        hs_bkg.GetXaxis()->SetTitleSize(0.05);
        hs_bkg.GetXaxis()->SetLabelSize(0.05);
    
        hs_bkg.GetYaxis()->SetTitle((titleY).c_str());
        hs_bkg.GetYaxis()->SetTitleOffset(1.3);
        hs_bkg.GetYaxis()->SetTitleSize(0.05);
        hs_bkg.GetYaxis()->SetLabelSize(0.05);
        

        //h_sig->Draw("histsame");
        h_data->Draw("EPsame");
        h_data->Draw("axissame");
        leg.Draw("same");
        
        c->SaveAs(("./img/"+vartmp+".eps").c_str());


        c->SetLogy(1);
        
        hs_bkg.SetMinimum(0.01);
        hs_bkg.SetMaximum(100*max);

        c->SaveAs(("./img/"+vartmp+"log.eps").c_str());

        c->Clear();

    } // loop over mylist


}
