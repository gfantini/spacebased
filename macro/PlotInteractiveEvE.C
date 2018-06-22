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

#define BIT( n)   (1ULL << (n))

using namespace std;

const double BaselineSlopeCut = 0.01;
const unsigned long long int kOverwrite = BIT(1);

#ifdef __CINT__
double ComputeDistance(int ch1,int ch2)
{
  gSystem->Load("libQAnalysisPlots"); // only INTERACTIVE                            
  if(ch1<0 || ch1>988)cerr << "Channel 1 not in range 0-988" << endl;
  if(ch2<0 || ch2>988)cerr << "Channel 2 not in range 0-988" << endl;
  double x1,y1,z1;
  double x2,y2,z2;
  QChannelUtils::ChannelPosition(ch1,x1,y1,z1);
  QChannelUtils::ChannelPosition(ch2,x2,y2,z2);
  return sqrt((x1-x2)**2+(y1-y2)**2+(z1-z2)**2);
}

void PlotRadiusMC(string file = "Sep2017Calibration_g4cuore.root",string outfile = "RadiusMC_CalibrationSet2017.root")
{
  string folder = "/nfs/cuore1/scratch/cjdavis/Calibration/Sep2017/";
  string outfolder = "/nfs/cuore1/scratch/gfantini/spacebased/out/";

  TFile* pFile = new TFile((folder+file).c_str(),"READ");
  TTree* outTree;
  pFile->GetObject("outTree",outTree);
  // read data                                        
  Double_t Ener2,ESum2;
  Int_t Detector;
  Short_t Multiplicity;
  Int_t Multiplet[988];
  outTree->SetBranchAddress("Detector",&Detector);
  outTree->SetBranchAddress("Ener2",&Ener2);
  outTree->SetBranchAddress("ESum2",&ESum2);
  outTree->SetBranchAddress("Multiplicity",&Multiplicity);
  outTree->SetBranchAddress("Multiplet",Multiplet);
  // output file                                      
  TFile* pFileOut = new TFile( (outfolder + outfile).c_str(), "RECREATE");
  // make histograms of radii                         
  TH1D* hRadiusS = new TH1D("hRadiusS","Radius distribution signal M2",1200,0.,1200.);
  TH1D* hRadiusB = new TH1D("hRadiusB","Radius distribution background M2",1200,0.,1200.);
  for(int i=0;i<outTree->GetEntries();i++){
    outTree->GetEntry(i);
    // select signal                                  
    if(Multiplicity == 2 && abs(ESum2-2615) < 10.)hRadiusS->Fill( ComputeDistance(Multiplet[0],Multiplet[1]) );
    // select background                              
    if(Multiplicity == 2 && ( abs(Ener2-2615) < 10. && ESum2 - Ener2 > 900.  ) || ( abs(ESum2-Ener2-2615) < 10. && Ener2 > 900. ) )hRadiusB->Fill( ComputeDistance(Multiplet[0],Multiplet[1]) );
  }
  // write results                                    
  hRadiusS->Write("hRadiusS",kOverwrite);
  hRadiusB->Write("hRadiusB",kOverwrite);
  cout << "Histograms of MC results written to: " << outfolder + outfile << endl;
  pFileOut->Close();
  delete pFileOut;
}


void PlotComparison()
{
  string folder = "/nfs/cuore1/scratch/gfantini/spacebased/out/";
  string datafile = "PlotCoincidencesEnergyVsEnergy[Coincidence1200_301530_C.list]_1200mm.root";
  string mcfile = "RadiusMC_CalibrationSet2017.root";

  TFile* pDATA = new TFile( (folder+datafile).c_str(),"READ");
  TFile* pMC = new TFile( (folder+mcfile).c_str(),"READ");

  TH1D* hDataS;
  TH1D* hDataB;
  TH1D* hMCS;
  TH1D* hMCB;
  pDATA->GetObject("hsumM2",hDataS);
  pDATA->GetObject("haccM2",hDataB);
  pMC->GetObject("hRadiusS",hMCS);
  pMC->GetObject("hRadiusB",hMCB);

  // binning 10 keV                                   
  hDataS->Rebin(10);
  hDataB->Rebin(10);
  hMCS->Rebin(10);
  hMCB->Rebin(10);
  // scale MC to DATA                                 
  hMCS->Scale( hDataS->GetEntries()/(double)hMCS->GetEntries() );
  hMCB->Scale( hDataB->GetEntries()/(double)hMCB->GetEntries() );

  TCanvas* c1 = new TCanvas();
  c1->Divide(1,2);
  // signal plots                                     
  c1->cd(1);
  hDataS->Draw();
  hMCS->SetLineColor(2);
  hMCS->Draw("SAME");
  // residuals
  c1->cd(2);
  TGraphErrors* ge = new TGraphErrors();
  int geN = 0;
  for(int i=0;i<hDataS->GetNbinsX();i++){
    if(hDataS->GetBinContent(i) > 1e-3 && hMCS->GetBinContent(i) > 1e-3){// fill only non-empty bins
      ge->SetPoint(geN, 5. + i*10., 0.);
      ge->SetPointError(geN, 0., sqrt( pow(hDataS->GetBinError(i),2) + pow( hMCS->GetBinError(i), 2) ) );
      geN++;
    }
    cout << "i \t Data[i] \t MC[i] \t" << i << "\t" << hDataS->GetBinContent(i) << "\t" << hMCS->GetBinContent(i) << endl;
  }
  TGraphErrors* ge2 = new TGraphErrors();
  for(int i=0; i<geN; i++){
    ge2->SetPoint(i,ge->GetX()[i],ge->GetY()[i]);
    ge2->SetPointError(i,ge->GetEX()[i],2.*ge->GetEY()[i]);
  }
  TGraph* gResidual = new TGraph();
  geN = 0;
  for(int i=0; i<hDataS->GetNbinsX(); i++){
    if(hDataS->GetBinContent(i)> 1e-3 && hMCS->GetBinContent(i) > 1e-3){// fill only non-empty bins
      gResidual->SetPoint(geN, 5. + 10.*i,  hDataS->GetBinContent(i) - hMCS->GetBinContent(i) );
      geN++;
    }
    cout << "[TGraph] i \t y(i) \t" << i << "\t" << hDataS->GetBinContent(i) - hMCS->GetBinContent(i) << endl;
  }
  // DRAW
  gResidual->SetMarkerStyle(20); // pallocchio
  gResidual->Draw("AP");
  ge2->SetFillColor(5); // yellow
  ge2->Draw("4same");
  ge->SetFillColor(8);
  ge->GetXaxis()->SetRangeUser(0.,1200.);
  ge->Draw("4same"); // smooth filled area (3 = non-smooth)
  gResidual->Draw("PSAME");
  c1->SaveAs( (folder+"PlotCompareDataMC_Signal.pdf").c_str() );
  
  // produce plot of residuals!
  TCanvas *cBackground = new TCanvas();
  cBackground->Divide(1,2);
  // background plots                                                                                                                                                                                   
  cBackground->cd(1);
  hMCB->SetLineColor(2); // 2 = red                                                                                                                                                                           
  hMCB->Draw();
  hDataB->Draw("SAME");
  // residuals
  cBackground->cd(2);
  TGraphErrors* geB = new TGraphErrors();
  geN=0;
  for(int i=0;i<hDataB->GetNbinsX();i++){
    if(hDataB->GetBinContent(i)> 1e-3 && hMCB->GetBinContent(i) > 1e-3){// fill only non-empty bins
      geB->SetPoint(geN, 5. +i*10., 0.);
      geB->SetPointError(geN, 0., sqrt( pow(hDataB->GetBinError(i),2) + pow( hMCB->GetBinError(i), 2) ) );
      geN++;
    }
  }
  TGraphErrors* geB2 = new TGraphErrors();
  for(int i=0; i<geN; i++){
    geB2->SetPoint(i,geB->GetX()[i],geB->GetY()[i]);
    geB2->SetPointError(i,geB->GetEX()[i],2.*geB->GetEY()[i]);
  }
  TGraph* gResidualB = new TGraph();
  geN=0;
  for(int i=0; i<hDataB->GetNbinsX(); i++){
    if(hDataB->GetBinContent(i)> 1e-3 && hMCB->GetBinContent(i) > 1e-3){// fill only non-empty bins
      gResidualB->SetPoint(geN, 5. + 10.*i,  hDataB->GetBinContent(i) - hMCB->GetBinContent(i) );
      geN++;
    }
  }
  //DRAW
  gResidualB->SetMarkerStyle(20); // pallocchio                                                                                                                                                              
  gResidualB->Draw("AP");

  geB2->SetFillColor(5);// yellow
  geB2->Draw("4same");
  geB->SetFillColor(8);
  geB->GetXaxis()->SetRangeUser(0.,1200.);
  geB->Draw("4same"); // smooth filled area (3 = non-smooth)
  gResidualB->Draw("PSAME");
  cBackground->SaveAs( (folder+"PlotCompareDataMC_Background.pdf").c_str() );
}
#endif

void PlotInteractiveEvE(string filename = "PlotCoincidencesEnergyVsEnergy.root")
{
  string input = "/nfs/cuore1/scratch/gfantini/spacebased/out/";
  input += filename;
  TFile* pFinput = new TFile(input.c_str(),"UPDATE");
  TTree* outTree;
  gDirectory->GetObject("outTree",outTree);
  outTree->Show(0);
  cout << "****************" << endl;
  
  
  TH2D* MCmatrixM2;
  //gDirectory->GetObject("MatrixAccidentalCorrection",MCmatrixM2);
  
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
      if( (abs(Energy - 2615) < 10 && TotalEnergy - Energy > 900.) || (abs(TotalEnergy - Energy - 2615) < 10. && Energy > 900.) ){// select 2615acc               
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
  h2615sum->Write("hsumMX",kOverwrite);
  h2615acc->Write("haccMX",kOverwrite);
  gBase->Write("gBaseMX",kOverwrite);
  g2615sum->Write("gsumMX",kOverwrite);
  g2615acc->Write("gaccMX",kOverwrite);

  h2615sumM2->Write("hsumM2",kOverwrite);
  h2615accM2->Write("haccM2",kOverwrite);
  gBaseM2->Write("gBaseM2",kOverwrite);
  g2615sumM2->Write("gsumM2",kOverwrite);
  g2615accM2->Write("gaccM2",kOverwrite);
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
