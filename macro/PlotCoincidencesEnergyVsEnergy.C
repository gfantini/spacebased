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

void NonInteractivePlot()
{
  string path = "/nfs/cuore1/scratch/gfantini/mydiana/output/ds3021/Coincidence1200_301530_C.list";
  QChain* ch = new QChain();
  ch->Add(path.c_str());
  
  ch->SetBranchStatus("*",0);
  ch->SetBranchStatus("DAQ@PulseInfo.*",1);
  ch->SetBranchStatus("RejectBadIntervals_AntiCoincidence_Tower@Passed.*",1);
  ch->SetBranchStatus("SampleInfoFilter@Passed.*",1);
  ch->SetBranchStatus("BadForAnalysis_Coincidence_Sync_GF@Passed.*",1);
  ch->SetBranchStatus("FilterInInterval_Coincidence_Tower@Passed.*",1);
  ch->SetBranchStatus("Coincidence_OFTime_Sync_20ms_150keV_1200mm@TotalEnergy.*",1);
  ch->SetBranchStatus("EnergySelector_QNDBD@Energy.*",1);
  ch->SetBranchStatus("Coincidence_OFTime_Sync_20ms_150keV_1200mm@CoincidenceData.*",1);
  
  // define things
  QPulseInfo* pPulseInfo= 0;
  ch->SetBranchAddress("DAQ@PulseInfo.",&pPulseInfo);
  QBool* pRejectBadIntervals= 0;
  ch->SetBranchAddress("RejectBadIntervals_AntiCoincidence_Tower@Passed.",&pRejectBadIntervals);
  QBool* pSampleInfoFilter= 0;
  ch->SetBranchAddress("SampleInfoFilter@Passed.",&pSampleInfoFilter);
  QBool* pBadForAnalysis= 0;
  ch->SetBranchAddress("BadForAnalysis_Coincidence_Sync_GF@Passed.",&pBadForAnalysis);
  QBool* pFilterInInterval= 0;
  ch->SetBranchAddress("FilterInInterval_Coincidence_Tower@Passed.",&pFilterInInterval);
  QDouble* pTotalEnergy= 0;
  ch->SetBranchAddress("Coincidence_OFTime_Sync_20ms_150keV_1200mm@TotalEnergy.",&pTotalEnergy);
  QDouble* pEnergy= 0;
  ch->SetBranchAddress("EnergySelector_QNDBD@Energy.",&pEnergy);
  QCoincidenceData* pCoincidenceData= 0;
  ch->SetBranchAddress("Coincidence_OFTime_Sync_20ms_150keV_1200mm@CoincidenceData.",&pCoincidenceData);
  // derived things
  Int_t Multiplicity;
  Bool_t IsSignal;
  
  // define plots...
  TGraph* gMx = new TGraph(); // X = Energy Y = TotalEnergy - Energy
  int counter_gMx = 0;
  TGraph* gMxSelected = new TGraph();
  int counter_gMxSelected = 0;

  // loop over events (once!)
  Int_t Nevents = ch->GetEntries();
  int i;
  for(i=0; i<Nevents; i++){
    ch->GetEntry(i);
    // compute derived things
    IsSignal = pPulseInfo->GetIsSignal();
    Multiplicity = pCoincidenceData->fMultiplicity;
    if(i%1000 == 1)cout << "Event " << i << " / " << Nevents << " ( " << 100*i/(double)Nevents << " % )" << endl;//printout
    
    if(IsSignal && *pRejectBadIntervals == true && *pSampleInfoFilter == true && *pBadForAnalysis == true && *pFilterInInterval == true){// base selection 
      if(*pTotalEnergy + *pEnergy < 3e3 && *pEnergy < 3e3){// select energy region (large)
	gMx->SetPoint(counter_gMx,*pEnergy,*pTotalEnergy - *pEnergy);
	if(abs(*pTotalEnergy - 2615) < 10 ){// TotalEnergy = 2615 +/- 10 keV
	  gMxSelected->SetPoint(counter_gMxSelected,*pEnergy,*pTotalEnergy - *pEnergy);
	  }
      }
    }
  }
  cout << "Event " << i << " / " << Nevents << " ( " << 100*i/(double)Nevents << " % )" << endl;//printout
  
  // do all the plotting
  TCanvas* c1 = new TCanvas();
  gMx->Draw("AP");
  gMxSelected->SetMarkerColor(kRed);
  gMxSelected->Draw("PSAME");
  
  string pathOutputFile = "/nfs/cuore1/scratch/gfantini/spacebased/out/PlotCoincidencesEnergyVsEnergy_fast.root";
  cout << "Writing output ROOT: " << pathOutputFile << endl;
  TFile* pOutputFile = new TFile(pathOutputFile.c_str(),"recreate");
  gMx->Write();
  gMxSelected->Write();
  c1->Write();
  pOutputFile->Close();
};

int main()
{

  NonInteractivePlot();
  return 0;

}
