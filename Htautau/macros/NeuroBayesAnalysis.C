#include <iostream>
#include <cmath>

#include "TFile.h"
#include "TH1F.h"
#include "TROOT.h"
#include "TString.h"

#include "TAMS/TAMS.h"
#include "macros/fom_plot.h"

using namespace std;

void NeuroBayesAnalysis() {

  gROOT->LoadMacro("TAMS/TAMS.h+");
  gROOT->LoadMacro("macros/fom_plot.h+");


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

  float fom = 0.0;
  bool rebin = false;
  bool useMinrb = false;
  for(unsigned int it = 0; it != Histograms.size(); it = it + 2){
    std::string HistoName = Histograms.at(it)->GetName();
    unsigned int name_end = HistoName.find(".nb_signal");
cout << HistoName << endl;

    std::string HistoNameSaved = "results/FoM_" + HistoName.substr(0, name_end ) + ".pdf";
    fom = fom_plot(Histograms.at(it), Histograms.at(it+1), HistoNameSaved, rebin, useMinrb);

    rebin = true;
    HistoNameSaved = "results/FoM_" + HistoName.substr(0, name_end ) + "_rebin.pdf";
    fom = fom_plot(Histograms.at(it), Histograms.at(it+1), HistoNameSaved, rebin, useMinrb);

  }

  HistoSourceFile->Close();

  return;
/*
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
  ta.calc();
  bef = ta.ams_syst_stat();
  std::cout << "The Figure of Merit befor rebinning is: " << bef << std::endl;

  string filename ("results/befbin_Htt.pdf" );
  ta.savePlot( filename );

  ta.rebin();
  ta.calc();
  aft = ta.ams_syst_stat();
  filename = "results/aftbin_Htt.pdf";
  std::cout << "The Figure of Merit after rebinning is: " << aft << std::endl;
  ta.savePlot( filename );

  HistoSourceFile->Close();

  return aft;
*/
}
