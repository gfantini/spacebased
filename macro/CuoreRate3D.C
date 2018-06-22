/*
 *  Author: G. Fantini
 *  Mailto: guido.fantini@gssi.it
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 *  Compilation:        g++ -std=c++0x `root-config --cflags --glibs` -lRooFit `diana-config --cflags --libs`'
 *  Usage instructions: compile it in order to process an ascii file <ch> <counts> <rate> and get a rootfile with a TH3D map
 *                      of the CUORE detector
 *                      Once you have the .root file, run interactively
 *                      bash$ root
 *                      root [0] .L CuoreRate3D.C
 *                      root [1] Draw( <path_to_your_rootfile> );
 *
 *  WARNING:            In order to run properly it needs libGL (some graphic libraries)
 *                      In order to check if they are installed try:
 *                      $ glxgears
 *                      If a window with some coloured rotating gears appears, libGL is installed.
 *                      Else -> check with your system administrator.
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */

#include <fstream>
#include <iostream>
#include "TH3D.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TF1.h"

using namespace std;

int CuoreRate3D(string inputFileASCII = "ChannelRate.txt",
		string outputFileRoot = "CuoreRate3D.root",
		bool display = true,
		string inputFileChannelPosition = "/nfs/cuore1/scratch/gfantini/spacebased/input/ChannelPosition.dat")
{
  // some hello message 
  cout << "Hello. Welcome to the CuoreRate3D." << endl;
  cout << "If this macro doesn't work, mailto: guido.fantini@gssi.it" << endl;
  cout << "inputFileASCII \t = " << inputFileASCII << endl;
  cout << "outputFileRoot \t = " << outputFileRoot << endl;
  cout << "display \t\t = " << display << endl;
  cout << "inputFileChannelPosition \t\t = " << inputFileChannelPosition << endl;
  cout << "* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *" << endl;
  
  // ascii file with channel positions
  ifstream inputChannelPosition;
  inputChannelPosition.open(inputFileChannelPosition.c_str(), std::ifstream::in );
  if(!inputChannelPosition.is_open()){
    cerr << "FATAL! could not open " << inputChannelPosition << endl;
    return 2;
  }
  double ChannelX[1000];
  double ChannelY[1000];
  double ChannelZ[1000];
  int ch;
  double x,y,z;
  cout << "Start loading channel position" << endl;
  while(inputChannelPosition.good() ){
    inputChannelPosition >> ch >> x >> y >> z;
    if(ch < 1 || ch > 988){// check
      cerr << "FATAL. Found channel " << ch << endl;
      cerr << "Check file " << inputFileChannelPosition << endl;
      return 2;
    }
    ChannelX[ch] = x;
    ChannelY[ch] = y;
    ChannelZ[ch] = z;
    cout << "Ch " << ch << " loaded." << endl;
  }
  cout << "Done!" << endl;

  // read from ascii the rates
  ifstream inputFile;
  inputFile.open(inputFileASCII.c_str() , std::ifstream::in );
  if(!inputFile.is_open()){
    cerr << "FATAL! could not open " << inputFileASCII << endl;
    return 2;
  }
  double ChannelRate[1000];
  double ChannelCounts[1000];
  int counts;
  double rate;
  cout << "Start reading rates." << endl;
  while(inputFile.good() ){
    inputFile >> ch >> counts >> rate;
    if(ch < 1 || ch > 988){ // check 
      cerr << "FATAL. Found channel " << ch << endl;
      cerr << "Check file " << inputFileChannelPosition<< endl;
      return 2;
    }
    if(counts < 0){
      cerr << "FATAL. Negative counts found." << endl;
      return 2;
    }
    if(rate < 0.){
      cerr << "FATAL. Negative rate found." << endl;
      return 2;
    }
    ChannelCounts[ch]=(double)counts;
    ChannelRate[ch]=rate;
    cout << "Loaded rate for channel " << ch << endl;
  }
  cout << "Done" << endl;
  
  // make the nice 3D plot and save it to .root file
  TFile* pOutFile = new TFile(outputFileRoot.c_str(),"RECREATE");
  cerr << "Opened root file for output: \t " <<outputFileRoot << endl;
  if(!pOutFile->IsZombie())cerr << "Opened correctly."<< endl;
  TH3D* CUORE = new TH3D("CUORE","CUORE detector rates",50,-500.,500.,50,-500.,500.,50,-500.,500.);
  cerr << "Created TH3D." << endl;

  // loop over the map and load the rates
  for(int i=1;i<=CUORE->GetXaxis()->GetNbins();i++)
    for(int j=1;j<=CUORE->GetYaxis()->GetNbins();j++)
      for(int k=1;k<=CUORE->GetZaxis()->GetNbins();k++)
	CUORE->SetBinContent(i,j,k,0.);
  int binX,binY,binZ;
  for(int i=1;i<=988;i++){
    binX = CUORE->GetXaxis()->FindBin( ChannelX[i] );
    binY = CUORE->GetYaxis()->FindBin( ChannelY[i] );
    binZ = CUORE->GetZaxis()->FindBin( ChannelZ[i] );
    //    CUORE->SetBinContent(binX,binY,binZ,ChannelRate[i]);
    CUORE->SetBinContent(binX,binY,binZ,ChannelCounts[i]);
  }


  // draw
  if(display){
    cerr << "Display is true. Trying to plot the TH3D" << endl;
    TCanvas* c1 = new TCanvas();
    c1->cd();
    gStyle->SetCanvasPreferGL(1);
    gStyle->SetPalette(1);
    CUORE->Draw("glcol");
  }
  // save to file
  cerr << "Writing the TH3D to file as CUORE." << endl;
  CUORE->Write("CUORE");
  cerr << "Done. Cleaning up . . ." << endl;
  pOutFile->Close();
  //  delete pOutFile;
  //  delete CUORE;

  cerr << "Done. Goodbye!" << endl;
  return 0;
}


Double_t transfer_function(Double_t *x, Double_t * /*param*/)
{
  //This function is required to suppress
  //boxes for empty bins - make them transparent.
  if (x)
    return *x > 0 ? 1. : 0.;
   
  return 0.;
}

void Draw(string inputFile)
{
  TFile* pf = new TFile(inputFile.c_str(),"READ");
  TH3D* h = (TH3D*)pf->Get("CUORE");
  // BEGIN magic
  // use this trick (I don't really understand how it works but it does...) to not display empty bins
  TList * const lof = h->GetListOfFunctions();
  lof->Add(new TF1("TransferFunction", transfer_function, 0., 1000., 0));
  // END magic

  // add labels on axis
  h->GetXaxis()->SetTitle("x [mm]");
  h->GetXaxis()->SetTitleOffset(2);
  h->GetYaxis()->SetTitle("y [mm]");
  h->GetYaxis()->SetTitleOffset(2);
  h->GetZaxis()->SetTitle("z [mm]");
  h->GetZaxis()->SetTitleOffset(2);

  // find some way to add text . . .

  gStyle->SetCanvasPreferGL(kTRUE);
  h->Draw("glcolz");
}


#ifndef __CINT__

int main(int argc, char* argv[]){
  CuoreRate3D("/nfs/cuore1/scratch/sachi/data/sr3/plots/ds3051_cali_302132/chan_2615Rate_caliDS3051.txt","../out/CuoreRate3D.root",false);
  return 0;
}
#endif
