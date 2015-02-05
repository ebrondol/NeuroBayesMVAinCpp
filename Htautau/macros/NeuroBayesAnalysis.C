#include <iostream>
#include <cmath>
#include <fstream>
#include <algorithm>
#include <map>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <utility>

#include "TFile.h"
#include "TH1F.h"
#include "TROOT.h"
#include "TString.h"

#include "TAMS/TAMS.h"
#include "macros/fom_plot.h"

using namespace std;


pair<string, double> find_max(map<string, double> m){

  double currentMax = 0;
  string arg_max;

  for (std::map<string,double>::iterator it1 = m.begin(); it1 != m.end(); ++it1){
    if (it1->second > currentMax) {
        currentMax = it1->second;
        arg_max = it1->first;
    }
  }

  return make_pair<string, double>(arg_max,currentMax);
}


void NeuroBayesAnalysis() {

  gROOT->LoadMacro("TAMS/TAMS.h+");
  gROOT->LoadMacro("macros/fom_plot.h+");

  // -----
  // Getting the histos
  TFile* HistoSourceFile = new TFile("results/HttEvaluationsPlots.root", "READ");
//  HistoSourceFile->GetListOfKeys()->Print();
  TList* KeyList = HistoSourceFile->GetListOfKeys();
  int nHistos = HistoSourceFile->GetListOfKeys()->GetEntries();

  vector<TH1F*> Histograms;
  map<string,double> HistoFom;
  for (int ientry = 0; ientry < nHistos; ientry++) {
//  for (int ientry = 0; ientry < 100; ientry++) {
    TString histname(KeyList->At(ientry)->GetName());
    TH1F* histo = (TH1F*) HistoSourceFile->Get( histname );

    Histograms.push_back(histo);
  }

  // -----
  // Computing the fom
  ofstream fom_summary("results/fom_summary.txt");
  fom_summary << "Name\t\t|" << "FoM not rebinned\t\t|" << endl;
  float fom = 0.0;
  float fom_rebin = 0.0;
  bool rebin = false;
  bool useMinrb = false;

  for(unsigned int it = 0; it != Histograms.size(); it = it + 2){
//  for(unsigned int it = 0; it != 100; it = it + 2){
    std::string HistoName = Histograms.at(it)->GetName();
    unsigned int name_end = HistoName.find(".nb_signal");

    std::string HistoNameSaved = "results/FoM_" + HistoName.substr(0, name_end ) + ".pdf";
    fom = fom_plot(Histograms.at(it), Histograms.at(it+1), HistoNameSaved, rebin, useMinrb);

//    rebin = true;
//    HistoNameSaved = "results/FoM_" + HistoName.substr(0, name_end ) + "_rebin.pdf";
//    fom_rebin = fom_plot(Histograms.at(it), Histograms.at(it+1), HistoNameSaved, rebin, useMinrb);
    HistoFom[HistoName.substr(0, name_end )] = fom;
    fom_summary << HistoName.substr(0, name_end ) << " " << fom << endl;

  }


  pair<string, double> fom_max = find_max(HistoFom);
  cout << "---------------------------------------------------------" << endl;
  cout << "Max value at " << fom_max.first << "\n\t with a FoM of " << fom_max.second << endl;


  fom_summary.close();
  HistoSourceFile->Close();

  return;
}
