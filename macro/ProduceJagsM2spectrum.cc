#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include "TFile.h"
#include "TH1D.h"
#include "TTree.h"

using namespace std;

// energy binning [keV]
const int gNbin = 10000;
const double gEmin = 0;
const double gEmax = 10000;
// coordinate (radius) binning [mm]
const int gNbinR = 1200;
const double gRmin = 0.;
const double gRmax = 1200.;

bool GetChannelPosition(const int ch,double& x,double& y,double& z){
  ifstream inputFile;
  //  string CUORE_INSTALL = getenv("CUORE_INSTALL");
  //  string path = "/pkg/modtiming/ChannelPosition.dat";
  //  inputFile.open((CUORE_INSTALL+path).c_str() );
  inputFile.open("/nfs/cuore1/scratch/gfantini/spacebased/input/ChannelPosition.dat");
  int Channel;
  double X,Y,Z;
  bool found=false;
  while(inputFile.good())
    {
      inputFile >> Channel >> X >> Y >> Z;
      if(Channel == ch){
        x=X;
        y=Y;
        z=Z;
        found = true;
        break;
      }
    }
  inputFile.close();
  return found;
}

double GetChannelDistance(const int ch1,const int ch2, const map<int,double> x,const map<int,double> y,const map<int,double> z)
{
  double R;
  R =  sqrt(pow(x.find(ch1)->second -x.find(ch2)->second ,2) + pow(y.find(ch1)->second - y.find(ch2)->second ,2) + pow(z.find(ch1)->second - z.find(ch2)->second ,2) );
  if(R < 0. || R > 1200.){
    cerr << "Fatal! GetChannelDistance error! Impossible distance < 0 OR > 1200 mm." << endl;
    exit(0);
  }
  return R;
}

void ProduceJagsM2spectrum()
{
  // File with result from bkg model
  string InputFile_b2g = "/nfs/cuore1/scratch/gfantini/spacebased/input/b2g_3018_3021.dat";
  string InputMcFolder = "/nfs/cuore1/data/simulation/CUORE/2017/ntp/";
  string OutputRootFile = "/nfs/cuore1/scratch/gfantini/spacebased/out2/ProduceJagsM2Spectrum.root";
  ifstream IF;
  IF.open(InputFile_b2g.c_str());

  // Save channel position
  map<int,double> PositionX;
  map<int,double> PositionY;
  map<int,double> PositionZ;
  for(int i=1; i<=988; i++)GetChannelPosition(i,PositionX[i],PositionY[i],PositionZ[i]);
  for(int i=1; i<=988; i++)cout << "Loaded position for channel "<< i << ": \t" << PositionX[i] << " / " << PositionY[i] << " / " << PositionZ[i] << endl;
 
  // do some printout (test)
  string Filename;
  string FunctionName;
  double Mu,Sigma; // gauss fit function
  double ReducedChi2,Pvalue,MinGauss,MaxGauss;
  string line;
  vector<string> McFileNames;
  vector<string> McFilePaths;
  vector<double> N;
  
  Mu=0.; Sigma=0.; ReducedChi2=0.; Pvalue=0.; MinGauss=0.; MaxGauss=0.; // init variables
  
  std::getline(IF,line); // throw away the 1st line
  int counter = 0;
  while(IF.good())
    {
      // read file and fill the vectors of MC files and normalization factors
      IF >> Filename >> FunctionName >> ReducedChi2 >> Mu >> Sigma >> Pvalue >> MinGauss >> MaxGauss;
      
      McFileNames.push_back(Filename);
      McFilePaths.push_back(InputMcFolder+Filename+".root");
      cout << "Added " << McFilePaths[counter] << endl;
      if(FunctionName == "gaus_n"){// the gaussian fit of the posterior converges --> contamination present!
	N.push_back(Mu);
      }else{ // the gaussian fit is inappropriate --> contamination absent (upper limit) --> set to 0.
	N.push_back(0.);
      }
      counter++;
      
      // print to screen what you just retrieved
      cout << "Filename:\t" << Filename << endl;
      cout << "FunctionName:\t" << FunctionName << endl;
      cout << "Mu:\t\t" << Mu << endl;
      cout << "Sigma:\t\t" << Sigma << endl;
      cout << "ReducedChi2:\t" << ReducedChi2 << endl;
      cout << "Pvalue:\t\t" << Pvalue << endl;
      cout << "MinGauss:\t"<< MinGauss << endl;
      cout << "MaxGauss:\t"<< MaxGauss << endl;
      cout << "************************************************************************" << endl;
    }

  // once the loop over the sources is done..
  if(N.size() != McFilePaths.size()){// consistency check
    cerr << "FATAL! Number of MC files is different from number of normalization coefficients." << endl;
    exit(1);
  }
  // file existence check  
  TFile *pMcFile;
  for(unsigned int i=0; i<N.size(); i++)
    {
      pMcFile = new TFile(McFilePaths[i].c_str(),"read");
      if(!pMcFile->IsOpen()){
	cerr << "FATAL! Could not open " << McFilePaths[i] << endl;
	exit(2);
      }else{
	cout << "[INFO] File " << McFilePaths[i] << " found and readable." << endl;
      }
      pMcFile->Close();
    }

  // compute sum of normalization factors (I assume this is correlated to the MC statistics..)
  double Nsum = 0.;
  for(unsigned int i=0; i<N.size(); i++)Nsum+=N[i];
  cout << "Nsum: \t " << Nsum << endl;

  // define vectors of histograms, one for each MC file
  vector<TH1D*> HistoM2;
  vector<TH1D*> HistoRadiusM2;
  TH1D* HistoTmp;
  TH1D* HistoRtmp;
  // the true final loop over MC files to load events and produce distributions
  for(unsigned int i=0; i<N.size(); i++)
    {
      HistoTmp = new TH1D(McFileNames[i].c_str(),McFileNames[i].c_str(),gNbin,gEmin,gEmax);
      HistoRtmp = new TH1D(McFileNames[i].c_str(),McFileNames[i].c_str(),gNbinR,gRmin,gRmax);
      HistoM2.push_back(HistoTmp);
      HistoRadiusM2.push_back(HistoRtmp);
      if(N[i] != 0.)
	{ // if the contamination is relevant
	  cout << "[INFO] Processing "<< McFilePaths[i] << endl;
	  pMcFile = new TFile(McFilePaths[i].c_str(),"read"); // open file
	  if(pMcFile->IsZombie() || !pMcFile->IsOpen()){
	    cerr << "FATAL! Could not open " << McFilePaths[i] << endl; 
	    exit(2);
	  }
	  // set the relevant variables
	  TTree *OutTree = 0;
	  Int_t Detector; // Channel number in DAQ numbering
	  Double_t Ener2; // Energy corrected by quenching and ...
	  Double_t ESum2; // TotalEnergy of the multiplet corrected by quenching etc..
	  Short_t Multiplicity; // self-explaining..
	  Short_t MultipletIndex; // index of the main event in the multiplet array
	  Int_t Multiplet[988]; // array filled every time from 0..Multiplicity-1 with the Detector number of coincident channels
	  
	  // get the tree of g4cuore and the variables I am looking for
	  pMcFile->GetObject("outTree",OutTree);
	  if(OutTree != 0){
	    cout << "[INFO] Loaded outTree." << endl;
	  }else{
	    cerr << "FATAL! Could not read outTree!" << endl;
	    exit(4);
	  }
	  OutTree->SetBranchAddress("Detector",&Detector);
	  OutTree->SetBranchAddress("Ener2",&Ener2);
	  OutTree->SetBranchAddress("ESum2",&ESum2);
	  OutTree->SetBranchAddress("Multiplicity",&Multiplicity);
	  OutTree->SetBranchAddress("MultipletIndex",&MultipletIndex);
	  OutTree->SetBranchAddress("Multiplet",Multiplet);
	  int Nentries = OutTree->GetEntries();
	  for(unsigned int j=0; j<OutTree->GetEntries(); j++)
	    { // loop over MC file "i" and fill the histogram
	      if(j%(Nentries/100) == 0)cout << "[INFO]" << j << "/" << Nentries << " completed." << endl;
	      OutTree->GetEntry(j);
	      if(Multiplicity == 2)
		{
		  HistoM2[i]->Fill(ESum2); // fill histo with energy M2sum
		  HistoRadiusM2[i]->Fill(GetChannelDistance(Multiplet[0],Multiplet[1],PositionX,PositionY,PositionZ) );
		}
	    }
	  cout << "[INFO] 100% completed." << endl;
	  cout << "================================" << endl;
	  pMcFile->Close();
	  // <---- normalize the histogram 
	}else{
	HistoM2[i]->Fill(gEmin - .5*(gEmax-gEmin) ); // fill only 1 underflow event, so that histo is not empty
	HistoRadiusM2[i]->Fill(-1.); // fill only 1 underflow event, so that histo is not empty 
      }
    }
  
  // put together all the histograms into the sum histogram
  TH1D* HistoM2all = new TH1D("M2spectrum","M2 spectrum (MC normalized)",gNbin,gEmin,gEmax);
  for(unsigned int i=0;i<HistoM2.size();i++)(*HistoM2all)=(*HistoM2all)+N[i]*(*HistoM2[i]);
  TH1D* HistoRm2All = new TH1D("RadiusM2","M2 spectrum Radius (MC normalized)",gNbinR,gRmin,gRmax);
  for(unsigned int i=0;i<HistoRadiusM2.size();i++)(*HistoRm2All)=(*HistoRm2All)+N[i]*(*HistoRadiusM2[i]);
  // open a ROOT file for output and dump everything inside
  TFile *RootFileOutput = new TFile(OutputRootFile.c_str(),"recreate");
  if(HistoM2.size() != HistoRadiusM2.size()){// sanity check
    cerr << "Fatal. Size of HistoM2 != HistoRadiusM2. Some bug is there." << endl;
    exit(1);
  }
  for(unsigned int i=0;i<HistoM2.size();i++){
    HistoM2[i]->Write();
    HistoRadiusM2[i]->Write();
  }
  // write the global histogram (sum with weights)
  HistoM2all->Write();
  HistoRm2All->Write();
  RootFileOutput->Close();
  cout << "[INFO] Written output into " << OutputRootFile << endl;
}


int main()
{

  ProduceJagsM2spectrum();
  return 0;
}

