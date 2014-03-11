#include "TFile.h"
#include "TTree.h"
#include "TString.h"


void drawHistos(TCanvas * C, TString filename, TString category, TTree* Tmine, TTree* Tother,TString var, int nbins, float xmin, float xmax, TString selection, TString myGroup, TString myRootFile, TString group, TString groupRootFile,TString mySel="1",TString groupSel="1"){

//   std::cout<<Tmine->GetName()<<" "<<myGroup<<" "<<myRootFile<<" "<<mySel<<" "<<std::endl;
//   std::cout<<Tother->GetName()<<" "<<group<<" "<<groupRootFile<<" "<<groupSel<<" "<<std::endl;
//   std::cout<<var<<" "<<nbins<<" "<<xmin<<" "<<xmax<<" "<<selection<<std::endl;
  

  TH1F* Hmine = new TH1F(TString("Hmine")+var,"",nbins,xmin,xmax); 
  Hmine->GetYaxis()->SetTitle(category);
  Hmine->GetXaxis()->SetTitle(var);
  Hmine->SetLineColor(1);
  Hmine->SetMarkerColor(1);
  Hmine->SetStats(0);
  TH1F* Hother = new TH1F(TString("Hother")+var,"",nbins,xmin,xmax); 
  Hother->GetYaxis()->SetTitle(category);
  Hother->GetXaxis()->SetTitle(var);
  Hother->SetLineColor(2);
  Hother->SetMarkerColor(2);
  Hother->SetStats(0);

  TText TXmine;
  TXmine.SetTextColor(1);
  TXmine.SetTextSize(.04);
  TText TXother;
  TXother.SetTextColor(2);
  TXother.SetTextSize(.04);


  Tmine->Draw(var+">>"+Hmine->GetName(),selection+"*("+mySel+")");
  Tother->Draw(var+">>"+Hother->GetName(),selection+"*("+groupSel+")");

 
  TPad pad1("pad1","",0,0.2,1,1);
  TPad pad2("pad2","",0,0,1,0.2);
    
  ////////////////////////////////////////////
  pad1.cd();

  ////Draw one histogram on top of the other
  if(Hmine->GetMaximum()>Hother->GetMaximum())
    Hmine->GetYaxis()->SetRangeUser(0,Hmine->GetMaximum()*1.1);
  else Hmine->GetYaxis()->SetRangeUser(0,Hother->GetMaximum()*1.1);
  Hmine->SetTitle(selection);
  Hmine->Draw("hist");
  Hother->Draw("histsame");

  //Print the integrals of the histograms a the top
  //TXmine.DrawTextNDC(.2,.965,myGroup+"_"+myRootFile+": "+(long)(Hmine->Integral(0,Hmine->GetNbinsX()+1)));
  //TXother.DrawTextNDC(.2,.93,group+"_"+groupRootFile+": "+(long)(Hother->Integral(0,Hother->GetNbinsX()+1)));
  TXmine.DrawTextNDC(.23,.84,myGroup+" : "+(long)(Hmine->Integral(0,Hmine->GetNbinsX()+1)));
  TXother.DrawTextNDC(.53,.84,group+": "+(long)(Hother->Integral(0,Hother->GetNbinsX()+1)));

  ////////////////////////////////////////////
  pad2.cd();

//   ///Draw the difference of the historgrams
//   TH1F*HDiff=(TH1F*)Hmine->Clone("HDiff");
//   HDiff->Add(Hother,-1);
//   int max= abs(HDiff->GetMaximum())>abs( HDiff->GetMinimum()) ?   abs(HDiff->GetMaximum()): abs( HDiff->GetMinimum());
//   HDiff->GetYaxis()->SetRangeUser(-2*(max>0?max:1),2*(max>0?max:1));
//   HDiff->Draw("hist");
//   TLine line;
//   line.DrawLine(HDiff->GetXaxis()->GetXmin(),0,HDiff->GetXaxis()->GetXmax(),0);

  ///Draw the ratio of the historgrams
  TH1F*HDiff=(TH1F*)Hother->Clone("HDiff");
  HDiff->Divide(Hmine);
  ///HDiff->GetYaxis()->SetRangeUser(0.9,1.1);
  HDiff->GetYaxis()->SetRangeUser(0.9,1.1);
  //HDiff->GetYaxis()->SetRangeUser(0.98,1.02);
  //HDiff->GetYaxis()->SetRangeUser(0.,2.0);
  HDiff->GetYaxis()->SetNdivisions(3);
  HDiff->GetYaxis()->SetLabelSize(0.1);
  HDiff->GetYaxis()->SetTitleSize(0.1);
  HDiff->GetYaxis()->SetTitleOffset(0.5);
  //HDiff->GetYaxis()->SetTitle(myGroup + " / " + group);
  HDiff->GetYaxis()->SetTitle("Ratio");
  HDiff->GetXaxis()->SetNdivisions(-1);
  HDiff->GetXaxis()->SetTitle("");
  HDiff->GetXaxis()->SetLabelSize(0.0001);
  HDiff->SetMarkerColor(2);
  HDiff->Draw("histp");
  TLine line;
  line.DrawLine(HDiff->GetXaxis()->GetXmin(),1,HDiff->GetXaxis()->GetXmax(),1);


  C->Clear();
  pad1.Draw();
  pad2.Draw();

  C->Print(filename);

  delete Hmine;
  delete Hother;
  delete HDiff;
}

void compareSync(TString channel,TString myGroup,TString myPath,TString myRootFile, TString myTree, TString group, TString groupPath, TString groupRootFile, TString groupTree,TString mySel="",TString groupSel=""){

  std::cout<<channel<<std::endl;
  std::cout<<myGroup<<"  "<<myPath<<"  "<<myRootFile<<"  "<<myTree<<"  "<<mySel<<std::endl;
  std::cout<<group<<"  "<<groupPath<<"  "<<groupRootFile<<"  "<<groupTree<<" "<<groupSel<<std::endl;
  
  if(mySel.CompareTo("")==0)mySel="1";
  if(groupSel.CompareTo("")==0)groupSel="1";


  TFile Fmine(myPath+"/"+myRootFile+".root");
  TTree*Tmine=(TTree*)Fmine.Get(myTree.Data());
  TFile Fother(groupPath+"/"+groupRootFile+".root");
  TTree*Tother=(TTree*)Fother.Get(groupTree.Data());
  if(!Tmine){std::cout<<" File "<<Fmine.GetName()<<" is empty "<<std::endl; gROOT->ProcessLine(".q");}
  if(!Tother){std::cout<<" File "<<Fother.GetName()<<" is empty "<<std::endl; gROOT->ProcessLine(".q");}

  
  ////////////////

  std::cout<<"Mine: "<<Fmine.GetName()<<std::endl;
  std::cout<<"Other: "<<Fother.GetName()<<std::endl;
  TCanvas C;

  //TString filename=TString("PlotsDiff_")+myGroup+"_"+myRootFile+"_"+group+"_"+groupRootFile+".pdf";
  TString filename=TString("PlotsDiff_")+channel+"_"+myGroup+"_"+group+".pdf";
  C.Print(filename+"[");  


  //inclusive
  TString selection="1";
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"run",200,190000,210000,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"lumi",200,0,2000,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"evt",200,0,3e9,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);


  drawHistos(&C,filename,"inclusive",Tmine,Tother,"npu",50,0,50,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"npv",50,0,50,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"rho",40,0,40,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);

  drawHistos(&C,filename,"inclusive",Tmine,Tother,"puweight",25,-.1,0.9,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);

  drawHistos(&C,filename,"inclusive",Tmine,Tother,"mvis",50,0,200,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);

  drawHistos(&C,filename,"inclusive",Tmine,Tother,"pt_1",100,0,100,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"phi_1",60,-3.2,3.2,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"eta_1",60,-3,3,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"m_1",100,-0.1,0.2,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"q_1",3,-1.5,1.5,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"iso_1",110,0.,0.11,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"d0_1",80,-0.04,0.04,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"dZ_1",60,-0.15,0.15,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"mt_1",100,0.,200.,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);

  drawHistos(&C,filename,"inclusive",Tmine,Tother,"pt_2",100,0,100,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"phi_2",60,-3.2,3.2,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"eta_2",60,-3,3,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"m_2",100,-0.5,3.5,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"q_2",3,-1.5,1.5,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"d0_2",80,-0.04,0.04,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"dZ_2",60,-0.15,0.15,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"byCombinedIsolationDeltaBetaCorrRaw3Hits_2",100,0.,1.6,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"againstMuonLoose2_2",2,-0.5,1.5,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"againstMuonMedium2_2",2,-0.5,1.5,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"againstMuonTight2_2",2,-0.5,1.5,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"mt_2",100,0.,200.,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);

  drawHistos(&C,filename,"inclusive",Tmine,Tother,"met",20,0,200,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"metphi",35,-3.5,3.5,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"metcov00",40,0,1000,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"metcov01",40,0,1000,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"metcov11",40,0,1000,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"mvamet",20,0,200,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"mvametphi",35,-3.5,3.5,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"mvacov00",40,0,1000,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"mvacov01",40,0,1000,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"mvacov11",40,0,1000,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);

  drawHistos(&C,filename,"inclusive",Tmine,Tother,"jpt_1",50,0,300,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"jeta_1",50,-5,5,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"jphi_1",60,-3.2,3.2,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"jmva_1",200,-1.1,1.1,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"jpass_1",2,-.5,1.5,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);

  drawHistos(&C,filename,"inclusive",Tmine,Tother,"jpt_2",50,0,300,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"jeta_2",50,-5,5,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"jphi_2",60,-3.2,3.2,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"jmva_2",200,-1.1,1.1,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"jpass_2",2,-.5,1.5,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);

  drawHistos(&C,filename,"inclusive",Tmine,Tother,"bpt",200,0.,2.0,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"beta",50,-5,5,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"bphi",60,-3.2,3.2,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);

  drawHistos(&C,filename,"inclusive",Tmine,Tother,"mjj",100,0.,1000.0,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"jdeta",50,0.,10.,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"njetingap",5,0.5,4.5,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);

  drawHistos(&C,filename,"inclusive",Tmine,Tother,"nbtag",6,0.5,5.5,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"njets",11,0.5,10.5,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);

  C.Print(filename+"]");
  gROOT->ProcessLine(".q");
}