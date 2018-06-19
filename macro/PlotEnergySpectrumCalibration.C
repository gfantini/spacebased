const unsigned long long int kOverwrite = 1ULL << (1);
const int Printout = 100000;

void PlotEnergySpectrumCalibration(string InputDataFile = "PlotCoincidencesEnergyVsEnergy[Coincidence1200_301530_C.list]_1200mm.root", 
				   string InputMonteCarloFile = "Sep2017Calibration_g4cuore.root")

{
  string FolderData = "/nfs/cuore1/scratch/gfantini/spacebased/out/";
  string FolderMonteCarlo = "/nfs/cuore1/scratch/cjdavis/Calibration/Sep2017/";

  string iData = FolderData + InputDataFile;
  string iMC   = FolderMonteCarlo + InputMonteCarloFile;
  
  cout << "Input file DAT: \t " << (iData) << endl;
  cout << "Input file MC : \t " << (iMC) << endl;
  cout << "Will append result to Input file DAT" << endl;

  // read data tree
  TFile *pFileDATA = new TFile( iData.c_str(), "UPDATE" ); 
  TTree *pTreeDATA;
  gDirectory->GetObject("outTree",pTreeDATA);

  // read MC tree
  TFile *pFileMC = new TFile( iMC.c_str(), "READ" );
  TTree *pTreeMC;
  gDirectory->GetObject("outTree",pTreeMC);

  Double_t TotalEnergyDATA;
  Double_t BaselineSlopeDATA;
  Int_t MultiplicityDATA;
  pTreeDATA->SetBranchAddress("TotalEnergy",&TotalEnergyDATA);
  pTreeDATA->SetBranchAddress("BaselineSlope",&BaselineSlopeDATA);
  pTreeDATA->SetBranchAddress("Multiplicity",&MultiplicityDATA);
  Int_t DetectorMC;
  Double_t Ener2MC;
  Short_t MultiplicityMC;
  Double_t ESum2MC;
  Bool_t AccidentalMC;
  pTreeMC->SetBranchAddress("Detector",&DetectorMC);
  pTreeMC->SetBranchAddress("Ener2",&Ener2MC);
  pTreeMC->SetBranchAddress("Multiplicity",&MultiplicityMC);
  pTreeMC->SetBranchAddress("ESum2",&ESum2MC);
  pTreeMC->SetBranchAddress("Accidental",&AccidentalMC);

  TH1D* hTotalEnergyDATA = new TH1D("hTotalEnergyDATA",InputDataFile.c_str(),6000,0.,6000.);
  TH1D* hTotalEnergyMC = new TH1D("hTotalEnergyMC",InputMonteCarloFile.c_str(),6000,0.,6000.);

  // will fill this with the true / (true + acc) MC ratio of events in M2
  TH2D* MatrixAccidentalCorrection = new TH2D("MatrixAccidentalCorrection","MC Acci / Tot",6000,0.,6e3,6000,0.,6e3); 
  TH2D* MatrixAcciMC = new TH2D("MatrixAcciMC","MC Acci",6000,0.,6e3,6000,0.,6e3);
  TH2D* MatrixTotalMC = new TH2D("MatrixTotalMC","MC True+Acci",6000,0.,6e3,6000,0.,6e3);

  // fill the DATA histo
  for(int i=0;i<pTreeDATA->GetEntries(); i++){
    if(i%Printout == 1)cout << 100.*(double)i/(double)pTreeDATA->GetEntries() << " %  completed." << endl;
    pTreeDATA->GetEntry(i);
    if(abs(BaselineSlopeDATA) < 0.01 && MultiplicityDATA == 2) hTotalEnergyDATA->Fill(TotalEnergyDATA);
  }
  cout << "****************************************************************************" << endl;
  // fill the MC things
  for(int i=0; i<pTreeMC->GetEntries(); i++){
    if(i%Printout == 1)cout << 100.*(double)i/(double)pTreeMC->GetEntries() << " %  completed." << endl;
    pTreeMC->GetEntry(i);
    if(MultiplicityMC == 2){
      hTotalEnergyMC->Fill(ESum2MC);
      MatrixTotalMC->Fill(Ener2MC,ESum2MC-Ener2MC);
      if(AccidentalMC)MatrixAcciMC->Fill(Ener2MC,ESum2MC-Ener2MC);
    }
  }
  // normalize
  hTotalEnergyMC->Scale( hTotalEnergyDATA->GetEntries()/(double)hTotalEnergyMC->GetEntries() );
  cout << "****************************************************************************" << endl;

  // compute ratio
  for(int i=0; i<MatrixAcciMC->GetNbinsX(); i++){
    for(int j=0; j<MatrixTotalMC->GetNbinsY(); j++){
      if( (i*MatrixAcciMC->GetNbinsY()+j)%Printout == 1 )
	cout << "Matrix computation completed: " << 100.*((double)i*(double)MatrixTotalMC->GetNbinsY()+(double)j)/((double)MatrixTotalMC->GetNbinsY()*(double)MatrixTotalMC->GetNbinsX() ) << " % " << endl;
      MatrixAccidentalCorrection->SetBinContent(i,j,MatrixAcciMC->GetBinContent(i,j)/( (double)MatrixTotalMC->GetBinContent(i,j)+1e-6) );
    }
  }
  cout << "****************************************************************************" << endl;
  // . . . . . . . .

  // make graphic things
  TCanvas *c1 = new TCanvas();
  TLegend *leg = new TLegend(.7,.7,1.,1.);
  leg->AddEntry(hTotalEnergyDATA,InputDataFile.c_str());
  leg->AddEntry(hTotalEnergyMC,InputMonteCarloFile.c_str());
  
  hTotalEnergyDATA->Draw();
  hTotalEnergyMC->SetLineColor(kRed);
  hTotalEnergyMC->Draw("SAME");
  leg->Draw("SAME");

  // writing to disk
  pFileDATA->cd();
  MatrixAccidentalCorrection->Write("MatrixAccidentalCorrection", kOverwrite);
  pFileDATA->Close();
  delete pFileDATA;
  cout << "Written output file." << endl;

  TCanvas *c2 = new TCanvas();
  MatrixAccidentalCorrection->Draw("COLZ");
  
}

/* OLD STUFF
TH2D* PlotScatter(string input = "PlotCoincidencesEnergyVsEnergy[Coincidence1200_301530_C.list]_1200mm.root")
{
  string inputFolder = "/nfs/cuore1/scratch/gfantini/spacebased/out/";
  cout << "Using " << inputFolder + input << endl;
  
  TFile* pFile = new TFile( (inputFolder+input).c_str(), "READ");
  TTree* pTree; 
  gDirectory->GetObject("outTree",pTree);
  
  // set branches
  Double_t TotalEnergy;
  Double_t Energy;
  Double_t BaselineSlope;
  Int_t Multiplicity;
  pTree->SetBranchAddress("TotalEnergy",&TotalEnergy);
  pTree->SetBranchAddress("Energy",&Energy);
  pTree->SetBranchAddress("BaselineSlope",&BaselineSlope);
  pTree->SetBranchAddress("Multiplicity",&Multiplicity);

  TGraph* pGraph  = new TGraph(); // fill with all events
  TGraph* pGraphS = new TGraph(); // fill sum 2615
  TGraph* pGraphB = new TGraph(); // fill accidentals
  int c,cS,cB;
  c  = 0;
  cS = 0;
  cB = 0;
  // TH2D TH2D(const char* name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup)
  TH2D* pHisto = new TH2D("h","Energy : Energy scatter plot",3300/10.,0.,3300.,3300/10.,0.,3300.);


  for(int i = 0; i < pTree->GetEntries(); i++){
    pTree->GetEntry(i);
    if( i % 100000 == 1) cout << 100.*(double)i/(double)pTree->GetEntries() << "% done " << endl;
    if(Multiplicity == 2 && TotalEnergy - Energy < 3e3 && Energy < 3e3 && abs(BaselineSlope) < .01){
      pGraph->SetPoint(c,Energy,TotalEnergy-Energy);
      c++;
      pHisto->Fill(Energy,TotalEnergy-Energy);
      if(abs(TotalEnergy -2615) < 10.){
	pGraphS->SetPoint(cS,Energy,TotalEnergy-Energy);
	cS++;
      }
      if(abs(Energy - 2615) < 10. || abs(TotalEnergy - Energy - 2615 ) < 10.){
	pGraphB->SetPoint(cB,Energy,TotalEnergy-Energy);
	cB++;
      }
    }
  }
  
  TCanvas* c1 = new TCanvas();
  pGraph->GetXaxis()->SetTitle("Energy [keV]");
  pGraph->Draw("AP");
  pGraphS->GetXaxis()->SetTitle("TotalEnergy - Energy [keV]");
  pGraphS->SetMarkerColor(kRed);
  pGraphS->Draw("P");
  pGraphB->SetMarkerColor(kBlue);
  pGraphB->Draw("P");

  TCanvas* c2 =new TCanvas();
  gStyle->SetPalette(1);                  // set color range 
  h->GetZaxis()->SetRangeUser(0.,200.);   // set event range
  pHisto->Draw("COLZ");

  return pHisto;
}
*/
