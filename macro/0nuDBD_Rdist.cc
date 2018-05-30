/*
 * Reads MC files and produces histogram of radii of M2 events
 *
 *
 *
 */


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

/* 1. Reads the g4cuore output
 * 2. Produces TTree with basic info on M2 events and their space distribution
 *    bugfix: load "detector"(DAQ numbering) instead of "channel"(MC numbering)
 * 
 * 
 * 
 */
void Rdist(TString pathMC = "/nfs/cuore1/scratch/gfantini/mcout/g4cuore-out/TeO2-OnuDBD-1e6-T3-g4cuore.root",TString outputRoot = "/nfs/cuore1/scratch/gfantini/spacebased/out/TeO2-0nuDBD-RadiiM2.root")
{

  TFile fileMC(pathMC,"read");
  if(fileMC.IsZombie()){
    cerr << "File " << pathMC <<" Is Zombie" << endl;
    exit();
  }
  cout << "Reading: " << pathMC << endl;

  // get output tree
  TTree *outTree;
  fileMC.GetObject("outTree",outTree);
  outTree->Show();

  Double_t Ener1,ESum1;
  Double_t Ener1_main;
  Double_t Ener1_coincident;
  Int_t Detector;
  Short_t Multiplicity;
  outTree->SetBranchAddress("Detector",&Detector);
  outTree->SetBranchAddress("Ener1",&Ener1);
  outTree->SetBranchAddress("ESum1",&ESum1);
  outTree->SetBranchAddress("Multiplicity",&Multiplicity);

  TH1D *hRadii = new TH1D("hRadii","Radius of M2 coincident events; R [mm]; events;",2000,0.,2000.);
  TH1D *hM2 = new TH1D("hM2","M2sum; Energy [keV]; Enrties/(0.5 keV);",10000,0.,5000.);
  Int_t ch1,ch2;
  Double_t R;
  // make a tree to store M2 information
  TFile *fOutput = new TFile(outputRoot,"recreate"); // output file
  TTree *myOutputTree = new TTree("M2tree","M2tree");
  myOutputTree->Branch("R",&R,"R/D");
  myOutputTree->Branch("E1",&Ener1_main,"E1/D");
  myOutputTree->Branch("E2",&Ener1_coincident,"E2/D");
  myOutputTree->Branch("ESum",&ESum1,"ESum1/D");
  myOutputTree->Branch("Ch1",&ch1,"Ch1/I");
  myOutputTree->Branch("Ch2",&ch2,"Ch2/I");
  // select M2 events, compute distance, fill histo, save to file
  auto N = outTree->GetEntries();
  cout << "outTree: reading " << N << " entries." << endl;
  for (Int_t i=0;i<N;i++)
    {
      outTree->GetEntry(i);
      if(Multiplicity == 2)// select M2
	{
	  ch1=Detector;
	  Ener1_main = Ener1;
	  i++; outTree->GetEntry(i); // get next event in order to read the #cry and energy release
	  ch2=Detector;
	  Ener1_coincident = Ener1;
	  R = ComputeDistance(ch1,ch2);
	  hRadii->Fill(R);
	  hM2->Fill(ESum1);
	  myOutputTree->Fill();
	  //	  cout << "SumE = " << Esum1 << endl;
	}
      
    }
  // write things to the output file
  hRadii->Write();
  hM2->Write();
  myOutputTree->Write();
  fOutput->Close();
}



// takes in input the .root produced by Rdist()
// REQUIRES: TTree M2tree
void EfficiencyM2(TString inputFile = "/nfs/cuore1/scratch/gfantini/spacebased/out/TeO2-0nuDBD-RadiiM2-T3.root",TString outputFile = "/nfs/cuore1/scratch/gfantini/spacebased/out/EffiCurve.root")
{
  const double FWHM = 5.;

  TFile *file = new TFile(inputFile,"read");
  TTree *M2tree;
  file->GetObject("M2tree",M2tree);
  
  Int_t N_M2 = M2tree->GetEntries(); 
  Double_t ESum,R;
  M2tree->SetBranchAddress("ESum",&ESum);
  M2tree->SetBranchAddress("R",&R);
  
  // loop over all possible R-cut
  vector<double> Efficiency;
  vector<double> ErrEfficiency;
  vector<double> Radius;
  double tmpE;

  double Rstep = 1; // mm
  double Rmax = 0.;
  Int_t Nsel = 0;
  while(Rmax < 1500)
    {
      cout << Rmax << " / 1500" << endl; 
      Nsel = 0;
      for(Int_t i=0;i<N_M2;i++)
	{
	  M2tree->GetEntry(i);
	  if(TMath::Abs(ESum-2527.515)<5.*FWHM && R < Rmax) // energy selection && space selection
	    Nsel++;
	}
      tmpE = Nsel/(double)N_M2;
      Efficiency.push_back(tmpE);
      ErrEfficiency.push_back(sqrt(tmpE*(1-tmpE)/(double)N_M2));
      Radius.push_back(Rmax);
      Rmax += Rstep;
    }
  TFile *fout = new TFile(outputFile,"recreate");
  cout << "Writing " << outputFile << endl;
  TGraphErrors *EffiR = new TGraphErrors(Radius.size(),&Radius[0],&Efficiency[0],&ErrEfficiency[0]);
  EffiR->Draw("AP");
  EffiR->Write();
}
