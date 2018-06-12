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
#include <QObject.hh>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <map>
#include <time.h>

#include <QBaseType.hh>
#include "QGFileHandler.hh"
#include <QChain.hh>
#include <QTree.hh>
#include <QInterval.hh>
#include <QRunDataHandle.hh>
#include <QCoincidenceData.hh>
#include <QSampleInfo.hh>
#include "QCountPulsesData.hh"
#include <QPulseInfo.hh>
#include <QHeader.hh>
#include <QEvent.hh>
#include <QTime.hh>
#include <QGlobalDataManager.hh>
#include <QBadChannelsHandle.hh>
#include <QEventLabel.hh>
#include <QModule.hh>
#include "QCuore.hh"
#include "QError.hh"
#include <QDetTrgParamsDerivative.hh>
#include <QBaselineData.hh>
#include "QAnalysisPlotsChannelUtils.hh" // this is for neighbor channels


#include <TFitResultPtr.h>
#include <TFitResult.h>
#include <THStack.h>

using namespace std;
using namespace Cuore;
using namespace QChannelUtils; // Jeremy -> neighbor channels

/*  
 *  
 * 
 *
 */


// include files below are needed only if the macro is being compiled with ACLiC 
#if !defined(__CINT__) || defined(__MAKECINT__)
#include "TCanvas.h"
#include "TGraph.h"
#include "TFile.h"
#include "TAxis.h"
// files below can be included only if diana-root.C has been executed,
// which sets the CUORE software include paths.
#include "QChain.hh"
#include "QHeader.hh"
#include "QPulse.hh"
#include "QPulseInfo.hh"
#include "QBaseType.hh"
#include "QRunDataHandle.hh"
#include "QGlobalDataManager.hh"
#include "QBaselineData.hh"
#include "QCountPulsesData.hh"
#include "QBool.hh"
#include <QCoincidenceData.hh>
#endif
// Avoid putting Cuore:: in front of objects defined in the Cuore namespace
using namespace Cuore;
using namespace std;
string Gpath = "/nfs/cuore1/scratch/gfantini/mydiana/output/ds3021/";

//void NonInteractivePlot(string inFileName = "Coincidence1200_301530_C.list",string outFileExtraLabel = "")
void NonInteractivePlot(string inFileName = "CoincidenceTest200_301530_C.list",string ExtraLabel = "200mm")
{
  string path = Gpath;
  path+=inFileName;
  QChain* ch = new QChain();
  ch->Add(path.c_str());
  // Coincidence_OFTime_Sync_20ms_150keV_1200mm@ == coincidence label (SEARCH && REPLACE)
  ch->SetBranchStatus("*",0);
  ch->SetBranchStatus("DAQ@PulseInfo.*",1);
  ch->SetBranchStatus("RejectBadIntervals_AntiCoincidence_Tower@Passed.*",1);
  ch->SetBranchStatus("SampleInfoFilter@Passed.*",1);
  ch->SetBranchStatus("BadForAnalysis_Coincidence_Sync_GF@Passed.*",1);
  ch->SetBranchStatus("FilterInInterval_Coincidence_Tower@Passed.*",1);
  ch->SetBranchStatus( Form("Coincidence_OFTime_Sync_20ms_150keV_%s@TotalEnergy.*",ExtraLabel.c_str()) ,1);
  ch->SetBranchStatus("EnergySelector_QNDBD@Energy.*",1);
  ch->SetBranchStatus(Form("Coincidence_OFTime_Sync_20ms_150keV_%s@CoincidenceData.*",ExtraLabel.c_str()) ,1);
  ch->SetBranchStatus(Form("Coincidence_OFTime_Sync_20ms_150keV_%s@Radius.*",ExtraLabel.c_str()) ,1);
  ch->SetBranchStatus("BCountPulses@CountPulsesData.*",1);
  ch->SetBranchStatus("BaselineModule@BaselineData.*",1);
  // define things
  QPulseInfo* pPulseInfo= 0;
  ch->SetBranchAddress("DAQ@PulseInfo.",&pPulseInfo);
  QBool* pRejectBadIntervals= 0;
  ch->SetBranchAddress("RejectBadIntervals_AntiCoincidence_Tower@Passed.",&pRejectBadIntervals);
  QBool* pSampleInfoFilter= 0; // This is SingleTrigger
  ch->SetBranchAddress("SampleInfoFilter@Passed.",&pSampleInfoFilter);
  QBool* pBadForAnalysis= 0;
  ch->SetBranchAddress("BadForAnalysis_Coincidence_Sync_GF@Passed.",&pBadForAnalysis);
  QBool* pFilterInInterval= 0;
  ch->SetBranchAddress("FilterInInterval_Coincidence_Tower@Passed.",&pFilterInInterval);
  QDouble* pTotalEnergy= 0;
  ch->SetBranchAddress(Form("Coincidence_OFTime_Sync_20ms_150keV_%s@TotalEnergy.",ExtraLabel.c_str()) ,&pTotalEnergy);
  QDouble* pEnergy= 0;
  ch->SetBranchAddress("EnergySelector_QNDBD@Energy.",&pEnergy);
  QCoincidenceData* pCoincidenceData= 0;
  ch->SetBranchAddress(Form("Coincidence_OFTime_Sync_20ms_150keV_%s@CoincidenceData.",ExtraLabel.c_str()) ,&pCoincidenceData);
  QDouble* pRadius = 0;
  ch->SetBranchAddress(Form("Coincidence_OFTime_Sync_20ms_150keV_%s@Radius.",ExtraLabel.c_str()) ,&pRadius);
  QCountPulsesData* pCountPulses = 0;
  ch->SetBranchAddress("BCountPulses@CountPulsesData.",&pCountPulses);
  QBaselineData* pBaselineData = 0;
  ch->SetBranchAddress("BaselineModule@BaselineData.",&pBaselineData);

  // derived things
  Int_t Multiplicity;
  Bool_t IsSignal;
  
  // define TTree
  TTree* outTree = new TTree("outTree","Tree for coincidence things");
  Int_t bMultiplicity;
  Double_t bTotalEnergy;
  Double_t bEnergy;
  Double_t bRadius;
  //  Int_t bNumberOfPulses;
  Double_t bBaselineSlope;
  outTree->Branch("Multiplicity",&bMultiplicity,"Multiplicity/I");
  outTree->Branch("TotalEnergy",&bTotalEnergy,"TotalEnergy/D");
  outTree->Branch("Energy",&bEnergy,"Energy/D");
  outTree->Branch("Radius",&bRadius,"Radius/D");
  outTree->Branch("BaselineSlope",&bBaselineSlope,"BaselineSlope/D");
  //  outTree->Branch("NumberOfPulses",&bNumberOfPulses,"NumberOfPulses/I");
  
  // loop over events (once!)
  Int_t Nevents = ch->GetEntries();
  int i;
  for(i=0; i<Nevents; i++){
    ch->GetEntry(i);
    // compute derived things
    IsSignal = pPulseInfo->GetIsSignal();
    Multiplicity = pCoincidenceData->fMultiplicity;
    if(i%1000 == 1)cout << "Event " << i << " / " << Nevents << " ( " << 100.*i/(double)Nevents << " % )" << endl;//printout
    
    if(IsSignal && *pRejectBadIntervals == true && pCountPulses->GetNumberOfPulses() == 1 && *pSampleInfoFilter == true && *pBadForAnalysis == true && *pFilterInInterval == true){// base selection 
      // fill variables to put into tree
      bMultiplicity = Multiplicity;
      bTotalEnergy = *pTotalEnergy;
      bEnergy = *pEnergy;
      bRadius = *pRadius;
      bBaselineSlope = pBaselineData->GetBaselineSlope();
      //      bNumberOfPulses = pCountPulses->GetNumberOfPulses();
      outTree->Fill();
      // -------------------------------
    }
  }
  cout << "Event " << i << " / " << Nevents << " ( " << 100.*(double)i/(double)Nevents << " % )" << endl;//printout

  // do all the output
  string pathOutputFile = "/nfs/cuore1/scratch/gfantini/spacebased/out/PlotCoincidencesEnergyVsEnergy";
  pathOutputFile += "["+inFileName+"]";
  pathOutputFile += "_"+ExtraLabel;
  pathOutputFile += ".root";
  cout << "Writing output ROOT: " << pathOutputFile << endl;
  TFile* pOutputFile = new TFile(pathOutputFile.c_str(),"RECREATE");
  if(pOutputFile->IsZombie()){
    cerr << "Could not create output file! Is Zombie!" << endl;
  }else{
    cout << "Output file is not zombie." << endl;
    outTree->Write();
    pOutputFile->Close();
    delete pOutputFile;
  }
  cout << "Goodbye!" << endl;
};

int main(int argc, char *argv[])
{
  if(argc == 3){
    NonInteractivePlot(argv[1],argv[2]); // argv[0] is the executable name
  }else{
    //    cout << "argv[0]: " << argv[0] << endl;
    //    cout << "argv[1]: " << argv[1] << endl;
    cout << "Call  with 2 arguments (inputFileName,ExtraLabel). argc = "<< argc << endl;
    cout << "Will look for inputFileName inside " << Gpath << endl;
  }
  return 0;

}
