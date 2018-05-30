#include "Riostream.h"
#include <vector>
#include "TROOT.h"
#include "TStyle.h"
#include "TStopwatch.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TFile.h"
#include "TTree.h"
#include "TPad.h"
#include "TCut.h"
#include "TCanvas.h"
#include "TString.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TDatime.h"
#include "TProfile.h"
#include "TObject.h"
#include <QChain.hh>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <map>
#include <time.h>

//#include <QBaseType.hh>
#include "QGFileHandler.hh"
#include <QChain.hh>
#include <QTree.hh>
//#include <QInterval.hh>
#include <QRunDataHandle.hh>
#include <QCoincidenceData.hh>
#include <QSampleInfo.hh>
#include "QCountPulsesData.hh"
#include <QPulseInfo.hh>
#include <QHeader.hh>
/*#include <QEvent.hh>
#include <QTime.hh>
#include <QGlobalDataManager.hh>
#include <QBadChannelsHandle.hh>
#include <QEventLabel.hh>
#include <QModule.hh>
#include "QCuore.hh"
#include "QError.hh" */
#include <QDetTrgParamsDerivative.hh>
#include <QBaselineData.hh>
#include "QAnalysisPlotsChannelUtils.hh" // this is for neighbor channels


#include <TFitResultPtr.h>
#include <TFitResult.h>
#include <THStack.h>

using namespace Cuore;
using namespace QChannelUtils; // Jeremy -> neighbor channels
using namespace std;



/*  Load reduced ntuples
 *  Produce tree of M2-sum spectra with different selections (coincid. algo)
 * 
 *
 */

#include <iostream>
#include <fstream>
#include <string>

double ComputeDistance(int ch1,int ch2,map<int,double> x, map<int,double> y, map<int,double> z)
{
  //gSystem->Load("libQAnalysisPlots"); // only INTERACTIVE                                                                                                        
  double x1,y1,z1;
  double x2,y2,z2;
  if(ch1<0 || ch1>988)cerr << "Channel 1 not in range 0-988" << endl;
  if(ch2<0 || ch2>988)cerr << "Channel 2 not in range 0-988" << endl;
  x1 = x[ch1];
  y1 = y[ch1];
  z1 = z[ch1];
  x2 = x[ch2];
  y2 = y[ch2];
  z2 = z[ch2];
  return sqrt(pow(x1-x2,2)+pow(y1-y2,2)+pow(z1-z2,2));
}

void DumpCoincidencesM2(string outputFile = "DumpCoincidencesM2bug.root")
{
  // load the channel positions
  string pathFile = "/nfs/cuore1/scratch/gfantini/cuoresw/pkg/modtiming/ChannelPosition.dat";
  ifstream F;
  F.open(pathFile.c_str());
  int ch;
  map<int,double> x,y,z;
  while(F.good())
    {
      F >> ch >> x[ch] >> y[ch] >> z[ch];
    }

  // Blinded_301530_C.list  Coincidence1200_301530_C.list  CoincidenceOfficial_301530_C.list  CoincidenceTest100_301530_C.list  CoincidenceTest200_301530_C.list
  string inputList = "CoincidenceTest200_301530_C.list";
  string inputFolder = "/nfs/cuore1/scratch/gfantini/mydiana/output/ds3021/";
  string outputFolder = "/nfs/cuore1/scratch/gfantini/spacebased/out/";

  // use only in interactive
  //gSystem->Load("libQAnalysisPlots");
  
  QChain* pChain = new QChain();
  pChain->Add( (inputFolder + inputList).c_str() );
  cout << "Reading from " << (inputFolder + inputList).c_str() << endl;
  cout << "Will output on " << (outputFolder+outputFile).c_str() << endl;
  // make this sw faster

  pChain->SetBranchStatus("*",0);
  pChain->SetBranchStatus("DAQ@PulseInfo.*",1);
  pChain->SetBranchStatus("RejectBadIntervals_AntiCoincidence_Tower@Passed.*",1);
  pChain->SetBranchStatus("BadForAnalysis_Coincidence_Tower@Passed.*",1);
  pChain->SetBranchStatus("SampleInfoFilter@Passed.*",1);
  pChain->SetBranchStatus("FilterInInterval_Coincidence_Tower@Passed.*",1);
  pChain->SetBranchStatus("EnergySelector_QNDBD@Energy.*",1);
  pChain->SetBranchStatus("Coincidence_OFTime_200mm@CoincidenceData.*",1);
  pChain->SetBranchStatus("Coincidence_OFTime_200mm@TotalEnergy.*",1);


  Int_t debug;
  // Variables to perform cuts
  Bool_t IsSignal = 0;
  QPulseInfo* pPulseInfo = 0;
  debug = pChain->SetBranchAddress("DAQ@PulseInfo.",&pPulseInfo); // i can get channel and IsSignal from here 
  QBool* RejectBadIntervals = 0;
  debug = pChain->SetBranchAddress("RejectBadIntervals_AntiCoincidence_Tower@Passed.",&RejectBadIntervals);
  QBool* BadForAnalysis = 0;
  debug = pChain->SetBranchAddress("BadForAnalysis_Coincidence_Tower@Passed.",&BadForAnalysis);
  QBool* SampleInfoFilter = 0;
  debug = pChain->SetBranchAddress("SampleInfoFilter@Passed.",&SampleInfoFilter);
  QBool* FilterInInterval_Coincidence_Tower = 0;
  debug = pChain->SetBranchAddress("FilterInInterval_Coincidence_Tower@Passed.",&FilterInInterval_Coincidence_Tower);

  // Variables "useful"
  QDouble* Energy;
  debug = pChain->SetBranchAddress("EnergySelector_QNDBD@Energy.",&Energy);
  QCoincidenceData* pCoincidenceData = 0;
  debug = pChain->SetBranchAddress("Coincidence_OFTime_200mm@CoincidenceData.",&pCoincidenceData);
  QDouble* TotalEnergy = 0;
  debug = pChain->SetBranchAddress("Coincidence_OFTime_200mm@TotalEnergy.",&TotalEnergy);

  int nTotEvents = pChain->GetEntries();
  cout << "Chain has " << nTotEvents << " entries." << endl;
  
  
  int nInvalid = 0;
  int Ch1,Ch2;
  Double_t E1,E2,R;
  QCoincidentChannel CC;

  // define TTree to store output quantities
  TTree * OutTree = new TTree("OutTree","The output Tree");
  OutTree->Branch("Ch1",&Ch1,"Ch1/I");
  OutTree->Branch("Ch2",&Ch2,"Ch2/I");
  OutTree->Branch("E1",&E1,"E1/D");
  OutTree->Branch("E2",&E2,"E2/D");
  OutTree->Branch("R",&R,"R/D");
  
  //  loop over entries in the QChain
  int ee;
  for(ee=0; ee < nTotEvents; ee++){
    pChain->GetEntry(ee);    
    if(ee%10000 == 0)cout << "Event "<< ee << " / " << nTotEvents << "("<< 100*ee/(double)nTotEvents << "%). Invalid: " << nInvalid << endl;
    
    // skip invalid
    if( !pCoincidenceData->IsValid() ){
      nInvalid++;
      continue;
    }
    
    // read other variables
    IsSignal = pPulseInfo->GetIsSignal();
    Ch1 = pPulseInfo->GetChannelId();
    
    // selection == JitterByCoincidence CFG 
    if( IsSignal && *RejectBadIntervals == true && *BadForAnalysis == true && *SampleInfoFilter == true && *FilterInInterval_Coincidence_Tower == true )
      { 
	
	if( pCoincidenceData->fMultiplicity == 2 ){
	  // read coincident channel ID
	  CC = pCoincidenceData->GetCoincidentChannel(0);
	  Ch2 = CC.fChannelId; 
	  if(Ch1 == Ch2){cerr << "Error in coincident channel extraction." << endl; exit(0);}
	  E1 = *Energy;
	  E2 = *TotalEnergy - *Energy;
	  R = ComputeDistance(Ch1,Ch2,x,y,z);
	  OutTree->Fill();
	}
	
      }
  }
  cout << "Created " << (outputFolder+outputFile).c_str() << " as output file." << endl;
  TFile* pFile = new TFile( (outputFolder+outputFile).c_str() ,"RECREATE");
  OutTree->Write();
  pFile->Close();
  
}


#ifndef __CINT__
int main(int argc, char **argv) {
  //  char* workingDir = getenv("CUORE_EXT_INSTALL");                                                                                                                                                   
  if(argc!=1)// # of arguments +1                                                                                                                                                        
    {
      cout<<"Run as ./"<<argv[0]<<endl;//" <inputfolder> "<<endl;                                                                                                
      //      cout<<"<inputfolder> = folder that contains root files named spectrum_dsXXXX_ch###.root" << endl;                                                                       
      return 1;
    }

  DumpCoincidencesM2();
  return 0;
}
#endif /* __CINT __ */
