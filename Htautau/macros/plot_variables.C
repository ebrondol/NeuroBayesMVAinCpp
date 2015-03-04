#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TSystem.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TLegend.h"


#if not defined(__CINT__) || defined(__MAKECINT__)
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"
#endif

// directory of the Inputfiles
const TString dir = "/scratch/ebrondol/NeuroBayesMVAinCpp/Htautau/input/";

//outputFile info
TFile* outFile;
std::string outFileName = "plotVar";
std::string extension = "pdf";

//histos maps
std::map<std::string,TH1F*> h_Sig;
std::map<std::string,TH1F*> h_Bkg;

bool checkFileExistence(string FileName){
  ifstream codegen(FileName.c_str(), ifstream::in);

  if(!codegen.is_open()) {
    cout << "PseudoCodegen-file " << FileName << " not found " << endl;
    return false;
  } else {
    return true;
  }

  return false;
}


void ReadMap(std::map<string,int> mymap){

  for (std::map<string,int>::iterator it = mymap.begin(); it != mymap.end(); ++it)
    std::cout << it->first << " => " << it->second << '\n';

  return;
}


std::map<string, int> readVarFile(string ConfigFileName, bool debug = false){

  ifstream codegen(ConfigFileName.c_str(), ifstream::in);
  map<string, int> VarProProFlagsMap;

  if(codegen.bad()) {
    cout << "PseudoCodegen-file " << ConfigFileName << " not found " << endl;
    return VarProProFlagsMap;
  } else {
    if(debug) cout << "Reading Pseudocodegenfile in " << ConfigFileName << endl;
  }

  string buffer;
  getline(codegen,buffer);              //STARTVARSET 

  while(true) {

    getline(codegen,buffer);
    unsigned int name_end = buffer.find(" ");
    if( buffer.substr(0, name_end ) == "ENDVARSET") {
      break;
    }

    if(name_end < buffer.size() ) {

      string flag = buffer.substr(name_end, buffer.size());
      VarProProFlagsMap[buffer.substr(0, name_end )] = atoi(flag.c_str());
    } else {
      VarProProFlagsMap[buffer.substr(0, name_end )] = 0;
    }


  }

  if(debug) cout << "Reading Map(VarProProFlagsMap):" << endl;
  if(debug) ReadMap(VarProProFlagsMap);

  return VarProProFlagsMap;
}


void DrawVar(const std::string& category, const int& rebin, const std::string& outFileName);

//TH1F* ratioHisto(TH1F* h_num, TH1F* h_den);

namespace VA {
  // for type declarations/linkings in the TMVATesting framework
  float weight, lumiWeight, splitFactor;
  int nbtag;
  vector<float> varfloat;
}

void plot_variables ( std::string varFile = "config/varFileList_complete", bool plotAllVariables = false )
{

  if(!checkFileExistence(varFile))
    return;

  std::cout << "==> Plotting Variables" << std::endl;

  TH1::SetDefaultSumw2(kTRUE); // All new histograms automatically store the sum of squares of errors

  //-----  
  //Load input variables options file(s)
  map<string, int> VarProProFlagsMap;
  VarProProFlagsMap = readVarFile(varFile, true);
  int nvar = VarProProFlagsMap.size();
  ofstream bkg_ASCII("bkg_train_ASCII.txt");
  ofstream sig_ASCII("sig_train_ASCII.txt");

  std::cout << "Plotting of " << nvar << " variables:" << std::endl;

  // Get signal and background event samples
  std::vector<TString> SignalFiles;
  std::vector<TString> BkgFiles;
  //train files
  SignalFiles.push_back(dir+"train_mvain_mu_sync_vbfhiggs_0.root");
  BkgFiles.push_back(dir+"train_mvain_mu_sync_dy1j_0.root");
  BkgFiles.push_back(dir+"train_mvain_mu_sync_dy2j_0.root");
  BkgFiles.push_back(dir+"train_mvain_mu_sync_dy3j_0.root");
  BkgFiles.push_back(dir+"train_mvain_mu_sync_dy4j_0.root");

  //test files
//  SignalFiles.push_back(dir+"test_mvain_mu_sync_vbfhiggs_0.root");
//  SignalFiles.push_back(dir+"test_mvain_mu_sync_ggfhiggs_0.root");
//  BkgFiles.push_back(dir+"test_mvain_mu_sync_dy1j_0.root");
//  BkgFiles.push_back(dir+"test_mvain_mu_sync_dy2j_0.root");
//  BkgFiles.push_back(dir+"test_mvain_mu_sync_dy3j_0.root");
//  BkgFiles.push_back(dir+"test_mvain_mu_sync_dy4j_0.root");

  std::cout << "Input files:" << std::endl;
  TChain* ntu_Sig = new TChain("TauCheck");
  for(unsigned int i = 0; i < SignalFiles.size(); i++){
    ntu_Sig -> Add(SignalFiles.at(i));
    std::cout << "\t" << SignalFiles.at(i) << " added at the Sig chain." << std::endl;
  }
  TChain* ntu_Bkg = new TChain("TauCheck");
  for(unsigned int i = 0; i < BkgFiles.size(); i++){
    ntu_Bkg -> Add(BkgFiles.at(i));
    std::cout << "\t" << BkgFiles.at(i) << " added at the Bkg chain." << std::endl;
  }

  if( ntu_Sig->GetEntries() == 0 || ntu_Bkg->GetEntries() == 0 )
  {
    std::cout << "Error!! at least one file is empty" << std::endl;
    return;
  }
  
  VA::varfloat = vector<float>(nvar); 
  float lumi = 0.0;
  float weight = 0.0;
  float split = 0.0;

  int ivar = 0;
  for(std::map<string,int>::iterator it = VarProProFlagsMap.begin(); it != VarProProFlagsMap.end(); ++it) {
    ntu_Sig->SetBranchAddress(it->first.c_str(), &VA::varfloat[ivar]);
    ivar++;
  }
  ntu_Sig->SetBranchAddress("lumiWeight", &lumi);
  ntu_Sig->SetBranchAddress("weight", &weight);
  ntu_Sig->SetBranchAddress("splitFactor", &split);

  //Creation of histos: max and min to be fixed
  int nEntries_Sig = ntu_Sig -> GetEntriesFast();
  ivar = 0;
  for(std::map<string,int>::iterator it = VarProProFlagsMap.begin(); it != VarProProFlagsMap.end(); ++it)
  {
    float min = 99999.;
    float max = 0.0;

    for(int ientry = 0; ientry < nEntries_Sig; ++ientry)
    {
      ntu_Sig -> GetEntry(ientry);
      
      if( VA::varfloat[ivar] < min )
        min = VA::varfloat[ivar];      
      if( VA::varfloat[ivar] > max )
        max = VA::varfloat[ivar];      

    }

    std::string category = (it->first.c_str());

    std::string histoName = "h_Sig_"+category;
    h_Sig[category] = new TH1F(histoName.c_str(),"", 100, min, max);
    h_Sig[category] -> Sumw2();

    histoName = "h_Bkg_"+category;
    h_Bkg[category] = new TH1F(histoName.c_str(),"", 100, min, max);
    h_Bkg[category] -> Sumw2();
 
    ivar++;
  } 

  category = "weight";
  std::string histoName = "h_Sig_"+category;
  h_Sig[category] = new TH1F(histoName.c_str(),"", 100, 0, 0.5);
  h_Sig[category] -> Sumw2();

  histoName = "h_Bkg_"+category;
  h_Bkg[category] = new TH1F(histoName.c_str(),"", 100, 0, 0.5);
  h_Bkg[category] -> Sumw2();

  //Filling histos
  int ivar = 0;
  for(std::map<string,int>::iterator it = VarProProFlagsMap.begin(); it != VarProProFlagsMap.end(); ++it) {
    ntu_Bkg->SetBranchAddress(it->first.c_str(), &VA::varfloat[ivar]);
    ivar++;
  }
  ntu_Bkg->SetBranchAddress("lumiWeight", &lumi);
  ntu_Bkg->SetBranchAddress("weight", &weight);
  ntu_Bkg->SetBranchAddress("splitFactor", &split);


  sig_ASCII << "SIGNAL" << endl;
  ivar = 0;
  for(std::map<string,int>::iterator it = VarProProFlagsMap.begin(); it != VarProProFlagsMap.end(); ++it)
  {
    sig_ASCII << it->first.c_str() << endl;
    for(int ientry = 0; ientry < nEntries_Sig; ++ientry)
    {
      ntu_Sig -> GetEntry(ientry);
      sig_ASCII << VA::varfloat[ivar] << " , " ;
      h_Sig[it->first.c_str()] -> Fill(VA::varfloat[ivar]);
    }
    ivar++;
    sig_ASCII << endl;
  }

  sig_ASCII << "tot_weight" << endl;
  for(int ientry = 0; ientry < nEntries_Sig; ++ientry)
  {
    ntu_Sig -> GetEntry(ientry);
    sig_ASCII << lumi*weight*split << " , ";
    h_Sig["weight"] -> Fill(lumi*weight*split);
  }
  sig_ASCII << endl;
  sig_ASCII.close();


  bkg_ASCII << "BACKGROUND" << endl;
  int nEntries_Bkg = ntu_Bkg -> GetEntriesFast();
  ivar = 0;
  for(std::map<string,int>::iterator it = VarProProFlagsMap.begin(); it != VarProProFlagsMap.end(); ++it)
  {
    bkg_ASCII << it->first.c_str() << endl;
    for(int ientry = 0; ientry < nEntries_Bkg; ++ientry)
    {
      ntu_Bkg -> GetEntry(ientry);
      bkg_ASCII << VA::varfloat[ivar] << " , " ;
      h_Bkg[it->first.c_str()] -> Fill(VA::varfloat[ivar]);
    }
    ivar++;
    bkg_ASCII << endl;
  }

  bkg_ASCII << "tot_weight" << endl;
  for(int ientry = 0; ientry < nEntries_Bkg; ++ientry)
  {
    ntu_Bkg -> GetEntry(ientry);
    bkg_ASCII << lumi*weight*split << " , ";
    h_Bkg["weight"] -> Fill(lumi*weight*split);
  }
  bkg_ASCII << endl;
  bkg_ASCII.close();

  //Plotting
  outFile = new TFile((outFileName+".root").c_str(),"RECREATE");

  TCanvas* dummy = new TCanvas("dummy","",0,0,700,600);
  dummy -> Print((outFileName+"."+extension+"[").c_str(),extension.c_str());

  for(std::map<string,int>::iterator it = VarProProFlagsMap.begin(); it != VarProProFlagsMap.end(); ++it)
  {
    DrawVar(it->first.c_str(), 1, outFileName);
  }

  DrawVar("weight", 1, outFileName);

  outFile -> Close();
  dummy -> Print((outFileName+"."+extension+"]").c_str(),extension.c_str());
  dummy -> Close();

}

//----------------------------------------------------

void DrawVar(const std::string& category, const int& rebin, const std::string& outFileName){

 TCanvas* c = new TCanvas(("c_"+category).c_str(),("Htautau - "+category).c_str(),0,0,700,600);
  c -> cd();
  c -> SetGridx();
  if( h_Sig[category]->Integral() > 0 )
    h_Sig[category] -> Scale( 1. * h_Bkg[category]->Integral() / h_Sig[category]->Integral() );

  char axisTitle[50];
  h_Sig[category] -> Rebin(rebin);
  sprintf(axisTitle,"events",h_Sig[category]->GetBinWidth(1));
 h_Sig[category] -> GetXaxis() -> SetLabelSize(0.04);
  h_Sig[category] -> GetXaxis() -> SetLabelFont(42);
  h_Sig[category] -> GetXaxis() -> SetTitleSize(0.04);
  h_Sig[category] -> GetXaxis() -> SetTitleOffset(1.10);
  h_Sig[category] -> GetXaxis() -> SetTitle(category.c_str());
  h_Sig[category] -> GetYaxis() -> SetLabelSize(0.04);
  h_Sig[category] -> GetYaxis() -> SetLabelFont(42);
  h_Sig[category] -> GetYaxis() -> SetTitleSize(0.04);
  h_Sig[category] -> GetYaxis() -> SetTitleOffset(1.60);
  h_Sig[category] -> GetYaxis() -> SetTitle(axisTitle);

  h_Sig[category] -> SetLineWidth(2);
  h_Sig[category] -> SetLineColor(4);
  h_Sig[category] -> SetFillColor(38);
  h_Sig[category] -> SetMarkerColor(4);
  h_Sig[category] -> SetMarkerSize(0);
  gPad->Update();

  h_Bkg[category] -> Rebin(rebin);
  h_Bkg[category] -> SetLineWidth(2);
  h_Bkg[category] -> SetLineColor(2);
  h_Bkg[category] -> SetFillColor(2);
  h_Bkg[category] -> SetFillStyle(3004);
  h_Bkg[category] -> SetMarkerColor(2);
  h_Bkg[category] -> SetMarkerStyle(20);
  h_Bkg[category] -> SetMarkerSize(0);
  gPad->Update();
 
  float maximum = h_Sig[category] -> GetMaximum();
  if( h_Bkg[category]->GetMaximum() > maximum) maximum = h_Bkg[category]->GetMaximum();

  h_Sig[category] -> SetMinimum(0.);
  h_Sig[category] -> SetMaximum(1.05*maximum);
  h_Sig[category] -> Draw("hist");
  h_Bkg[category] -> Draw("hist,sames");

  TLegend* legend = new TLegend(0.55, 0.75, 0.80, 0.85);
  legend -> SetFillColor(kWhite);
  legend -> SetFillStyle(1001);
  legend -> SetTextFont(42);
  legend -> SetTextSize(0.035);

  legend -> AddEntry(h_Sig[category],"VBF + ggH Signal","PL");
  legend -> AddEntry(h_Bkg[category],"DY Background","PL");

  legend->Draw("same");

  gPad -> Update();

  c -> Print((outFileName+"."+extension).c_str(),extension.c_str());
  delete c;

  outFile -> cd();

  h_Sig[category] -> Write();
  h_Bkg[category] -> Write();


}
