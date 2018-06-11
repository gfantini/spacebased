#include "TROOT.h"
#include "TRint.h"

#include <TFile.h>
#include <string>
#include <TGraph.h>
#include <TH1.h>
#include <TH2.h>
#include <TTree.h>
#include <iostream>
#include <TCanvas.h>

using namespace std;

void PlotInteractiveEvE()
{
  string input = "/nfs/cuore1/scratch/gfantini/spacebased/out/PlotCoincidencesEnergyVsEnergy.root";
  TFile* pFinput = new TFile(input.c_str(),"READ");
  TTree* outTree;
  gDirectory->GetObject("outTree",outTree);
  outTree->Show(0);
  cout << "****************" << endl;
  /*
  TCut cutBase = "abs(BaselineSlope) < 0.01 && Energy < 3e3 && TotalEnergy - Energy < 3e3";
  TCut cut2615sum = "abs(TotalEnergy - 2615) < 10";
  TCut cut2615acc = "abs(Energy - 2615) < 10";
  TCut cutM2 = "Multiplicity == 2";
  */
  Double_t TotalEnergy,Energy,Radius,BaselineSlope;
  Int_t Multiplicity;
  outTree->SetBranchAddress("TotalEnergy",&TotalEnergy);
  outTree->SetBranchAddress("Energy",&Energy);
  outTree->SetBranchAddress("BaselineSlope",&BaselineSlope);
  outTree->SetBranchAddress("Radius",&Radius);
  outTree->SetBranchAddress("Multiplicity",&Multiplicity);
  gROOT->cd(); // Wil. E. Coyote
  
  const int cNbin = 3000;
  const double cEmin = 0.;
  const double cEmax = 3000.;

  TGraph* gBase = new TGraph();
  int gBaseCounter = 0;
  TGraph* g2615sum = new TGraph();
  int g2615sumCounter = 0;
  TGraph* g2615acc = new TGraph();
  int g2615accCounter = 0;

  TH1D* h2615sum = new TH1D("h2615sum","h2615sum",1200,0.,1200.);
  TH1D* h2615acc = new TH1D("h2615acc","h2615acc",1200,0.,1200.);
  //  TH2D* hBase = new TH2D("hBase","hBase",cNbin,cEmin,cEmax,cNbin,cEmin,cEmax);
  //  outTree->Draw(Form("TotalEnergy - Energy : Energy >>hBase(%d,%lf,%lf,%d,%lf,%lf)",cNbin,cEmin,cEmax,cNbin,cEmin,cEmax),cutBase);
  for(int i = 0; i < outTree->GetEntries(); i++){
    outTree->GetEntry(i);
    if(i%100000 == 1)cout << 100.*(double)i/(double)outTree->GetEntries() << " % complete." << endl;
    if(abs(BaselineSlope) < 0.01 && Energy < 3e3 && TotalEnergy - Energy < 3e3){ // select base
      gBase->SetPoint(gBaseCounter,Energy,TotalEnergy - Energy);
      gBaseCounter++;
      if(abs(TotalEnergy - 2615) < 10){// select 2615sum
	g2615sum->SetPoint(g2615sumCounter,Energy,TotalEnergy - Energy);
	g2615sumCounter++;
	h2615sum->Fill(Radius);
      }
      if(abs(Energy - 2615) < 10){// select 2615acc                                                                                                                                       
	g2615acc->SetPoint(g2615accCounter,Energy,TotalEnergy - Energy);
	g2615accCounter++;
	h2615acc->Fill(Radius);
      }
      
    }
  }

  cout <<   h2615sum->GetEntries() << endl;
  for(int i = 0; i < h2615sum->GetNbinsX(); i++)cout << i << "\t: " << h2615sum->GetBinContent(i) << endl;
  

  //TCanvas* c1 = new TCanvas();
  //  c1->Divide(2,1);
  //  c1->cd(1);
  //  gBase->Draw("AP");
  //g2615sum->SetMarkerColor(kRed);
  //g2615sum->Draw("PSAME");
  //g2615acc->SetMarkerColor(kBlue);
  //g2615acc->Draw("PSAME");
  //  c1->cd(2);
  //TCanvas* c2 = new TCanvas();
  //c2->cd();
  //h2615sum->SetLineColor(kRed);
  //h2615sum->Draw();

  //  h2615acc->SetLineColor(kBlue);
  //  h2615acc->Draw("SAME");

//  delete pFinput;
//  cout << "Producing output on /nfs/cuore1/scratch/gfantini/spacebased/out/PlotInteractiveEvE.root" << endl;
//  TFile* pFout = new TFile("/nfs/cuore1/scratch/gfantini/spacebased/out/PlotInteractiveEvE.root","RECREATE");
	TFile* pFout = pFinput;
	cout << "Appending output to " << input << endl;
  if(pFout->IsZombie())cerr << "Aiuto, uno zombie" << endl;

  h2615sum->Write("hsum");
  //  h2615acc->Write("hacc");
  pFout->Write();
  pFout->Close();
  
}


int main()
{

  PlotInteractiveEvE();
  return 0;
}
