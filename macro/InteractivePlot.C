
#include "QChain.hh"
#include <QCoincidenceData.hh>
#include <QSampleInfo.hh>
#include "QCountPulsesData.hh"
#include <QPulseInfo.hh>
#include <QBaseType.hh>
#include "QCoincidenceData.hh"


void InteractivePlot()
{
  string path = "/nfs/cuore1/scratch/gfantini/mydiana/output/ds3021/Coincidence1200_301530_C.list";
  QChain* ch = new QChain();
  ch->Add(path.c_str());
  ch->ls();

  TCut IsSignal = "DAQ@PulseInfo.GetIsSignal()";
  TCut RejectBadIntervals = "RejectBadIntervals_AntiCoincidence_Tower@Passed.fValue";
  TCut SampleInfoFilter = "SampleInfoFilter@Passed.fValue";
  TCut BadForAnalysis = "BadForAnalysis_Coincidence_Sync_GF@Passed.fValue";
  TCut FilterInInterval = "FilterInInterval_Coincidence_Tower@Passed.fValue";
  TCut cut0 = IsSignal && RejectBadIntervals && SampleInfoFilter && BadForAnalysis && FilterInInterval;

  TCut cutY = "Coincidence_OFTime_Sync_20ms_150keV_1200mm@TotalEnergy.fValue - EnergySelector_QNDBD@Energy.fValue < 3e3";
  TCut cutX = "EnergySelector_QNDBD@Energy.fValue < 3e3";
  TCut cutM2 = "Coincidence_OFTime_Sync_20ms_150keV_1200mm@CoincidenceData.fMultiplicity == 2"; // select M2
  TCut cut2615sum = "abs(Coincidence_OFTime_Sync_20ms_150keV_1200mm@TotalEnergy.fValue - 2615) < 10"; // select sum energy == tallium
  TCut cut2615E1 = "abs(EnergySelector_QNDBD@Energy.fValue-2615) < 10";
  TCut cut2615E2 = "abs( Coincidence_OFTime_Sync_20ms_150keV_1200mm@TotalEnergy.fValue - EnergySelector_QNDBD@Energy.fValue - 2615) < 10";
  //  TCanvas* c1 = new TCanvas();
  //  ch->Draw("(Coincidence_OFTime_Sync_20ms_150keV_1200mm@TotalEnergy.fValue - EnergySelector_QNDBD@Energy.fValue) : EnergySelector_QNDBD@Energy.fValue",cut0 && cutY && cutY && cutM2 && cut2615);


  // plot Mx and highlight the selected events
  TCanvas* c2 = new TCanvas();
  TH2D* h1 = new TH2D("h1","Histo",3000,0.,3000.,3000,0.,3000.);
  TH2D* h2 = new TH2D("h2","Histo",3000,0.,3000.,3000,0.,3000.);
  h2->SetMarkerColor(kRed);
  ch->Draw("(Coincidence_OFTime_Sync_20ms_150keV_1200mm@TotalEnergy.fValue - EnergySelector_QNDBD@Energy.fValue) : EnergySelector_QNDBD@Energy.fValue >> h1",cut0 && cutY && cutX);
  ch->Draw("(Coincidence_OFTime_Sync_20ms_150keV_1200mm@TotalEnergy.fValue - EnergySelector_QNDBD@Energy.fValue) : EnergySelector_QNDBD@Energy.fValue >> h2",cut0 && cutY && cutX && cut2615sum,"SAME");
  //  ch->Draw("Coincidence_OFTime_Sync_20ms_150keV_1200mm@Radius.fValue",cut0 && cutY && cutX && cutM2 && cut2615sum);
  //TCanvas* c3 = new TCanvas();
  //ch->Draw("Coincidence_OFTime_Sync_20ms_150keV_1200mm@Radius.fValue",cut0 && cutY && cutX && cutM2 && (cut2615E1 || cut2615E2) );
  
};

void NonInteractivePlot()
{
  string path = "/nfs/cuore1/scratch/gfantini/mydiana/output/ds3021/Coincidence1200_301530_C.list";
  QChain* ch = new QChain();
  ch->Add(path.c_str());
  
  // define things
  QPulseInfo* pPulseInfo= 0;
  ch->SetBranchAddress("DAQ@PulseInfo",&pPulseInfo);
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
  for(int i=0; i<Nevents; i++){
    ch->GetEntry(i);
    // compute derived things
    IsSignal = pPulseInfo->GetIsSignal();
    Multiplicity = pCoincidenceData->fMultiplicity;
    if(i%1000 == 1)cout << "Event " << i << " / " << Nevents << " ( " << 100*i/(double)Nevents << " % )" << endl;//printout
    
    if(IsSignal && *pRejectBadIntervals == true && *pSampleInfoFilter == true && *pBadForAnalysis == true && *pFilterInInterval == true){// base selection 
      if(*pTotalEnergy + *pEnergy < 3e3 && *pEnergy < 3e3){// select energy region (large)
	gMx->SetPoint(counter_gMx,*pEnergy,*pTotalEnergy - *pEnergy);
	if(abs(*pTotalEnergy - 2615) < 10 ){// TotalEnergy = 2615 +/- 10 keV
	  gMxSelected(counter_gMxSelected,*pEnergy,*pTotalEnergy - *pEnergy);
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

};
