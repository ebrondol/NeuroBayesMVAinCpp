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
}
