int ComputeOptimalCut(string filename ="PlotCoincidencesEnergyVsEnergy.root",char multiplicity = '2'/* 2 / X */){
  string input = "/nfs/cuore1/scratch/gfantini/spacebased/out/";
  input += filename;
  cout << "Opening " << input << endl;
  TFile* inputFile = new TFile(input.c_str(),"UPDATE");
  TH1D *hSignal = 0; // 100,0,1060
  TH1D *hBackground = 0;
  
  if(multiplicity == '2'){
    gDirectory->GetObject("hsumM2",hSignal);
    gDirectory->GetObject("haccM2",hBackground);
  }

  if(multiplicity == 'X'){
    gDirectory->GetObject("hsumMX",hSignal);
    gDirectory->GetObject("haccMX",hBackground);
  }

  if(multiplicity != '2' && multiplicity != 'X'){
    cerr << "FATAL! multiplicity should be either 2 or X" << endl;
    cerr << "multiplicity = " << multiplicity << endl;
    return 1;
  }

  if(hSignal == 0 || hBackground == 0){
    cerr << "FATAL! Probably you did not process with the step that creates hsumM* / haccM*" << endl;
    return 1;
  }
  // computation of efficiency
  TGraph * gEffiSignal = new TGraph();
  int NSignal = 0;
  cout << "hSignal: GetNbinsX = " << hSignal->GetNbinsX() << endl;
  cout << "hSignal: GetEntries= " << hSignal->GetEntries() << endl;
  for(int i=1;i<hSignal->GetNbinsX();i++){
    NSignal += hSignal->GetBinContent(i);
    gEffiSignal->SetPoint(i-1,hSignal->GetBinCenter(i),NSignal/(double)hSignal->GetEntries());
  }

  TGraph * gEffiBackground = new TGraph();
  int NBackground = 0;  
  cout << "hBackground: GetNbinsX = " << hBackground->GetNbinsX() << endl;
  cout << "hBackground: GetEntries= " << hBackground->GetEntries() << endl;
  for(int i=1;i<hBackground->GetNbinsX();i++){
    NBackground += hBackground->GetBinContent(i);
    gEffiBackground->SetPoint(i-1,hBackground->GetBinCenter(i),NBackground/(double)hBackground->GetEntries());
  }

  // compute score function
  TGraph* gScore = new TGraph();
  double R = 0.;
  double Rstep = 1.;
  int j = 0;
  double nS = hSignal->GetEntries();
  double nB = hBackground->GetEntries();
  while(R < 1200.){
    gScore->SetPoint(j,R,nS*gEffiSignal->Eval(R)/sqrt(1e-6+nS*gEffiSignal->Eval(R) + nB*gEffiBackground->Eval(R)) );
    R+=Rstep;
    j++;
  }

  // compute efficiency vs purity == Nsignal / (Nsignal + Nbackground)
  TGraph * gRvsEffiSignal = new TGraph(gEffiSignal->GetN(),gEffiSignal->GetY(),gEffiSignal->GetX());
  TGraph * gRvsEffiBackground = new TGraph(gEffiBackground->GetN(),gEffiBackground->GetY(),gEffiBackground->GetX());
  /* debug
  TCanvas* c1 = new TCanvas();
  gRvsEffiSignal->Draw("AP");
  TCanvas* c2 = new TCanvas();
  gRvsEffiBackground->Draw("AP");
  */
  
  TGraph * gPurityVsEffi = new TGraph();
  const double Rstep = 1; // mm
  const double Rmax = 1100; // mm
  double R=0;
  double EffiSignal;
  double EffiBackground;
  int k = 0;
  double Purity;

  cout << "NSignal:\t"<< NSignal << endl;
  cout << "NBackground:\t"<< NBackground << endl;
  while(R<Rmax){
    EffiSignal = gEffiSignal->Eval(R); // get efficiency given R
    EffiBackground = gEffiBackground->Eval(R);
    cout << "R: EffiS / EffiB = "<< R << ":\t " << EffiSignal << " / " << EffiBackground << endl;
    if(EffiSignal == 0. && EffiBackground == 0){
      k++; R+=Rstep;
      continue;
    }
    Purity = NSignal*EffiSignal/(NSignal*EffiSignal+NBackground*EffiBackground);
    gPurityVsEffi->SetPoint(k,EffiSignal,Purity);
    k++;
    R += Rstep;
  }

  cout << "Appending output to " << input << endl;
  gEffiSignal->Write(Form("gEffiSignalM%c",multiplicity),1ULL << (2)); // 1ULL << (2) sarebbe kWriteDelete
  gEffiBackground->Write(Form("gEffiBackgroundM%c",multiplicity),1ULL << (2));
  gPurityVsEffi->Write(Form("gPurityVsEffiM%c",multiplicity),1ULL << (2));
  gScore->Write(Form("gScoreM%c",multiplicity),1ULL << (2));

  MakeFancyPlot(gEffiSignal,gEffiBackground,gScore,gPurityVsEffi,multiplicity,filename);
  
  delete inputFile; // free the memory

  return 0;
}
// nS == total signal events nB == total bkg events
void MakeFancyPlot(TGraph* eS,TGraph* eB,TGraph* gScore,TGraph* PvsE,char multiplicity,string filename,string output = "/nfs/cuore1/scratch/gfantini/spacebased/out/"){
  TCanvas* c1 = new TCanvas("c1","",1600,900);
  c1->Divide(2,1);
  c1->cd(1);
  // ......
  eS->SetLineColor(8); // nice green
  eS->SetLineWidth(3); // thick enough
  eS->GetXaxis()->SetTitle("R [mm]");
  eS->GetYaxis()->SetTitle("Efficiency");
  eS->SetTitle(Form("Multiplicity %c\n %s",multiplicity,filename.c_str()) );
  eS->Draw("APL");
  // ------
  eB->SetLineColor(2);
  eB->SetLineWidth(3);
  eB->Draw("PL");
  // ******

  TLegend* leg = new TLegend(0.5,0.2,.9,.6,"Radial Cut Performance");
  leg->AddEntry(eS,"Signal Efficiency");
  leg->AddEntry(eB,"Background Efficiency");
  leg->Draw();

  
  // rescale TGraph
  Double_t max = 0.;
  for(int i=0;i<gScore->GetN(); i++)
    if(max < gScore->GetY()[i]) max = gScore->GetY()[i];
  Double_t rightmax = max;
  Double_t scale = gPad->GetUymax()/rightmax;
  double x,y;
  for(int i=0; i<gScore->GetN(); i++){
    gScore->GetPoint(i,x,y);
    gScore->SetPoint(i,x,scale*y);
  }
  gScore->GetXaxis()->SetTitle("R [mm]");
  gScore->GetYaxis()->SetTitle("S/#sqrt{S+B}");
  gScore->SetLineColor(4);
  gScore->SetLineWidth(2);
  gScore->Draw("PLSAME");

  cout << "DEBUG: rightmax = \t" << rightmax << endl;
  cout << "DEBUG: scale = \t" << scale << endl;

  //draw an axis on the right side                                                                                                                                                                                                             // xmin, ymin, xmax, ymax, <Highest value for the tick mark labels written on the axis>, Number of divisions, Drawing options 
  TGaxis *axis = new TGaxis(0.95*gScore->GetXaxis()->GetXmax(),0.,
                            0.95*gScore->GetXaxis()->GetXmax(), 1.1 ,.1,rightmax,20,"+L");
  axis->SetLineColor(kBlue);
  axis->SetTextColor(kBlue);
  axis->SetTitle("S/#sqrt{S+B}");
  axis->SetTitleOffset(1.35);
  axis->Draw("SAME");

  c1->cd(2);
  PvsE->GetXaxis()->SetTitle("Efficiency (signal)");
  PvsE->GetYaxis()->SetTitle("Purity");
  PvsE->SetMarkerStyle(kOpenCircle);
  PvsE->SetLineColor(1);
  PvsE->SetLineWidth(4);
  PvsE->SetTitle(Form("Multiplicity %c\n %s",multiplicity,filename.c_str()) );
  PvsE->Draw("APL");

  c1->SaveAs(Form("%s/FancyPlot_M%c_File%s.pdf",output.c_str(),multiplicity,filename.c_str()) );
  cout << "Written plot into " << Form("%s/FancyPlot_M%c_File%s.pdf",output.c_str(),multiplicity,filename.c_str()) << endl;
}
