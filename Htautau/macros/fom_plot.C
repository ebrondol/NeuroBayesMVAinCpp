#include <iostream>
#include <cmath>

#include "TFile.h"
#include "TH1F.h"
#include "TROOT.h"

#include "TAMS/TAMS.h"

using namespace std;

float fom_plot ( ) {

  gROOT->LoadMacro("TAMS/TAMS.h+");

  float bef;
  float aft;
  TFile* HistoSourceFile = new TFile("results/HttEvaluationsPlots.root", "READ");
  HistoSourceFile->GetListOfKeys()->Print();
  TList* KeyList = HistoSourceFile->GetListOfKeys();
  int nHistos = HistoSourceFile->GetListOfKeys()->GetEntries();

  vector<TH1F*> Histograms;
  for (int ientry = 0; ientry < nHistos; ientry++) {
    TString histname(KeyList->At(ientry)->GetName());
    TH1F* histo = (TH1F*) HistoSourceFile->Get( histname );

    Histograms.push_back(histo);
  }

  float sys = 0.2; // systematic error
  TAMS ta (Histograms[0], Histograms[1], sys);
  //ta.calc();
  bef = ta.ams_syst_stat();
  std::cout << "The Figure of Merit befor rebinning is: " << bef << std::endl;

  string filename ("results/befbin_Htt.pdf" );
  ta.savePlot( filename );

  //ta.rebin();
  //ta.calc();
  aft = ta.ams_syst_stat();
  filename = "results/aftbin_Htt.pdf";
  std::cout << "The Figure of Merit after rebinning is: " << aft << std::endl;
  ta.savePlot( filename );

  HistoSourceFile->Close();

  return aft;

}
