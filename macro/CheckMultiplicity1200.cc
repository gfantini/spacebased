



void CheckMultiplicity1200()
{

  // histogram made out of the CoincidenceOfficial, IsSignal && RejectBadIntervals_Sync@Passed.fValue && BadForAnalysis_Coincidence_Sync@Passed.fValue
  TH1D *Histogram1__1 = new TH1D("Histogram1__1","",78,0,78);
  Histogram1__1->SetBinContent(2,1122758);
  Histogram1__1->SetBinContent(3,563073);
  Histogram1__1->SetBinContent(4,310057);
  Histogram1__1->SetBinContent(5,161765);
  Histogram1__1->SetBinContent(6,79304);
  Histogram1__1->SetBinContent(7,37831);
  Histogram1__1->SetBinContent(8,16226);
  Histogram1__1->SetBinContent(9,7314);
  Histogram1__1->SetBinContent(10,3184);
  Histogram1__1->SetBinContent(11,1541);
  Histogram1__1->SetBinContent(12,754);
  Histogram1__1->SetBinContent(13,226);
  Histogram1__1->SetBinContent(14,156);
  Histogram1__1->SetBinContent(15,112);
  Histogram1__1->SetBinContent(16,73);
  Histogram1__1->SetBinContent(17,47);
  Histogram1__1->SetBinContent(18,28);
  Histogram1__1->SetBinContent(19,3);
  Histogram1__1->SetBinContent(21,2);
  Histogram1__1->SetBinContent(22,26);
  Histogram1__1->SetBinContent(23,10);
  Histogram1__1->SetBinContent(24,5);
  Histogram1__1->SetBinContent(25,2);
  Histogram1__1->SetBinContent(29,4);
  Histogram1__1->SetBinContent(30,15);
  Histogram1__1->SetBinContent(31,2);
  Histogram1__1->SetBinContent(32,2);
  Histogram1__1->SetBinContent(34,1);
  Histogram1__1->SetBinContent(35,1);
  Histogram1__1->SetBinContent(37,2);
  Histogram1__1->SetBinContent(38,2);
  Histogram1__1->SetBinContent(39,4);
  Histogram1__1->SetBinContent(40,3);
  Histogram1__1->SetBinContent(41,2);
  Histogram1__1->SetBinContent(42,2);
  Histogram1__1->SetBinContent(44,3);
  Histogram1__1->SetBinContent(45,5);
  Histogram1__1->SetBinContent(46,51);
  Histogram1__1->SetBinContent(47,8);
  Histogram1__1->SetBinContent(48,14);
  Histogram1__1->SetBinContent(49,2);
  Histogram1__1->SetBinContent(56,55);
  Histogram1__1->SetBinContent(57,16);
  Histogram1__1->SetBinContent(77,76);
  Histogram1__1->SetBinContent(78,12);
  Histogram1__1->SetEntries(2304779);


  TH1D *Histogram1__3 = new TH1D("Histogram1__3","",78,0,78);
  Histogram1__3->SetBinContent(2,1122758);
  Histogram1__3->SetBinContent(3,563073);
  Histogram1__3->SetBinContent(4,310057);
  Histogram1__3->SetBinContent(5,161765);
  Histogram1__3->SetBinContent(6,79304);
  Histogram1__3->SetBinContent(7,37831);
  Histogram1__3->SetBinContent(8,16226);
  Histogram1__3->SetBinContent(9,7314);
  Histogram1__3->SetBinContent(10,3184);
  Histogram1__3->SetBinContent(11,1541);
  Histogram1__3->SetBinContent(12,754);
  Histogram1__3->SetBinContent(13,226);
  Histogram1__3->SetBinContent(14,156);
  Histogram1__3->SetBinContent(15,112);
  Histogram1__3->SetBinContent(16,73);
  Histogram1__3->SetBinContent(17,47);
  Histogram1__3->SetBinContent(18,28);
  Histogram1__3->SetBinContent(19,3);
  Histogram1__3->SetBinContent(21,2);
  Histogram1__3->SetBinContent(22,26);
  Histogram1__3->SetBinContent(23,10);
  Histogram1__3->SetBinContent(24,5);
  Histogram1__3->SetBinContent(25,2);
  Histogram1__3->SetBinContent(29,4);
  Histogram1__3->SetBinContent(30,15);
  Histogram1__3->SetBinContent(31,2);
  Histogram1__3->SetBinContent(32,2);
  Histogram1__3->SetBinContent(34,1);
  Histogram1__3->SetBinContent(35,1);
  Histogram1__3->SetBinContent(37,2);
  Histogram1__3->SetBinContent(38,2);
  Histogram1__3->SetBinContent(39,4);
  Histogram1__3->SetBinContent(40,3);
  Histogram1__3->SetBinContent(41,2);
  Histogram1__3->SetBinContent(42,2);
  Histogram1__3->SetBinContent(44,3);
  Histogram1__3->SetBinContent(45,5);
  Histogram1__3->SetBinContent(46,51);
  Histogram1__3->SetBinContent(47,8);
  Histogram1__3->SetBinContent(48,14);
  Histogram1__3->SetBinContent(49,2);
  Histogram1__3->SetBinContent(56,55);
  Histogram1__3->SetBinContent(57,16);
  Histogram1__3->SetBinContent(77,76);
  Histogram1__3->SetBinContent(78,12);
  Histogram1__3->SetEntries(2304779);

  TCanvas* can = new TCanvas();
  Histogram1__1->Draw();
  Histogram1__1->SetFillColorAlpha(kBlue, 0.35);
  Histogram1__1->SetFillStyle(3004);
  Histogram1__3->SetLineColor(kRed);
  Histogram1__3->SetFillColorAlpha(kRed, 0.35);
  Histogram1__3->SetFillStyle(3005);
  Histogram1__3->Draw("SAME");

  TLegend* leg = new TLegend(0.8,0.8,1.,1.);
  leg->SetHeader("Coincidence validation");
  leg->AddEntry(Histogram1__1,"cuoresw master v3.0.1");
  leg->AddEntry(Histogram1__3,"proposed upgrade: space based (default radius: 1200mm)");
  leg->Draw("SAME");
}
