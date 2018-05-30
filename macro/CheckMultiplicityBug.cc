/*
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
*/

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

void CheckMultiplicityBug(const char* inputList = "CoincidenceTest1200_301530_C.list",const char* inputFolder = "/nfs/cuore1/scratch/gfantini/mydiana/output/ds3021/",const char* outputFolder = "/nfs/cuore1/scratch/gfantini/spacebased/out/"){
  // enable this to print debug messages
  bool debug = false;
  // use only in interactive
  //  gSystem->Load("libQAnalysisPlots");

  QChain* pChain = new QChain();
  char* inputFile = Form("%s/%s",inputFolder,inputList);
  pChain->Add(inputFile);
  cout << "Reading from " << inputFile << endl;
  
  // variables from either std processing / old coincidences
  // --> meaning Sync_10ms_150keV
  Double_t QNDBDOfficialEnergy = 0;
  Int_t OldMultiplicity = 0;
  Double_t OldTotalEnergy = 0;
  QCoincidenceData* pCoincidenceDataOld = 0;
  // variables from new coincidences == new + 200mm cut
  Int_t NewMultiplicity = 0;
  Double_t NewTotalEnergy = 0;
  QCoincidenceData* pCoincidenceDataNew = 0;
  //********************************************
  // some more stuff...
  QBool* RejectBadIntervals_AntiCoincidence = 0; // RejBI_Sync_GF
  QBool* BadForAnalysis_Coincidence = 0; // _Coincidence_Sync_GF
  QPulseInfo* pPulseInfo = 0;
  QCountPulsesData* pCountPulses = 0;
  QBaselineData* pBaselineData = 0;

  Double_t BaselineSlope = 0;
  Bool_t IsSignal = 0;
  QBool SingleTrigger = 0;
  Int_t NumberOfPulses = 0;
  Bool_t RejectBadIntervals = 0;
  Bool_t BadForAnalysis = 0;
  /*
  pChain->SetBranchStatus("*",0);
  pChain->SetBranchStatus("RejectBadIntervals_Sync_GF@Passed.",1);
  pChain->SetBranchStatus("BadForAnalysis_Coincidence_Sync_GF@Passed.",1);
  pChain->SetBranchStatus("BaselineModule@BaselineData.",1);
  pChain->SetBranchStatus("EnergySelector_QNDBD@Energy.",1);
  pChain->SetBranchStatus("DAQ@PulseInfo.",1); // i can get channel from here   
  pChain->SetBranchStatus("SampleInfoFilter@Passed.",1);
  pChain->SetBranchStatus("BCountPulses@CountPulsesData.",1);
  pChain->SetBranchStatus("Coincidence_OFTime_Sync_10ms_150keV@TotalEnergy.",1);
  pChain->SetBranchStatus("Coincidence_OFTime_Sync_10ms_150keV@CoincidenceData.",1);
  pChain->SetBranchStatus("Coincidence_OFTime_Sync_10ms_150keV_1200mm@TotalEnergy.",1);
  pChain->SetBranchStatus("Coincidence_OFTime_Sync_10ms_150keV_1200mm@CoincidenceData.",1);
  */
  pChain->SetBranchAddress("RejectBadIntervals_Sync_GF@Passed.",&RejectBadIntervals_AntiCoincidence);
  pChain->SetBranchAddress("BadForAnalysis_Coincidence_Sync_GF@Passed.",&BadForAnalysis_Coincidence);
  pChain->SetBranchAddress("BaselineModule@BaselineData.",&pBaselineData);
  pChain->SetBranchAddress("EnergySelector_QNDBD@Energy.",&QNDBDOfficialEnergy);
  pChain->SetBranchAddress("DAQ@PulseInfo.",&pPulseInfo); // i can get channel from here
  
  pChain->SetBranchAddress("SampleInfoFilter@Passed.",&SingleTrigger);
  pChain->SetBranchAddress("BCountPulses@CountPulsesData.",&pCountPulses);
  // coincidences sync 10ms 150keV (Old)
  pChain->SetBranchAddress("Coincidence_OFTime_Sync_10ms_150keV@TotalEnergy.",&OldTotalEnergy);
  pChain->SetBranchAddress("Coincidence_OFTime_Sync_10ms_150keV@CoincidenceData.",&pCoincidenceDataOld);
  // coincidences New (+= 200mm cut)
  pChain->SetBranchAddress("Coincidence_OFTime_Sync_10ms_150keV_1200mm@TotalEnergy.",&NewTotalEnergy);
  pChain->SetBranchAddress("Coincidence_OFTime_Sync_10ms_150keV_1200mm@CoincidenceData.",&pCoincidenceDataNew);

  cout << "Chain has " << pChain->GetEntries() << " entries." << endl;
  // what I want to do is ... print, for those events where the Multiplicity is different, infos like
  // Main Event id, channel, Energy
  // Coincident events: id, channel, Energy, TotalEnergy New / Old
  int nInvalid = 0;
  int nInvalidSelected = 0;
  int nRejectBadIntervalsPassed = 0;
  int ch,ch_coincident;
  std::vector<int> neighbors;
  bool is_neighbor;
  QCoincidentChannel CoincidentChannelOld;
  QCoincidentChannel CoincidentChannelNew;

  for(int ee=0; ee<pChain->GetEntries(); ee++){ // main loop over events
    pChain->GetEntry(ee);
    if(ee%10000 == 0)cout << "Event "<< ee << " invalid " << nInvalid << endl;
    
    //    cout << pCoincidenceDataOld->IsValid() << " / " << pCoincidenceDataNew->IsValid() << endl;
    
    // skip invalid
    if( !(pCoincidenceDataOld->IsValid() && pCoincidenceDataNew->IsValid()) ){
      if(ee%10000 == 0)pCoincidenceDataOld->Dump(std::cout);
      //      cout << "ciao "<< pCoincidenceDataOld->IsValid() << " - " << pCoincidenceDataNew->IsValid() << endl;
      nInvalid++;
      continue;
    }
        
    IsSignal = pPulseInfo->GetIsSignal();    
    NumberOfPulses = pCountPulses->GetNumberOfPulses();
    BaselineSlope = pBaselineData->GetBaselineSlope();
    RejectBadIntervals = (*RejectBadIntervals_AntiCoincidence);
    BadForAnalysis = (*BadForAnalysis_Coincidence);
    
    if(RejectBadIntervals == true)nRejectBadIntervalsPassed++;

    if(debug == true && ee%10000 == 0){// do some debug printout
      cout << "****** DEBUG PRINTOUT ********" << endl;
      cout << "IsSignal = \t" << IsSignal << endl;
      cout << "RejectBadIntervals_AntiCoincidence = \t" << RejectBadIntervals << endl;
      cout << "BadForAnalysis_Coincidence = \t" << BadForAnalysis << endl;
      cout << "SingleTrigger = \t" << SingleTrigger << endl;
      cout << "NumberOfPulses = \t" << NumberOfPulses << endl;
      cout << "BaselineSlope = \t" << BaselineSlope << endl;
      cout << "QNDBDOfficialEnergy = \t" << QNDBDOfficialEnergy << endl;
      cout << ">>>> nRejectBadIntervalsPassed =\t" << nRejectBadIntervalsPassed << endl;
      cout << "*****************************************************************************" << endl;
    }
    
    // where the juice is ...
    
    if( IsSignal && RejectBadIntervals && BadForAnalysis && SingleTrigger && (NumberOfPulses == 1) && QNDBDOfficialEnergy > 0 && QNDBDOfficialEnergy < 1e4 )
      {
	
	if(debug)cout << "Hello" << endl;
	// passed the base cuts
	OldMultiplicity = pCoincidenceDataOld->fMultiplicity;
	NewMultiplicity = pCoincidenceDataNew->fMultiplicity;
	if(OldMultiplicity != NewMultiplicity)
	  {// These events are the bug... -> print the relevant info
	    ch = pPulseInfo->GetChannelId(); // CHANNEL
	    cout << "** Dump of OLD coincidence data:" << endl;
	    pCoincidenceDataOld->Dump(std::cout);
	    cout << "** Dump of NEW coincidence data:" <<endl;
	    pCoincidenceDataNew->Dump(std::cout);
	    cout << "********************************" <<endl;
	  }
	
      }
    
    
  }
  
  
}


/*
  //DEBUG: check validity                                                                                                                                               
  cout << "pCoinData valid " <<  pCoinData->IsValid() << endl;
  cout << "pTime valid " <<  pTime->IsValid() << endl;
  cout << "pTotalEnergy valid " <<  pTotalEnergy->IsValid() << endl;
  cout << "pPulseInfo valid " <<  pPulseInfo->IsValid() << endl;
  cout << "pST valid " << pST->IsValid() << endl;
  cout << "pBCountPulses valid " << pBCountPulses->IsValid() << endl;
  cout << "pBaselineData valid " << pBaselineData->IsValid() << endl;
  cout << "pBadIntervals valid " << pBadIntervals->IsValid() << endl;
  cout << "pBadAnalysis valid " << pBadAnalysis->IsValid() << endl;
  cout << "pMainEnergy valid " << pMainEnergy->IsValid() << endl;
  cout << "***** E " << (double)*pMainEnergy << endl;
  exit(0);
*/

#ifndef __CINT__
int main(int argc, char **argv) {

  if(argc!=1)// # of arguments +1                                                                                                                                                        
    {
      cout<<"Run as ./"<<argv[0]<<endl;//" <inputfolder> "<<endl;                                                                                                
      //      cout<<"<inputfolder> = folder that contains root files named spectrum_dsXXXX_ch###.root" << endl;                                                                       
      return 1;
    }

  CheckMultiplicityBug();
  return 0;
}
#endif /* __CINT __ */

