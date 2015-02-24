#include <iostream>
#include <cmath>
#include <fstream>
#include <algorithm>
#include <map>
#include <string>
#include <vector>
#include <utility>

#include "TFile.h"
#include "TH1F.h"
#include "TROOT.h"
#include "TString.h"

#include "TAMS/TAMS.h"
#include "macros/fom_plot.h"

using namespace std;

void cleaningDirectory(){

  TObject* old=gDirectory->GetList()->FindObject("s");
  gDirectory->GetList()->Remove(old);
  old=gDirectory->GetList()->FindObject("b");
  gDirectory->GetList()->Remove(old);
  old=gDirectory->GetList()->FindObject("ss");
  gDirectory->GetList()->Remove(old);
  old=gDirectory->GetList()->FindObject("bs");
  gDirectory->GetList()->Remove(old);

}

void NeuroBayesAnalysis(int maxEntries = 0, string outputName = "results/HttEvaluationsPlots.root" ) {

  gROOT->SetBatch();

  gROOT->LoadMacro("TAMS/TAMS.h+");
  gROOT->LoadMacro("macros/fom_plot.h+");
  gROOT->LoadMacro("macros/sorting.C+");

  // -----
  // Getting the histos
  TFile* HistoSourceFile = new TFile(outputName.c_str(), "READ");
//  HistoSourceFile->GetListOfKeys()->Print();
  TList* KeyList = HistoSourceFile->GetListOfKeys();
  int nHistos = HistoSourceFile->GetListOfKeys()->GetEntries();
  if (maxEntries != 0) nHistos = maxEntries;

  vector<TH1F*> Histograms;
  map<string,double> HistoFom;

  for (int ientry = 0; ientry < nHistos; ientry++) {

    TString histname(KeyList->At(ientry)->GetName());
    TH1F* histo = (TH1F*) HistoSourceFile->Get( histname );
    Histograms.push_back(histo);

  }

  // -----
  // Computing the fom
  float fom = 0.0;
  float fom_rebin = 0.0;
  bool rebin = false;
  bool useMinrb = false;

  for(unsigned int it = 0; it != Histograms.size(); it = it + 2){

    std::string HistoName = Histograms.at(it)->GetName();
    unsigned int name_end = HistoName.find(".nb_signal");

    std::string HistoNameSaved = "results/FoM_" + HistoName.substr(0, name_end ) + ".pdf";
    fom = fom_plot(Histograms.at(it), Histograms.at(it+1), HistoNameSaved, rebin, useMinrb);

    cleaningDirectory();

    rebin = true;
    HistoNameSaved = "results/FoM_" + HistoName.substr(0, name_end ) + "_rebin.pdf";
    fom_rebin = fom_plot(Histograms.at(it), Histograms.at(it+1), HistoNameSaved, rebin, useMinrb);

    HistoFom[HistoName.substr(0, name_end )] = fom;

    cleaningDirectory();

  }

  HistoSourceFile->Close();

  // -----
  // Sorting and printing the max
  sorting(HistoFom);

  return;
}
