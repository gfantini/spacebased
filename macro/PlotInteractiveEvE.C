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

const double BaselineSlopeCut = 0.01;

void PlotInteractiveEvE(string filename = "PlotCoincidencesEnergyVsEnergy.root")
{
  string input = "/nfs/cuore1/scratch/gfantini/spacebased/out/";
  input += filename;
  TFile* pFinput = new TFile(input.c_str(),"UPDATE");
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
  //gROOT->cd(); // Wil. E. Coyote
  
  const int cNbin = 3000;
  const double cEmin = 0.;
  const double cEmax = 3000.;

  TGraph* gBase = new TGraph(); // plot TotalEnergy - Energy : Energy
  int gBaseCounter = 0;
  TGraph* g2615sum = new TGraph(); // same plot with TotalEnergy ~ 2615 selection
  int g2615sumCounter = 0;
  TGraph* g2615acc = new TGraph(); // same plot with Energy ~ 2615 selection
  int g2615accCounter = 0;
  TH1D* h2615sum = new TH1D("h2615sum","h2615sum",1200,0.,1200.);
  TH1D* h2615acc = new TH1D("h2615acc","h2615acc",1200,0.,1200.);

  TGraph* gBaseM2 = new TGraph(); // plot TotalEnergy - Energy : Energy          
  int gBaseCounterM2 = 0;
  TGraph* g2615sumM2 = new TGraph(); // same plot with TotalEnergy ~ 2615 selection      
  int g2615sumCounterM2 = 0;
  TGraph* g2615accM2 = new TGraph(); // same plot with Energy ~ 2615 selection  
  int g2615accCounterM2 = 0;
  TH1D* h2615sumM2 = new TH1D("h2615sumM2","h2615sumM2",1200,0.,1200.);
  TH1D* h2615accM2 = new TH1D("h2615accM2","h2615accM2",1200,0.,1200.);

  for(int i = 0; i < outTree->GetEntries(); i++){
    outTree->GetEntry(i);
    if(i%100000 == 1)cout << 100.*(double)i/(double)outTree->GetEntries() << " % complete." << endl;
    if(abs(BaselineSlope) < BaselineSlopeCut && Energy < 3e3 && TotalEnergy - Energy < 3e3){ // select base
      gBase->SetPoint(gBaseCounter,Energy,TotalEnergy - Energy);
      gBaseCounter++;
      if(Multiplicity == 2)gBaseM2->SetPoint(gBaseCounterM2,Energy,TotalEnergy - Energy);

      if(abs(TotalEnergy - 2615) < 10){// select 2615sum
	g2615sum->SetPoint(g2615sumCounter,Energy,TotalEnergy - Energy);
	g2615sumCounter++;
	h2615sum->Fill(Radius);
	if(Multiplicity == 2){
	  g2615sumM2->SetPoint(g2615sumCounterM2,Energy,TotalEnergy - Energy);
	  g2615sumCounterM2++;
	  h2615sumM2->Fill(Radius);
	}
	
      }
      if(abs(Energy - 2615) < 10){// select 2615acc               
	g2615acc->SetPoint(g2615accCounter,Energy,TotalEnergy - Energy);
	g2615accCounter++;
	h2615acc->Fill(Radius);
	if(Multiplicity == 2){
	  g2615accM2->SetPoint(g2615accCounterM2,Energy,TotalEnergy - Energy);
	  g2615accCounterM2++;
	  h2615accM2->Fill(Radius);
	}
      }
      
    }
  }

  cout <<   h2615sum->GetEntries() << endl;
  for(int i = 0; i < h2615sum->GetNbinsX(); i++)cout << i << "\t: " << h2615sum->GetBinContent(i) << endl;

  cout << "Appending output to " << input << endl;
  if(pFinput->IsZombie())cerr << "Aiuto, uno zombie" << endl;
  h2615sum->Write("hsumMX");
  h2615acc->Write("haccMX");
  gBase->Write("gBaseMX");
  g2615sum->Write("gsumMX");
  g2615acc->Write("gaccMX");

  h2615sumM2->Write("hsumM2");
  h2615accM2->Write("haccM2");
  gBaseM2->Write("gBaseM2");
  g2615sumM2->Write("gsumM2");
  g2615accM2->Write("gaccM2");
  //  pFinput->Write();
  pFinput->Close();
  
}


int main(int argc, char** argv)
{
  if(argc != 2){
    cerr << "Abort. Should call ./PlotInteractiveEvE <inputfile>" << endl;
    return 1;
  }
  PlotInteractiveEvE(argv[1]);
  return 0;
}
